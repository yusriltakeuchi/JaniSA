// main.cpp — GTA SA Definitive Edition cheat overlay for Nintendo Switch (Atmosphere + Tesla)
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
// Injector — frame-accurate combo playback through hid:dbg HDLS virtual pad.
// HDLS virtual pad IS read by retail games (DebugPad autopilot is NOT — that's
// why Confirm did nothing). We attach a virtual FullKey (Pro Controller) and
// push button state via hiddbgSetHdlsState.
// Supports single cheats AND sequences of cheats (bundles).
// ---------------------------------------------------------------------------
struct Injector {
    bool playing = false;
    std::vector<const CheatEntry*> queue;   // sequence of cheats to play
    size_t qIndex = 0;                       // current cheat in queue
    const CheatEntry* cur = nullptr;
    int idx = 0;
    int hold = 0;
    static constexpr int HOLD_FRAMES = 2;   // frames each button is held

    HiddbgHdlsSessionId session = {};
    HiddbgHdlsHandle handle = {};
    u8 workBuffer[0x4000] alignas(0x1000);  // HDLS transfer memory (16KB)
    bool attached = false;

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
        // battery/flags so the pad looks powered-on to the game
        HiddbgHdlsState s = {}; s.flags = 0b11; s.battery_level = 4;
        hiddbgSetHdlsState(handle, &s);
        return true;
    }

    void exit() {
        if (!attached) return;
        hiddbgDetachHdlsVirtualDevice(handle);
        hiddbgReleaseHdlsWorkBuffer(session);
        attached = false;
    }

    // play a single cheat (existing behavior)
    void start(const CheatEntry* c) { queue.clear(); queue.push_back(c); begin(); }
    // play a list of cheats back-to-back (bundles)
    void startSequence(const std::vector<const CheatEntry*>& seq) { queue = seq; qIndex = 0; begin(); }
    void begin() {
        if (queue.empty()) { playing = false; cur = nullptr; return; }
        if (!init()) { playing = false; cur = nullptr; return; }
        playing = true; idx = 0; hold = 0; cur = queue[0];
    }
    void stop()  { playing = false; cur = nullptr; queue.clear(); }
    bool active() const { return playing; }
    size_t remaining() const { return queue.empty() ? 0 : queue.size() - qIndex; }

    // Push raw button bitmask to the HDLS virtual pad (read by retail games).
    void setButtons(u64 mask) {
        HiddbgHdlsState state = {};
        state.flags = 0b11;            // powered + charging
        state.battery_level = 4;       // full battery — game ignores pad if low
        state.buttons = mask & 0xfffffffff00fffffULL;   // mask valid bits
        hiddbgSetHdlsState(handle, &state);
    }

    void tick() {
        if (!playing || !cur) return;
        if (hold == 0) setButtons(cur->combo[idx]);
        if (++hold >= HOLD_FRAMES) {
            setButtons(0);               // release
            idx++; hold = 0;
            if (idx >= (int)cur->len) {  // this cheat done -> next in queue
                idx = 0; qIndex++;
                if (qIndex < queue.size()) { cur = queue[qIndex]; }
                else { setButtons(0); stop(); }
            }
        }
    }
};
static Injector gInjector;

// ---------------------------------------------------------------------------
// Input guard base — swallows input for the first N frames after a Gui spawns.
// Fixes: A/B still HELD from the previous screen (e.g. selecting the overlay in
// Tesla) bleeding through and instantly triggering the first list item.
// ---------------------------------------------------------------------------
constexpr int INPUT_GUARD_FRAMES = 20;   // ~333ms @60fps — enough for held buttons to release

class GuardedGui : public tsl::Gui {
public:
    virtual void update() override {
        m_frame++;
        gInjector.tick();
    }
    virtual bool handleInput(u64 keysDown, u64 keysHeld,
        const HidTouchState &touch, HidAnalogStickState l, HidAnalogStickState r) override {
        return m_frame < INPUT_GUARD_FRAMES;   // swallow all input during spawn
    }
protected:
    int m_frame = 0;
};

// ---------------------------------------------------------------------------
// Gui: cheat list for a category
// ---------------------------------------------------------------------------
class GuiCheats : public GuardedGui {
public:
    GuiCheats(int catIdx) : m_cat(catIdx) {}

    virtual tsl::elm::Element* createUI() override {
        auto& cat = CATEGORIES[m_cat];
        auto frame = new tsl::elm::OverlayFrame(cat.title, "GTA SA Cheats");
        auto list  = new tsl::elm::List();
        for (u32 i = 0; i < cat.count; i++) {
            auto item = new tsl::elm::ListItem(cat.items[i].name);
            item->setClickListener([this, i](u64){
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
        confirm->setClickListener([order](u64){
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
        auto frame = new tsl::elm::OverlayFrame("Bundling", "multi-cheat");
        auto list  = new tsl::elm::List();
        for (u32 i = 0; i < BUNDLE_COUNT; i++) {
            auto item = new tsl::elm::ListItem(BUNDLES[i].title);
            item->setValue(BUNDLES[i].desc);
            item->setClickListener([i](u64){ tsl::changeTo<GuiBundleDetail>(i); return true; });
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
        auto frame = new tsl::elm::OverlayFrame("GTA SA Cheats", "82 cheats");
        auto list  = new tsl::elm::List();

        // Bundling entry at top of root menu
        auto b = new tsl::elm::ListItem("Bundling");
        b->setValue("run several cheats at once");
        b->setClickListener([](u64){ tsl::changeTo<GuiBundles>(); return true; });
        list->addItem(b);

        for (int i = 0; i < CATEGORY_COUNT; i++) {
            auto item = new tsl::elm::ListItem(CATEGORIES[i].title);
            char sub[32]; snprintf(sub, sizeof sub, "%u cheats", CATEGORIES[i].count);
            item->setValue(sub);
            item->setClickListener([i](u64){ tsl::changeTo<GuiCheats>(i); return true; });
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