#include "../../include/core/shortcut_fixer.h"
#include "../../include/utils/string_utils.h"
#include <shobjidl.h>
#include <objbase.h>

// 修复快捷方式路径
BOOL FixShortcutPath(LPCWSTR shortcutPath, LPCWSTR oldPath, LPCWSTR newPath)
{
    IShellLinkW* psl = NULL;
    IPersistFile* ppf = NULL;
    HRESULT hr;
    BOOL result = FALSE;
    
    hr = CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, 
                          &IID_IShellLinkW, (LPVOID*)&psl);
    
    if (SUCCEEDED(hr)) {
        hr = psl->lpVtbl->QueryInterface(psl, &IID_IPersistFile, (LPVOID*)&ppf);
        
        if (SUCCEEDED(hr)) {
            hr = ppf->lpVtbl->Load(ppf, shortcutPath, STGM_READWRITE);
            
            if (SUCCEEDED(hr)) {
                WCHAR target[MAX_PATH * 2];
                WCHAR args[MAX_PATH];
                WCHAR workingDir[MAX_PATH];
                
                psl->lpVtbl->GetPath(psl, target, MAX_PATH, NULL, 0);
                psl->lpVtbl->GetArguments(psl, args, MAX_PATH);
                psl->lpVtbl->GetWorkingDirectory(psl, workingDir, MAX_PATH);
                
                // 替换目标路径
                if (wcsstr(target, oldPath) != NULL) {
                    WCHAR newTarget[MAX_PATH * 2];
                    if (ReplacePathInString(target, oldPath, newPath, newTarget, MAX_PATH * 2)) {
                        psl->lpVtbl->SetPath(psl, newTarget);
                        result = TRUE;
                    }
                }
                
                // 替换工作目录
                if (wcsstr(workingDir, oldPath) != NULL) {
                    WCHAR newWorkingDir[MAX_PATH * 2];
                    if (ReplacePathInString(workingDir, oldPath, newPath, newWorkingDir, MAX_PATH * 2)) {
                        psl->lpVtbl->SetWorkingDirectory(psl, newWorkingDir);
                        result = TRUE;
                    }
                }
                
                if (result) {
                    ppf->lpVtbl->Save(ppf, shortcutPath, TRUE);
                }
            }
        }
    }
    
    if (ppf) ppf->lpVtbl->Release(ppf);
    if (psl) psl->lpVtbl->Release(psl);
    
    return result;
}

// 在指定文件夹中查找并修复快捷方式
BOOL FindShortcutsInFolder(LPCWSTR folderPath, BrowserType type, LPCWSTR oldPath, LPCWSTR newPath)
{
    WIN32_FIND_DATAW findData;
    WCHAR searchPath[MAX_PATH];
    BOOL foundAny = FALSE;
    
    wcscpy_s(searchPath, _countof(searchPath), folderPath);
    wcscat_s(searchPath, _countof(searchPath), L"\\*.lnk");
    
    HANDLE hFind = FindFirstFileW(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    
    do {
        WCHAR shortcutPath[MAX_PATH];
        wcscpy_s(shortcutPath, _countof(shortcutPath), folderPath);
        wcscat_s(shortcutPath, _countof(shortcutPath), L"\\");
        wcscat_s(shortcutPath, _countof(shortcutPath), findData.cFileName);
        
        if (FixShortcutPath(shortcutPath, oldPath, newPath)) {
            foundAny = TRUE;
        }
    } while (FindNextFileW(hFind, &findData));
    
    FindClose(hFind);
    return foundAny;
}

// 修复所有相关快捷方式
BOOL FixAllShortcuts(BrowserType type, LPCWSTR oldPath, LPCWSTR newPath)
{
    WCHAR path[MAX_PATH];
    LPCWSTR shortcutName;
    
    // 桌面快捷方式 (当前用户)
    if (type == BROWSER_CHROME) {
        shortcutName = L"Google Chrome.lnk";
    } else {
        shortcutName = L"Microsoft Edge.lnk";
    }
    
    if (SHGetFolderPathW(NULL, CSIDL_DESKTOP, NULL, 0, path) == S_OK) {
        wcscat_s(path, MAX_PATH, L"\\");
        wcscat_s(path, MAX_PATH, shortcutName);
        FixShortcutPath(path, oldPath, newPath);
    }
    
    // 桌面快捷方式 (所有用户)
    if (SHGetFolderPathW(NULL, CSIDL_COMMON_DESKTOPDIRECTORY, NULL, 0, path) == S_OK) {
        wcscat_s(path, MAX_PATH, L"\\");
        wcscat_s(path, MAX_PATH, shortcutName);
        FixShortcutPath(path, oldPath, newPath);
    }
    
    // 开始菜单快捷方式 (当前用户)
    if (SHGetFolderPathW(NULL, CSIDL_PROGRAMS, NULL, 0, path) == S_OK) {
        FindShortcutsInFolder(path, type, oldPath, newPath);
    }
    
    // 开始菜单快捷方式 (所有用户)
    if (SHGetFolderPathW(NULL, CSIDL_COMMON_PROGRAMS, NULL, 0, path) == S_OK) {
        FindShortcutsInFolder(path, type, oldPath, newPath);
    }
    
    // 任务栏快捷方式（Windows 10/11）
    if (SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, path) == S_OK) {
        wcscat_s(path, MAX_PATH, L"\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar");
        FindShortcutsInFolder(path, type, oldPath, newPath);
    }
    
    return TRUE;
}

// 查找并修复所有相关快捷方式
BOOL FindAndFixShortcuts(BrowserInfo* browser)
{
    WCHAR oldPath[MAX_PATH];
    WCHAR newPath[MAX_PATH];
    
    // 修复应用程序快捷方式
    if (browser->appMoved) {
        wcscpy_s(oldPath, MAX_PATH, browser->defaultAppPath);
        wcscpy_s(newPath, MAX_PATH, browser->appTargetPath);
        FixAllShortcuts(browser->type, oldPath, newPath);
    }
    
    // 用户数据不需要修复快捷方式
    
    return TRUE;
}