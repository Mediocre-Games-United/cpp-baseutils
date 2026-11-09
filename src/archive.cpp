
#include "find_config.hpp"
#ifdef ARCHIVE_ENABLED

#include "archive.hpp"
#include "file.hpp"
#include "logger.hpp"

#include <algorithm>
#include <format>
#include <stdexcept>
#include "binary.hpp"

namespace {
    static void replace_backslashes(std::string& s) {
        std::replace(s.begin(), s.end(), '\\', '/');
    }

    static void strip_leading_dots(std::string& s) {
        // Simple policy: remove leading "./" and leading '.' segments.
        // ".hidden" will become "hidden" due to leading-dot removal.
        while (s.size() >= 2 && s[0] == '.' && s[1] == '/') {
            s.erase(s.begin()); // remove first '.'
        }
        while (!s.empty() && s.front() == '.') {
            s.erase(s.begin());
        }
        while (!s.empty() && s.front() == '/') s.erase(s.begin());
    }
}

CompressedArchive::CompressedArchive() = default;

CompressedArchive::CompressedArchive(const std::filesystem::path& diskPath) {
    if (!read_from_file_safe(diskPath)) {
        cbu::log_error(true, "Failed to read archive: " + diskPath.string());
    }
}

bool CompressedArchive::is_empty() const {
    return entries_.empty();
}

bool CompressedArchive::has_file(std::string rpath) {
    try {
        rpath = normalize_rpath(std::move(rpath));
    } catch (const std::exception& e) {
        cbu::log_warn(std::string("has_file(): invalid rpath: ") + e.what());
        return false;
    }
    return entries_.find(rpath) != entries_.end();
}

CompressedArchive::BYTEARRAY CompressedArchive::require_file(std::string rpath) {
    rpath = normalize_rpath(std::move(rpath));
    auto it = entries_.find(rpath);
    if (it == entries_.end()) {
        cbu::log_error(true, "File not found in archive: " + rpath);
    }
    return it->second.data; // copy
}

void CompressedArchive::write_file(std::string rpath, BYTEARRAY& data) {
    rpath = normalize_rpath(std::move(rpath));

    Entry e;
    e.type = EntryType::RegularFile;
    e.data = std::move(data); // moves from caller; change to copy if desired
    e.perm = 0644;
    e.mtime = 0;

    entries_[rpath] = std::move(e);
    cbu::log_debug("write_file(): '" + rpath + "', bytes=" +
    std::to_string(entries_[rpath].data.size()));
}

bool CompressedArchive::write_to_file(std::filesystem::path fpath) {
    cbu::log_info("Building .tar.gz bytes for: " + fpath.string());

    BYTEARRAY data = write_to_bytes_safe();

    if (data.empty() && !entries_.empty()) {
        cbu::log_error(false, "cbu::write_to_file_safe_binary(): archive build produced empty bytes");
        return false;
    }

    cbu::write_to_file_safe_binary(std::move(fpath), data); // call your file.hpp function
    cbu::log_success("Wrote archive bytes to disk. bytes=" + std::to_string(data.size()));
    return true;
}

bool CompressedArchive::read_from_file_safe(std::filesystem::path fpath) {
    entries_.clear();

    cbu::log_info("Reading archive: " + fpath.string());
    bool ok = read_tar_gz_from_disk(fpath);
    if (!ok) {
        cbu::log_error(false, "read_from_file_safe(): failed to read '" + fpath.string() + "'");
        return false;
    }
    cbu::log_success("Loaded archive. entries=" + std::to_string(entries_.size()));
    return true;
}

CompressedArchive::BYTEARRAY CompressedArchive::write_to_bytes_safe() {
    struct ByteVectorOut {
        std::vector<U8> out;
    } vo;

    auto vectorWrite = [](struct archive*, void* client_data,
                          const void* buff, size_t n) -> la_ssize_t {
                              auto* v = static_cast<ByteVectorOut*>(client_data);
                              const U8* p = static_cast<const U8*>(buff);
                              v->out.insert(v->out.end(), p, p + n);
                              return static_cast<la_ssize_t>(n);
                          };

                          archive* a = archive_write_new();
                          if (!a) {
                              cbu::log_error(false, "archive_write_new() failed");
                              return {};
                          }

                          // Explicitly log compression + container format choices.
                          cbu::log_verbose("write_to_bytes_safe(): set format = gnutar");
                          cbu::log_verbose("write_to_bytes_safe(): set filter  = gzip");

                          archive_write_set_format_gnutar(a);
                          archive_write_add_filter_gzip(a);

                          // Use libarchive memory-backed output via callbacks
                          int r = archive_write_open2(a, &vo, nullptr, vectorWrite, nullptr, NULL);
                          if (r != ARCHIVE_OK) {
                              std::string err = archive_error_string(a);
                              archive_write_free(a);
                              cbu::log_error(false, "archive_write_open2() failed: " + err);
                              return {};
                          }

                          for (const auto& [name, e] : entries_) {
                              cbu::log_verbose("write_to_bytes_safe(): adding entry '" + name +
                              "' bytes=" + std::to_string(e.data.size()));
                              if (!write_entry_to_archive(a, name, e)) {
                                  std::string err = archive_error_string(a);
                                  archive_write_close(a);
                                  archive_write_free(a);
                                  cbu::log_error(false, "Failed writing entry '" + name + "': " + err);
                                  return {};
                              }
                          }

                          archive_write_close(a);
                          archive_write_free(a);

                          return std::move(vo.out);
}

std::string CompressedArchive::normalize_rpath(std::string rpath) {
    replace_backslashes(rpath);

    while (!rpath.empty() && rpath.front() == '/') rpath.erase(rpath.begin());
    strip_leading_dots(rpath);

    if (is_path_unsafe_in_archive(rpath)) {
        throw std::invalid_argument("Unsafe/invalid archive rpath: " + rpath);
    }
    return rpath;
}

bool CompressedArchive::is_path_unsafe_in_archive(const std::string& s) {
    if (s.empty()) return true;
    if (s.front() == '/') return true;
    if (s.find("..") != std::string::npos) return true;
    if (s.find('\0') != std::string::npos) return true;
    return false;
}

bool CompressedArchive::read_tar_gz_from_disk(const std::filesystem::path& diskPath) {
    archive* a = archive_read_new();
    if (!a) {
        cbu::log_error(false, "archive_read_new() failed");
        return false;
    }

    // Force: gzip-compressed tar
    cbu::log_verbose("read_tar_gz_from_disk(): configuring reader: filter=gzip, format=gnutar");
    archive_read_support_filter_gzip(a);
    archive_read_support_format_tar(a);

    int r = archive_read_open_filename(a, diskPath.string().c_str(), 10240);
    if (r != ARCHIVE_OK) {
        std::string err = archive_error_string(a);
        archive_read_free(a);
        cbu::log_error(false, "archive_read_open_filename() failed: " + err);
        return false;
    }

    // Optional: log what libarchive reports as the active filter/format after open
    const char* detectedFmt = archive_format_name(a);
    const char* detectedFilter = archive_filter_name(a,0);
    if (detectedFmt) {
        cbu::log_verbose(std::string("read_tar_gz_from_disk(): detected format = ") + detectedFmt);
    } else {
        cbu::log_verbose("read_tar_gz_from_disk(): detected format = (unknown/null)");
    }
    if (detectedFilter) {
        cbu::log_verbose(std::string("read_tar_gz_from_disk(): detected filter = ") + detectedFilter);
    } else {
        cbu::log_verbose("read_tar_gz_from_disk(): detected filter = (unknown/null)");
    }

    archive_entry* entry = nullptr;
    while (true) {
        int nr = archive_read_next_header(a, &entry);
        if (nr == ARCHIVE_EOF) {
            cbu::log_verbose("read_tar_gz_from_disk(): reached EOF");
            break;
        }

        if (nr != ARCHIVE_OK) {
            std::string err = archive_error_string(a);
            archive_read_close(a);
            archive_read_free(a);
            cbu::log_error(false, "archive_read_next_header() failed: " + err);
            return false;
        }

        const char* rawPathname = entry ? archive_entry_pathname(entry) : nullptr;
        cbu::log_verbose(std::string("read_tar_gz_from_disk(): found entry header: pathname=") +
        (rawPathname ? rawPathname : "(null)"));

        if (!read_entry_from_archive(a, entry, entries_)) {
            archive_read_close(a);
            archive_read_free(a);
            cbu::log_error(false, "Failed reading an entry");
            return false;
        }
    }

    archive_read_close(a);
    archive_read_free(a);
    return true;
}


bool CompressedArchive::read_entry_from_archive(struct archive* a,
                                                struct archive_entry* entry,
                                                std::map<std::string, Entry>& out) {
    const char* pathname = archive_entry_pathname(entry);
    if (!pathname) {
        archive_read_data_skip(a);
        cbu::log_debug("Skipping entry with null pathname");
        return true;
    }

    std::string name = pathname;

    // Normalize separators
    std::replace(name.begin(), name.end(), '\\', '/');

    // Remove leading './' or leading dots
    if (!name.empty() && name.front() == '.') {
        strip_leading_dots(name);
    } else {
        while (!name.empty() && name.front() == '/') name.erase(name.begin());
    }

    cbu::log_verbose("read_entry_from_archive(): normalized entry name='" + name + "'");

    if (is_path_unsafe_in_archive(name)) {
        archive_read_data_skip(a);
        cbu::log_warn("Skipping suspicious entry name: '" + name + "'");
        return true;
    }

    const int filetype = archive_entry_filetype(entry);
    const mode_t perm = static_cast<mode_t>(archive_entry_perm(entry));
    const S64 mtime = static_cast<S64>(archive_entry_mtime(entry));

    // Log metadata we use/see for the member.
    cbu::log_verbose("read_entry_from_archive(): entry='" + name +
    "' filetype=" + std::to_string(filetype) +
    " perm=" + std::to_string(static_cast<unsigned long>(perm)) +
    " mtime=" + std::to_string(mtime));

    if (filetype == AE_IFREG) {
        const S64 size = archive_entry_size(entry);

        Entry e;
        e.type = EntryType::RegularFile;
        e.perm = perm;
        e.mtime = mtime;

        cbu::log_verbose("read_entry_from_archive(): reading regular file '" + name +
        "' declared_size=" + std::to_string(size));

        if (size >= 0) {
            e.data.resize(static_cast<size_t>(size));
            size_t off = 0;
            while (off < static_cast<size_t>(size)) {
                ssize_t got = archive_read_data(
                    a,
                    e.data.data() + off,
                                                static_cast<size_t>(size) - off
                );
                if (got <= 0) break;
                off += static_cast<size_t>(got);
            }

            if (e.data.size() < 1000) {
                cbu::log_verbose(std::format("Data: {}",cbu::bytes_to_string(e.data.data(),e.data.size())));
            } else {
                cbu::log_verbose("Did not print, size is too large");
            }
            cbu::log_verbose("read_entry_from_archive(): completed read '" + name +
            "' bytes_read=" + std::to_string(off));
        } else {
            // Unknown size
            cbu::log_verbose("read_entry_from_archive(): unknown size for '" + name +
            "', reading until EOF");

            std::vector<U8> buf(64 * 1024);
            while (true) {
                ssize_t got = archive_read_data(a, buf.data(), buf.size());
                if (got == 0) break;
                if (got < 0) break;
                e.data.insert(e.data.end(), buf.begin(), buf.begin() + got);
            }


            cbu::log_verbose("read_entry_from_archive(): completed read '" + name +
            "' bytes_read=" + std::to_string(e.data.size()));
        }

        out[name] = std::move(e);
        cbu::log_verbose("read_entry_from_archive(): stored entry '" + name +
        "' total_bytes=" + std::to_string(out[name].data.size()));
        return true;
    }

    // Skip non-regular files for now
    archive_read_data_skip(a);
    cbu::log_verbose("read_entry_from_archive(): skipping non-regular entry '" + name + "'");
    return true;
                                                }

                                                bool CompressedArchive::write_entry_to_archive(struct archive* a,
                                                                                               const std::string& name,
                                                                                               const Entry& e) {
                                                    archive_entry* ent = archive_entry_new();
                                                    if (!ent) return false;

                                                    archive_entry_set_pathname(ent, name.c_str());
                                                    archive_entry_set_filetype(ent, AE_IFREG);
                                                    archive_entry_set_perm(ent, e.perm);

                                                    if (e.mtime > 0) {
                                                        archive_entry_set_mtime(ent, static_cast<time_t>(e.mtime), 0);
                                                    }

                                                    archive_entry_set_size(ent, static_cast<S64>(e.data.size()));

                                                    int r = archive_write_header(a, ent);
                                                    if (r != ARCHIVE_OK) {
                                                        archive_entry_free(ent);
                                                        return false;
                                                    }

                                                    const U8* p = e.data.data();
                                                    size_t remaining = e.data.size();

                                                    while (remaining > 0) {
                                                        size_t chunk = std::min<size_t>(remaining, 64 * 1024);
                                                        S64 wrote = archive_write_data(a, p, chunk);
                                                        if (wrote < 0) {
                                                            archive_entry_free(ent);
                                                            return false;
                                                        }
                                                        p += static_cast<size_t>(wrote);
                                                        remaining -= static_cast<size_t>(wrote);
                                                    }

                                                    archive_entry_free(ent);
                                                    return true;
                                                                                               }


#endif
