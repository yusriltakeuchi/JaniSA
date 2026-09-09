#pragma once
// JaniSA user-defined bundle persistence.
// Stores the custom bundle (list of cheat NAMES) in an INI-style file on SD:
//   sdmc:/config/JaniSA/JaniSA.ini
// Same pattern as libtesla's own config read/write (fsFsOpenFile on sdmc:).
// Reserved at most MAX_CUSTOM_CHEATS cheats in the custom bundle.
#include <tesla.hpp>   // brings in <switch.h> (Fs*, sdmc devices)
#include <cstring>
#include <cstdio>
#include <string>
#include <vector>

namespace janisaConfig {

static constexpr const char* CONFIG_DIR  = "/config/JaniSA";
static constexpr const char* CONFIG_FILE = "/config/JaniSA/JaniSA.ini";
static constexpr const int    MAX_CUSTOM_CHEATS = 16;

// Load the custom bundle. Returns a vector of cheat names (empty if none/file error).
static inline std::vector<std::string> loadCustomBundle() {
    std::vector<std::string> out;

    FsFileSystem fs;
    if (R_FAILED(fsOpenSdCardFileSystem(&fs))) return out;
    tsl::hlp::ScopeGuard fsGuard([&]{ fsFsClose(&fs); });

    FsFile f;
    if (R_FAILED(fsFsOpenFile(&fs, CONFIG_FILE, FsOpenMode_Read, &f))) return out;
    tsl::hlp::ScopeGuard fGuard([&]{ fsFileClose(&f); });

    s64 size = 0;
    if (R_FAILED(fsFileGetSize(&f, &size)) || size <= 0) return out;

    std::string data((size_t)size, '\0');
    u64 read = 0;
    if (R_FAILED(fsFileRead(&f, 0, data.data(), (u64)size, FsReadOption_None, &read)) || read != (u64)size)
        return out;

    // parse lines: "Name" one per line (first line is the bundle title, optional)
    // Format: title on first line, then one cheat name per line.
    size_t start = 0;
    while (start < data.size()) {
        size_t nl = data.find('\n', start);
        std::string line = data.substr(start, nl == std::string::npos ? std::string::npos : nl - start);
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (!line.empty()) out.push_back(line);
        if (nl == std::string::npos) break;
        start = nl + 1;
    }
    return out;
}

// Save the custom bundle. lines[0] = title, lines[1..] = cheat names.
static inline bool saveCustomBundle(const std::vector<std::string>& lines) {
    FsFileSystem fs;
    if (R_FAILED(fsOpenSdCardFileSystem(&fs))) return false;
    tsl::hlp::ScopeGuard fsGuard([&]{ fsFsClose(&fs); });

    // mkdir config dir (ignore failure — may already exist)
    fsFsCreateDirectory(&fs, "/config");
    fsFsCreateDirectory(&fs, CONFIG_DIR);

    // create the file if it doesn't exist yet (Write mode alone won't create)
    FsFile f;
    if (R_FAILED(fsFsOpenFile(&fs, CONFIG_FILE, FsOpenMode_Read | FsOpenMode_Write, &f))) {
        if (R_FAILED(fsFsCreateFile(&fs, CONFIG_FILE, 0, 0))) return false;
        if (R_FAILED(fsFsOpenFile(&fs, CONFIG_FILE, FsOpenMode_Read | FsOpenMode_Write, &f))) return false;
    }
    tsl::hlp::ScopeGuard fGuard([&]{ fsFileClose(&f); });

    std::string out;
    for (const auto& l : lines) { out += l; out += "\n"; }
    Result rc = fsFileWrite(&f, 0, out.data(), out.size(), FsWriteOption_Flush);
    return R_SUCCEEDED(rc);
}

} // namespace janisaConfig
