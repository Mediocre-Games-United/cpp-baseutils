#include "archive.hpp"
#include <filesystem>
#ifdef CBU_ARCHIVE_ENABLED

#include "file.hpp"
#include "logger.hpp"

#include <algorithm>
#include <format>
#include <stdexcept>
#include "binary.hpp"
#include "archive.h"

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

bool BaseArchive::load_from_stream(BYTEARRAY stream) {
    archive* a = archive_read_new();
    if (!a) {
        cbu::log_error(false, "archive_read_new() failed");
        return false;
    }

    set_read_filters(a);

    int r = archive_read_open_memory(
        a,
        stream.empty() ? nullptr : stream.data(),
                                     stream.size()
    );

    if (r != ARCHIVE_OK) {
        const char* message = archive_error_string(a);
        std::string err = message ? message : "unknown error";
        archive_read_free(a);
        cbu::log_error(false, "archive_read_open_memory() failed: " + err);
        return false;
    }

    cbu::log_verbose("Beginning to read from archive");
    archive_entry* entry = nullptr;

    while (true) {
        int nr = archive_read_next_header(a, &entry);
        if (nr == ARCHIVE_EOF) {
            cbu::log_verbose("Reached archive EOF");
            break;
        }

        if (nr != ARCHIVE_OK) {
            const char* message = archive_error_string(a);
            std::string err = message ? message : "unknown error";
            archive_read_close(a);
            archive_read_free(a);
            cbu::log_error(false, "archive_read_next_header() failed: " + err);
            return false;
        }

        const char* rawPathname = entry ? archive_entry_pathname(entry) : nullptr;
        cbu::log_verbose(std::string("load_from_stream(): found entry header: pathname=") +
        (rawPathname ? rawPathname : "(null)"));

        struct entry et;
        if (!read_entry_from_archive(a, entry, &et)) {
            archive_read_close(a);
            archive_read_free(a);
            cbu::log_error(false, "Failed reading an entry");
            return false;
        }

        _entries[et.rel] = et;
    }

    archive_read_close(a);
    archive_read_free(a);
    return true;
}
bool BaseArchive::load_from_file(fpath apath) {
    archive* a = archive_read_new();
    _archive_path = apath;
    if (!a) {
        cbu::log_error(false, "archive_read_new() failed");
        return false;
    }
    set_read_filters(a);

    int r = archive_read_open_filename(a,apath,10240);
    if (r != ARCHIVE_OK) {
        std::string err = archive_error_string(a);
        archive_read_free(a);
        cbu::log_error(false, "archive_read_open_filename() failed: " + err);
        return false;
    }

    cbu::log_verbose("Beginning to read from archive");
    archive_entry *entry = NULL;
    while (true) {
        int nr = archive_read_next_header(a,&entry);
        if (nr == ARCHIVE_EOF) {
            cbu::log_verbose("Reached archive EOF");
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

        struct entry et;
        if (!read_entry_from_archive(a,entry,&et)) {
            archive_read_close(a);
            archive_read_free(a);
            cbu::log_error(false, "Failed reading an entry");
            return false;
        } else _entries[et.rel] = et;
    }

    archive_read_close(a);
    archive_read_free(a);
    return true;
}


bool BaseArchive::save_to_file(fpath tpath) {

    auto buildBytes = [tpath,this]() -> BYTEARRAY {
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

                              set_write_filters(a);

                              // Use libarchive memory-backed output via callbacks
                              int r = archive_write_open2(a, &vo, nullptr, vectorWrite, nullptr, NULL);
                              if (r != ARCHIVE_OK) {
                                  std::string err = archive_error_string(a);
                                  archive_write_free(a);
                                  cbu::log_error(false, "archive_write_open2() failed: " + err);
                                  return {};
                              }

                              for (const auto& [name, e] : _entries) {
                                  cbu::log_verbose("write_to_bytes_safe(): adding entry '" + string(name) +
                                  "' bytes=" + std::to_string(e.data.size()));
                                  if (!write_entry_to_archive(a, name, e)) {
                                      std::string err = archive_error_string(a);
                                      archive_write_close(a);
                                      archive_write_free(a);
                                      cbu::log_error(false, "Failed writing entry '" + string(name) + "': " + err);
                                      return {};
                                  }
                              }

                              archive_write_close(a);
                              archive_write_free(a);

                              return vo.out;
    };

    BYTEARRAY built = buildBytes();

    if (built.empty()) {
        cbu::log_error(false,"Archive is empty! Skipping...");
        return false;
    }

    cbu::write_to_file_safe_binary(tpath,built);
    return true;
}

bool path_is_unsafe(const std::string& s) {
    if (s.empty()) return true;
    if (s.front() == '/') return true;
    if (s.find("..") != std::string::npos) return true;
    if (s.find('\0') != std::string::npos) return true;
    return false;
}
bool BaseArchive::read_entry_from_archive(struct archive* a,
                                                struct archive_entry* entry,
                                                struct entry *out) {
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

    if (path_is_unsafe(name)) {
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
        out->exists = true;
        out->data.clear();
        out->rel = name;
        out->perm = perm;

        cbu::log_verbose("read_entry_from_archive(): reading regular file '" + name +
        "' declared_size=" + std::to_string(size));

        if (size >= 0) {
            out->data.resize(static_cast<size_t>(size));
            size_t off = 0;
            while (off < static_cast<size_t>(size)) {
                ssize_t got = archive_read_data(
                    a,
                    out->data.data() + off,
                                                static_cast<size_t>(size) - off
                );
                if (got <= 0) break;
                off += static_cast<size_t>(got);
            }

            if (out->data.size() < 1000) {
                cbu::log_verbose(std::format("Data: {}",cbu::bytes_to_string(out->data.data(),out->data.size())));
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
                out->data.insert(out->data.end(), buf.begin(), buf.begin() + got);
            }


            cbu::log_verbose("read_entry_from_archive(): completed read '" + name +
            "' bytes_read=" + std::to_string(out->data.size()));
        }

        cbu::log_verbose("read_entry_from_archive(): stored entry '" + name +
        "' total_bytes=" + std::to_string(out->data.size()));
        return true;
    }

    // Skip non-regular files for now
    archive_read_data_skip(a);
    cbu::log_verbose("read_entry_from_archive(): skipping non-regular entry '" + name + "'");
    return true;
}
bool BaseArchive::write_entry_to_archive(struct archive* a,
                                               fpath relpath,
                                               struct entry in) {
    archive_entry* ent = archive_entry_new();
    if (!ent) return false;

    archive_entry_set_pathname(ent,relpath);
    archive_entry_set_filetype(ent,AE_IFREG);
    archive_entry_set_perm(ent,in.perm);

    // if (e.mtime > 0) {
    //     archive_entry_set_mtime(ent, static_cast<time_t>(e.mtime), 0);
    // }

    archive_entry_set_size(ent, static_cast<S64>(in.data.size()));

    int r = archive_write_header(a, ent);
    if (r != ARCHIVE_OK) {
        archive_entry_free(ent);
        return false;
    }

    const U8* p = in.data.data();
    size_t remaining = in.data.size();

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

bool BaseArchive::has_entry(fpath relpath) {
    return _entries.contains(relpath);
}
void BaseArchive::add_entry(fpath relpath,BYTEARRAY stream) {
    _entries[relpath] = {
        .exists = true,
        .data = stream,
        .rel = relpath,
        .perm = 0644
    };

    cbu::log_verbose(std::format("Added {} to archive, archive now has {} entries",relpath,_entries.size()));
}
BYTEARRAY BaseArchive::get_entry(fpath relpath) {
    if (!has_entry(relpath)) cbu::log_error(true,std::format("{} does not exist in archive!",relpath));

    return _entries[relpath].data;
}


void ArchiveTarGZ::set_read_filters(archive *a) {
    cbu::log_verbose("read archive: filter=gzip, format=gnutar");

    archive_read_support_filter_gzip(a);
    archive_read_support_format_tar(a);
}
void ArchiveTarGZ::set_write_filters(archive *a) {
    cbu::log_verbose("write archive: filter=gzip, format=gnutar");

    archive_write_add_filter_gzip(a);
    archive_write_set_format_gnutar(a);
}
void ArchiveTarXZ::set_read_filters(archive *a) {
    cbu::log_verbose(std::format("read archive: filter=xz, level={}, format=gnutar",lvl));
    archive_read_support_filter_xz(a);
    archive_read_support_format_tar(a);

    archive_read_set_options(a,std::format("xz:compression-level={}",lvl).c_str());
}
void ArchiveTarXZ::set_write_filters(archive *a) {
    cbu::log_verbose(std::format("write archive: filter=xz, level={}, format=gnutar",lvl));

    archive_write_add_filter_xz(a);
    archive_write_set_format_gnutar(a);

    archive_write_set_options(a,std::format("xz:compression-level={}",lvl).c_str());
}
void ArchiveZIP::set_read_filters(archive *a) {
    cbu::log_verbose("read archive: format=zip");
    archive_read_support_format_zip(a);
}
void ArchiveZIP::set_write_filters(archive *a) {
    cbu::log_verbose("write archive: format=zip");
    archive_write_set_format_zip(a);
}
void ArchiveTar::set_read_filters(archive *a) {
    cbu::log_verbose("read archive: format=gnutar");
    archive_read_support_format_tar(a);
}
void ArchiveTar::set_write_filters(archive *a) {
    cbu::log_verbose("write archive: format=gnutar");

    archive_write_set_format_gnutar(a);
}

static void recurse_folders(fpath f,vector<fpath> &entries) {
    for (fpath s : cbu::iterate_dir(f)) {
        if (s.is_directory()) {
            recurse_folders(s,entries);
            continue;
        }
        if (!s.is_file()) continue;

        entries.push_back(s);
    }
}
bool BaseArchive::compress_folder_single(fpath folderpath,fpath outpath,string pad) {
    _entries.clear();

    vector<fpath> entries;
    recurse_folders(folderpath,entries);

    for (fpath e : entries) {
        fpath rel = e.relative(folderpath);
        try {
            BYTEARRAY data = cbu::require_file_binary_path(e);
            if (pad.empty()) add_entry(rel,data);
            else add_entry(fpath(pad) / rel,data);
        } catch (...) {
            cbu::log_error(false,std::format("Failed to read file {}",e));
            return false;
        }
    }

    return save_to_file(outpath);
}
bool BaseArchive::extract_archive_single(fpath archivepath,fpath folderpath) {
    std::filesystem::remove_all(folderpath);

    if (!load_from_file(archivepath)) {
        cbu::log_error(false,"Failed to read archive");
        return false;
    }

    for (auto [rel,et] : _entries) {
        fpath abs = folderpath / rel;
        std::filesystem::create_directories(abs.parent_path());

        cbu::write_to_file_safe_binary(abs,et.data);
    }

    return true;
}

#endif
