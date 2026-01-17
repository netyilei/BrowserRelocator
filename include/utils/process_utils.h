#ifndef PROCESS_UTILS_H
#define PROCESS_UTILS_H

#include "../common.h"

BOOL IsProcessRunning(LPCWSTR processName);
BOOL TerminateProcessByName(LPCWSTR processName);
BOOL TerminateBrowserProcesses(BrowserType type);
BOOL WaitForProcessExit(LPCWSTR processName, DWORD timeoutMs);

#endif // PROCESS_UTILS_H