/*
 * Ugaris C++ Demo Mod
 *
 * A demonstration of native mod development using modern C++.
 * Shows basic API usage with C++ features like classes, RAII, and type safety.
 *
 * Commands:
 *   #hello   - Display a greeting message
 *   #stats   - Show current HP/Mana/Gold
 *   #overlay - Toggle a simple HUD overlay
 */

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <atomic>

// Platform-specific export macro
#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __attribute__((visibility("default")))
#endif

// ============================================================================
// Constants
// ============================================================================

constexpr int V_HP = 0;
constexpr int V_MANA = 2;
constexpr int V_WIS = 3;
constexpr int V_INT = 4;
constexpr int V_AGI = 5;
constexpr int V_STR = 6;
constexpr int V_MAX = 200;
constexpr int DOT_TL = 0;

// Color helper (RGB 5-5-5)
constexpr uint16_t IRGB(uint16_t r, uint16_t g, uint16_t b) {
    return (r << 10) | (g << 5) | b;
}

// ============================================================================
// FFI Declarations
// ============================================================================

extern "C" {
    // Logging
    int note(const char* format, ...);
    void addline(const char* format, ...);

    // Rendering
    void render_rect(int sx, int sy, int ex, int ey, uint16_t color);
    void render_line(int fx, int fy, int tx, int ty, uint16_t color);
    int render_text(int sx, int sy, uint16_t color, int flags, const char* text);

    // GUI helpers
    int dotx(int didx);
    int doty(int didx);

    // Utilities
    int exp2level(int val);

    // Game state
    extern int hp;
    extern int mana;
    extern int gold;
    extern int experience;
    extern int value[2][V_MAX];
    extern char username[40];

    // Colors
    extern uint16_t whitecolor;
    extern uint16_t textcolor;
    extern uint16_t healthcolor;
    extern uint16_t manacolor;
}

// ============================================================================
// Mod State (using C++ features)
// ============================================================================

namespace {
    std::atomic<bool> show_overlay{false};
    std::atomic<uint32_t> frame_count{0};
}

// ============================================================================
// Helper class for formatted strings
// ============================================================================

class FormattedString {
public:
    template<typename... Args>
    FormattedString(const char* fmt, Args... args) {
        std::snprintf(buffer_, sizeof(buffer_), fmt, args...);
    }

    const char* c_str() const { return buffer_; }
    operator const char*() const { return buffer_; }

private:
    char buffer_[256];
};

// ============================================================================
// Mod Callbacks
// ============================================================================

extern "C" {

DLL_EXPORT const char* amod_version() {
    return "C++ Demo Mod 1.0.0";
}

DLL_EXPORT void amod_init() {
    note("C++ Demo Mod initializing...");
}

DLL_EXPORT void amod_exit() {
    note("C++ Demo Mod shutting down.");
}

DLL_EXPORT void amod_gamestart() {
    note("C++ Demo Mod: Game started! Welcome, %s", username);
    addline("C++ Demo Mod loaded. Type #hello for commands.");
}

DLL_EXPORT void amod_tick() {
    // Called 24 times per second
}

DLL_EXPORT void amod_frame() {
    frame_count.fetch_add(1, std::memory_order_relaxed);

    if (!show_overlay.load(std::memory_order_relaxed)) {
        return;
    }

    const int x = dotx(DOT_TL) + 10;
    const int y = doty(DOT_TL) + 10;
    constexpr int w = 180;
    constexpr int h = 80;

    // Panel background
    render_rect(x, y, x + w, y + h, IRGB(4, 4, 6));

    // Panel border
    constexpr auto border_color = IRGB(12, 12, 16);
    render_line(x, y, x + w, y, border_color);
    render_line(x, y + h, x + w, y + h, border_color);
    render_line(x, y, x, y + h, border_color);
    render_line(x + w, y, x + w, y + h, border_color);

    // Title
    render_text(x + 4, y + 4, whitecolor, 0, "C++ Demo Mod");

    int text_y = y + 20;

    // HP
    render_text(x + 4, text_y, healthcolor, 0,
                FormattedString("HP: %d / %d", hp, value[0][V_HP]));
    text_y += 14;

    // Mana
    render_text(x + 4, text_y, manacolor, 0,
                FormattedString("Mana: %d / %d", mana, value[0][V_MANA]));
    text_y += 14;

    // Gold
    render_text(x + 4, text_y, IRGB(31, 31, 0), 0,
                FormattedString("Gold: %d", gold));
    text_y += 14;

    // Frame counter
    render_text(x + 4, text_y, textcolor, 0,
                FormattedString("Frame: %u", frame_count.load()));
}

DLL_EXPORT void amod_mouse_move(int, int) {}

DLL_EXPORT int amod_mouse_click(int, int, int) {
    return 0;
}

DLL_EXPORT int amod_keydown(int) {
    return 0;
}

DLL_EXPORT int amod_keyup(int) {
    return 0;
}

DLL_EXPORT int amod_client_cmd(const char* buf) {
    if (std::strcmp(buf, "#hello") == 0) {
        addline("=== C++ Demo Mod Commands ===");
        addline("#hello   - Show this help");
        addline("#stats   - Display current stats");
        addline("#overlay - Toggle HUD overlay");
        return 1;
    }

    if (std::strcmp(buf, "#stats") == 0) {
        const int level = exp2level(experience);
        addline("=== Player Stats (from C++) ===");
        addline("Level: %d  Experience: %d", level, experience);
        addline("HP: %d/%d  Mana: %d/%d", hp, value[0][V_HP], mana, value[0][V_MANA]);
        addline("STR: %d  AGI: %d  INT: %d  WIS: %d",
                value[0][V_STR], value[0][V_AGI], value[0][V_INT], value[0][V_WIS]);
        addline("Gold: %d", gold);
        return 1;
    }

    if (std::strcmp(buf, "#overlay") == 0) {
        bool new_state = !show_overlay.load();
        show_overlay.store(new_state);
        addline("Overlay: %s", new_state ? "ON" : "OFF");
        return 1;
    }

    return 0;
}

} // extern "C"
