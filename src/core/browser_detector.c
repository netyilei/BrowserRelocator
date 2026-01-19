#include "../../include/core/browser_detector.h"
#include "../../include/core/symlink.h"
#include "../../include/browser_types.h"

// 默认路径定义
#define CHROME_APP_PATH_64  L"C:\\Program Files\\Google\\Chrome\\Application"
#define CHROME_APP_PATH_32  L"C:\\Program Files (x86)\\Google\\Chrome\\Application"
#define CHROME_DATA_PATH    L"%LOCALAPPDATA%\\Google\\Chrome\\User Data"

#define EDGE_APP_PATH_64    L"C:\\Program Files\\Microsoft\\Edge\\Application"
#define EDGE_APP_PATH_32    L"C:\\Program Files (x86)\\Microsoft\\Edge\\Application"
#define EDGE_DATA_PATH      L"%LOCALAPPDATA%\\Microsoft\\Edge\\User Data"

// 检查浏览器是否安装
BOOL IsBrowserInstalled(BrowserType type, BrowserInfo* info)
{
    const WCHAR* paths[2];
    int pathCount = 0;
    
    if (type == BROWSER_CHROME) {
        paths[0] = CHROME_APP_PATH_64;
        paths[1] = CHROME_APP_PATH_32;
        pathCount = 2;
        wcscpy_s(info->exeName, _countof(info->exeName), L"chrome.exe");
    } else if (type == BROWSER_EDGE) {
        paths[0] = EDGE_APP_PATH_64;
        paths[1] = EDGE_APP_PATH_32;
        pathCount = 2;
        wcscpy_s(info->exeName, _countof(info->exeName), L"msedge.exe");
    } else {
        return FALSE;
    }
    
    for (int i = 0; i < pathCount && paths[i] != NULL; i++) {
        if (PathFileExistsW(paths[i])) {
            wcscpy_s(info->defaultAppPath, _countof(info->defaultAppPath), paths[i]);
            wcscpy_s(info->appPath, _countof(info->appPath), paths[i]);
            info->isInstalled = TRUE;
            
            // 获取版本信息
            GetBrowserVersion(type, info->version, _countof(info->version));
            
            // 检测用户数据路径
            DetectUserDataPath(type, info);
            
            return TRUE;
        }
    }
    
    info->isInstalled = FALSE;
    return FALSE;
}

// 检测用户数据路径
void DetectUserDataPath(BrowserType type, BrowserInfo* info)
{
    WCHAR expandedPath[MAX_PATH];
    LPCWSTR templatePath;
    
    if (type == BROWSER_CHROME) {
        templatePath = CHROME_DATA_PATH;
    } else {
        templatePath = EDGE_DATA_PATH;
    }
    
    ExpandEnvironmentStringsW(templatePath, expandedPath, MAX_PATH);
    
    if (PathFileExistsW(expandedPath)) {
        wcscpy_s(info->defaultDataPath, _countof(info->defaultDataPath), expandedPath);
        wcscpy_s(info->dataPath, _countof(info->dataPath), expandedPath);
    }
}

// 获取浏览器版本
BOOL GetBrowserVersion(BrowserType type, WCHAR* version, int size)
{
    WCHAR exePath[MAX_PATH];
    DWORD dwSize = size;
    
    if (type == BROWSER_CHROME) {
        wcscpy_s(exePath, _countof(exePath), L"chrome.exe");
    } else {
        wcscpy_s(exePath, _countof(exePath), L"msedge.exe");
    }
    
    // 从注册表读取版本
    HKEY hKey;
    LPCWSTR regPaths[3];
    int regPathCount = 0;
    
    if (type == BROWSER_CHROME) {
        regPaths[0] = L"SOFTWARE\\Google\\Chrome\\BLBeacon";
        regPathCount = 1;
    } else {
        // Edge 可能在多个注册表位置
        regPaths[0] = L"SOFTWARE\\Microsoft\\Edge\\BLBeacon";
        regPaths[1] = L"SOFTWARE\\WOW6432Node\\Microsoft\\Edge\\BLBeacon";
        regPathCount = 2;
    }
    
    for (int i = 0; i < regPathCount; i++) {
        if (RegOpenKeyExW(HKEY_CURRENT_USER, regPaths[i], 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            if (RegQueryValueExW(hKey, L"version", NULL, NULL, (LPBYTE)version, &dwSize) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return TRUE;
            }
            RegCloseKey(hKey);
        }
        // 尝试HKEY_LOCAL_MACHINE
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, regPaths[i], 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            if (RegQueryValueExW(hKey, L"version", NULL, NULL, (LPBYTE)version, &dwSize) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return TRUE;
            }
            RegCloseKey(hKey);
        }
    }
    
    // 备用方法：读取文件版本
    if (GetBrowserAppPath(type, exePath, _countof(exePath))) {
        VS_FIXEDFILEINFO* pFileInfo;
        DWORD dwHandle;
        UINT dwLen = GetFileVersionInfoSizeW(exePath, &dwHandle);
        
        if (dwLen > 0) {
            BYTE* pVersionInfo = (BYTE*)malloc(dwLen);
            if (GetFileVersionInfoW(exePath, dwHandle, dwLen, pVersionInfo)) {
                if (VerQueryValueW(pVersionInfo, L"\\", (LPVOID*)&pFileInfo, &dwLen)) {
                    swprintf_s(version, size, L"%d.%d.%d.%d",
                        HIWORD(pFileInfo->dwFileVersionMS),
                        LOWORD(pFileInfo->dwFileVersionMS),
                        HIWORD(pFileInfo->dwFileVersionLS),
                        LOWORD(pFileInfo->dwFileVersionLS));
                    free(pVersionInfo);
                    return TRUE;
                }
            }
            free(pVersionInfo);
        }
    }
    
    wcscpy_s(version, size, L"Unknown");
    return FALSE;
}

// 获取浏览器应用路径
BOOL GetBrowserAppPath(BrowserType type, WCHAR* path, int size)
{
    if (type == BROWSER_CHROME) {
        if (PathFileExistsW(CHROME_APP_PATH_64)) {
            wcscpy_s(path, size, CHROME_APP_PATH_64);
            PathAppendW(path, L"chrome.exe");
            return TRUE;
        } else if (PathFileExistsW(CHROME_APP_PATH_32)) {
            wcscpy_s(path, size, CHROME_APP_PATH_32);
            PathAppendW(path, L"chrome.exe");
            return TRUE;
        }
    } else if (type == BROWSER_EDGE) {
        if (PathFileExistsW(EDGE_APP_PATH_64)) {
            wcscpy_s(path, size, EDGE_APP_PATH_64);
            PathAppendW(path, L"msedge.exe");
            return TRUE;
        } else if (PathFileExistsW(EDGE_APP_PATH_32)) {
            wcscpy_s(path, size, EDGE_APP_PATH_32);
            PathAppendW(path, L"msedge.exe");
            return TRUE;
        }
    }
    return FALSE;
}

// 初始化浏览器信息
void InitializeBrowserInfo(BrowserInfo* chrome, BrowserInfo* edge)
{
    ZeroMemory(chrome, sizeof(BrowserInfo));
    ZeroMemory(edge, sizeof(BrowserInfo));
    
    chrome->type = BROWSER_CHROME;
    wcscpy_s(chrome->name, _countof(chrome->name), L"chrome");
    wcscpy_s(chrome->displayName, _countof(chrome->displayName), L"Google Chrome");
    
    edge->type = BROWSER_EDGE;
    wcscpy_s(edge->name, _countof(edge->name), L"edge");
    wcscpy_s(edge->displayName, _countof(edge->displayName), L"Microsoft Edge");
    
    IsBrowserInstalled(BROWSER_CHROME, chrome);
    IsBrowserInstalled(BROWSER_EDGE, edge);
}

// 刷新浏览器状态
void RefreshBrowserStatus(BrowserInfo* info)
{
    if (info->isInstalled) {
        // 检查应用程序目录是否是符号链接
        info->appMoved = IsJunction(info->appPath);
        
        // 检查用户数据是否是符号链接
        info->dataMoved = IsJunction(info->dataPath);
    }
}