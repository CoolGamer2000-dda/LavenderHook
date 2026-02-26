#pragma once
#include "../../imgui/imgui.h"
#include <string>

namespace LavenderHook::UI::Lavender {

    struct GradientStyle {
        ImGuiCol colorA;
        ImGuiCol colorB;
        float speed = 0.30f;
    };

    // Global style
    GradientStyle& Gradient();

    // Draw text
    void GradientText(const std::string& text, float alpha = 1.0f);
}
