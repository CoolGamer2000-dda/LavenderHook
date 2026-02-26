#include "InputAutomation.h"
#include "../misc/Globals.h"

namespace LavenderHook::Input {

    // Policy
    constexpr bool kRequireForeground = false;

    bool AutomationAllowed()
    {
        if (LavenderHook::Globals::show_menu)
            return false;

        if (kRequireForeground &&
            GetForegroundWindow() != LavenderHook::Globals::window_handle)
            return false;

        return true;
    }

    // Key primitives
    void PressVK(WORD vk)
    {
        HWND hwnd = LavenderHook::Globals::window_handle;
        if (!hwnd) return;

        PostMessage(hwnd, WM_KEYDOWN, vk, 1);
        PostMessage(hwnd, WM_KEYUP, vk, (1 << 30) | (1 << 31));
    }

    void PressDownVK(WORD vk)
    {
        HWND hwnd = LavenderHook::Globals::window_handle;
        if (!hwnd) return;

        PostMessage(hwnd, WM_KEYDOWN, vk, 1);
    }

    void PressUpVK(WORD vk)
    {
        HWND hwnd = LavenderHook::Globals::window_handle;
        if (!hwnd) return;

        PostMessage(hwnd, WM_KEYUP, vk, (1 << 30) | (1 << 31));
    }

    // Hold helper
    void HoldVK(bool enabled, WORD vk, HoldState& state)
    {
        if (!AutomationAllowed())
            enabled = false;

        if (enabled && !state.isDown) {
            PressDownVK(vk);
            state.isDown = true;
        }
        else if (!enabled && state.isDown) {
            PressUpVK(vk);
            state.isDown = false;
        }
    }

} // namespace LavenderHook::Input
