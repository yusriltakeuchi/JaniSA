#pragma once
// JaniSA custom bundle persistence.
// Multiple user-defined bundles stored in an INI-style file on SD:
//   sdmc:/config/JaniSA/JaniSA.ini
// Format:
//   [Bundle "Name"]
//   CheatName1
//   CheatName2
//   ...
// Each section = one bundle. Built-in bundles are compile-time (see BUNDLES[]).
#include <tesla.hpp>   // brings in <switch.h> (Fs*, sdmc devices)
#include <cstring>
#include <cstdio>
#include <string>
#include <vector>

namespace janisaConfig {

static constexpr const char* CONFIG_DIR  = "/config/JaniSA";
static constexpr const char* CONFIG_FILE = "/config/JaniSA/JaniSA.ini";
static constexpr const int    MAX_CUSTOM_CHEATS = 16;
static constexpr const int    MAX_CUSTOM_BUNDLES = 8;

struct CustomBundleDef {
    std::string title;
    std::vector<std::string> cheats;
};

// Load all custom bundles. Returns empty on file error / no sections.
static inline std::vector<CustomBundleDef> loadBundles() {
    std::vector<CustomBundleDef> out;

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

    CustomBundleDef cur;
    bool inSection = false;
    size_t start = 0;
    while (start < data.size()) {
        size_t nl = data.find('\n', start);
        std::string line = data.substr(start, nl == std::string::npos ? std::string::npos : nl - start);
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (!line.empty()) {
            if (line.front() == '[' && line.back() == ']') {
                if (inSection && !cur.title.empty()) out.push_back(cur);
                cur = CustomBundleDef();
                cur.title = line.substr(1, line.size() - 2);   // "Bundle \"Name\"" kept verbatim
                inSection = true;
            } else if (inSection) {
                cur.cheats.push_back(line);
            }
        }
        if (nl == std::string::npos) break;
        start = nl + 1;
    }
    if (inSection && !cur.title.empty()) out.push_back(cur);

    // drop empty sections (title-only)
    for (size_t i = 0; i < out.size(); ) {
        if (out[i].cheats.empty()) out.erase(out.begin() + i);
        else i++;
    }
    return out;
}

// Save all custom bundles (replaces file contents).
static inline bool saveBundles(const std::vector<CustomBundleDef>& bundles) {
    FsFileSystem fs;
    if (R_FAILED(fsOpenSdCardFileSystem(&fs))) return false;
    tsl::hlp::ScopeGuard fsGuard([&]{ fsFsClose(&fs); });

    fsFsCreateDirectory(&fs, "/config");
    fsFsCreateDirectory(&fs, CONFIG_DIR);

    FsFile f;
    if (R_FAILED(fsFsOpenFile(&fs, CONFIG_FILE, FsOpenMode_Read | FsOpenMode_Write, &f))) {
        if (R_FAILED(fsFsCreateFile(&fs, CONFIG_FILE, 0, 0))) return false;
        if (R_FAILED(fsFsOpenFile(&fs, CONFIG_FILE, FsOpenMode_Read | FsOpenMode_Write, &f))) return false;
    }
    tsl::hlp::ScopeGuard fGuard([&]{ fsFileClose(&f); });

    std::string out;
    for (const auto& b : bundles) {
        out += "["; out += b.title; out += "]\n";
        for (const auto& c : b.cheats) { out += c; out += "\n"; }
    }
    Result rc = fsFileSetSize(&f, (s64)out.size());
    if (R_SUCCEEDED(rc) && !out.empty())
        rc = fsFileWrite(&f, 0, out.data(), out.size(), FsWriteOption_Flush);
    if (R_SUCCEEDED(rc))
        rc = fsFsCommit(&fs);
    return R_SUCCEEDED(rc);
}

} // namespace janisaConfig