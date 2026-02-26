#include "version_utils.h"
#include <windows.h>
#include <vector>
#include <sstream>

std::string get_local_dll_version(const std::filesystem::path &dllPath)
{
    std::wstring wpath = dllPath.wstring();
    DWORD dummy;
    DWORD size = GetFileVersionInfoSizeW(wpath.c_str(), &dummy);
    if (size == 0) return {};

    std::vector<char> data(size);
    if (!GetFileVersionInfoW(wpath.c_str(), 0, size, data.data())) return {};

    VS_FIXEDFILEINFO *fileInfo = nullptr;
    UINT len = 0;
    if (!VerQueryValueW(data.data(), L"\\", reinterpret_cast<LPVOID*>(&fileInfo), &len)) return {};
    if (!fileInfo) return {};

    DWORD ms = fileInfo->dwFileVersionMS;
    DWORD ls = fileInfo->dwFileVersionLS;
    DWORD major = HIWORD(ms);
    DWORD minor = LOWORD(ms);
    DWORD build = HIWORD(ls);
    DWORD revision = LOWORD(ls);

    std::ostringstream oss;
    oss << major << "." << minor << "." << build << "." << revision;
    return oss.str();
}
