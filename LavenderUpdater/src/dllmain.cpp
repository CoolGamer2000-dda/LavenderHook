#include <windows.h>
#include "updater_core.h"

static DWORD WINAPI UpdaterThreadProc(LPVOID)
{
    RunUpdater();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD ul_reason_for_call, LPVOID /*lpReserved*/)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        HANDLE h = CreateThread(NULL, 0, UpdaterThreadProc, NULL, 0, NULL);
        if (h) CloseHandle(h);
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
