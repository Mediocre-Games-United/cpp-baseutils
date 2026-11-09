#pragma once

#include "find_config.hpp"
#ifdef ARCHIVE_ENABLED

#include "archive.h"
#include "archive_entry.h"
#include "file.hpp"
#include <map>

class CompressedArchive {
public:
    using BYTEARRAY = std::vector<U8>;
    using fpath = std::filesystem::path;
    using string = std::string;

    CompressedArchive();
    explicit CompressedArchive(const fpath& diskPath);

    bool has_file(string rpath);
    BYTEARRAY require_file(string rpath);
    void write_file(string rpath, BYTEARRAY& data);

    bool is_empty() const;

    bool read_from_file_safe(fpath fpath);

    // Writes the in-memory archive back to disk using file.hpp
    bool write_to_file(fpath fpath);

private:
    enum class EntryType { RegularFile };

    struct Entry {
        EntryType type = EntryType::RegularFile;
        std::vector<U8> data;
        mode_t perm = 0644;
        S64 mtime = 0;
    };

    std::map<string, Entry> entries_;

    static string normalize_rpath(string rpath);
    static bool is_path_unsafe_in_archive(const string& s);

    bool read_tar_gz_from_disk(const fpath& diskPath);

    static bool read_entry_from_archive(struct archive* a,
                                        struct archive_entry* entry,
                                        std::map<string, Entry>& out);

    static bool write_entry_to_archive(struct archive* a,
                                       const string& name,
                                       const Entry& e);

    // Builds a .tar.gz into memory and returns the resulting bytes.
    BYTEARRAY write_to_bytes_safe();
};

#endif
