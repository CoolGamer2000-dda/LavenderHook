#include "ProcessThumbnailOverlay.h"
#include "../../misc/Globals.h"
#include "../../imgui/imgui.h"

#include <windows.h>
#include <dwmapi.h>
#include <windowsx.h>

// Theme color globals
extern ImVec4 MAIN_RED;
extern ImVec4 MID_RED;
extern ImVec4 DARK_RED;

namespace LavenderHook {
    namespace UI {

        static const wchar_t kThumbnailClass[] = L"LvHkThumbnailOverlay";

        static constexpr int kTitleH      = 24;
        static constexpr int kBorder      = 6; 
        static constexpr int kVideoBorder = 2;
        static constexpr int kCornerR     = 8;

        // GDI colors
        struct OverlayColors {
            COLORREF bg;
            COLORREF main;
            COLORREF mid;
            COLORREF dark;
        };
        static OverlayColors g_colors;

        static HWND       g_overlayHwnd   = nullptr;
        static HANDLE     g_overlayThread = nullptr;
        static HWND       g_gameHwnd      = nullptr;
        static HTHUMBNAIL g_thumbnail     = nullptr;
        static float      g_aspectRatio   = 0.f;  // game client W/H, 0 = unknown

        // Helpers 
        static COLORREF ImVecToColorref(const ImVec4& c)
        {
            return RGB(static_cast<BYTE>(c.x * 255.f),
                       static_cast<BYTE>(c.y * 255.f),
                       static_cast<BYTE>(c.z * 255.f));
        }

        static void ApplyWindowRgn(HWND hwnd)
        {
            RECT wr{};
            GetWindowRect(hwnd, &wr);
            const int w = wr.right  - wr.left;
            const int h = wr.bottom - wr.top;

            HRGN rr   = CreateRoundRectRgn(0, 0,
                                            w + 1, h + kCornerR * 2 + 1,
                                            kCornerR * 2, kCornerR * 2);
            HRGN rect = CreateRectRgn(0, 0, w + 1, h + 1);
            HRGN rgn  = CreateRectRgn(0, 0, 0, 0);
            CombineRgn(rgn, rr, rect, RGN_AND);
            DeleteObject(rr);
            DeleteObject(rect);
            SetWindowRgn(hwnd, rgn, TRUE);
        }

        // Fits the DWM thumbnail inside the border strips.
        static void UpdateThumbnailRect(HWND hwnd)
        {
            if (!g_thumbnail || !hwnd)
                return;

            RECT rc{};
            GetClientRect(hwnd, &rc);

            RECT dest = {
                rc.left   + kVideoBorder,
                rc.top    + kTitleH,
                rc.right  - kVideoBorder,
                rc.bottom - kVideoBorder
            };
            if (dest.right <= dest.left || dest.bottom <= dest.top)
                return;

            DWM_THUMBNAIL_PROPERTIES props{};
            props.dwFlags               = DWM_TNP_SOURCECLIENTAREAONLY
                                        | DWM_TNP_VISIBLE
                                        | DWM_TNP_RECTDESTINATION
                                        | DWM_TNP_OPACITY;
            props.fSourceClientAreaOnly = TRUE;
            props.fVisible              = TRUE;
            props.rcDestination         = dest;
            props.opacity               = 255;

            DwmUpdateThumbnailProperties(g_thumbnail, &props);
        }

        static void DrawTitleBar(HDC hdc, const RECT& cr)
        {
            // Background fill
            HBRUSH bg = CreateSolidBrush(g_colors.bg);
            RECT titleRc = { cr.left, cr.top, cr.right, cr.top + kTitleH };
            FillRect(hdc, &titleRc, bg);
            DeleteObject(bg);

            // MAIN_RED on top row, DARK_RED on bottom row
            HBRUSH s1 = CreateSolidBrush(g_colors.main);
            RECT sep1 = { cr.left, kTitleH - 2, cr.right, kTitleH - 1 };
            FillRect(hdc, &sep1, s1);
            DeleteObject(s1);

            HBRUSH s2 = CreateSolidBrush(g_colors.dark);
            RECT sep2 = { cr.left, kTitleH - 1, cr.right, kTitleH };
            FillRect(hdc, &sep2, s2);
            DeleteObject(s2);

            // Title text DARK_RED
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, g_colors.dark);
            HFONT font = CreateFontW(
                13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            HFONT oldFont = static_cast<HFONT>(SelectObject(hdc, font));
            RECT tr = { cr.left + 10, cr.top, cr.right - 10, kTitleH - 2 };
            DrawTextW(hdc, L"LavenderHook  \u2014  Overlay", -1, &tr,
                      DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            SelectObject(hdc, oldFont);
            DeleteObject(font);
        }

        // Window procedure

        static LRESULT CALLBACK ThumbnailWndProc(HWND hwnd, UINT msg,
                                                 WPARAM wp, LPARAM lp)
        {
            switch (msg)
            {
            case WM_NCHITTEST:
            {
                POINT pt = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
                RECT  wr{};
                GetWindowRect(hwnd, &wr);

                const bool onLeft   = pt.x <  wr.left   + kBorder;
                const bool onRight  = pt.x >  wr.right  - kBorder;
                const bool onTop    = pt.y <  wr.top    + kBorder;
                const bool onBottom = pt.y >  wr.bottom - kBorder;

                if (onLeft  && onTop)    return HTTOPLEFT;
                if (onRight && onTop)    return HTTOPRIGHT;
                if (onLeft  && onBottom) return HTBOTTOMLEFT;
                if (onRight && onBottom) return HTBOTTOMRIGHT;
                if (onLeft)              return HTLEFT;
                if (onRight)             return HTRIGHT;
                if (onTop)               return HTTOP;
                if (onBottom)            return HTBOTTOM;

                return HTCAPTION;
            }

            case WM_NCRBUTTONDOWN:
            case WM_NCRBUTTONUP:
            case WM_NCLBUTTONDBLCLK:
                return 0;

            // Lock resize to the source window's aspect ratio.
            case WM_SIZING:
            {
                if (g_aspectRatio <= 0.f)
                    break;

                RECT* r = reinterpret_cast<RECT*>(lp);
                const int frameW = kVideoBorder * 2;
                const int frameH = kTitleH + kVideoBorder;

                const int videoW = (r->right  - r->left) - frameW;
                const int videoH = (r->bottom - r->top)  - frameH;

                const bool dragH = (wp == WMSZ_LEFT   || wp == WMSZ_RIGHT  ||
                                    wp == WMSZ_TOPLEFT || wp == WMSZ_TOPRIGHT ||
                                    wp == WMSZ_BOTTOMLEFT || wp == WMSZ_BOTTOMRIGHT);

                if (dragH) {
                    const int newH = (videoW > 0)
                        ? static_cast<int>(videoW / g_aspectRatio) + frameH
                        : frameH + 1;
                    if (wp == WMSZ_TOPLEFT || wp == WMSZ_TOPRIGHT)
                        r->top = r->bottom - newH;
                    else
                        r->bottom = r->top + newH;
                } else {
                    const int newW = (videoH > 0)
                        ? static_cast<int>(videoH * g_aspectRatio) + frameW
                        : frameW + 1;
                    r->right = r->left + newW;
                }
                return TRUE;
            }

            case WM_SIZE:
                ApplyWindowRgn(hwnd);
                UpdateThumbnailRect(hwnd);
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;

            case WM_ERASEBKGND:
                return 1;

            case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC  hdc = BeginPaint(hwnd, &ps);
                RECT rc{};
                GetClientRect(hwnd, &rc);

                // Dark fill for the whole client area
                HBRUSH bgBr = CreateSolidBrush(g_colors.bg);
                FillRect(hdc, &rc, bgBr);
                DeleteObject(bgBr);

                DrawTitleBar(hdc, rc);

                // Border strips around the video
                HBRUSH vBr = CreateSolidBrush(g_colors.main);
                RECT strip = { rc.left, rc.top + kTitleH,
                               rc.left + kVideoBorder, rc.bottom };
                FillRect(hdc, &strip, vBr);  // left
                strip = { rc.right - kVideoBorder, rc.top + kTitleH,
                          rc.right, rc.bottom };
                FillRect(hdc, &strip, vBr);  // right
                strip = { rc.left, rc.bottom - kVideoBorder,
                          rc.right, rc.bottom };
                FillRect(hdc, &strip, vBr);  // bottom
                DeleteObject(vBr);


                HPEN   bPen   = CreatePen(PS_INSIDEFRAME, kVideoBorder, g_colors.dark);
                HPEN   oldPen = static_cast<HPEN>(SelectObject(hdc, bPen));
                HBRUSH oldBr  = static_cast<HBRUSH>(
                                    SelectObject(hdc, GetStockObject(NULL_BRUSH)));
                RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom,
                          kCornerR * 2, kCornerR * 2);
                SelectObject(hdc, oldPen);
                SelectObject(hdc, oldBr);
                DeleteObject(bPen);

                EndPaint(hwnd, &ps);
                return 0;
            }

            case WM_GETMINMAXINFO:
            {
                auto* mmi = reinterpret_cast<MINMAXINFO*>(lp);
                const int minVideoW = 200;
                const int minVideoH = (g_aspectRatio > 0.f)
                    ? static_cast<int>(minVideoW / g_aspectRatio)
                    : 120;
                mmi->ptMinTrackSize = {
                    minVideoW + kVideoBorder * 2,
                    minVideoH + kTitleH + kVideoBorder
                };
                return 0;
            }

            case WM_DESTROY:
                if (g_thumbnail)
                {
                    DwmUnregisterThumbnail(g_thumbnail);
                    g_thumbnail = nullptr;
                }
                PostQuitMessage(0);
                return 0;
            }

            return DefWindowProcW(hwnd, msg, wp, lp);
        }

        // Background thread

        static DWORD WINAPI ThumbnailThreadProc(LPVOID)
        {
            WNDCLASSEXW wc{};
            wc.cbSize        = sizeof(wc);
            wc.lpfnWndProc   = ThumbnailWndProc;
            wc.hInstance     = LavenderHook::Globals::dll_module;
            wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
            wc.lpszClassName = kThumbnailClass;
            RegisterClassExW(&wc);

            int initW = 480;
            int initH = 290 + kTitleH;
            if (g_gameHwnd)
            {
                RECT gr{};
                GetWindowRect(g_gameHwnd, &gr);
                int qw = (gr.right  - gr.left) / 4;
                int qh = (gr.bottom - gr.top)  / 4 + kTitleH;
                initW = qw > 240           ? qw : 240;
                initH = qh > kTitleH + 120 ? qh : kTitleH + 120;
            }

            HWND hwnd = CreateWindowExW(
                WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
                kThumbnailClass,
                L"LavenderHook",
                WS_POPUP,
                100, 100, initW, initH,
                nullptr, nullptr,
                LavenderHook::Globals::dll_module,
                nullptr
            );

            if (!hwnd)
                return 1;

            ApplyWindowRgn(hwnd);

            if (g_gameHwnd &&
                SUCCEEDED(DwmRegisterThumbnail(hwnd, g_gameHwnd, &g_thumbnail)))
            {
                UpdateThumbnailRect(hwnd);
            }

            g_overlayHwnd = hwnd;
            ShowWindow(hwnd, SW_SHOW);
            UpdateWindow(hwnd);

            MSG m{};
            while (GetMessageW(&m, nullptr, 0, 0) > 0)
            {
                TranslateMessage(&m);
                DispatchMessageW(&m);
            }

            g_overlayHwnd = nullptr;
            return 0;
        }

        // API 

        void CreateProcessOverlay(HWND gameHwnd)
        {
            if (g_overlayHwnd)
                return;

            // Snapshot ImGui theme colors
            g_colors.bg   = RGB(18, 12, 28);
            g_colors.main = ImVecToColorref(MAIN_RED);
            g_colors.mid  = ImVecToColorref(MID_RED);
            g_colors.dark = ImVecToColorref(DARK_RED);

            // Capture the source aspect ratio for resize locking
            g_aspectRatio = 0.f;
            if (gameHwnd) {
                RECT cr{};
                GetClientRect(gameHwnd, &cr);
                const int w = cr.right  - cr.left;
                const int h = cr.bottom - cr.top;
                if (w > 0 && h > 0)
                    g_aspectRatio = static_cast<float>(w) / static_cast<float>(h);
            }

            g_gameHwnd      = gameHwnd;
            g_overlayThread = CreateThread(nullptr, 0,
                                           ThumbnailThreadProc, nullptr, 0, nullptr);
        }

        void DestroyProcessOverlay()
        {
            if (g_overlayHwnd)
                PostMessageW(g_overlayHwnd, WM_CLOSE, 0, 0);
            if (g_overlayThread)
            {
                WaitForSingleObject(g_overlayThread, 1000);
                CloseHandle(g_overlayThread);
                g_overlayThread = nullptr;
            }
        }

    } // namespace UI
} // namespace LavenderHook
