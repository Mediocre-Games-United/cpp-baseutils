#pragma once

#include "get_enabled_modules.hpp"
#ifdef CBU_ARCHIVE_ENABLED

#include "archive.h"
#include "archive_entry.h"
#include "file.hpp"
#include <map>
#include <filesystem>
#include <stdexcept>

#include <sys/types.h>

class BaseArchive {
public:
    BaseArchive() {}

    bool compress_folder_single(fpath folderpath,fpath outpath,string pad = "");
    bool extract_archive_single(fpath archivepath,fpath folderpath);
    bool load_from_stream(BYTEARRAY stream);

    void add_entry(fpath relpath,BYTEARRAY stream);
    BYTEARRAY get_entry(fpath relpath);
    bool has_entry(fpath relpath);

    bool save_to_file(fpath apath);
    bool load_from_file(fpath apath);

    inline vector<fpath> get_entries() {
        vector<fpath> list;
        for (auto [rel,et] : _entries) {
            list.push_back(rel);
        }

        return list;
    }
private:
    struct entry {
        bool exists = false;
        BYTEARRAY data;
        fpath rel;
        mode_t perm;
    };
    virtual void set_read_filters(archive *a) = 0;
    virtual void set_write_filters(archive *a) = 0;
    bool read_entry_from_archive(archive *a,archive_entry *entry,struct entry *out);
    bool write_entry_to_archive(archive *a,fpath relpath,struct entry in);

    fpath _archive_path;
    umap<fpath,entry> _entries;
};

class ArchiveTarGZ : public BaseArchive {
public:

private:
    void set_read_filters(archive *a) override;
    void set_write_filters(archive *a) override;
};
class ArchiveTarXZ : public BaseArchive {
public:
    ArchiveTarXZ(U8 lvl) : lvl(lvl) {
        if (lvl > 9) throw std::overflow_error("XZ Compression level cannot be higher than 9");
    }
private:
    U8 lvl;
    void set_read_filters(archive *a) override;
    void set_write_filters(archive *a) override;
};
class ArchiveZIP : public BaseArchive {
public:

private:
    void set_read_filters(archive *a) override;
    void set_write_filters(archive *a) override;
};
class ArchiveTar : public BaseArchive {
public:

private:
    void set_read_filters(archive *a) override;
    void set_write_filters(archive *a) override;
};


#endif
