// main.cpp — JaniSA: GTA SA cheat overlay for Nintendo Switch (Atmosphere + Tesla)
//
// Build: devkitPro toolchain (switch-dev), Tesla-Template layout.
//   #define TESLA_INIT_IMPL in exactly ONE translation unit.
// Deploy: <SD>/switch/.overlays/JaniSA.ovl
//
// UX: Tesla open-combo (default L+DDOWN) -> category list -> cheat list -> pick -> auto-inject.
// All 82 cheats, grouped into 7 categories. Data in gta_cheats_data.hpp.
//
// Injection: uses hid:dbg HDLS virtual pad (hiddbgAttachHdlsVirtualDevice +
// hiddbgSetHdlsState) — the path retail games actually read.
// (libnx hiddbg.h). This path can push synthetic buttons to the system HID layer that
// native retail games receive — no sysmodule needed for the overlay itself as long as
// hid:dbg is accessible from the applet context. If hid:dbg is blocked, use the
// hid-mitm / sysmodule companion approach from the skill.
// TESLA_INIT_IMPL defines the overlay's config globals (Framebuffer size, etc.)
// and stb_truetype implementation — must be defined in exactly ONE translation unit.
#define TESLA_INIT_IMPL
#include <tesla.hpp>
#include "gta_cheats_data.hpp"
#include "gta_bundles_data.hpp"
#include "gta_config.hpp"

#include <vector>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <malloc.h>
#include <switch/services/audout.h>
// ---------------------------------------------------------------------------
// CheckListItem — ListItem variant that draws a checkbox (square + checkmark)
// using the renderer directly instead of relying on a font glyph (the shared
// Nintendo fonts have no "✓" codepoint, so plain strings render blank).
// ---------------------------------------------------------------------------
class CheckListItem : public tsl::elm::ListItem {
public:
    CheckListItem(const std::string& text, bool checked)
        : tsl::elm::ListItem(text), m_checked(checked) {
        refresh();
    }

    void setChecked(bool checked) { m_checked = checked; refresh(); }
    bool isChecked() const { return m_checked; }

    virtual void draw(tsl::gfx::Renderer* renderer) override {
        // Reuse ListItem::draw for the text + click animation + borders.
        ListItem::draw(renderer);

        // Draw the checkbox in the value area (right side), replacing the
        // normal value string. Same anchor as ListItem::draw for m_value:
        //   getX() + m_maxWidth + 45, centered vertically at getY()+45.
        int cx = this->getX() + this->m_maxWidth + 45;
        int cy = this->getY() + 36;
        const int S = 18;   // box size

        auto border = tsl::gfx::Renderer::a(tsl::style::color::ColorText);
        auto check  = tsl::gfx::Renderer::a(tsl::style::color::ColorHighlight);

        renderer->drawRect(cx, cy, S, 1, border);
        renderer->drawRect(cx, cy + S, S, 1, border);
        renderer->drawRect(cx, cy, 1, S, border);
        renderer->drawRect(cx + S, cy, 1, S, border);

        if (m_checked) {
            // checkmark: two thick diagonal strokes
            renderer->drawRect(cx + 3, cy + 9, 5, 3, check);
            renderer->drawRect(cx + 6, cy + 12, 9, 3, check);
            renderer->drawRect(cx + 6, cy + 12, 3, -3, check);
        }
    }

private:
    void refresh() {
        // Keep ListItem::draw's value-width math happy — the box is drawn
        // manually so the value string is empty.
        this->setValue(m_checked ? "  " : "  ");
    }

    bool m_checked;
};

// ---------------------------------------------------------------------------
// Custom bundle state — loaded from config on boot, edited via GuiBundleEditor,
// injected from the Bundling menu. Multiple user bundles supported.
// ---------------------------------------------------------------------------
static std::vector<janisaConfig::CustomBundleDef> gCustomBundles;

// ---------------------------------------------------------------------------
// SFX engine — simple PCM beeps via audout for user feedback.
// Init once at overlay open, play short sine waves on actions.
// ---------------------------------------------------------------------------

static const u32 SFX_SR  = 48000;            // sample rate
static const u32 SFX_CH  = 2;                // stereo
static const float SFX_PI = 3.14159265f;

static void* gSfxOn    = nullptr;  // cheat toggle ON
static void* gSfxOff   = nullptr;  // cheat toggle OFF
static void* gSfxOk    = nullptr;  // confirm / apply
static void* gSfxDel   = nullptr;  // delete / hapus
static u32   gSfxBytes = 0;        // aligned buffer size (0x1000)
static bool  gSfxReady = false;

static void exitSfx();   // forward decl — called from initSfx on failure

// Generate a mono sine beep into a stereo 16-bit PCM buffer (0x1000-aligned).
// len_ms: milliseconds; freq: Hz; vol: 0.0-1.0
static void genBeep(void* buf, u32 len_ms, float freq, float vol) {
    s16* p = (s16*)buf;
    u32 samples = SFX_SR * len_ms / 1000;
    for (u32 i = 0; i < samples; i++) {
        float t = (float)i / SFX_SR;
        float env = 1.0f - (float)i / samples;   // linear fade-out
        s16 v = (s16)(vol * env * sinf(2.0f * SFX_PI * freq * t) * 32767);
        p[i * 2]     = v;   // L
        p[i * 2 + 1] = v;   // R
    }
}

static void initSfx() {
    // list devices
    char devNames[0x100] = {};
    u32 devCount = 0;
    Result rc = audoutListAudioOuts(devNames, 1, &devCount);
    if (R_FAILED(rc) || devCount == 0) return;

    u32 sr = SFX_SR, ch = SFX_CH;
    PcmFormat fmt = PcmFormat_Int16;
    AudioOutState state;
    rc = audoutOpenAudioOut(devNames, devNames, sr, ch, &sr, &ch, &fmt, &state);
    if (R_FAILED(rc)) return;
    audoutStartAudioOut();

    // buffer: 100ms, 0x1000-aligned
    u32 raw = SFX_SR * 100 / 1000 * SFX_CH * sizeof(s16);  // 19200
    gSfxBytes = (raw + 0xFFF) & ~0xFFF;                     // 20480

    gSfxOn  = memalign(0x1000, gSfxBytes);
    gSfxOff = memalign(0x1000, gSfxBytes);
    gSfxOk  = memalign(0x1000, gSfxBytes);
    gSfxDel = memalign(0x1000, gSfxBytes);
    if (!gSfxOn || !gSfxOff || !gSfxOk || !gSfxDel) { exitSfx(); return; }

    // clear padding
    memset(gSfxOn,  0, gSfxBytes);
    memset(gSfxOff, 0, gSfxBytes);
    memset(gSfxOk,  0, gSfxBytes);
    memset(gSfxDel, 0, gSfxBytes);

    genBeep(gSfxOn,  60, 880.0f, 0.25f);   // high blip — ON
    genBeep(gSfxOff, 60, 440.0f, 0.20f);   // low blip  — OFF
    genBeep(gSfxOk,  80, 660.0f, 0.25f);   // mid       — confirm
    genBeep(gSfxDel, 80, 330.0f, 0.20f);   // low       — delete
    gSfxReady = true;
}

static void exitSfx() {
    if (gSfxReady) {
        audoutStopAudioOut();
        audoutExit();
        gSfxReady = false;
    }
    free(gSfxOn);  gSfxOn  = nullptr;
    free(gSfxOff); gSfxOff = nullptr;
    free(gSfxOk);  gSfxOk  = nullptr;
    free(gSfxDel); gSfxDel = nullptr;
    gSfxBytes = 0;
}

// Play a pre-generated beep (blocking, ~60-100 ms)
static void playSfx(void* buf) {
    if (!gSfxReady || !buf) return;
    AudioOutBuffer src = {};
    src.buffer      = buf;
    src.buffer_size = gSfxBytes;
    src.data_size   = gSfxBytes;
    AudioOutBuffer* released = nullptr;
    audoutPlayBuffer(&src, &released);
}

// ---------------------------------------------------------------------------
// Injector — plays cheat combos through the hid:dbg HDLS virtual pad.
// KEY INSIGHT: the injected buttons live in the SAME HID layer Tesla's overlay
// reads. Most GTA combos start with L / D-pad — the same keys as the Tesla
// open-combo — so injecting while the overlay is open re-triggers Tesla and the
// game never sees the input. Fix: hide the overlay FIRST, then play the combo
// from a background thread using REAL-TIME sleeps (update() stops while hidden,
// so frame-based ticking cannot drive the injection), then close the overlay.
// ---------------------------------------------------------------------------
struct Injector {
    std::vector<const CheatEntry*> queue;
    size_t qIndex = 0;

    HiddbgHdlsSessionId session = {};
    HiddbgHdlsHandle handle = {};
    u8 workBuffer[0x4000] alignas(0x1000);  // HDLS transfer memory (16KB)
    std::atomic<bool> attached{false};   // HDLS virtual device attached
    Thread m_thread = {};
    std::atomic<bool> m_busy{false};        // a run is in-flight
    std::atomic<bool> m_scheduleClose{false}; // bg thread finished -> UI closes overlay
    Mutex m_queueLock = {};                 // protects queue/qIndex

    // per-button timing (ms) — GTA DE needs ~50ms+ to register a press
    static constexpr u64 HOLD_NS = 60ULL * 1'000'000ULL;        // 60ms hold
    static constexpr u64 GAP_NS  = 40ULL * 1'000'000ULL;        // 40ms release gap
    static constexpr u64 BETWEEN_CHEATS_NS = 400ULL * 1'000'000ULL; // 400ms between bundle cheats

    bool init() {
        if (attached) return true;
        Result rc = hiddbgAttachHdlsWorkBuffer(&session, workBuffer, sizeof(workBuffer));
        if (R_FAILED(rc)) return false;
        HiddbgHdlsDeviceInfo info = {};
        info.deviceType = HidDeviceType_FullKey3;   // Pro Controller
        info.npadInterfaceType = HidNpadInterfaceType_USB;
        rc = hiddbgAttachHdlsVirtualDevice(&handle, &info);
        if (R_FAILED(rc)) { hiddbgReleaseHdlsWorkBuffer(session); return false; }
        attached = true;
        HiddbgHdlsState s = {}; s.flags = 0b11; s.battery_level = 4;
        hiddbgSetHdlsState(handle, &s);
        return true;
    }

    void exit() {
        // Cancel any in-flight run and ALWAYS reap the bg thread before releasing
        // hid:dbg. Previously we only joined when m_busy was set — but the run may
        // have finished (m_busy already false) while m_thread was still never
        // threadClose()d, leaking the thread object across overlay relaunches.
        if (m_thread.handle != 0) {
            cancel();
            threadWaitForExit(&m_thread);
            threadClose(&m_thread);
            m_thread = {};
        }
        if (!attached) return;
        hiddbgDetachHdlsVirtualDevice(handle);
        hiddbgReleaseHdlsWorkBuffer(session);
        attached = false;
    }

    void cancel() {
        // best-effort: stop the current playback by releasing buttons
        if (attached) setButtons(0);
        m_busy = false;   // bg thread checks this each press
    }

    // Push raw button bitmask to the HDLS virtual pad.
    // NOTE: firmware masks buttons internally (0xfffffffff00fffff) — HOME/Capture
    // are dropped. HidNpadButton_* layout matches HiddbgHdlsState.buttons.
    void setButtons(u64 mask) {
        if (!attached) return;
        HiddbgHdlsState state = {};
        state.flags = 0b11;            // powered + charging
        state.battery_level = 4;       // full battery
        state.buttons = mask;          // firmware applies the mask
        hiddbgSetHdlsState(handle, &state);
    }

    // Reset transient injector state (queue etc.) — safe to call anytime.
    void reset() {
        mutexLock(&m_queueLock);
        queue.clear(); qIndex = 0;
        mutexUnlock(&m_queueLock);
        m_busy = false;
        m_scheduleClose = false;
    }

    bool isBusy() const { return m_busy.load(); }
    bool shouldClose() { return m_scheduleClose.exchange(false); }

    // UI thread: called during input processing (NOT update/render). Executes the
    // pending close on the UI thread (never call Overlay::close() from the bg thread
    // or from update() — it must not race the renderer).
    void poll() {
        if (shouldClose()) tsl::Overlay::get()->close();
    }

private:
    void playCheat(const CheatEntry* c) {
        for (u32 i = 0; i < c->len; i++) {
            if (!m_busy.load()) return;   // cancelled
            setButtons(c->combo[i]);
            svcSleepThread(HOLD_NS);
            setButtons(0);
            svcSleepThread(GAP_NS);
        }
    }

    void runSync() {
        // give the overlay a moment to actually disappear before injecting
        svcSleepThread(300ULL * 1'000'000ULL);
        if (!m_busy.load() || !init()) { m_busy = false; m_scheduleClose = true; return; }

        mutexLock(&m_queueLock);
        std::vector<const CheatEntry*> snapshot = queue;
        mutexUnlock(&m_queueLock);

        for (size_t i = 0; i < snapshot.size() && m_busy.load(); i++) {
            playCheat(snapshot[i]);
            if (i + 1 < snapshot.size() && m_busy.load()) svcSleepThread(BETWEEN_CHEATS_NS);
        }
        if (attached) setButtons(0);
        m_busy = false;
        m_scheduleClose = true;   // UI thread closes the overlay
    }

    static void threadEntry(void* arg) {
        static_cast<Injector*>(arg)->runSync();
    }

    void launch() {
        if (m_busy.load()) return;   // already injecting — ignore duplicate tap
        // Reap any previous run's thread before creating a new one. Without this
        // the stale Thread handle gets overwritten and never closed — leaks the
        // kernel object and can corrupt state on repeated injections.
        if (m_thread.handle != 0) {
            threadWaitForExit(&m_thread);
            threadClose(&m_thread);
            m_thread = {};
        }
        m_busy = true;
        tsl::Overlay::get()->hide();   // critical: hide BEFORE injecting
        threadCreate(&m_thread, threadEntry, this, nullptr, 0x8000, 0x2c, -2);
        threadStart(&m_thread);
    }

public:
    // Called from the UI thread (click listeners): set the work, then launch.
    void start(const CheatEntry* c) {
        if (m_busy.load()) return;
        mutexLock(&m_queueLock);
        queue.clear(); queue.push_back(c);
        mutexUnlock(&m_queueLock);
        launch();
    }
    void startSequence(const std::vector<const CheatEntry*>& seq) {
        if (m_busy.load()) return;
        mutexLock(&m_queueLock);
        queue = seq;
        mutexUnlock(&m_queueLock);
        launch();
    }
};
static Injector gInjector;

// ---------------------------------------------------------------------------
// Input guard — swallows input for a short real-time window after a Gui spawns.
// Fixes: A/B still HELD from the previous screen (e.g. selecting the overlay in
// Tesla) bleeding through — the held key gets buffered in keysDownPending during
// the fade-in animation and fires onClick on the first item the moment input
// processing resumes. A frame counter CANNOT guard this (update() runs during
// fade while handleInput does not), so we use real time.
// Must gate BOTH the click listeners (which Overlay::handleInput fires BEFORE
// Gui::handleInput) AND the Gui handleInput (for D-pad focus / back).
// ---------------------------------------------------------------------------
constexpr std::chrono::milliseconds INPUT_GUARD_MS(500);

struct InputGuard {
    std::chrono::steady_clock::time_point armed = std::chrono::steady_clock::now();
    bool active() const {
        return std::chrono::steady_clock::now() - armed < INPUT_GUARD_MS;
    }
    void reset() { armed = std::chrono::steady_clock::now(); }
};
static InputGuard gGuard;

class GuardedGui : public tsl::Gui {
public:
    virtual void update() override {
        // close() must not race the renderer — handled in handleInput() via poll()
    }
    virtual bool handleInput(u64 keysDown, u64 keysHeld,
        const HidTouchState &touch, HidAnalogStickState l, HidAnalogStickState r) override {
        gInjector.poll();   // UI thread: close overlay once bg inject finishes
        return gGuard.active();   // swallow ALL input (incl. B = back) during guard window
    }
protected:
    // re-arm the guard on every screen change
    void beginInputGuard() { gGuard.reset(); }
};

// ---------------------------------------------------------------------------
// Gui: cheat list for a category
// ---------------------------------------------------------------------------
class GuiCheats : public GuardedGui {
public:
    GuiCheats(int catIdx) : m_cat(catIdx) {}

    virtual tsl::elm::Element* createUI() override {
        beginInputGuard();
        auto& cat = CATEGORIES[m_cat];
        auto frame = new tsl::elm::OverlayFrame(cat.title, "JaniSA");
        auto list  = new tsl::elm::List();
        for (u32 i = 0; i < cat.count; i++) {
            auto item = new tsl::elm::ListItem(cat.items[i].name);
            item->setValue("▶");   // nav arrow on the right
            item->setClickListener([this, i](u64 keys){
                if (!(keys & HidNpadButton_A)) return false;   // only A activates
                playSfx(gSfxOk);
                gInjector.start(&CATEGORIES[m_cat].items[i]);
                return true;
            });
            list->addItem(item);
        }
        frame->setContent(list);
        return frame;
    }

private:
    int m_cat;
};

// ---------------------------------------------------------------------------
// Gui: bundle detail — read-only cheat list + Confirm (runs ALL cheats in order)
// ---------------------------------------------------------------------------
class GuiBundleDetail : public GuardedGui {
public:
    GuiBundleDetail(int bundleIdx) : m_bundle(&BUNDLES[bundleIdx]) {}

    virtual tsl::elm::Element* createUI() override {
        beginInputGuard();
        char subtitle[64];
        snprintf(subtitle, sizeof subtitle, "%u cheats · confirm to run", m_bundle->count);
        auto frame = new tsl::elm::OverlayFrame(m_bundle->title, subtitle);
        auto list  = new tsl::elm::List();

        // resolve bundle -> ordered list of CheatEntry*, skipping unknown names
        std::vector<const CheatEntry*> order;
        for (u32 i = 0; i < m_bundle->count; i++) {
            const CheatEntry* c = findCheat(m_bundle->cheatNames[i]);
            if (!c) continue;
            order.push_back(c);
            list->addItem(new tsl::elm::ListItem(c->name));   // read-only row
        }

        auto confirm = new tsl::elm::ListItem("Confirm");
        confirm->setValue("▶");
        confirm->setClickListener([order](u64 keys){
            if (!(keys & HidNpadButton_A)) return false;   // only A activates
            if (!order.empty()) { playSfx(gSfxOk); gInjector.startSequence(order); }   // run ALL, no toggles
            return true;
        });
        list->addItem(confirm);

        frame->setContent(list);
        return frame;
    }

private:
    const BundleDef* m_bundle;
};


// ===========================================================================
// BUNDLE GUI LAYER
// ===========================================================================

// ---------------------------------------------------------------------------
// Gui: bundle list (root-level Bundling entry)
// Built-in bundles -> detail/run, custom bundles -> run, Custom Bundle -> hub
// ---------------------------------------------------------------------------
class GuiBundleEditor;   // forward decl
class GuiCustomBundleHub;

class GuiBundles : public GuardedGui {
public:
    virtual tsl::elm::Element* createUI() override {
        beginInputGuard();
        auto frame = new tsl::elm::OverlayFrame("Bundling", "multi-cheat");
        auto list  = new tsl::elm::List();

        // Built-in bundles (click -> detail)
        for (u32 i = 0; i < BUNDLE_COUNT; i++) {
            auto item = new tsl::elm::ListItem(BUNDLES[i].title);
            item->setValue(BUNDLES[i].desc);   // e.g. "3 cheats"
            item->setClickListener([i](u64 keys){ if (!(keys & HidNpadButton_A)) return false; tsl::changeTo<GuiBundleDetail>(i); return true; });
            list->addItem(item);
        }

        // Custom bundles — click to RUN (no editing here)
        for (size_t i = 0; i < gCustomBundles.size(); i++) {
            const auto& b = gCustomBundles[i];
            char sub[48]; snprintf(sub, sizeof sub, "%u cheats", (unsigned)b.cheats.size());
            auto item = new tsl::elm::ListItem(b.title);
            item->setValue(sub);
            item->setClickListener([i](u64 keys){
                if (!(keys & HidNpadButton_A)) return false;
                std::vector<const CheatEntry*> order;
                for (const auto& name : gCustomBundles[i].cheats)
                    if (const CheatEntry* c = findCheat(name.c_str())) order.push_back(c);
                if (!order.empty()) gInjector.startSequence(order);
                return true;
            });
            list->addItem(item);
        }

        // Custom Bundle hub — create / edit bundles
        auto hub = new tsl::elm::ListItem("Custom Bundle");
        hub->setValue("\u25B6");
        hub->setClickListener([](u64 keys){ if (!(keys & HidNpadButton_A)) return false; tsl::changeTo<GuiCustomBundleHub>(); return true; });
        list->addItem(hub);

        frame->setContent(list);
        return frame;
    }
};

// ---------------------------------------------------------------------------
// Gui: custom bundle hub — create new bundle or edit existing ones
// ---------------------------------------------------------------------------
class GuiEditBundleList;

class GuiCustomBundleHub : public GuardedGui {
public:
    virtual tsl::elm::Element* createUI() override {
        beginInputGuard();
        auto frame = new tsl::elm::OverlayFrame("Custom Bundle", "manage your bundles");
        auto list  = new tsl::elm::List();

        auto create = new tsl::elm::ListItem("Create Bundle");
        create->setValue("+");
        create->setClickListener([](u64 keys){
            if (!(keys & HidNpadButton_A)) return false;
            // Auto-name: Custom 1, Custom 2, ...
            char nameBuf[32];
            snprintf(nameBuf, sizeof nameBuf, "Custom %zu", gCustomBundles.size() + 1);
            janisaConfig::CustomBundleDef nb;
            nb.title = nameBuf;
            nb.cheats.clear();
            gCustomBundles.push_back(nb);
            janisaConfig::saveBundles(gCustomBundles);
            tsl::changeTo<GuiBundleEditor>(gCustomBundles.size() - 1);
            return true;
        });
        list->addItem(create);

        auto edit = new tsl::elm::ListItem("Edit Bundle");
        edit->setValue("\u25B6");
        edit->setClickListener([](u64 keys){ if (!(keys & HidNpadButton_A)) return false; tsl::changeTo<GuiEditBundleList>(); return true; });
        list->addItem(edit);

        frame->setContent(list);
        return frame;
    }
};

// ---------------------------------------------------------------------------
// Gui: edit bundle list — pick an existing custom bundle to edit
// ---------------------------------------------------------------------------
class GuiEditBundleList : public GuardedGui {
public:
    virtual tsl::elm::Element* createUI() override {
        beginInputGuard();
        auto frame = new tsl::elm::OverlayFrame("Edit Bundle", "select bundle to edit");
        auto list  = new tsl::elm::List();

        for (size_t i = 0; i < gCustomBundles.size(); i++) {
            const auto& b = gCustomBundles[i];
            char sub[48]; snprintf(sub, sizeof sub, "%u cheats", (unsigned)b.cheats.size());
            auto item = new tsl::elm::ListItem(b.title);
            item->setValue(sub);
            item->setClickListener([i](u64 keys){ if (!(keys & HidNpadButton_A)) return false; tsl::changeTo<GuiBundleEditor>(i); return true; });
            list->addItem(item);
        }

        frame->setContent(list);
        return frame;
    }
};

// ---------------------------------------------------------------------------
// Gui: custom bundle editor — checklist of cheats + Apply + Hapus
// ---------------------------------------------------------------------------
class GuiBundleEditor : public GuardedGui {
public:
    GuiBundleEditor(size_t idx) : m_idx(idx) {}

    virtual tsl::elm::Element* createUI() override {
        beginInputGuard();
        auto& b = gCustomBundles[m_idx];
        auto frame = new tsl::elm::OverlayFrame("Edit Bundle", b.title.c_str());
        auto list  = new tsl::elm::List();

        // Apply — save selection and go back
        auto apply = new tsl::elm::ListItem("Apply");
        apply->setValue("\u25B6");
        apply->setClickListener([this](u64 keys){
            if (!(keys & HidNpadButton_A)) return false;
            playSfx(gSfxOk);
            gCustomBundles[m_idx].cheats = m_selected;
            janisaConfig::saveBundles(gCustomBundles);
            tsl::changeTo<GuiBundles>();
            return true;
        });
        list->addItem(apply);

        // Hapus — delete this bundle entirely
        auto hapus = new tsl::elm::ListItem("Hapus");
        hapus->setValue("\u25B6");
        hapus->setClickListener([this](u64 keys){
            if (!(keys & HidNpadButton_A)) return false;
            playSfx(gSfxDel);
            gCustomBundles.erase(gCustomBundles.begin() + m_idx);
            janisaConfig::saveBundles(gCustomBundles);
            tsl::changeTo<GuiBundles>();
            return true;
        });
        list->addItem(hapus);

        // Build selection mirror from existing cheats
        m_selected.clear();
        for (const auto& s : gCustomBundles[m_idx].cheats) m_selected.push_back(s);

        // Cheat checklist (all 82)
        for (int c = 0; c < CATEGORY_COUNT; c++)
            for (u32 i = 0; i < CATEGORIES[c].count; i++) {
                bool sel = false;
                for (const auto& s : m_selected) if (s == CATEGORIES[c].items[i].name) { sel = true; break; }
                auto item = new CheckListItem(CATEGORIES[c].items[i].name, sel);
                item->setClickListener([this, i, c, item](u64 keys){
                    if (!(keys & HidNpadButton_A)) return false;
                    bool on = !isSelected(CATEGORIES[c].items[i].name);
                    setSelected(CATEGORIES[c].items[i].name, on);
                    item->setChecked(on);
                    playSfx(on ? gSfxOn : gSfxOff);
                    return true;
                });
                list->addItem(item);
            }

        frame->setContent(list);
        return frame;
    }

private:
    size_t m_idx;
    std::vector<std::string> m_selected;

    bool isSelected(const char* name) const {
        for (const auto& s : m_selected) if (s == name) return true;
        return false;
    }
    void setSelected(const char* name, bool on) {
        for (size_t i = 0; i < m_selected.size(); i++)
            if (m_selected[i] == name) { if (!on) m_selected.erase(m_selected.begin() + i); return; }
        if (on && m_selected.size() < janisaConfig::MAX_CUSTOM_CHEATS) m_selected.push_back(name);
    }
};
// ---------------------------------------------------------------------------
// Gui: category list (root)
// ---------------------------------------------------------------------------
class GuiCategories : public GuardedGui {
public:
    virtual tsl::elm::Element* createUI() override {
        beginInputGuard();
        auto frame = new tsl::elm::OverlayFrame("JaniSA", "by Jani \u00b7 82 cheats");
        auto list  = new tsl::elm::List();

        // Bundling entry at top of root menu
        auto b = new tsl::elm::ListItem("Bundling");
        b->setValue("Packs");
        b->setClickListener([](u64 keys){ if (!(keys & HidNpadButton_A)) return false; tsl::changeTo<GuiBundles>(); return true; });
        list->addItem(b);

        for (int i = 0; i < CATEGORY_COUNT; i++) {
            auto item = new tsl::elm::ListItem(CATEGORIES[i].title);
            char sub[32]; snprintf(sub, sizeof sub, "%u cheats", CATEGORIES[i].count);
            item->setValue(sub);   // count — no nav arrow on category rows
            item->setClickListener([i](u64 keys){ if (!(keys & HidNpadButton_A)) return false; tsl::changeTo<GuiCheats>(i); return true; });
            list->addItem(item);
        }
        frame->setContent(list);
        return frame;
    }
};

// ---------------------------------------------------------------------------
// OverlayGTA — Tesla overlay entry point
// ---------------------------------------------------------------------------
class OverlayGTA : public tsl::Overlay {
public:
    virtual void initServices() override {
        gInjector.exit();   // idempotent
        gInjector.reset();
        hiddbgInitialize();
        initSfx();                          // audout PCM for SFX feedback
        gCustomBundles = janisaConfig::loadBundles();
    }
    virtual void exitServices() override {
        gInjector.exit();
        exitSfx();                          // release audout before hiddbg
        hiddbgExit();
    }
    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override {
        return initially<GuiCategories>();
    }
};

int main(int argc, char **argv) {
    return tsl::loop<OverlayGTA>(argc, argv);
}