#include "../../include/ui/main_window.h"
#include "../../include/ui/progress_dialog.h"
#include "../../include/core/file_mover.h"
#include "../../include/core/symlink.h"
#include "../../include/core/registry_fixer.h"
#include "../../include/core/shortcut_fixer.h"
#include "../../include/utils/process_utils.h"
#include "../../include/utils/disk_utils.h"

// 进度回调函数
static void ProgressCallback(int current, int total, LPCWSTR status)
{
    UpdateProgress(current, total, status);
}

// 执行搬家操作
static BOOL ExecuteMoveOperation(BrowserInfo* browser)
{
    const LanguageStrings* lang = GetCurrentLanguageStrings();
    
    // 1. 关闭浏览器
    if (g_config.closeBrowserBeforeMove) {
        UpdateProgress(1, 10, lang->closingBrowser);
        SetProgressDetail(L"");
        
        TerminateBrowserProcesses(browser->type);
        WaitForProcessExit(browser->exeName, 5000);
        
        Sleep(1000);
    }
    
    int step = 2;
    
    // 2. 搬家应用程序目录
    if (g_config.moveApp && !browser->appMoved) {
        UpdateProgress(step, 10, lang->movingFiles);
        SetProgressDetail(browser->appPath);
        
        // 创建目标目录
        CreateDirectoryW(browser->appTargetPath, NULL);
        
        // 移动文件
        if (!MoveDirectory(browser->appPath, browser->appTargetPath, ProgressCallback)) {
            MessageBoxW(g_hMainWindow, L"Failed to move application directory", L"Error", MB_OK | MB_ICONERROR);
            return FALSE;
        }
        
        // 创建符号链接
        UpdateProgress(step + 1, 10, lang->creatingSymlink);
        SetProgressDetail(browser->appPath);
        
        if (!CreateJunction(browser->appPath, browser->appTargetPath)) {
            MessageBoxW(g_hMainWindow, L"Failed to create symlink", L"Error", MB_OK | MB_ICONERROR);
            return FALSE;
        }
        
        browser->appMoved = TRUE;
        step += 2;
    }
    
    // 3. 搬家用户数据
    if (g_config.moveData && !browser->dataMoved) {
        UpdateProgress(step, 10, lang->movingFiles);
        SetProgressDetail(browser->dataPath);
        
        // 创建目标目录
        CreateDirectoryW(browser->dataTargetPath, NULL);
        
        // 移动文件
        if (!MoveDirectory(browser->dataPath, browser->dataTargetPath, ProgressCallback)) {
            MessageBoxW(g_hMainWindow, L"Failed to move user data", L"Error", MB_OK | MB_ICONERROR);
            return FALSE;
        }
        
        // 创建符号链接
        UpdateProgress(step + 1, 10, lang->creatingSymlink);
        SetProgressDetail(browser->dataPath);
        
        if (!CreateJunction(browser->dataPath, browser->dataTargetPath)) {
            MessageBoxW(g_hMainWindow, L"Failed to create symlink", L"Error", MB_OK | MB_ICONERROR);
            return FALSE;
        }
        
        browser->dataMoved = TRUE;
        step += 2;
    }
    
    // 4. 修复注册表
    if (g_config.fixShortcuts) {
        UpdateProgress(step, 10, lang->fixingRegistry);
        SetProgressDetail(L"");
        
        FixAllRegistryPaths(browser);
        step++;
    }
    
    // 5. 修复快捷方式
    if (g_config.fixShortcuts) {
        UpdateProgress(step, 10, lang->fixingShortcuts);
        SetProgressDetail(L"");
        
        FindAndFixShortcuts(browser);
        step++;
    }
    
    // 6. 完成
    UpdateProgress(10, 10, lang->completed);
    SetProgressDetail(L"");
    
    return TRUE;
}

// 执行还原操作
static BOOL ExecuteRestoreOperation(BrowserInfo* browser)
{
    const LanguageStrings* lang = GetCurrentLanguageStrings();
    
    int step = 1;
    
    // 1. 还原应用程序目录
    if (browser->appMoved) {
        UpdateProgress(step, 5, L"Restoring application...");
        SetProgressDetail(browser->defaultAppPath);
        
        // 删除符号链接
        DeleteJunction(browser->appPath);
        
        // 移回文件
        MoveDirectory(browser->appTargetPath, browser->appPath, ProgressCallback);
        
        browser->appMoved = FALSE;
        step++;
    }
    
    // 2. 还原用户数据
    if (browser->dataMoved) {
        UpdateProgress(step, 5, L"Restoring user data...");
        SetProgressDetail(browser->defaultDataPath);
        
        // 删除符号链接
        DeleteJunction(browser->dataPath);
        
        // 移回文件
        MoveDirectory(browser->dataTargetPath, browser->dataPath, ProgressCallback);
        
        browser->dataMoved = FALSE;
        step++;
    }
    
    UpdateProgress(5, 5, L"Restore completed!");
    return TRUE;
}

// 搬家按钮点击事件
void OnMoveButtonClicked()
{
    BrowserInfo* browser = &g_browsers[g_config.selectedBrowser];
    
    if (!browser->isInstalled) {
        const LanguageStrings* lang = GetCurrentLanguageStrings();
        MessageBoxW(g_hMainWindow, L"Browser is not installed", L"Error", MB_OK | MB_ICONWARNING);
        return;
    }
    
    // 检查磁盘空间
    ULONGLONG requiredSpace = 0;
    if (g_config.moveApp && !browser->appMoved) {
        requiredSpace += browser->appSize;
    }
    if (g_config.moveData && !browser->dataMoved) {
        requiredSpace += browser->dataSize;
    }
    
    if (requiredSpace > 0 && !HasEnoughDiskSpace(g_config.targetBasePath, requiredSpace)) {
        WCHAR msg[512];
        swprintf_s(msg, 512, L"Not enough disk space on target drive\nRequired: %llu MB", requiredSpace / (1024 * 1024));
        MessageBoxW(g_hMainWindow, msg, L"Error", MB_OK | MB_ICONERROR);
        return;
    }
    
    // 显示进度对话框
    CreateProgressDialog(g_hMainWindow);
    
    // 执行搬家
    BOOL success = ExecuteMoveOperation(browser);
    
    // 关闭进度对话框
    CloseProgressDialog();
    
    if (success) {
        UpdateUIState();
        MessageBoxW(g_hMainWindow, L"Browser moved successfully!", L"Success", MB_OK | MB_ICONINFORMATION);
    }
}

// 还原按钮点击事件
void OnRestoreButtonClicked()
{
    BrowserInfo* browser = &g_browsers[g_config.selectedBrowser];
    
    if (!browser->isInstalled || (!browser->appMoved && !browser->dataMoved)) {
        const LanguageStrings* lang = GetCurrentLanguageStrings();
        MessageBoxW(g_hMainWindow, L"Nothing to restore", L"Info", MB_OK | MB_ICONINFORMATION);
        return;
    }
    
    // 关闭浏览器
    if (g_config.closeBrowserBeforeMove) {
        TerminateBrowserProcesses(browser->type);
        WaitForProcessExit(browser->exeName, 5000);
    }
    
    // 显示进度对话框
    CreateProgressDialog(g_hMainWindow);
    
    // 执行还原
    BOOL success = ExecuteRestoreOperation(browser);
    
    // 关闭进度对话框
    CloseProgressDialog();
    
    if (success) {
        UpdateUIState();
        MessageBoxW(g_hMainWindow, L"Restore completed successfully!", L"Success", MB_OK | MB_ICONINFORMATION);
    }
}

// 路径浏览按钮点击事件
void OnPathBrowseClicked(HWND hEdit, BOOL isTarget, BOOL isApp)
{
    BrowserInfo* browser = &g_browsers[g_config.selectedBrowser];
    WCHAR selectedPath[MAX_PATH];
    LPCWSTR title;
    
    // 设置初始路径
    if (isTarget) {
        wcscpy_s(selectedPath, MAX_PATH, g_config.targetBasePath);
        title = isApp ? L"Select Application Target Folder" : L"Select Data Target Folder";
    } else {
        wcscpy_s(selectedPath, MAX_PATH, isApp ? browser->appPath : browser->dataPath);
        title = isApp ? L"Select Application Source Folder" : L"Select Data Source Folder";
    }
    
    // 打开文件夹选择对话框
    if (BrowseForFolder(g_hMainWindow, title, selectedPath, MAX_PATH)) {
        // 更新编辑框
        SetWindowTextW(hEdit, selectedPath);
        
        // 如果是目标路径，更新配置
        if (isTarget) {
            wcscpy_s(g_config.targetBasePath, MAX_PATH, selectedPath);
            UpdateUIState();
        }
    }
}