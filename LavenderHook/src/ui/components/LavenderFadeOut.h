#pragma once
#include "../../imgui/imgui.h"
#include <cmath>

namespace LavenderHook::UI
{
    class LavenderFadeOut
    {
    public:
        // Single call update 
        void Tick(bool wantVisible)
        {
            SetVisible(wantVisible);
            Update();
        }

        void SetVisible(bool v)
        {
            m_target = v ? 1.0f : 0.0f;
        }

        void Update()
        {
            float dt = ImGui::GetIO().DeltaTime;
            m_alpha += (m_target - m_alpha) * m_speed * dt;

            // snap
            if (fabsf(m_alpha) < 0.001f) m_alpha = 0.0f;
            if (fabsf(m_alpha - 1.0f) < 0.001f) m_alpha = 1.0f;
        }

        float Alpha() const { return m_alpha; }

        // Render while fading or visible
        bool ShouldRender() const
        {
            return m_alpha > 0.0f || m_target > 0.0f;
        }

        bool IsFullyVisible() const { return m_alpha >= 1.0f; }
        bool IsFullyHidden()  const { return m_alpha <= 0.0f; }

        void SetSpeed(float s) { m_speed = s; }

    private:
        float m_alpha = 0.0f;
        float m_target = 0.0f;
        float m_speed = 8.0f;
    };
}
