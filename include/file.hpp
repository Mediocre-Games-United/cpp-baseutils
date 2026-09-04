#pragma once

#include "base_types.hpp"
#include "find_config.h"
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>

#ifdef _WIN32
#include "windows.h"
#include <shlobj.h>
#endif


typedef std::vector<uint8_t> BYTEARRAY;

inline std::string path_to_utf8(const fpath& p) {
    #ifdef _WIN32
    std::wstring ws = p.wstring(); // UTF-16 on Windows
    if (ws.empty()) return {};

    int needed = WideCharToMultiByte(CP_UTF8, 0,
                                     ws.data(), (int)ws.size(),
                                     nullptr, 0, nullptr, nullptr);
    std::string out(needed, '\0');
    WideCharToMultiByte(CP_UTF8, 0,
                        ws.data(), (int)ws.size(),
                        out.data(), needed, nullptr, nullptr);
    return out;
    #else
    // Non-Windows: assume filenames are already representable as narrow bytes for your environment.
    // (This avoids wchar_t/wide conversions.)
    return p.string();
    #endif
}
static fpath get_userdata_dir() {
    static const fpath k = []() -> fpath {
        #ifdef _WIN32
        wchar_t path[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, path))) {
            fpath base(path);
            return base / CMP_NAME / APP_NAME;;
        }
        return fpath(std::getenv("APPDATA") ? std::getenv("APPDATA") : ".") / CMP_NAME / APP_NAME;;

        #elif defined(__APPLE__)
        const char* home = std::getenv("HOME");
        return (home ? fpath(home) : fpath(".")) / "Library" / "Application Support" / CMP_NAME / APP_NAME;

        #else
        const char* xdg = std::getenv("XDG_DATA_HOME");
        if (xdg && *xdg) return fpath(xdg) / CMP_NAME / APP_NAME;

        const char* home = std::getenv("HOME");
        return (home ? fpath(home) : fpath(".")) / ".local/share" / CMP_NAME / APP_NAME;
        #endif
    }();
    return k;
}
inline fpath exe_dir() {
    #if defined(_WIN32)
    wchar_t buf[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, buf, MAX_PATH);
    return fpath(buf);
    #elif defined(__linux__)
    return std::filesystem::read_symlink("/proc/self/exe");
    #elif defined(__APPLE__)
    // Simple fallback if you don’t want full macOS code:
    // (tell me if you need the exact macOS implementation)
    return std::filesystem::current_path();
    #else
    return std::filesystem::current_path();
    #endif
}
inline fpath get_parent_dir() {
    fpath p = exe_dir();

    return p.parent_path();
}
inline fpath lang_path() {
    fpath p = get_parent_dir() / "lang";

    return p;
}
inline fpath get_user_path() {
    fpath p = get_userdata_dir();

    return p;
}


static fpath get_res_dir() {
    fpath p = get_parent_dir() / "resources";

    return p;
}
static fpath get_user_dir() {
    fpath p = get_userdata_dir();

    return p;
}
inline fpath resolve_path(fpath p) {
    std::string str = std::string(p);
    fpath path;
    if (str.substr(0,6) == "res://") {
        path = get_res_dir() / str.substr(6);
    }
    else if (str.substr(0,7) == "user://") {
        path = get_user_dir() / str.substr(7);
    }
    else path = p;

    return path;
}
inline std::string require_file_path(fpath path) {
    std::ifstream file(path,std::ios::in);
    if (!file) throw std::runtime_error(std::string("Failed to open: ") + path_to_utf8(path).c_str());

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}
inline BYTEARRAY require_file_binary_path(fpath path) {
    std::ifstream file(path, std::ios::binary | std::ios::in);
    if (!file) throw std::runtime_error(std::string("Failed to open: ") + path_to_utf8(path).c_str());

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    if (size < 0) throw std::runtime_error(std::string("Failed to read: ") + path_to_utf8(path).c_str());
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> data(static_cast<size_t>(size));
    if (size > 0 && !file.read(reinterpret_cast<char*>(data.data()), size))
        throw std::runtime_error(std::string("Failed to read: ") + path_to_utf8(path).c_str());

    return data;
}

inline std::string require_file(const char* path) {
    return require_file_path(resolve_path(path));
}
inline BYTEARRAY require_file_binary(const char* path) {
    return require_file_binary_path(resolve_path(path));
}
inline void write_to_file_safe_binary(fpath fpath,BYTEARRAY &data) {
    if (fpath.has_parent_path()) {
        std::filesystem::create_directories(fpath.parent_path());
    }

    std::ofstream ofs(fpath, std::ios::binary | std::ios::out | std::ios::trunc);
    ofs.write(reinterpret_cast<const char*>(data.data()),
              static_cast<std::streamsize>(data.size()));
}

inline std::vector<fpath> iterate_dir(fpath dir) {
    std::vector<fpath> l;
    try {
        if (!std::filesystem::exists(dir)) return {};
        if (!std::filesystem::is_directory(dir)) return {};

        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            l.push_back(entry.path());
        }
        return l;
    } catch (std::filesystem::filesystem_error err) {
        return {};
    }
}


#ifdef EDITOR
fpath get_editor_path() {
    return get_parent_dir() / "../";
}
#endif
inline fpath get_exe_path() {
    return exe_dir();
}

typedef std::vector<fpath> PATHARRAY;
