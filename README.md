# GTA Switch Cheat Overlay

Tesla overlay untuk **GTA San Andreas – Definitive Edition** di Nintendo Switch (Atmosphere CFW).
Buka overlay pas lagi main → pilih cheat dari 7 kategori → kombo tombol ke-inject otomatis.

> Dibangun dengan libtesla + libnx (devkitPro). Dites build di devkitA64 gcc 16.1.0.

---

## Daftar Isi

- [Fitur](#fitur)
- [Cara Aktifin di Switch](#cara-aktifin-di-switch)
- [Tombol Navigasi](#tombol-navigasi)
- [Bundling (Multi-Cheat)](#bundling-multi-cheat)
- [Daftar Cheat Lengkap](#daftar-cheat-lengkap)
- [Cara Build dari Source](#cara-build-dari-source)
- [Cara Tambah Cheat / Bundle Baru](#cara-tambah-cheat--bundle-baru)
- [Troubleshooting](#troubleshooting)
- [Lisensi](#lisensi)

---

## Fitur

- **82 cheat** resmi GTA SA DE (data dari IGN wiki, versi tombol Switch)
- 7 kategori biar gampang dicari: **Player & Wanted**, **Combat & Peds**, **Vehicles**, **Traffic & Driving**, **Time & Physics**, **Weather**, **Other**
- **Bundling** — jalanin beberapa cheat sekaligus berurutan (misal money + armor + weapon)
- Kombo tombol ke-inject **otomatis** (frame-accurate) — gak perlu pencet manual
- Background hitam semi-transparan
- Kecil: `.ovl` cuma **~310 KB**

---

## Cara Aktifin di Switch

### 1. Prasyarat

Switch lo harus udah pakai CFW **Atmosphere** + **Tesla**:

| Komponen | Link | Catatan |
|----------|------|---------|
| **Atmosphere** | [Atmosphere-NX/Atmosphere](https://github.com/Atmosphere-NX/Atmosphere) | 1.7.0+ |
| **Tesla-Menu** | [WerWolv/Tesla-Menu](https://github.com/WerWolv/Tesla-Menu) | Overlay loader |
| **nx-ovlloader** | [Atmosphere-NX/nx-ovlloader](https://github.com/Atmosphere-NX/nx-ovlloader) | Loader overlay di Tesla |

Kalau Tesla belom terpasang: install dulu lewat
[The Ultimate AIO Switch Updater](https://github.com/HamletDuFromage/aio-switch-updater)
atau setup manual.

### 2. Pasang overlay

1. Colok **SD card** ke komputer
2. Copy file **`gta-cheats.ovl`** ke:
   ```
   /atmosphere/overlays/gta-cheats.ovl
   ```
   > Path lengkap di SD: `sdmc:/atmosphere/overlays/gta-cheats.ovl`
   > (folder `overlays` biasanya udah ada — kalau belum, bikin sendiri)
3. Eject SD dengan aman → masukin balik ke Switch

### 3. Aktifkan dan pakai

1. Nyalakan Switch (masuk ke Atmosphere)
2. Buka game **GTA San Andreas – Definitive Edition**
3. Tekan dan tahan kombo buka Tesla:
   - **`L` + `DPAD-DOWN`** (default Tesla) — atau kombo yang lo set di Tesla
4. Menu Tesla muncul → pilih **GTA Cheats**
5. Pilih **kategori** (misal *Weapons* atau *Player & Wanted*)
6. Pilih **cheat** → kombo tombol ke-inject otomatis, cheat langsung aktif
7. **Bundling** (opsional): pilih *Bundling* di menu utama → pilih bundle → liat list cheat-nya → **Confirm ▶** → semua cheat di bundle ke-inject berurutan
8. Tekan **`B`** buat tutup menu (atau kombo Tesla lagi)

> **Catatan penting:** Injection pakai service `hid:dbg`. Pada beberapa setup Atmosphere
> applet overlay gak dapat akses service ini. Kalau cheat gak ke-inject (game gak ngerespon),
> itu tandanya butuh fallback sysmodule — lihat [Troubleshooting](#troubleshooting).

---

## Tombol Navigasi

| Tombol      | Aksi                               |
|-------------|------------------------------------|
| `D-pad ↑/↓` | Pindah pilihan                     |
| `A`         | Pilih cheat / kategori / Confirm   |
| `B`         | Balik ke layar sebelumnya          |
| `L + DPAD-DOWN` | Buka menu Tesla (default)      |
| `L + DPAD-DOWN` | Tutup menu pas lagi di overlay |

> Overlay ini gak ganti kombo Tesla — kombo buka/tutup tetap dikontrol Tesla-Menu
> (default `L+DDOWN`, bisa diubah di pengaturan Tesla).

---

## Bundling (Multi-Cheat)

Bundling jalanin **beberapa cheat sekaligus berurutan** dari satu konfirmasi —
gak perlu pilih cheat satu-satu.

### Cara pakai

1. Buka overlay → pilih **Bundling** (item paling atas di menu utama)
2. Pilih salah satu bundle:
   - **Bundling A** — Money & Health
   - **Bundling B** — Weapons Pack
   - **Bundling C** — Wanted Control
   - **Bundling D** — Traffic Mayhem
3. Muncul list cheat yang bakal dijalanin (read-only, buat preview aja)
4. Pilih **Confirm ▶** → semua cheat di bundle ke-inject berurutan, otomatis

### Isi tiap bundle

| Bundle | Cheat yang dijalanin |
|--------|----------------------|
| **A — Money & Health** | `Health, Armor, and Money ($250,000)`, `Max Muscle`, `Max Fat` |
| **B — Weapons Pack** | `Weapons 1`, `Weapons 2`, `Weapons 3`, `Infinite Ammo` |
| **C — Wanted Control** | `Wanted Level Down`, `Lock Wanted Level` |
| **D — Traffic Mayhem** | `Spawn Rhino Tank`, `Spawn Hydra`, `Blow Up All Cars`, `Aggressive Traffic` |

> Bundle jalan berurutan sesuai urutan di atas (bukan paralel). Injector nunggu
> tiap cheat kelar (kombo selesai) baru lanjut ke cheat berikutnya.

---

## Daftar Cheat Lengkap

82 cheat dalam 7 kategori. Kombo di bawah persis dari `source/gta_cheats_data.hpp`.

### Player & Wanted (12)

| # | Cheat | Kombo |
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

| # | Cheat | Kombo |
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

> Nama lengkap Weapons 1/2/3:
> - **Weapons 1**: Bat, Pistol, Shotgun, Mini SMG, AK 47, Rocket Launcher, Molotov Cocktail, Spray Can, Brass Knuckles.
> - **Weapons 2**: Knife, Pistol, Sawed-Off Shotgun, Tec 9, Sniper Rifle, Flamethrower, Grenades, Fire Extinguisher.
> - **Weapons 3**: Chainsaw, Silenced Pistol, Combat Shotgun, M4, Bazooka, Plastic Explosive

### Vehicles (19)

| # | Cheat | Kombo |
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

| # | Cheat | Kombo |
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

| # | Cheat | Kombo |
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

| # | Cheat | Kombo |
|---|-------|-------|
| 1 | Cloudy Weather | `ZL ↓ ↓ ← Y ← ZR Y B R L L` |
| 2 | Foggy Weather | `ZR B L L ZL ZL ZL B` |
| 3 | Stormy Weather | `ZR B L L ZL ZL ZL A` |
| 4 | Sunny Weather | `ZR B L L ZL ZL ZL Y` |
| 5 | Very Sunny Weather | `ZR B L L ZL ZL ZL ↓` |
| 6 | Sandstorm | `↑ ↓ L L ZL ZL L ZL R ZR` |

### Other (5)

| # | Cheat | Kombo |
|---|-------|-------|
| 1 | Beach Party Mode | `↑ ↑ ↓ ↓ Y A L R X ↓` |
| 2 | Funhouse Theme | `X X L Y Y A Y ↓ A` |
| 3 | Triad Theme | `B B ↓ ZR ZL A R A Y` |
| 4 | Rural Theme | `L L R R ZL L ZR ↓ ← ↑` |
| 5 | Kinky Theme | `Y → Y Y ZL B X B X` |

---

## Cara Build dari Source

### Prasyarat

- **devkitPro** (devkitA64 + libnx 4.x) — cara install: https://devkitpro.org/wiki/Getting_Started
- libtesla (udah ke-track di `libs/` — repo ini self-contained, gak butuh submodule update)

### Build

```bash
export DEVKITPRO=/opt/devkitpro   # atau lokasi devkitPro lo
export PATH=$DEVKITPRO/tools/bin:$PATH

cd GTA-Switch-Cheat-Overlay
make clean && make
```

Hasil: `gta-cheats.ovl` di root project.

> Kalau `make` error `nacptool: command not found`, pastikan `$DEVKITPRO/tools/bin`
> ada di PATH (tools dari package `switch-tools`).

### Struktur project

```
GTA-Switch-Cheat-Overlay/
├── source/
│   ├── main.cpp              — overlay UI + injector (hid:dbg)
│   ├── gta_cheats_data.hpp   — 82 cheat & kombo (auto-generated, jangan diedit manual)
│   └── gta_bundles_data.hpp  — definisi bundle multi-cheat
├── libs/libtesla/            — framework overlay (ke-track langsung di repo)
├── Makefile
├── gta-cheats.ovl            — hasil build (deployable)
└── README.md
```

---

## Cara Tambah Cheat / Bundle Baru

### Tambah cheat baru

Cheat harus ditambah ke **`source/gta_cheats_data.hpp`**:

```cpp
// 1. definisikan kombo (baru, di bagian atas file):
static const u64 CB_Player__Wanted_99[] = { HidNpadButton_A, HidNpadButton_B, HidNpadButton_X, 0 };

// 2. daftarkan di kategori:
static const CheatEntry CHEATS_Player__Wanted[] = {
    { "Nama Cheat", CB_Player__Wanted_99, 3 },
    // ... sisanya
};
```

### Tambah bundle baru

Edit **`source/gta_bundles_data.hpp`**:

```cpp
// 1. bikin array nama cheat (harus persis nama di gta_cheats_data.hpp):
static const char* const BUNDLE_E_NAMES[] = {
    "Health, Armor, and Money ($250,000)",
    "Spawn Hydra",
};

// 2. daftarkan di BUNDLES[]:
static const BundleDef BUNDLES[] = {
    { "Bundling E", "Title Desc", BUNDLE_E_NAMES, sizeof(BUNDLE_E_NAMES)/sizeof(BUNDLE_E_NAMES[0]) },
    // ...
};
```

> Kalau nama cheat salah ketik, bundle tetap jalan — cheat yang gak ketemu
> bakal di-skip otomatis (gak nge-crash).

---

## Troubleshooting

### Overlay kagak muncul pas tombol kombo ditekan

- Pastikan Tesla terinstall & overlay terdaftar di menu Tesla
- Pastikan file `.ovl` ada di `/atmosphere/overlays/`
- Pastikan kombo Tesla gak bentrok sama kombo lain (ubah di pengaturan Tesla)
- Coba kombo lain (misal `L+DPAD-DOWN` default, atau set ulang di Tesla)

### Overlay muncul tapi cheat gak ke-inject

- Ini kemungkinan besar **`hid:dbg` di-block** di konteks applet. Solusi: pakai
  **sysmodule companion** (hid-mitm / virtual pad) sebagai pengganti.
- Cek juga: beberapa cheat GTA SA DE cuma butuh tombol **satu kali** — kalau kombo
  ke-inject sebelum game "siap nerima", inject ulang bisa nepak. Tunggu sebentar,
  pilih cheat yang sama lagi.

### Bundling: cheat ke-2 ke-swallow game

- Kombo berurutan mungkin butuh jeda antar cheat (~0.2–1 s). Kalau cheat berikutnya
  gak aktif, tambah `BETWEEN_FRAMES` di `source/main.cpp` (Injector) — konstanta
  jeda di antara dua cheat. Rebuild. Default 0 (langsung lanjut); mulai dari 30
  (~0.5 s @60fps) dan tuning.

### Game nge-freeze / stutter pas pilih cheat

- Kurangi `HOLD_FRAMES` di `source/main.cpp` (default 2) — makin kecil makin cepat
  kombo jalan. Rebuild.

### Mau ganti warna / tampilan

- Background hitam semi-transparan = `ColorFrameBackground` bawaan libtesla.
  Ubah alpha di `tsl::style` kalau mau lebih transparan/gelap.

---

## Lisensi

MIT. Terinspirasi dari Tesla-Template (WerWolv) & libtesla (WerWolv).

GTA San Andreas © Rockstar Games. Overlay ini cuma tools input — bukan contain
konten game, gak ada aset game di dalamnya.