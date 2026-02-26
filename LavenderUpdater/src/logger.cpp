#include "logger.h"
#include <windows.h>
#include <shlobj.h>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>

static std::filesystem::path get_module_dir_for_logger()
{
    wchar_t path[MAX_PATH];
    HMODULE hm = NULL;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCWSTR>(&get_module_dir_for_logger), &hm);
    if (hm)
    {
        DWORD len = GetModuleFileNameW(hm, path, MAX_PATH);
        if (len > 0 && len < MAX_PATH)
        {
            std::filesystem::path p(path);
            return p.parent_path();
        }
    }
    return std::filesystem::current_path();
}

void log_message(const std::string &msg)
{
    std::wstring wmsg(msg.begin(), msg.end());
    OutputDebugStringW((wmsg + L"\r\n").c_str());

    // Resolve %APPDATA%
    wchar_t *p = nullptr;
    std::filesystem::path dir;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &p)))
    {
        dir = std::filesystem::path(p) / L"LavenderHook" / L"LavenderUpdaterLogs";
        CoTaskMemFree(p);
    }
    else
    {
        wchar_t env[MAX_PATH] = {};
        GetEnvironmentVariableW(L"APPDATA", env, MAX_PATH);
        dir = std::filesystem::path(env) / L"LavenderHook" / L"LavenderUpdaterLogs";
    }
    std::error_code ec;
    std::filesystem::create_directories(dir, ec);
    std::filesystem::path logPath = dir / "updater.log";

    std::ofstream log(logPath, std::ios::app);
    if (!log.is_open()) return;

    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    log << "[" << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S") << "] " << msg << std::endl;
}
