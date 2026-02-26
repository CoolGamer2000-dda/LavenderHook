#include "updater_core.h"
#include "version_utils.h"
#include "http_client.h"
#include "logger.h"

#include <windows.h>
#include <filesystem>
#include <string>

static std::wstring get_module_dir()
{
    wchar_t path[MAX_PATH];
    HMODULE hm = NULL;
    // Get module handle from this address
    // When using GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS the parameter is actually treated as an address.
    // Cast the function pointer to LPCWSTR to satisfy the API prototype in the headers.
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCWSTR>(&get_module_dir), &hm);
    if (hm)
    {
        DWORD len = GetModuleFileNameW(hm, path, MAX_PATH);
        if (len > 0 && len < MAX_PATH)
        {
            std::filesystem::path p(path);
            return p.parent_path().wstring();
        }
    }
    // fallback to current directory
    return std::filesystem::current_path().wstring();
}

extern "C" __declspec(dllexport) void RunUpdater()
{
    std::wstring moduleDir = get_module_dir();
    std::filesystem::path dllPath = std::filesystem::path(moduleDir) / L"LavenderHook" / L"LavenderHook.dll";

    std::string localVersion = get_local_dll_version(dllPath);

    std::string remoteText;
    std::wstring url = L"https://raw.githubusercontent.com/KaMuZunai/LavenderHook/main/VERSION";
    bool fetched = fetch_url(url, remoteText);
    // simple trim
    auto trim = [](const std::string &s){ size_t a = s.find_first_not_of(" \t\r\n"); if (a==std::string::npos) return std::string(); size_t b = s.find_last_not_of(" \t\r\n"); return s.substr(a, b-a+1); };
    std::string remoteVersion = fetched ? trim(remoteText) : std::string();

    if (localVersion.empty())
    {
        log_message(std::string("Local DLL not found or version unavailable: ") + std::string("LavenderHook\\LavenderHook.dll"));
        return;
    }
    if (!fetched)
    {
        log_message(std::string("Failed to fetch remote VERSION"));
        return;
    }

    std::string msg = std::string("Local version=") + localVersion + ", Remote version=" + remoteVersion + " -> ";
    if (localVersion == remoteVersion)
        msg += "MATCH";
    else
        msg += "DID NOT MATCH";

    log_message(msg);
}
