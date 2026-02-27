#include "http_client.h"
#include <windows.h>
#include <winhttp.h>
#include <vector>

bool fetch_url(const std::wstring &url, std::string &out)
{
    URL_COMPONENTS components = {0};
    components.dwStructSize = sizeof(components);
    components.dwSchemeLength = (DWORD)-1;
    components.dwHostNameLength = (DWORD)-1;
    components.dwUrlPathLength = (DWORD)-1;
    components.dwExtraInfoLength = (DWORD)-1;

    if (!WinHttpCrackUrl(url.c_str(), (DWORD)url.length(), 0, &components)) return false;

    std::wstring scheme(components.lpszScheme, components.dwSchemeLength);
    std::wstring host(components.lpszHostName, components.dwHostNameLength);
    std::wstring path(components.lpszUrlPath, components.dwUrlPathLength);
    std::wstring extra(components.lpszExtraInfo, components.dwExtraInfoLength);
    std::wstring fullPath = path + extra;

    HINTERNET hSession = WinHttpOpen(L"LavenderUpdater/1.0", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return false;

    INTERNET_PORT port = components.nPort;
    HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(), port, 0);
    if (!hConnect) { WinHttpCloseHandle(hSession); return false; }

    DWORD flags = 0;
    if (_wcsicmp(scheme.c_str(), L"https") == 0) flags |= WINHTTP_FLAG_SECURE;

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", fullPath.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
    if (!hRequest) { WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return false; }

    BOOL ok = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
    if (!ok) { WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return false; }

    ok = WinHttpReceiveResponse(hRequest, NULL);
    if (!ok) { WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return false; }

    out.clear();
    const DWORD bufferSize = 4096;
    std::vector<char> buffer(bufferSize);
    DWORD bytesRead = 0;
    while (WinHttpQueryDataAvailable(hRequest, &bytesRead) && bytesRead > 0)
    {
        DWORD toRead = bytesRead;
        if (toRead > bufferSize) toRead = bufferSize;
        DWORD actuallyRead = 0;
        if (!WinHttpReadData(hRequest, buffer.data(), toRead, &actuallyRead)) break;
        if (actuallyRead == 0) break;
        out.append(buffer.data(), actuallyRead);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return true;
}
