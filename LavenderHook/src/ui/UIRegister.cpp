#include "../ui/UIRegister.h"
#include "../misc/Globals.h"

#include "../windows/GeneralButtonsWindow.h"
#include "../windows/MiscButtonsWindow.h"
#include "../windows/PerformanceOverlayWindow.h"
#include "../windows/ToggleMenuWindow.h"
#include "../ui/components/LavenderBackgroundDim.h"
#include "../ui/components/console.h"
#include "../windows/MenuLogoWindow.h"


void RegisterUIWindows()
{
    auto& ui = UIRegistry::Get();

	static LavenderHook::UI::LavenderBackgroundDim s_menuDim;

	ui.Register(UIWindowEntry{
		[] {
			s_menuDim.Tick(LavenderHook::Globals::show_menu);
		},
		[] {
			s_menuDim.Render();
		},
		nullptr
		});

    ui.Register(UIWindowEntry{
        [] {
            LavenderHook::UI::Windows::GeneralButtonsWindow::UpdateActions();
        },
        [] {
            LavenderHook::UI::Windows::GeneralButtonsWindow::Render(
                LavenderHook::Globals::show_menu &&
                LavenderHook::Globals::show_general_window
            );
        },
        nullptr
        });

    ui.Register(UIWindowEntry{
    [] {
        LavenderHook::UI::Windows::MiscButtonsWindow::UpdateActions();
    },
    [] {
        LavenderHook::UI::Windows::MiscButtonsWindow::Render(
            LavenderHook::Globals::show_menu &&
            LavenderHook::Globals::show_misc_window
        );
    },
    nullptr
        });

    ui.Register(UIWindowEntry{
        nullptr,
        [] {
            LavenderHook::UI::Windows::PerformanceOverlayWindow::Render();
        },
        nullptr
        });

    ui.Register(UIWindowEntry{
        nullptr,
        [] {
            LavenderHook::UI::Windows::RenderMenuSelectorWindow(
                LavenderHook::Globals::show_menu &&
                LavenderHook::Globals::show_menu_selector_window
            );
        },
        nullptr
        });

    ui.Register(UIWindowEntry{
        nullptr,
        [] {
            LavenderConsole::GetInstance().Render(
                LavenderHook::Globals::show_menu &&
                LavenderHook::Globals::show_console
            );
        },
        nullptr
        });

    ui.Register(UIWindowEntry{
        nullptr,
        [] {
            LavenderHook::UI::Windows::ImageDragWindow::Render(
                LavenderHook::Globals::show_menu &&
                LavenderHook::Globals::show_menu_logo
            );
        },
        nullptr
        });
}
