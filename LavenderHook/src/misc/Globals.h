#pragma once

#include <windows.h>
#include <string>

class CodeEditor;

namespace LavenderHook::Globals
{
    inline uintptr_t base_address = 0;
    inline size_t    module_size = 0;

    // Window info
    inline HWND window_handle = nullptr;
    inline int window_width = 0;
    inline int window_height = 0;
    inline bool menu_animating = false;

    // UI toggles
    inline bool show_menu = false;
    inline bool show_console = false;
    inline bool show_info_overlay = true;
    inline bool show_general_window = true;
    inline bool show_misc_window = true;
    inline bool show_performance_overlay = true;
    inline bool show_menu_selector_window = false;
    inline bool show_menu_logo = true;

    // Performance Overlay Settings
    inline bool show_perf_fps = true;
    inline bool show_perf_ram = true;
    inline bool show_perf_cpu = true;
    inline bool show_perf_gpu = true;

    // Info Overlay
    inline bool show_ping = true;
    inline bool show_server = true;

    // Stop on Fail
	inline bool stop_on_fail = true;
    
    // Keep custom triangle cursor visible even when the menu is hidden
    inline bool show_triangle_when_menu_hidden = false;

    // video overlay
    inline bool show_process_overlay_on_hide = false;

    // Sound volume percentage
    inline int sound_volume = 100;
	inline bool mute_buttons = false;
	inline bool mute_fail = false;

    // Focus shim
    inline bool simulate_unfocused = true;

    // DLL module handle
    inline HMODULE dll_module = nullptr;
    // Tray callback message id
    inline UINT tray_callback_message = WM_APP + 100;

    // Request for chopped Minimize to clear its toggle after regaining focus
    inline bool minimize_auto_clear_requested = false;

    // Set while the game window is hidden (WS_EX_LAYERED alpha=0).
    // Used by WndProc to eat mouse button events while passing WM_MOUSEMOVE through.
    inline bool window_hidden = false;
}
