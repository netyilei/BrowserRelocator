#ifndef PROCESS_UTILS_H
#define PROCESS_UTILS_H

#include "../common.h"
#include "../browser_types.h"

BOOL IsProcessRunning(LPCWSTR processName);
BOOL TerminateProcessByName(LPCWSTR processName);
BOOL TerminateBrowserProcesses(BrowserType type);
BOOL GrantFullControl(LPCWSTR path);  // 夺取所有权并授权
BOOL RestoreBrowserServices(BrowserType type);  // 搬家完成后恢复服务
BOOL WaitForBrowserExit(BrowserType type, DWORD timeoutMs);
BOOL WaitForProcessExit(LPCWSTR processName, DWORD timeoutMs);

#endif // PROCESS_UTILS_H