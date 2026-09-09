# Changelog

All notable changes to JaniSA are documented here.

Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/) and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [0.2.0] — 2026-09-09

### Added
- **Bundle E — Custom user-defined bundle** 🎛️
  - New **"Edit Custom Bundle"** screen in the Bundling menu — pick any subset of the 82 cheats (A toggles, up to 16).
  - Selection persists to `/config/JaniSA/JaniSA.ini` on the SD card (survives reboots).
  - A **Custom** entry appears in the Bundling list; confirm to inject all selected cheats in order.
- Config persistence layer (`gta_config.hpp`) — INI-style read/write on `sdmc:` (same pattern as libtesla's own config).

### Changed
- None

### Removed / Won't do
- **Haptic feedback** — removed. `HiddbgHdlsState` has no vibration field (only battery/flags/buttons/sticks/indicator); vibration needs a separate `hid` service handle owned by the game, not reachable from an applet overlay.
- **Additional cheat categories** — removed. All 82 GTA SA DE cheats are already present; nothing to add.

---

## [0.1.0] — 2026-09-09

### 🎉 Initial public release

#### Added
- **82 official GTA SA Definitive Edition cheats** (Switch version combos)
- **7 category system**: Player & Wanted, Combat & Peds, Vehicles, Traffic & Driving, Time & Physics, Weather, Other
- **Bundling** — run multiple cheats back-to-back with one Confirm:
  - Bundle A — Money & Health (3 cheats)
  - Bundle B — Weapons Pack (4 cheats)
  - Bundle C — Wanted Control (2 cheats)
  - Bundle D — Traffic Mayhem (4 cheats)
- **Automatic button combo injection** via `hid:dbg` service (frame-accurate, `svcSleepThread` real-time)
- **Background injection** — overlay hides before injection, cheats inject from a background thread, overlay closes after completion
- **Thread-safe injection engine**:
  - Mutex-protected cheat queue
  - Atomic busy flag (prevents double-tap)
  - Background thread join on exit (no dangling thread / use-after-free)
  - Close via UI thread (no renderer race)
- Semi-transparent dark overlay background (libtesla `ColorFrameBackground`)
- Customizable hotkey via Tesla-Menu settings

#### Technical
- Built on [libtesla](https://github.com/WerWolv/libtesla) (WerWolv) overlay framework
- NACP metadata: JaniSA, author Jani, version 0.1.0
- All cheat combos verified against [IGN Switch cheat list](https://www.ign.com/wikis/gta-san-andreas-definitive-edition/All_Switch_Cheats_and_Codes)
- `HidNpadButton` bitmask verified safe within firmware mask `0xfffffffff00fffff`
- Build output: `build/JaniSA.ovl` (~320 KB)
- MIT License

#### Known limitations
- Requires Atmosphere CFW + Tesla-Menu installed
- `hid:dbg` may be blocked in some applet overlay setups → requires sysmodule fallback
- No online multiplayer (GTA SA DE single-player only)
- Combos are the Switch button layout only (not PC/mobile)

---

## [Unreleased]

### Planned (roadmap)
- **Sysmodule fallback** for applet-blocked `hid:dbg` — out-of-scope for this overlay; will be a separate companion project.
