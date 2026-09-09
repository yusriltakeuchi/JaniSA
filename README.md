# GTA Switch Cheat Overlay

Tesla overlay untuk **GTA San Andreas – Definitive Edition** di Nintendo Switch (Atmosphere CFW).
Buka overlay pas lagi main → pilih cheat dari 7 kategori → kombo tombol ke-inject otomatis.

> Dibangun dengan libtesla + libnx (devkitPro). Dites build di devkitA64 gcc 16.1.0.

---

## Fitur

- **82 cheat** resmi GTA SA DE (data dari IGN wiki, tombol versi Switch)
- 7 kategori biar gampang dicari: Player & Wanted, Combat & Peds, Vehicles, Traffic & Driving, Time & Physics, Weather, Other
- **Bundling** — jalanin beberapa cheat sekaligus berurutan (misal money + armor + weapon)
- Kombo tombol ke-inject **otomatis** (frame-accurate) — gak perlu pencet manual
- Background hitam semi-transparan; navigasi D-pad, A pilih, B balik
- Kecil: `.ovl` cuma **~300 KB**

---

## Cara Aktifin di Switch

### 1. Prasyarat

Switch lo harus udah pakai CFW **Atmosphere** + **Tesla**:
- [Atmosphere](https://github.com/Atmosphere-NX/Atmosphere) (1.7.0+)
- [Tesla-Menu](https://github.com/WerWolv/Tesla-Menu) — overlay loader
- [nx-ovlloader](https://github.com/Atmosphere-NX/nx-ovlloader) — dipasang lewat Tesla

Kalau Tesla belom terpasang: install dulu lewat [The Ultimate AIO Switch Updater](https://github.com/HamletDuFromage/aio-switch-updater) atau setup manual.

### 2. Pasang overlay

1. Colok SD ke komputer
2. Copy file **`gta-cheats.ovl`** ke:
   ```
   /atmosphere/overlays/gta-cheats.ovl
   ```
3. Eject SD dengan aman → masukin balik ke Switch

### 3. Aktifkan dan pakai

1. Nyalakan Switch (masuk Atmosphere)
2. Buka **game GTA SA – Definitive Edition**
3. Tekan dan tahan kombo buka Tesla:
   - **`L` + `DPAD-DOWN`** (default Tesla) — atau kombo yang lo set di Tesla
4. Menu Tesla muncul → pilih **GTA Cheats** overlay
5. Pilih **kategori** (misal *Weapons* atau *Player & Wanted*)
6. Pilih **cheat** → kombo tombol ke-inject otomatis, cheat langsung aktif
7. **Bundling** (opsional): pilih *Bundling* di menu utama → pilih bundle → liat list cheat-nya → **Confirm ▶** → semua cheat di bundle ke-inject berurutan
8. Tekan **`B`** buat tutup menu (atau kombo Tesla lagi)

> **Catatan penting:** Injection pakai service `hid:dbg`. Pada beberapa setup Atmosphere
> applet overlay gak dapat akses service ini. Kalau cheat gak ke-inject (game gak ngerespon),
> itu tandanya butuh fallback sysmodule — lihat [Troubleshooting](#troubleshooting).

### Tombol navigasi

| Tombol      | Aksi                    |
|-------------|-------------------------|
| `D-pad ↑/↓` | Pindah pilihan          |
| `A`         | Pilih cheat / kategori  |
| `B`         | Balik ke kategori / tutup |
| `L+DPAD-DOWN` | Buka menu Tesla (default) |

---

## Cara Build dari Source

### Prasyarat

- **devkitPro** (devkitA64 + libnx 4.x) — cara install: https://devkitpro.org/wiki/Getting_Started
- libtesla (submodule udah ada di `libs/`)

### Build

```bash
export DEVKITPRO=/opt/devkitpro   # atau lokasi devkitPro lo
export PATH=$DEVKITPRO/tools/bin:$PATH

cd GTA-Switch-Cheat-Overlay
git submodule update --init --recursive   # kalau libs/libtesla kosong
make
```

Hasil: `gta-cheats.ovl` di root project.

### Struktur

```
source/
  main.cpp              — overlay UI + injector (hid:dbg)
  gta_cheats_data.hpp   — 82 cheat & kombo (auto-generated, jangan diedit manual)
  gta_bundles_data.hpp  — definisi bundle multi-cheat
libs/libtesla/          — framework overlay (submodule)
Makefile
```

---

## Cheat List

82 cheat dalam 7 kategori. Nama & kombo lengkap ada di:
- `source/gta_cheats_data.hpp`
- [IGN Wiki — Nintendo Switch Cheats and Secrets](https://www.ign.com/wikis/grand-theft-auto-san-andreas/Nintendo_Switch_Cheats_and_Secrets)

Contoh beberapa (kombo dari data asli):

| Kategori | Cheat | Kombo (Switch) |
|----------|-------|----------------|
| Player & Wanted | Health, Armor, and Money ($250,000) | `R ZR L B ← ↓ → ↑ ← ↓ → ↑` |
| Player & Wanted | Wanted Level Up | `R R A ZR ← → ← → ← →` |
| Player & Wanted | Max Muscle | *(lihat data)* |
| Weapons | Slot 1 – Fist | *(lihat data)* |
| Vehicles | Spawn Rhino Tank | *(lihat data)* |
| Traffic & Driving | *(lihat data)* |
| Weather | *(lihat data)* |

> Seluruh 82 cheat + kombo lengkap ada di `source/gta_cheats_data.hpp` — tabel di atas cuma contoh.

---

## Troubleshooting

### Overlay kagak muncul pas tombol kombo ditekan
- Pastikan Tesla terinstall & overlay terdaftar di menu Tesla (Overlay directory)
- Pastikan file `.ovl` ada di `/atmosphere/overlays/`
- Pastikan kombo Tesla gak bentrok sama kombo lain

### Overlay muncul tapi cheat gak ke-inject
- Ini kemungkinan besar **`hid:dbg` di-block** di konteks applet. Solusi: pakai
  **sysmodule companion** (hid-mitm / virtual pad) sebagai pengganti — approach ada di
  skill `nintendo-switch-overlay` §5. Overlay ini udah punya seam buat itu
  (`Injector::setButtons()`).
- Cek juga: beberapa cheat GTA SA DE cuma butuh campur tangan tombol **satu kali** —
  kalau kombo ke-inject sebelum game "siap nerima", inject ulang bisa nepak.

### Game nge-freeze / stutter pas pilih cheat
- Kurangi `HOLD_FRAMES` di `source/main.cpp` (default 2) — makin kecil makin cepat
  kombo jalan. Rebuild.

---

## Lisensi

MIT. Terinspirasi Tesla-Template (WerWolv) & libtesla. GTA SA © Rockstar Games —
overlay ini cuma tools input, bukan contain konten game.