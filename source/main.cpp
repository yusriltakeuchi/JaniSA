// main.cpp — GTA SA Definitive Edition cheat overlay for Nintendo Switch (Atmosphere + Tesla)
//
// Build: devkitPro toolchain (switch-dev), Tesla-Template layout.
//   #define TESLA_INIT_IMPL in exactly ONE translation unit.
// Deploy: <SD>/atmosphere/overlays/gta-cheats.ovl
//
// UX: Tesla open-combo (default L+DDOWN) -> category list -> cheat list -> pick -> auto-inject.
// All 82 cheats, grouped into 7 categories. Data in gta_cheats_data.hpp.
//
// Injection: uses hid:dbg hiddbgSetDebugPadAutoPilotState / hiddbgSetAutoPilotVirtualPadState
// (libnx hiddbg.h). This path can push synthetic buttons to the system HID layer that
// native retail games receive — no sysmodule needed for the overlay itself as long as
// hid:dbg is accessible from the applet context. If hid:dbg is blocked, use the
// hid-mitm / sysmodule companion approach from the skill.
// TESLA_INIT_IMPL defines the overlay's config globals (Framebuffer size, etc.)
// and stb_truetype implementation — must be defined in exactly ONE translation unit.
#define TESLA_INIT_IMPL
#include <tesla.hpp>
#include "gta_cheats_data.hpp"

#include <vector>
#include <cstdio>

// ---------------------------------------------------------------------------
// Injector — frame-accurate combo playback through hid:dbg autopilot
// ---------------------------------------------------------------------------
struct Injector {
    bool playing = false;
    const CheatEntry* cur = nullptr;
    int idx = 0;
    int hold = 0;
    static constexpr int HOLD_FRAMES = 2;   // frames each button is held

    void start(const CheatEntry* c) { cur = c; idx = 0; hold = 0; playing = true; }
    void stop()  { playing = false; cur = nullptr; }
    bool active() const { return playing; }

    // Send raw button mask to hid:dbg. Replace with your hid path if needed.
    void setButtons(u64 mask) {
        HiddbgDebugPadAutoPilotState state = {};
        state.buttons = (u32)mask;
        hiddbgSetDebugPadAutoPilotState(&state);
        // On atmosphere with virtual-pad support you may prefer:
        //   HiddbgAbstractedPadState vstate = {}; vstate.buttons = mask;
        //   hiddbgSetAutoPilotVirtualPadState(0, &vstate);
    }

    void tick() {
        if (!playing || !cur) return;
        if (hold == 0) setButtons(cur->combo[idx]);
        if (++hold >= HOLD_FRAMES) {
            setButtons(0);               // release
            idx++; hold = 0;
            if (idx >= (int)cur->len) { setButtons(0); stop(); }
        }
    }
};
static Injector gInjector;

// ---------------------------------------------------------------------------
// Gui: cheat list for a category
// ---------------------------------------------------------------------------
class GuiCheats : public tsl::Gui {
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

    virtual void update() override { gInjector.tick(); }

    virtual bool handleInput(u64 keysDown, u64 keysHeld,
        const HidTouchState &touch, HidAnalogStickState l, HidAnalogStickState r) override {
        return false;
    }

private:
    int m_cat;
};

// ---------------------------------------------------------------------------
// Gui: category list (root)
// ---------------------------------------------------------------------------
class GuiCategories : public tsl::Gui {
public:
    virtual tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("GTA SA Cheats", "82 cheats");
        auto list  = new tsl::elm::List();
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

    virtual void update() override { gInjector.tick(); }
    virtual bool handleInput(u64 keysDown, u64 keysHeld,
        const HidTouchState &touch, HidAnalogStickState l, HidAnalogStickState r) override {
        return false;
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
        hiddbgSetDebugPadAutoPilotState(nullptr);   // clear
        hiddbgExit();
    }
    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override {
        return initially<GuiCategories>();
    }
};

int main(int argc, char **argv) {
    return tsl::loop<OverlayGTA>(argc, argv);
}