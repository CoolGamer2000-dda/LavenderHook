#pragma once

#include <Windows.h>

namespace LavenderHook::UI::Lavender {

    struct KeyEntry { int vk; const char* name; };

    static const KeyEntry VK_TABLE[] = {
        {VK_F1,"F1"},{VK_F2,"F2"},{VK_F3,"F3"},{VK_F4,"F4"},
        {VK_F5,"F5"},{VK_F6,"F6"},{VK_F7,"F7"},{VK_F8,"F8"},
        {VK_F9,"F9"},{VK_F10,"F10"},{VK_F11,"F11"},{VK_F12,"F12"},
        {'0',"0"},{'1',"1"},{'2',"2"},{'3',"3"},{'4',"4"},
        {'5',"5"},{'6',"6"},{'7',"7"},{'8',"8"},{'9',"9"},
        {'A',"A"},{'B',"B"},{'C',"C"},{'D',"D"},{'E',"E"},
        {'F',"F"},{'G',"G"},{'H',"H"},{'I',"I"},{'J',"J"},
        {'K',"K"},{'L',"L"},{'M',"M"},{'N',"N"},{'O',"O"},
        {'P',"P"},{'Q',"Q"},{'R',"R"},{'S',"S"},{'T',"T"},
        {'U',"U"},{'V',"V"},{'W',"W"},{'X',"X"},{'Y',"Y"},{'Z',"Z"},
        {VK_DELETE,"DEL"},{VK_HOME,"HOME"},{VK_END,"END"},
        {VK_PRIOR,"PGUP"},{VK_NEXT,"PGDN"},
        {VK_LEFT,"LEFT"},{VK_RIGHT,"RIGHT"},
        {VK_UP,"UP"},{VK_DOWN,"DOWN"},
        {VK_TAB,"TAB"},{VK_SPACE,"SPACE"},
        {VK_ESCAPE,"ESC"},{VK_RETURN,"ENTER"},
        {VK_BACK,"BKSP"},
        {VK_LSHIFT,"LSHIFT"},{VK_RSHIFT,"RSHIFT"},
        {VK_CONTROL,"CTRL"},{VK_LCONTROL,"LCTRL"},{VK_RCONTROL,"RCTRL"},
        {VK_LMENU,"LALT"},{VK_RMENU,"RALT"},
        {VK_NUMPAD0,"NUM0"},{VK_NUMPAD1,"NUM1"},{VK_NUMPAD2,"NUM2"},
        {VK_NUMPAD3,"NUM3"},{VK_NUMPAD4,"NUM4"},{VK_NUMPAD5,"NUM5"},
        {VK_NUMPAD6,"NUM6"},{VK_NUMPAD7,"NUM7"},{VK_NUMPAD8,"NUM8"},
        {VK_NUMPAD9,"NUM9"},
        {VK_MBUTTON,"MOUSE3"},
        {VK_XBUTTON1,"MOUSE4"},
        {VK_XBUTTON2,"MOUSE5"},
    };

    inline bool IsBindableVk(int vk)
    {
        // L+R Mouse, Insert and CTRL + F1 not allowed as Hotkeys
        if (vk == VK_LBUTTON || vk == VK_RBUTTON) return false;
        if (vk == VK_INSERT) return false;
        if ((vk & 0xFFFF0000) != 0) {
            int first = vk & 0xFFFF;
            int second = (vk >> 16) & 0xFFFF;
            auto isCtrl = [](int k) { return k == VK_CONTROL || k == VK_LCONTROL || k == VK_RCONTROL; };
            if ((first == VK_F1 && isCtrl(second)) || (second == VK_F1 && isCtrl(first)))
                return false;
        }
        return true;
    }

    inline const char* VkToString(int vk)
    {
        // Combo Hotkeys
        if ((vk & 0xFFFF0000) != 0) {
            int first = vk & 0xFFFF;
            int second = (vk >> 16) & 0xFFFF;
            static thread_local std::string combo;
            combo.clear();
            const char* f = VkToString(first);
            const char* s = VkToString(second);
            combo = std::string(f) + "+" + std::string(s);
            return combo.c_str();
        }

        if (!vk) return "None";
        for (auto& e : VK_TABLE)
            if (e.vk == vk) return e.name;

        // UTF-8 UI
        static thread_local std::string s;
        s.clear();

        wchar_t wbuf[64] = {0};
        UINT sc = MapVirtualKeyW((UINT)vk, MAPVK_VK_TO_VSC);
        if (sc != 0)
        {
            LONG lParam = (sc << 16);
            if (vk == VK_RCONTROL || vk == VK_RMENU || vk == VK_INSERT || vk == VK_DELETE ||
                vk == VK_HOME || vk == VK_END || vk == VK_PRIOR || vk == VK_NEXT ||
                vk == VK_LEFT || vk == VK_RIGHT || vk == VK_UP || vk == VK_DOWN)
            {
                lParam |= (1 << 24);
            }

            if (GetKeyNameTextW(lParam, wbuf, (int)std::size(wbuf)) > 0)
            {
                int needed = WideCharToMultiByte(CP_UTF8, 0, wbuf, -1, NULL, 0, NULL, NULL);
                if (needed > 0)
                {
                    s.resize(needed - 1);
                    WideCharToMultiByte(CP_UTF8, 0, wbuf, -1, &s[0], needed, NULL, NULL);
                    return s.c_str();
                }
            }
        }

        return "VK?";
    }
}