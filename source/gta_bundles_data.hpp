#pragma once
// Bundle (multi-cheat) definitions — reference existing cheats by NAME.
// Names MUST exactly match entries in gta_cheats_data.hpp.
// findCheat() resolves at runtime; unknown names are skipped (never null-deref).
#include "gta_cheats_data.hpp"

struct BundleDef {
    const char* title;             // shown in bundle list
    const char* desc;              // short subtitle
    const char* const* cheatNames; // cheat names, resolved at runtime
    u32 count;
};

// Resolve a cheat name -> CheatEntry* (searches all categories).
static inline const CheatEntry* findCheat(const char* name) {
    for (u32 c = 0; c < CATEGORY_COUNT; c++)
        for (u32 i = 0; i < CATEGORIES[c].count; i++)
            if (strcmp(CATEGORIES[c].items[i].name, name) == 0)
                return &CATEGORIES[c].items[i];
    return nullptr;
}

static const char* const BUNDLE_A_NAMES[] = {
    "Health, Armor, and Money ($250,000)",
    "Max Muscle",
    "Max Fat",
};

static const char* const BUNDLE_B_NAMES[] = {
    "Weapons 1 (Bat, Pistol, Shotgun, Mini SMG, AK 47, Rocket Launcher, Molotov Cocktail, Spray Can, Brass Knuckles.)",
    "Weapons 2 (Knife, Pistol, Sawed-Off Shotgun, Tec 9, Sniper Rifle, Flamethrower, Grenades, Fire Extinguisher.)",
    "Weapons 3 (Chainsaw, Silenced Pistol, Combat Shotgun, M4, Bazooka, Plastic Explosive)",
    "Infinite Ammo",
};

static const char* const BUNDLE_C_NAMES[] = {
    "Wanted Level Down",
    "Lock Wanted Level",
};

static const char* const BUNDLE_D_NAMES[] = {
    "Spawn Rhino Tank",
    "Spawn Hydra",
    "Blow Up All Cars",
    "Aggressive Traffic",
};

static const BundleDef BUNDLES[] = {
    { "Money & Health",      "3 cheats",   BUNDLE_A_NAMES, sizeof(BUNDLE_A_NAMES)/sizeof(BUNDLE_A_NAMES[0]) },
    { "Weapons Pack",        "4 cheats",   BUNDLE_B_NAMES, sizeof(BUNDLE_B_NAMES)/sizeof(BUNDLE_B_NAMES[0]) },
    { "Wanted Control",      "2 cheats",   BUNDLE_C_NAMES, sizeof(BUNDLE_C_NAMES)/sizeof(BUNDLE_C_NAMES[0]) },
    { "Traffic Mayhem",      "4 cheats",   BUNDLE_D_NAMES, sizeof(BUNDLE_D_NAMES)/sizeof(BUNDLE_D_NAMES[0]) },
};
static const u32 BUNDLE_COUNT = sizeof(BUNDLES)/sizeof(BUNDLES[0]);