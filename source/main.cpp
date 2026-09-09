// main.cpp — JaniSA: GTA SA cheat overlay for Nintendo Switch (Atmosphere + Tesla)
//
// Build: devkitPro toolchain (switch-dev), Tesla-Template layout.
//   #define TESLA_INIT_IMPL in exactly ONE translation unit.
// Deploy: <SD>/atmosphere/overlays/gta-cheats.ovl
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

#include <vector>
#include <cstdio>

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
    bool attached = false;
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
        // cancel any in-flight run and wait for the bg thread to finish
        // BEFORE releasing hid:dbg — otherwise the thread may call
        // hiddbgSetHdlsState on a torn-down session (use-after-free).
        if (m_busy.load()) {
            cancel();
            if (m_thread.handle != 0) {
                threadWaitForExit(&m_thread);
                threadClose(&m_thread);
                m_thread = {};
            }
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

    bool isBusy() const { return m_busy.load(); }
    bool shouldClose() { return m_scheduleClose.exchange(false); }

    // UI thread: called every frame. Executes the pending close on the UI thread
    // (never call Overlay::close() from the bg thread — it touches the renderer).
    void tick() {
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
        m_busy = true;
        tsl::Overlay::get()->hide();   // critical: hide BEFORE injecting
        threadCreate(&m_thread, threadEntry, this, nullptr, 0x4000, 0x2c, -2);
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
        gInjector.tick();   // UI thread: close overlay when bg inject finishes
    }
    virtual bool handleInput(u64 keysDown, u64 keysHeld,
        const HidTouchState &touch, HidAnalogStickState l, HidAnalogStickState r) override {
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
            item->setClickListener([this, i](u64 keys){
                if (!(keys & HidNpadButton_A)) return false;   // only A activates
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

        auto confirm = new tsl::elm::ListItem("Confirm ▶");
        confirm->setClickListener([order](u64 keys){
            if (!(keys & HidNpadButton_A)) return false;   // only A activates
            if (!order.empty()) gInjector.startSequence(order);   // run ALL, no toggles
            return true;
        });
        list->addItem(confirm);

        frame->setContent(list);
        return frame;
    }

private:
    const BundleDef* m_bundle;
};

// ---------------------------------------------------------------------------
// Gui: bundle list (root-level Bundling entry)
// ---------------------------------------------------------------------------
class GuiBundles : public GuardedGui {
public:
    virtual tsl::elm::Element* createUI() override {
        beginInputGuard();
        auto frame = new tsl::elm::OverlayFrame("Bundling", "multi-cheat");
        auto list  = new tsl::elm::List();
        for (u32 i = 0; i < BUNDLE_COUNT; i++) {
            auto item = new tsl::elm::ListItem(BUNDLES[i].title);
            item->setValue(BUNDLES[i].desc);
            item->setClickListener([i](u64 keys){ if (!(keys & HidNpadButton_A)) return false; tsl::changeTo<GuiBundleDetail>(i); return true; });
            list->addItem(item);
        }
        frame->setContent(list);
        return frame;
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
        b->setValue("run several cheats at once");
        b->setClickListener([](u64 keys){ if (!(keys & HidNpadButton_A)) return false; tsl::changeTo<GuiBundles>(); return true; });
        list->addItem(b);

        for (int i = 0; i < CATEGORY_COUNT; i++) {
            auto item = new tsl::elm::ListItem(CATEGORIES[i].title);
            char sub[32]; snprintf(sub, sizeof sub, "%u cheats", CATEGORIES[i].count);
            item->setValue(sub);
            item->setClickListener([i](u64 keys){ if (!(keys & HidNpadButton_A)) return false; tsl::changeTo<GuiCheats>(i); return true; });
            list->addItem(item);
        }
        frame->setContent(list);
        return frame;
    }
};

// ---------------------------------------------------------------------------
class OverlayGTA : public tsl::Overlay {
public:
    virtual void initServices() override {
        // hid:dbg needed for injector
        hiddbgInitialize();
    }
    virtual void exitServices() override {
        gInjector.exit();   // detach HDLS virtual pad + release buffer
        hiddbgExit();
    }
    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override {
        return initially<GuiCategories>();
    }
};

int main(int argc, char **argv) {
    return tsl::loop<OverlayGTA>(argc, argv);
}