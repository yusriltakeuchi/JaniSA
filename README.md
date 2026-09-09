# 🚗 JaniSA — GTA San Andreas Cheat Overlay for Nintendo Switch

**JaniSA** is a Tesla overlay for **GTA San Andreas – Definitive Edition** on Nintendo Switch (Atmosphere CFW). Open the overlay mid-game, pick a cheat from one of 7 categories (or a multi-cheat bundle), and the button combo is injected **automatically** — no manual button mashing.

![Author](https://img.shields.io/badge/author-Jani-0088cc)
![Version](https://img.shields.io/badge/version-0.1.0-88c100)
![License](https://img.shields.io/badge/license-MIT-blue)
![Cheats](https://img.shields.io/badge/cheats-82-orange)

---

## Table of Contents

- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Controls](#controls)
- [Bundling (Multi-Cheat)](#bundling-multi-cheat)
- [Cheat List](#cheat-list)
- [Building from Source](#building-from-source)
- [Adding Cheats / Bundles](#adding-cheats--bundles)
- [Troubleshooting](#troubleshooting)
- [FAQ](#faq)
- [License](#license)

---

## Features

- 🎮 **82 official GTA SA DE cheats** (from the IGN Switch list)
- 🗂️ **7 categories** for quick lookup: **Player & Wanted**, **Combat & Peds**, **Vehicles**, **Traffic & Driving**, **Time & Physics**, **Weather**, **Other**
- 📦 **Bundling** — run several cheats back-to-back in one confirmation (e.g. money + armor + weapons)
- ⚡ **Automatic combo injection** — no need to press anything manually
- 🖥️ Semi-transparent dark background
- 📦 Small: `.ovl` is only **~320 KB**

---

## Installation

### Prerequisites

Your Switch must run custom firmware **Atmosphere** with **Tesla** installed:

| Component | Link | Notes |
|-----------|------|-------|
| **Atmosphere** | [Atmosphere-NX/Atmosphere](https://github.com/Atmosphere-NX/Atmosphere) | 1.7.0+ |
| **Tesla-Menu** | [WerWolv/Tesla-Menu](https://github.com/WerWolv/Tesla-Menu) | Overlay loader |
| **nx-ovlloader** | [Atmosphere-NX/nx-ovlloader](https://github.com/Atmosphere-NX/nx-ovlloader) | Loads overlays at boot |

If Tesla isn't installed yet, install it via [The Ultimate AIO Switch Updater](https://github.com/HamletDuFromage/aio-switch-updater) or a manual setup.

### Install the overlay

1. Put your **SD card** into your computer.
2. Copy **`gta-cheats.ovl`** to:
   ```
   /atmosphere/overlays/gta-cheats.ovl
   ```
   > Full SD path: `sdmc:/atmosphere/overlays/gta-cheats.ovl` (the `overlays` folder usually already exists — create it if not).
3. Eject the SD card safely and put it back into the Switch.

### Enable and use

1. Power on the Switch into Atmosphere.
2. Start **GTA San Andreas – Definitive Edition**.
3. Press and hold the Tesla open combo:
   - **`L` + `DPAD-DOWN`** (default) — or the combo you configured in Tesla.
4. The Tesla menu opens → select **GTA Cheats**.
5. Pick a **category** (e.g. *Weapons* or *Player & Wanted*).
6. Pick a **cheat** → the button combo is injected automatically, cheat activates.
7. **Bundling** (optional): pick *Bundling* from the main menu → pick a bundle → review the cheat list → press **Confirm ▶** → all cheats in the bundle are injected in sequence.
8. Press **`B`** to close the menu (or trigger the Tesla combo again).

> **Important:** Injection uses the `hid:dbg` service. On some Atmosphere setups the applet overlay cannot access this service. If cheats don't inject (game doesn't respond), you need the fallback sysmodule — see [Troubleshooting](#troubleshooting).

---

## Controls

| Button | Action |
|--------|--------|
| `D-pad ↑/↓` | Move selection |
| `A` | Select cheat / category / Confirm |
| `B` | Go back to previous screen |
| `L + DPAD-DOWN` | Open Tesla menu (default) |
| `L + DPAD-DOWN` | Close menu while in overlay |

> This overlay does **not** change your Tesla combo — open/close is always controlled by Tesla-Menu (default `L+DDOWN`, changeable in Tesla settings).

---

## Bundling (Multi-Cheat)

Bundling runs **several cheats in sequence from a single confirmation** — no need to select cheats one by one.

### How to use

1. Open the overlay → select **Bundling** (top item in the main menu).
2. Pick a bundle:
   - **A — Money & Health**
   - **B — Weapons Pack**
   - **C — Wanted Control**
   - **D — Traffic Mayhem**
3. A read-only list of the cheats that will be run is shown (preview).
4. Select **Confirm ▶** → all cheats in the bundle are injected back-to-back, automatically.

### Bundle contents

| Bundle | Cheats |
|--------|--------|
| **A — Money & Health** | `Health, Armor, and Money ($250,000)`, `Max Muscle`, `Max Fat` |
| **B — Weapons Pack** | `Weapons 1`, `Weapons 2`, `Weapons 3`, `Infinite Ammo` |
| **C — Wanted Control** | `Wanted Level Down`, `Lock Wanted Level` |
| **D — Traffic Mayhem** | `Spawn Rhino Tank`, `Spawn Hydra`, `Blow Up All Cars`, `Aggressive Traffic` |

> Bundles run **sequentially** in the order above (not in parallel). The injector waits for each cheat to finish (combo complete) before starting the next one.

---

## Cheat List

82 cheats across 7 categories. Combos below are exactly as defined in `source/gta_cheats_data.hpp`.

### Player & Wanted (12)

| # | Cheat | Combo |
|---|-------|-------|
| 1 | Health, Armor, and Money ($250,000) | `R ZR L B ← ↓ → ↑ ← ↓ → ↑` |
| 2 | Wanted Level Down | `R R A ZR ↑ ↓ ↑ ↓ ↑ ↓` |
| 3 | Wanted Level Up | `R R A ZR ← → ← → ← →` |
| 4 | Lock Wanted Level | `A → A → ← Y X ↑` |
| 5 | Maximum Wanted Level | `A → A → ← Y B ↓` |
| 6 | Max Muscle | `X ↑ ↑ ← → Y A ←` |
| 7 | Max Fat | `X ↑ ↑ ← → Y A ↓` |
| 8 | Minimum Muscle and Fat | `X ↑ ↑ ← → Y A →` |
| 9 | Infinite Lung Capacity | `↓ ← L ↓ ↓ ZR ↓ ZL ↓` |
| 10 | Maximum Respect | `L R X ↓ ZR B L ↑ ZL ZL L L` |
| 11 | Never Get Hungry | `Y ZL R B X ↑ Y ZL ↑ B` |
| 12 | Maximum Sex Appeal | `A X X ↑ A R ZL ↑ X L L L` |

### Combat & Peds (15)

| # | Cheat | Combo |
|---|-------|-------|
| 1 | Infinite Ammo | `L R Y R ← ZR R ← Y ↓ L L` |
| 2 | Weapons 1 | `R ZR L ZR ← ↓ → ↑ ← ↓ → ↑` |
| 3 | Weapons 2 | `R ZR L ZR ← ↓ → ↑ ← ↓ ↓ ←` |
| 4 | Weapons 3 | `R ZR L ZR ← ↓ → ↑ ← ↓ ↓ ↓` |
| 5 | Pedestrian Riot | `↓ ← ↑ ← B ZR R ZL L` |
| 6 | Chaos Mode | `ZL → L X → → R L → L L L` |
| 7 | Bounty on Your Head | `↓ ↑ ↑ ↑ B ZR R ZL ZL` |
| 8 | Pedestrians Attack (With Guns) | `B L ↑ Y ↓ B ZL X ↓ R L L` |
| 9 | Pedestrians Have Weapons | `ZR R B X B X ↑ ↓` |
| 10 | All Pedestrians Are Elvis | `L A X L L Y ZL ↑ ↓ ←` |
| 11 | Hitman Level For All Weapons | `↓ Y B ← R ZR ← ↓ ↓ L L L` |
| 12 | Gang Control | `ZL ↑ R R ← R R ZR → ↓` |
| 13 | Recruit Anyone (Pistol) | `↓ Y ↑ ZR ZR ↑ → → ↑` |
| 14 | Gang Members Mode | `← → → → ← B ↓ ↑ Y → ↓` |
| 15 | Recruit Anyone (Rocket Launcher) | `ZR ZR ZR B ZL L R ZL ↓ B` |

> Full weapon names:
> - **Weapons 1**: Bat, Pistol, Shotgun, Mini SMG, AK 47, Rocket Launcher, Molotov Cocktail, Spray Can, Brass Knuckles.
> - **Weapons 2**: Knife, Pistol, Sawed-Off Shotgun, Tec 9, Sniper Rifle, Flamethrower, Grenades, Fire Extinguisher.
> - **Weapons 3**: Chainsaw, Silenced Pistol, Combat Shotgun, M4, Bazooka, Plastic Explosive

### Vehicles (19)

| # | Cheat | Combo |
|---|-------|-------|
| 1 | Spawn Rhino Tank | `A A L A A A L ZL R X A X` |
| 2 | Spawn Jetpack | `L ZL R ZR ↑ ↓ ← → L ZL R ZR ↑ ↓ ← →` |
| 3 | Spawn Hydra | `X X Y A B L L ↓ ↑` |
| 4 | Spawn Hunter | `A B L A A L A R ZR ZL L L` |
| 5 | Spawn Stunt Plane | `A ↑ L ZL ↓ R L L ← ← B X` |
| 6 | Spawn Bloodring Banger | `↓ R A ZL ZL B R L ← ←` |
| 7 | Spawn Vortex Hovercraft | `X X Y A B L ZL ↓ ↓` |
| 8 | Spawn Parachute | `← → L ZL R ZR ZR ↑ ↓ → L` |
| 9 | Spawn Monster | `→ ↑ R R R ↓ X X B A L L` |
| 10 | Spawn Hotring Racer #1 | `R A ZR → L ZL B B Y R` |
| 11 | Spawn Hotring Racer #2 | `ZR L A → L R → ↑ A ZR` |
| 12 | Spawn Romero | `↓ ZR ↓ R ZL ← R L ← →` |
| 13 | Spawn Stretch | `ZR ↑ ZL ← ← R L A →` |
| 14 | Spawn Trashmaster | `A R A R ← ← R L A →` |
| 15 | Spawn Caddy | `A L ↑ R ZL B R L A B` |
| 16 | Spawn Quad | `← ← ↓ ↓ ↑ ↑ Y A X R ZR` |
| 17 | Spawn Dozer | `ZR L L → → ↑ ↑ B L ←` |
| 18 | Spawn Tanker Truck | `R ↑ ← → ZR ↑ → Y → ZL L L` |
| 19 | Spawn Rancher | `↑ → → L → ↑ B ZL` |

### Traffic & Driving (16)

| # | Cheat | Combo |
|---|-------|-------|
| 1 | Blow Up All Cars | `ZR ZL R L ZL ZR Y X A X ZL L` |
| 2 | Traffic Lights Stay Green | `→ R ↑ ZL ZL ← R L R R` |
| 3 | Aggressive Traffic | `ZR A R ZL ← R L ZR ZL` |
| 4 | Increase Car Speed | `→ R ↑ ZL ZL ← R L R R` |
| 5 | All Cars Have Nitrous | `← X R L ↑ Y X ↓ A ZL L L` |
| 6 | Flying Boats | `ZR A ↑ L → R → ↑ Y X` |
| 7 | Drive on Water | `→ ZR A R ZL Y R ZR` |
| 8 | Perfect Vehicle Handling | `X R R ← R L ZR L` |
| 9 | Reduced Traffic | `B ↓ ↑ ZR ↓ X L X ←` |
| 10 | Pink Cars | `A L ↓ ZL ← B R L → A` |
| 11 | Black Cars | `A ZL ↑ R ← B R L ← A` |
| 12 | Sports Cars | `↑ L R ↑ → ↑ B ZL B L` |
| 13 | Junk Cars | `ZL → L ↑ B L ZL ZR R L L L` |
| 14 | Flying Cars | `Y ↓ ZL ↑ L A ↑ B ←` |
| 15 | Invisible Cars | `X L X ZR Y L L` |
| 16 | Moon Car Gravity | `Y ZR ↓ ↓ ← ↓ ← ← ZL B` |

### Time & Physics (9)

| # | Cheat | Combo |
|---|-------|-------|
| 1 | Mega Jump | `↑ ↑ X X ↑ ↑ ← → Y ZR ZR` |
| 2 | Always 21:00 | `← ← ZL R → Y Y L ZL B` |
| 3 | Slow Motion | `X ↑ → ↓ Y ZR R` |
| 4 | Fast Motion | `X ↑ → ↓ ZL L Y` |
| 5 | Faster Clock | `A A L Y L Y Y Y L X A X` |
| 6 | Super Bunny Hop | `X Y A A Y A A L ZL ZL R ZR` |
| 7 | Super Punch | `↑ ← B X R Y Y Y ZL` |
| 8 | Always Midnight | `Y L R → B ↑ L ← ←` |
| 9 | Free Aim While Driving | `↑ ↑ Y ZL → B R ↓ ZR A` |

### Weather (6)

| # | Cheat | Combo |
|---|-------|-------|
| 1 | Cloudy Weather | `ZL ↓ ↓ ← Y ← ZR Y B R L L` |
| 2 | Foggy Weather | `ZR B L L ZL ZL ZL B` |
| 3 | Stormy Weather | `ZR B L L ZL ZL ZL A` |
| 4 | Sunny Weather | `ZR B L L ZL ZL ZL Y` |
| 5 | Very Sunny Weather | `ZR B L L ZL ZL ZL ↓` |
| 6 | Sandstorm | `↑ ↓ L L ZL ZL L ZL R ZR` |

### Other (5)

| # | Cheat | Combo |
|---|-------|-------|
| 1 | Beach Party Mode | `↑ ↑ ↓ ↓ Y A L R X ↓` |
| 2 | Funhouse Theme | `X X L Y Y A Y ↓ A` |
| 3 | Triad Theme | `B B ↓ ZR ZL A R A Y` |
| 4 | Rural Theme | `L L R R ZL L ZR ↓ ← ↑` |
| 5 | Kinky Theme | `Y → Y Y ZL B X B X` |

---

## Building from Source

### Prerequisites

- **devkitPro** (devkitA64 + libnx 4.x) — install guide: https://devkitpro.org/wiki/Getting_Started
- libtesla is vendored in `libs/` — the repo is self-contained, no submodule update needed.

### Build

```bash
export DEVKITPRO=/opt/devkitpro   # or wherever devkitPro lives
export PATH=$DEVKITPRO/tools/bin:$PATH

cd GTA-Switch-Cheat-Overlay
make clean && make
```

The output `gta-cheats.ovl` lands in **`build/`**.

> If `make` errors with `nacptool: command not found`, make sure `$DEVKITPRO/tools/bin` is on your PATH (tools come from the `switch-tools` package).

### Project structure

```
GTA-Switch-Cheat-Overlay/
├── source/
│   ├── main.cpp              — overlay UI + injector (hid:dbg)
│   ├── gta_cheats_data.hpp   — 82 cheats & combos (auto-generated, do not edit by hand)
│   └── gta_bundles_data.hpp  — multi-cheat bundle definitions
├── libs/libtesla/            — overlay framework (vendored in-repo)
├── Makefile
├── LICENSE
└── README.md
```

> `build/` is generated — the deployable `.ovl` lives there after `make`.

---

## Adding Cheats / Bundles

### Add a new cheat

Add it to **`source/gta_cheats_data.hpp`**:

```cpp
// 1. define the combo (new, at the top of the file):
static const u64 CB_Player__Wanted_99[] = { HidNpadButton_A, HidNpadButton_B, HidNpadButton_X, 0 };

// 2. register it in the category:
static const CheatEntry CHEATS_Player__Wanted[] = {
    { "Cheat Name", CB_Player__Wanted_99, 3 },
    // ... rest
};
```

### Add a new bundle

Edit **`source/gta_bundles_data.hpp`**:

```cpp
// 1. create a cheat-name array (must exactly match names in gta_cheats_data.hpp):
static const char* const BUNDLE_E_NAMES[] = {
    "Health, Armor, and Money ($250,000)",
    "Spawn Hydra",
};

// 2. register it in BUNDLES[]:
static const BundleDef BUNDLES[] = {
    { "Bundling E", "Title Desc", BUNDLE_E_NAMES, sizeof(BUNDLE_E_NAMES)/sizeof(BUNDLE_E_NAMES[0]) },
    // ...
};
```

> If a bundle cheat name is mistyped, the bundle still runs — the missing cheat is skipped automatically (no crash).

---

## Troubleshooting

### Overlay doesn't appear when pressing the combo

- Make sure Tesla is installed and the overlay is registered in the Tesla menu.
- Make sure the `.ovl` file is in `/atmosphere/overlays/`.
- Make sure your Tesla combo doesn't conflict with another combo (change it in Tesla settings).
- Try another combo (e.g. the default `L+DPAD-DOWN`, or re-set in Tesla).

### Overlay appears but cheats don't inject

- This is most likely **`hid:dbg` blocked** in the applet context. Solution: use a **sysmodule companion** (hid-mitm / virtual pad) as a replacement.
- Also note: some GTA SA DE cheats only need the button press **once** — if the combo is injected before the game is "ready", re-inject. Wait a moment, select the same cheat again.

### Bundling: the 2nd cheat is swallowed by the game

- Sequential combos may need a delay between cheats (~0.2–1 s). If the next cheat doesn't activate, increase `BETWEEN_CHEATS_NS` in `source/main.cpp` (Injector) — the inter-cheat delay constant. Rebuild. Default is 400 ms.

### Game freezes / stutters when selecting a cheat

- Reduce `HOLD_NS` in `source/main.cpp` (default 60 ms) — smaller = faster combo playback. Rebuild.

### Want to change the look

- Semi-transparent black background = libtesla's default `ColorFrameBackground`. Change the alpha in `tsl::style` if you want it more transparent / darker.

---

## FAQ

**Q: Will this get my Switch banned?**
A: Homebrew / CFW always carries some risk, especially if you go online. Use at your own risk, like any CFW homebrew. This overlay itself only reads your controller input — it does not touch system NAND or online services.

**Q: Is this a mod / does it include game assets?**
A: No. It's an input tool — it presses buttons for you. No game assets, no file replacement, no memory patches.

**Q: Does it work on emulators (Yuzu / Ryujinx)?**
A: Not directly. It relies on Tesla + `hid:dbg`, which are Switch hardware/Atmosphere features. On emulators you're better off just typing cheats manually or using emulator-native cheat systems.

**Q: Does it work in GTA SA for other platforms (PC / mobile)?**
A: No — the combos in the data are the **Switch** button layout of GTA SA Definitive Edition.

**Q: Can I use this while playing online?**
A: GTA SA DE has no multiplayer. Single-player only — no online risk from the cheats themselves.

---

## License

MIT. Inspired by Tesla-Template (WerWolv) and libtesla (WerWolv).

GTA San Andreas © Rockstar Games. This overlay is an input tool — it does not contain or extract any game content or assets.