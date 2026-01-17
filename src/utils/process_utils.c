#include "../../include/utils/process_utils.h"
#include <tlhelp32.h>
#include <psapi.h>

// 检查进程是否运行
BOOL IsProcessRunning(LPCWSTR processName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    
    PROCESSENTRY32W pe32 = {0};
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    
    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            if (_wcsicmp(pe32.szExeFile, processName) == 0) {
                CloseHandle(hSnapshot);
                return TRUE;
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }
    
    CloseHandle(hSnapshot);
    return FALSE;
}

// 通过进程名终止进程
BOOL TerminateProcessByName(LPCWSTR processName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    
    PROCESSENTRY32W pe32 = {0};
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    
    BOOL found = FALSE;
    
    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            if (_wcsicmp(pe32.szExeFile, processName) == 0) {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
                if (hProcess) {
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);
                    found = TRUE;
                }
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }
    
    CloseHandle(hSnapshot);
    return found;
}

// 终止浏览器进程
BOOL TerminateBrowserProcesses(BrowserType type)
{
    LPCWSTR processName;
    
    if (type == BROWSER_CHROME) {
        processName = L"chrome.exe";
    } else if (type == BROWSER_EDGE) {
        processName = L"msedge.exe";
    } else {
        return FALSE;
    }
    
    return TerminateProcessByName(processName);
}

// 等待进程退出
BOOL WaitForProcessExit(LPCWSTR processName, DWORD timeoutMs)
{
    DWORD startTime = GetTickCount();
    
    while (IsProcessRunning(processName)) {
        DWORD elapsed = GetTickCount() - startTime;
        if (elapsed >= timeoutMs) {
            return FALSE;
        }
        
        Sleep(500);
    }
    
    return TRUE;
}