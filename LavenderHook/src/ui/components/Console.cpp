#include "console.h"
#include "LavenderFadeOut.h"
#include "../../misc/Globals.h"

// Fade controller
static LavenderHook::UI::LavenderFadeOut g_console_fade;

void LavenderConsole::Render(bool wantVisible)
{
    // Tick fade every frame
    g_console_fade.Tick(wantVisible);

    if (!g_console_fade.ShouldRender())
        return;

    float alpha = g_console_fade.Alpha();

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

    ImGui::Begin("Lavender Console", nullptr,
        ImGuiWindowFlags_NoSavedSettings);

    ImGui::BeginChild("Console");

    for (const std::string& line : this->buffer)
    {
        ImGui::TextUnformatted(line.c_str());
    }

    ImGui::EndChild();
    ImGui::End();

    ImGui::PopStyleVar();
}

void LavenderConsole::Log(std::string line)
{
    std::cout << line << std::endl;
    this->buffer.push_back(std::move(line));
}
