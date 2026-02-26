#include "hooks.h"

#include "Direct11.h"
#include "../input/FocusShim.h"

GUI* gui = nullptr;

// Renderer state
LavenderHook::Hooks::RendererType LavenderHook::Hooks::g_activeRenderer =
LavenderHook::Hooks::RendererType::None;

// Initialize MinHook
bool LavenderHook::Hooks::Initialize()
{
    if (MH_Initialize() != MH_OK)
    {
        LavenderConsole::GetInstance().Log("Failed to initialize MinHook.");
        return false;
    }

    return true;
}

bool LavenderHook::Hooks::Hook()
{
    bool dx11_ok = LavenderHook::Hooks::Present11::Hook();

    if (!dx11_ok)
    {
        LavenderConsole::GetInstance().Log("LavenderHook: failed to hook DX11.");
        return false;
    }

    LavenderConsole::GetInstance().Log(
        "LavenderHook: renderer hooks installed."
    );

    // Install FocusShim
    if (!LavenderHook::Input::FocusShim::Install())
    {
        LavenderConsole::GetInstance().Log("FocusShim: failed to install.");
    }
    else
    {
        LavenderConsole::GetInstance().Log("FocusShim: installed.");
    }

    // Shared hooks
    if (!LavenderHook::Hooks::WndProc::Hook())
    {
        LavenderConsole::GetInstance().Log("LavenderHook: failed to hook WndProc.");
        return false;
    }

    return true;
}

// Unhook everything
bool LavenderHook::Hooks::Unhook()
{

    // Restore WndProc
    if (LavenderHook::Hooks::WndProc::original_wndproc &&
        LavenderHook::Globals::window_handle)
    {
        SetWindowLongPtrW( 
            LavenderHook::Globals::window_handle,
            GWLP_WNDPROC,
            reinterpret_cast<LONG_PTR>(
                LavenderHook::Hooks::WndProc::original_wndproc
                )
        );

        LavenderHook::Hooks::WndProc::original_wndproc = nullptr;
    }

    // Renderer specific shutdown
    switch (g_activeRenderer)
    {
    case RendererType::DX11:
        LavenderHook::Hooks::Present11::Unhook();
        LavenderConsole::GetInstance().Log("LavenderHook: DirectX 11 unhooked.");
        break;

    default:
        LavenderHook::Hooks::Present11::Unhook();
        break;
    }

    g_activeRenderer = RendererType::None;

    // Remove FocusShim
    LavenderHook::Input::FocusShim::Remove();

    // MinHook cleanup
    MH_DisableHook(MH_ALL_HOOKS);
    MH_RemoveHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    LavenderConsole::GetInstance().Log("LavenderHook: clean unhook complete.");
    return true;
}
