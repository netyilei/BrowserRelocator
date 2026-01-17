#include "../../include/core/registry_fixer.h"
#include "../../include/utils/string_utils.h"

typedef struct {
    HKEY hRoot;
    LPCWSTR subKey;
    LPCWSTR valueName;
    LPCWSTR oldPath;
    LPCWSTR newPath;
} RegistryEntry;

// Chrome 注册表路径映射
static RegistryEntry chromeRegEntries[] = {
    {HKEY_LOCAL_MACHINE, L"SOFTWARE\\Google\\Update\\Clients\\{8A69D345-D564-463C-AFF1-A69D9E530F96}", L"InstallLocation", NULL, NULL},
    {HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Google Chrome", L"InstallLocation", NULL, NULL},
    {HKEY_LOCAL_MACHINE, L"SOFTWARE\\Clients\\StartMenuInternet\\Google Chrome\\DefaultIcon", NULL, NULL, NULL},
    {HKEY_LOCAL_MACHINE, L"SOFTWARE\\Clients\\StartMenuInternet\\Google Chrome\\shell\\open\\command", NULL, NULL, NULL},
    {HKEY_CLASSES_ROOT, L"ChromeHTML\\shell\\open\\command", NULL, NULL, NULL},
    {HKEY_LOCAL_MACHINE, L"SOFTWARE\\Google\\Chrome", L"version", NULL, NULL},
};

// Edge 注册表路径映射
static RegistryEntry edgeRegEntries[] = {
    {HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\EdgeUpdate\\Clients\\{56EB18F8-B008-4CBD-B6D2-8C97FE7E9062}", L"InstallLocation", NULL, NULL},
    {HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Microsoft\\EdgeUpdate\\Clients\\{56EB18F8-B008-4CBD-B6D2-8C97FE7E9062}", L"InstallLocation", NULL, NULL},
    {HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Microsoft Edge", L"InstallLocation", NULL, NULL},
    {HKEY_CLASSES_ROOT, L"MSEdgeHTM\\shell\\open\\command", NULL, NULL, NULL},
    {HKEY_CLASSES_ROOT, L"MSEdgeHTM\\DefaultIcon", NULL, NULL, NULL},
};

// 修复单个注册表项
BOOL FixRegistryEntry(RegistryEntry* entry)
{
    HKEY hKey;
    DWORD type;
    WCHAR data[MAX_PATH * 4];
    DWORD dataSize = sizeof(data);
    
    // 打开注册表键
    LONG result = RegOpenKeyExW(entry->hRoot, entry->subKey, 0, KEY_READ | KEY_WRITE, &hKey);
    if (result != ERROR_SUCCESS) {
        return FALSE;
    }
    
    // 如果指定了值名，读取该值
    if (entry->valueName != NULL) {
        result = RegQueryValueExW(hKey, entry->valueName, NULL, &type, (LPBYTE)data, &dataSize);
        if (result == ERROR_SUCCESS) {
            WCHAR newData[MAX_PATH * 4];
            if (ReplacePathInString(data, entry->oldPath, entry->newPath, newData, _countof(newData))) {
                RegSetValueExW(hKey, entry->valueName, 0, type, (LPBYTE)newData, (wcslen(newData) + 1) * sizeof(WCHAR));
                RegCloseKey(hKey);
                return TRUE;
            }
        }
    } else {
        // 遍历所有值
        DWORD valueCount = 0;
        DWORD maxNameLen = 0;
        RegQueryInfoKeyW(hKey, NULL, NULL, NULL, NULL, NULL, NULL, &valueCount, &maxNameLen, NULL, NULL, NULL);
        
        WCHAR* valueName = (WCHAR*)malloc((maxNameLen + 1) * sizeof(WCHAR));
        DWORD i;
        BOOL fixed = FALSE;
        
        for (i = 0; i < valueCount; i++) {
            DWORD nameLen = maxNameLen + 1;
            result = RegEnumValueW(hKey, i, valueName, &nameLen, NULL, &type, (LPBYTE)data, &dataSize);
            if (result == ERROR_SUCCESS) {
                WCHAR newData[MAX_PATH * 4];
                if (ReplacePathInString(data, entry->oldPath, entry->newPath, newData, _countof(newData))) {
                    RegSetValueExW(hKey, valueName, 0, type, (LPBYTE)newData, (wcslen(newData) + 1) * sizeof(WCHAR));
                    fixed = TRUE;
                }
            }
            dataSize = sizeof(data);
        }
        
        free(valueName);
        RegCloseKey(hKey);
        return fixed;
    }
    
    RegCloseKey(hKey);
    return FALSE;
}

// 修复 Chrome 注册表
BOOL FixChromeRegistry(LPCWSTR oldPath, LPCWSTR newPath)
{
    int count = sizeof(chromeRegEntries) / sizeof(RegistryEntry);
    
    for (int i = 0; i < count; i++) {
        chromeRegEntries[i].oldPath = oldPath;
        chromeRegEntries[i].newPath = newPath;
        FixRegistryEntry(&chromeRegEntries[i]);
    }
    
    return TRUE;
}

// 修复 Edge 注册表
BOOL FixEdgeRegistry(LPCWSTR oldPath, LPCWSTR newPath)
{
    int count = sizeof(edgeRegEntries) / sizeof(RegistryEntry);
    
    for (int i = 0; i < count; i++) {
        edgeRegEntries[i].oldPath = oldPath;
        edgeRegEntries[i].newPath = newPath;
        FixRegistryEntry(&edgeRegEntries[i]);
    }
    
    return TRUE;
}

// 修复浏览器注册表
BOOL FixBrowserRegistry(BrowserType type, LPCWSTR oldPath, LPCWSTR newPath)
{
    if (type == BROWSER_CHROME) {
        return FixChromeRegistry(oldPath, newPath);
    } else if (type == BROWSER_EDGE) {
        return FixEdgeRegistry(oldPath, newPath);
    }
    return FALSE;
}

// 修复所有注册表路径
BOOL FixAllRegistryPaths(BrowserInfo* browser)
{
    WCHAR oldPath[MAX_PATH];
    WCHAR newPath[MAX_PATH];
    
    // 修复应用程序路径
    if (browser->appMoved) {
        wcscpy_s(oldPath, MAX_PATH, browser->defaultAppPath);
        wcscpy_s(newPath, MAX_PATH, browser->appTargetPath);
        FixBrowserRegistry(browser->type, oldPath, newPath);
    }
    
    // 修复用户数据路径
    if (browser->dataMoved) {
        wcscpy_s(oldPath, MAX_PATH, browser->defaultDataPath);
        wcscpy_s(newPath, MAX_PATH, browser->dataTargetPath);
        // 用户数据通常不需要修改注册表，但某些浏览器可能有
        FixBrowserRegistry(browser->type, oldPath, newPath);
    }
    
    return TRUE;
}

// 备份注册表
BOOL BackupRegistry(BrowserType type)
{
    // TODO: 实现注册表备份功能
    return TRUE;
}

// 还原注册表备份
BOOL RestoreRegistryBackup(BrowserType type)
{
    // TODO: 实现注册表还原功能
    return TRUE;
}