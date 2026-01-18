#include "../../include/utils/process_utils.h"
#include <tlhelp32.h>
#include <psapi.h>
#include <aclapi.h>
#include <sddl.h>

// 夺取目录所有权并授予管理员完全控制权限
BOOL GrantFullControl(LPCWSTR path)
{
    WCHAR cmd[512];
    STARTUPINFOW si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    // 1. 使用 takeown 夺取所有权（去掉 /r，不进行递归）
    swprintf_s(cmd, 512, L"takeown /f \"%s\" /d y", path);
    if (CreateProcessW(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, 5000);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    // 2. 使用 icacls 授予 Everyone 完全控制权限（去掉 /t，不进行递归）
    swprintf_s(cmd, 512, L"icacls \"%s\" /grant everyone:F /q", path);
    if (CreateProcessW(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, 5000);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return TRUE;
    }

    return FALSE;
}

// 使用taskkill强制终止进程（包括子进程）
static void ForceKillProcess(LPCWSTR processName)
{
    WCHAR cmd[256];
    swprintf_s(cmd, 256, L"taskkill /F /IM %s /T", processName);
    
    STARTUPINFOW si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    
    if (CreateProcessW(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, 5000);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

// 服务状态记录（保存搬家前的状态）
typedef struct {
    BOOL edgeupdateWasRunning;
    BOOL edgeupdatemWasRunning;
    BOOL edgeElevationWasRunning;
    BOOL gupdateWasRunning;
    BOOL gupdatemWasRunning;
    BOOL chromeElevationWasRunning;
} ServiceStateBackup;

static ServiceStateBackup g_serviceBackup = {0};

// 检查服务是否正在运行
static BOOL IsServiceRunning(LPCWSTR serviceName)
{
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!hSCM) {
        return FALSE;
    }
    
    SC_HANDLE hService = OpenServiceW(hSCM, serviceName, SERVICE_QUERY_STATUS);
    if (!hService) {
        CloseServiceHandle(hSCM);
        return FALSE;
    }
    
    SERVICE_STATUS status;
    BOOL running = FALSE;
    if (QueryServiceStatus(hService, &status)) {
        running = (status.dwCurrentState == SERVICE_RUNNING);
    }
    
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCM);
    return running;
}

// 停止Windows服务
static BOOL StopWindowsService(LPCWSTR serviceName)
{
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!hSCM) {
        return FALSE;
    }
    
    SC_HANDLE hService = OpenServiceW(hSCM, serviceName, SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (!hService) {
        CloseServiceHandle(hSCM);
        return FALSE;
    }
    
    SERVICE_STATUS status;
    BOOL result = ControlService(hService, SERVICE_CONTROL_STOP, &status);
    
    // 等待服务停止
    if (result) {
        int retries = 10;
        while (retries-- > 0) {
            if (QueryServiceStatus(hService, &status)) {
                if (status.dwCurrentState == SERVICE_STOPPED) {
                    break;
                }
            }
            Sleep(500);
        }
    }
    
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCM);
    return result;
}

// 启动Windows服务
static BOOL StartWindowsService(LPCWSTR serviceName)
{
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!hSCM) {
        return FALSE;
    }
    
    SC_HANDLE hService = OpenServiceW(hSCM, serviceName, SERVICE_START | SERVICE_QUERY_STATUS);
    if (!hService) {
        CloseServiceHandle(hSCM);
        return FALSE;
    }
    
    BOOL result = StartServiceW(hService, 0, NULL);
    
    // 等待服务启动
    if (result) {
        SERVICE_STATUS status;
        int retries = 10;
        while (retries-- > 0) {
            if (QueryServiceStatus(hService, &status)) {
                if (status.dwCurrentState == SERVICE_RUNNING) {
                    break;
                }
            }
            Sleep(500);
        }
    }
    
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCM);
    return result;
}

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

// 改变服务启动类型
static BOOL SetServiceStartType(LPCWSTR serviceName, DWORD startType)
{
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!hSCM) return FALSE;
    
    SC_HANDLE hService = OpenServiceW(hSCM, serviceName, SERVICE_CHANGE_CONFIG);
    if (!hService) {
        CloseServiceHandle(hSCM);
        return FALSE;
    }
    
    BOOL result = ChangeServiceConfigW(
        hService, SERVICE_NO_CHANGE, startType, SERVICE_NO_CHANGE,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL
    );
    
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCM);
    return result;
}

// 停止浏览器进程
BOOL TerminateBrowserProcesses(BrowserType type)
{
    if (type == BROWSER_CHROME) {
        // 记录Chrome服务原始状态
        g_serviceBackup.gupdateWasRunning = IsServiceRunning(L"gupdate");
        g_serviceBackup.gupdatemWasRunning = IsServiceRunning(L"gupdatem");
        g_serviceBackup.chromeElevationWasRunning = IsServiceRunning(L"GoogleChromeElevationService");
        
        // 暂时禁用服务防止自启动
        SetServiceStartType(L"gupdate", SERVICE_DISABLED);
        SetServiceStartType(L"gupdatem", SERVICE_DISABLED);
        SetServiceStartType(L"GoogleChromeElevationService", SERVICE_DISABLED);
        
        // 停止Chrome相关服务
        StopWindowsService(L"gupdate");
        StopWindowsService(L"gupdatem");
        StopWindowsService(L"GoogleChromeElevationService");
        
        // 强制终止主进程（包括子进程）
        ForceKillProcess(L"chrome.exe");
        
        // 终止其他相关进程
        TerminateProcessByName(L"GoogleUpdate.exe");
        TerminateProcessByName(L"GoogleUpdateCore.exe");
        TerminateProcessByName(L"GoogleUpdateBroker.exe");
        TerminateProcessByName(L"GoogleUpdateOnDemand.exe");
        TerminateProcessByName(L"GoogleCrashHandler.exe");
        TerminateProcessByName(L"GoogleCrashHandler64.exe");
        TerminateProcessByName(L"software_reporter_tool.exe");
        TerminateProcessByName(L"chrome_pwa_launcher.exe");
        TerminateProcessByName(L"notification_helper.exe");
        TerminateProcessByName(L"elevation_service.exe");
        TerminateProcessByName(L"nacl64.exe");
    } else if (type == BROWSER_EDGE) {
        // 记录Edge服务原始状态
        g_serviceBackup.edgeupdateWasRunning = IsServiceRunning(L"edgeupdate");
        g_serviceBackup.edgeupdatemWasRunning = IsServiceRunning(L"edgeupdatem");
        g_serviceBackup.edgeElevationWasRunning = IsServiceRunning(L"MicrosoftEdgeElevationService");
        
        // 暂时禁用服务防止自启动（关键：解决SYSTEM权限锁定问题）
        SetServiceStartType(L"edgeupdate", SERVICE_DISABLED);
        SetServiceStartType(L"edgeupdatem", SERVICE_DISABLED);
        SetServiceStartType(L"MicrosoftEdgeElevationService", SERVICE_DISABLED);
        
        // 停止Edge相关Windows服务
        StopWindowsService(L"edgeupdate");
        StopWindowsService(L"edgeupdatem");
        StopWindowsService(L"MicrosoftEdgeElevationService");
        
        // 强制终止主进程（包括所有子进程）
        ForceKillProcess(L"msedge.exe");
        ForceKillProcess(L"msedgewebview2.exe");
        ForceKillProcess(L"MicrosoftEdge.exe");
        
        // 终止更新相关进程（包含 EdgeUpdate 目录下的所有进程）
        ForceKillProcess(L"MicrosoftEdgeUpdate.exe");
        ForceKillProcess(L"MicrosoftEdgeUpdateCore.exe");
        ForceKillProcess(L"MicrosoftEdgeUpdateBroker.exe");
        ForceKillProcess(L"MicrosoftEdgeUpdateOnDemand.exe");
        ForceKillProcess(L"MicrosoftEdgeUpdateTaskMachineCore.exe");
        ForceKillProcess(L"MicrosoftEdgeUpdateTaskMachineUA.exe");
        ForceKillProcess(L"MicrosoftEdgeUpdateTaskUserMachineCore.exe");
        ForceKillProcess(L"MicrosoftEdgeUpdateTaskUserMachineUA.exe");
        
        // 终止崩溃处理器
        TerminateProcessByName(L"MicrosoftEdgeCrashHandler.exe");
        TerminateProcessByName(L"MicrosoftEdgeCrashHandler64.exe");
        
        // 终止辅助进程
        TerminateProcessByName(L"identity_helper.exe");
        TerminateProcessByName(L"elevation_service.exe");
        TerminateProcessByName(L"msedge_pwa_launcher.exe");
        TerminateProcessByName(L"cookie_exporter.exe");
        TerminateProcessByName(L"notification_helper.exe");
        TerminateProcessByName(L"pwahelper.exe");
        TerminateProcessByName(L"msedge_proxy.exe");
        TerminateProcessByName(L"ie_to_edge_stub.exe");
        TerminateProcessByName(L"setup.exe");
        
        // 再次强制终止主进程（确保彻底关闭）
        Sleep(1000);
        ForceKillProcess(L"msedge.exe");
        ForceKillProcess(L"msedgewebview2.exe");
    } else {
        return FALSE;
    }
    
    return TRUE;
}

// 恢复浏览器服务（恢复到搬家前的状态）
BOOL RestoreBrowserServices(BrowserType type)
{
    if (type == BROWSER_CHROME) {
        // 先恢复启动类型为手动
        SetServiceStartType(L"gupdate", SERVICE_DEMAND_START);
        SetServiceStartType(L"gupdatem", SERVICE_DEMAND_START);
        SetServiceStartType(L"GoogleChromeElevationService", SERVICE_DEMAND_START);
        
        // 只恢复之前正在运行的服务
        if (g_serviceBackup.gupdateWasRunning) {
            StartWindowsService(L"gupdate");
        }
        if (g_serviceBackup.gupdatemWasRunning) {
            StartWindowsService(L"gupdatem");
        }
        if (g_serviceBackup.chromeElevationWasRunning) {
            StartWindowsService(L"GoogleChromeElevationService");
        }
    } else if (type == BROWSER_EDGE) {
        // 先恢复启动类型为手动
        SetServiceStartType(L"edgeupdate", SERVICE_DEMAND_START);
        SetServiceStartType(L"edgeupdatem", SERVICE_DEMAND_START);
        SetServiceStartType(L"MicrosoftEdgeElevationService", SERVICE_DEMAND_START);
        
        // 只恢复之前正在运行的服务
        if (g_serviceBackup.edgeupdateWasRunning) {
            StartWindowsService(L"edgeupdate");
        }
        if (g_serviceBackup.edgeupdatemWasRunning) {
            StartWindowsService(L"edgeupdatem");
        }
        if (g_serviceBackup.edgeElevationWasRunning) {
            StartWindowsService(L"MicrosoftEdgeElevationService");
        }
    } else {
        return FALSE;
    }
    
    return TRUE;
}

// 等待浏览器所有进程退出
BOOL WaitForBrowserExit(BrowserType type, DWORD timeoutMs)
{
    DWORD startTime = GetTickCount();
    
    while (TRUE) {
        BOOL hasProcess = FALSE;
        
        if (type == BROWSER_CHROME) {
            hasProcess = IsProcessRunning(L"chrome.exe") ||
                        IsProcessRunning(L"GoogleUpdate.exe") ||
                        IsProcessRunning(L"GoogleCrashHandler.exe") ||
                        IsProcessRunning(L"GoogleCrashHandler64.exe");
        } else if (type == BROWSER_EDGE) {
            hasProcess = IsProcessRunning(L"msedge.exe") ||
                        IsProcessRunning(L"msedgewebview2.exe") ||
                        IsProcessRunning(L"MicrosoftEdge.exe") ||
                        IsProcessRunning(L"MicrosoftEdgeUpdate.exe") ||
                        IsProcessRunning(L"MicrosoftEdgeUpdateCore.exe") ||
                        IsProcessRunning(L"MicrosoftEdgeCrashHandler.exe") ||
                        IsProcessRunning(L"MicrosoftEdgeCrashHandler64.exe") ||
                        IsProcessRunning(L"identity_helper.exe") ||
                        IsProcessRunning(L"elevation_service.exe");
        }
        
        if (!hasProcess) {
            return TRUE;
        }
        
        // 超时后再次尝试强制终止
        DWORD elapsed = GetTickCount() - startTime;
        if (elapsed >= timeoutMs) {
            // 最后一次强制终止
            TerminateBrowserProcesses(type);
            Sleep(2000);
            return !hasProcess;
        }
        
        Sleep(500);
    }
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