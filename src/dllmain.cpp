#include "util/core.h"
#include "anticheat.h"

void start()
{
    std::thread acThread(AntiCheat::Load);
    acThread.detach();
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        start();
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
		AntiCheat::Unload();
        break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}