#pragma once
#include <windows.h>
#include <chrono>

namespace LavenderHook::Input {

    // Automation rules
    bool AutomationAllowed();

    // Key primitives
    void PressVK(WORD vk);
    void PressDownVK(WORD vk);
    void PressUpVK(WORD vk);

    // Hold helper
    struct HoldState {
        bool isDown = false;
    };

    void HoldVK(bool enabled, WORD vk, HoldState& state);

    // Cadence helper
    template <class Duration, class Func>
    void TickEvery(bool enabled,
        std::chrono::steady_clock::time_point& last_fire,
        Duration interval,
        const Func& action)
    {
        if (!enabled) return;
        if (!AutomationAllowed()) return;

        const auto now = std::chrono::steady_clock::now();
        if (last_fire.time_since_epoch().count() == 0 ||
            (now - last_fire) >= interval)
        {
            action();
            last_fire = now;
        }
    }

} // namespace LavenderHook::Input
