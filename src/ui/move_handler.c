#include "../../include/ui/main_window.h"
#include "../../include/ui/progress_dialog.h"
#include "../../include/ui/path_selector.h"
#include "../../include/core/file_mover.h"
#include "../../include/core/symlink.h"
#include "../../include/core/registry_fixer.h"
#include "../../include/core/shortcut_fixer.h"
#include "../../include/utils/process_utils.h"
#include "../../include/utils/disk_utils.h"

// 检查管理员权限
static BOOL IsAdmin()
{
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;
    SID_IDENTIFIER_AUTHORITY authority = SECURITY_NT_AUTHORITY;
    
    if (AllocateAndInitializeSid(&authority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup)) {
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }
    return isAdmin;
}

// 递归创建目录（如果不存在）
static BOOL EnsureDirectoryExists(LPCWSTR path)
{
    if (PathFileExistsW(path)) {
        return TRUE;  // 已存在
    }
    
    // 使用SHCreateDirectoryExW递归创建多级目录
    int ret = SHCreateDirectoryExW(NULL, path, NULL);
    return (ret == ERROR_SUCCESS || ret == ERROR_ALREADY_EXISTS);
}

// 进度回调函数
static void MoveProgressCallback(int current, int total, LPCWSTR status)
{
    SetStatusProgress(current, total);
    SetStatusText(status);
    // 处理消息以保持UI响应
    MSG msg;
    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

// ============== 共用函数 ==============

// 操作前准备：关闭浏览器进程
static BOOL PrepareOperation(BrowserInfo* browser, const LanguageStrings* lang, int totalSteps)
{
    // 显示进度条
    ShowStatusProgress(TRUE);
    SetStatusProgress(0, totalSteps);
    SetStatusText(lang->preparing);
    
    // 禁用按钮
    EnableWindow(g_ui.hMoveButton, FALSE);
    EnableWindow(g_ui.hRestoreButton, FALSE);
    
    // 关闭浏览器
    SetStatusProgress(1, totalSteps);
    SetStatusText(lang->closingBrowser);
    
    // 终止所有相关进程（会记录服务状态）
    TerminateBrowserProcesses(browser->type);
    
    // 等待所有进程退出（最多15秒）
    if (!WaitForBrowserExit(browser->type, 15000)) {
        MessageBoxW(g_hMainWindow, L"无法关闭所有浏览器进程，请手动关闭后重试", lang->errorTitle, MB_OK | MB_ICONWARNING);
        return FALSE;
    }
    
    Sleep(3000);  // 额外等待3秒确保文件句柄释放
    
    // 再次强制终止所有Edge/Chrome进程（确保彻底关闭）
    TerminateBrowserProcesses(browser->type);
    Sleep(2000);  // 再等待2秒
    
    return TRUE;
}

// 操作后清理：恢复服务和按钮
static void FinishOperation(BrowserInfo* browser, BOOL success, const LanguageStrings* lang, LPCWSTR successMsg)
{
    MessageBoxW(g_hMainWindow, L"[DEBUG] 进入 FinishOperation", L"DEBUG", MB_OK);
    
    // 恢复按钮
    EnableWindow(g_ui.hMoveButton, TRUE);
    EnableWindow(g_ui.hRestoreButton, TRUE);
    MessageBoxW(g_hMainWindow, L"[DEBUG] 按钮已恢复", L"DEBUG", MB_OK);
    
    // 恢复浏览器服务（恢复到操作前的状态）
    MessageBoxW(g_hMainWindow, L"[DEBUG] 开始恢复浏览器服务...", L"DEBUG", MB_OK);
    RestoreBrowserServices(browser->type);
    MessageBoxW(g_hMainWindow, L"[DEBUG] 浏览器服务已恢复", L"DEBUG", MB_OK);
    
    if (success) {
        MessageBoxW(g_hMainWindow, L"[DEBUG] 成功分支，开始更新UI状态...", L"DEBUG", MB_OK);
        UpdateUIState();
        MessageBoxW(g_hMainWindow, L"[DEBUG] UI状态已更新", L"DEBUG", MB_OK);
        Sleep(1000);
        ResetStatusBar();
        MessageBoxW(g_hMainWindow, L"[DEBUG] 准备显示成功消息", L"DEBUG", MB_OK);
        MessageBoxW(g_hMainWindow, successMsg, lang->successTitle, MB_OK | MB_ICONINFORMATION);
    } else {
        ResetStatusBar();
    }
    MessageBoxW(g_hMainWindow, L"[DEBUG] FinishOperation 完成", L"DEBUG", MB_OK);
}

// 执行搬家操作
static BOOL ExecuteMoveOperation(BrowserInfo* browser)
{
    const LanguageStrings* lang = GetCurrentLanguageStrings();
    WCHAR debugMsg[1024];
    
    // DEBUG: 显示路径信息和权限状态
    swprintf_s(debugMsg, 1024, 
        L"[DEBUG] 搬家信息:\n\n"
        L"管理员权限: %s\n\n"
        L"源应用路径: %s\n"
        L"目标应用路径: %s\n\n"
        L"源数据路径: %s\n"
        L"目标数据路径: %s\n\n"
        L"moveApp: %d, moveData: %d",
        IsAdmin() ? L"是" : L"否",
        browser->appPath, browser->appTargetPath,
        browser->dataPath, browser->dataTargetPath,
        g_config.moveApp, g_config.moveData);
    MessageBoxW(g_hMainWindow, debugMsg, L"DEBUG - 路径信息", MB_OK | MB_ICONINFORMATION);
    
    int step = 2;
    
    // 2. 搬家应用程序目录
    if (g_config.moveApp && !browser->appMoved) {
        SetStatusProgress(step, 10);
        SetStatusText(lang->movingFiles);
        
        // DEBUG: 检查源目录是否存在
        if (!PathFileExistsW(browser->appPath)) {
            swprintf_s(debugMsg, 1024, L"[DEBUG] 源应用目录不存在:\n%s", browser->appPath);
            MessageBoxW(g_hMainWindow, debugMsg, L"DEBUG - 错误", MB_OK | MB_ICONERROR);
            return FALSE;
        }
        
        // 创建目标目录（递归创建所有父目录）
        if (!EnsureDirectoryExists(browser->appTargetPath)) {
            DWORD err = GetLastError();
            swprintf_s(debugMsg, 1024, L"[DEBUG] 无法创建目标目录:\n%s\n错误码: %lu", browser->appTargetPath, err);
            MessageBoxW(g_hMainWindow, debugMsg, L"DEBUG - 错误", MB_OK | MB_ICONERROR);
            return FALSE;
        }
        
        // 移动文件（改为：复制 -> 改名备份 -> 创建链接）
        MessageBoxW(g_hMainWindow, L"[DEBUG] 开始复制应用程序目录...", L"DEBUG", MB_OK);
        
        // 1. 复制目录
        BOOL copySuccess = FALSE;
        for (int retry = 0; retry < 3; retry++) {
            if (retry > 0) {
                TerminateBrowserProcesses(browser->type);
                Sleep(2000);
            }
            if (CopyDirectory(browser->appPath, browser->appTargetPath, MoveProgressCallback)) {
                copySuccess = TRUE;
                break;
            }
        }
        
        if (!copySuccess) {
            DWORD err = GetLastError();
            swprintf_s(debugMsg, 1024, L"复制应用程序目录失败!\n错误码: %lu", err);
            MessageBoxW(g_hMainWindow, debugMsg, lang->errorTitle, MB_OK | MB_ICONERROR);
            return FALSE;
        }

        // 2. 授权并重命名（仅针对文件夹本身，避开内部受限对象）
        WCHAR appPathBak[MAX_PATH];
        swprintf_s(appPathBak, MAX_PATH, L"%s_bak", browser->appPath);
        
        if (PathFileExistsW(appPathBak)) {
            DeleteDirectory(appPathBak, NULL);
        }
        
        // 尝试夺取文件夹本身（非递归）的所有权和权限
        WCHAR parentPath[MAX_PATH];
        wcscpy_s(parentPath, MAX_PATH, browser->appPath);
        PathRemoveFileSpecW(parentPath);
        
        GrantFullControl(parentPath);  // 授权父目录，允许在其中重命名
        GrantFullControl(browser->appPath); // 授权目标目录本身
        
        BOOL renameSuccess = MoveFileW(browser->appPath, appPathBak);
        
        // 如果 MoveFileW 失败，尝试更高级的 shell 移动方式
        if (!renameSuccess) {
            SHFILEOPSTRUCTW fileOp = {0};
            fileOp.hwnd = g_hMainWindow;
            fileOp.wFunc = FO_MOVE;
            WCHAR fromPath[MAX_PATH + 1] = {0};
            WCHAR toPath[MAX_PATH + 1] = {0};
            wcscpy_s(fromPath, MAX_PATH, browser->appPath);
            wcscpy_s(toPath, MAX_PATH, appPathBak);
            fileOp.pFrom = fromPath;
            fileOp.pTo = toPath;
            fileOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;
            
            if (SHFileOperationW(&fileOp) == 0) {
                renameSuccess = TRUE;
            }
        }
        
        if (!renameSuccess) {
            DWORD err = GetLastError();
            if (err == ERROR_ACCESS_DENIED) {
                swprintf_s(debugMsg, 1024, 
                    L"无法处理 Edge 应用程序目录（访问被拒绝）。\n\n"
                    L"原因：Windows 系统保护了该目录，需要 SYSTEM 权限。\n"
                    L"解决办法：\n"
                    L"1. 请手动将此文件夹：\n   %s\n"
                    L"   重命名为：\n   %s\n"
                    L"2. 重命名成功后，再次回到本程序点击“开始搬家”。\n\n"
                    L"注意：如果无法手动重命名，请尝试右键文件夹 -> 属性 -> 安全 -> 高级 -> 更改所有者为 Administrators。", 
                    browser->appPath, appPathBak);
                MessageBoxW(g_hMainWindow, debugMsg, L"需要手动干预", MB_OK | MB_ICONEXCLAMATION);
                return FALSE;
            }
            swprintf_s(debugMsg, 1024, L"无法备份原目录（改名失败）!\n错误码: %lu", err);
            MessageBoxW(g_hMainWindow, debugMsg, lang->errorTitle, MB_OK | MB_ICONERROR);
            return FALSE;
        }
        
        MessageBoxW(g_hMainWindow, L"[DEBUG] 原应用目录已改名为 _bak 备份", L"DEBUG", MB_OK);

        // 3. 创建符号链接
        MessageBoxW(g_hMainWindow, L"[DEBUG] 准备为应用程序创建Junction...", L"DEBUG", MB_OK);
        SetStatusProgress(step + 1, 10);
        SetStatusText(lang->creatingSymlink);
        
        if (!CreateJunction(browser->appPath, browser->appTargetPath)) {
            DWORD err = GetLastError();
            // 失败了，尝试把备份改名回来
            MoveFileW(appPathBak, browser->appPath);
            
            swprintf_s(debugMsg, 1024, L"创建Junction失败!\n错误码: %lu\n原始目录已恢复。", err);
            MessageBoxW(g_hMainWindow, debugMsg, L"DEBUG - CreateJunction失败", MB_OK | MB_ICONERROR);
            return FALSE;
        }
        
        browser->appMoved = TRUE;
        step += 2;
    }
    
    // 3. 搬家用户数据
    if (g_config.moveData && !browser->dataMoved) {
        SetStatusProgress(step, 10);
        SetStatusText(lang->movingFiles);
        
        // DEBUG: 检查源目录是否存在
        if (!PathFileExistsW(browser->dataPath)) {
            swprintf_s(debugMsg, 1024, L"[DEBUG] 源数据目录不存在:\n%s", browser->dataPath);
            MessageBoxW(g_hMainWindow, debugMsg, L"DEBUG - 错误", MB_OK | MB_ICONERROR);
            return FALSE;
        }
        
        // 创建目标目录（递归创建所有父目录）
        if (!EnsureDirectoryExists(browser->dataTargetPath)) {
            DWORD err = GetLastError();
            swprintf_s(debugMsg, 1024, L"[DEBUG] 无法创建目标目录:\n%s\n错误码: %lu", browser->dataTargetPath, err);
            MessageBoxW(g_hMainWindow, debugMsg, L"DEBUG - 错误", MB_OK | MB_ICONERROR);
            return FALSE;
        }
        
        // 移动文件（改为：复制 -> 改名备份 -> 创建链接）
        MessageBoxW(g_hMainWindow, L"[DEBUG] 开始复制用户数据目录...", L"DEBUG", MB_OK);
        
        // 1. 复制目录
        BOOL copySuccess = FALSE;
        for (int retry = 0; retry < 3; retry++) {
            if (retry > 0) {
                TerminateBrowserProcesses(browser->type);
                Sleep(2000);
            }
            if (CopyDirectory(browser->dataPath, browser->dataTargetPath, MoveProgressCallback)) {
                copySuccess = TRUE;
                break;
            }
        }
        
        if (!copySuccess) {
            DWORD err = GetLastError();
            swprintf_s(debugMsg, 1024, L"复制用户数据失败!\n错误码: %lu", err);
            MessageBoxW(g_hMainWindow, debugMsg, lang->errorTitle, MB_OK | MB_ICONERROR);
            return FALSE;
        }

        // 2. 将原目录改名为备份目录
        WCHAR dataPathBak[MAX_PATH];
        swprintf_s(dataPathBak, MAX_PATH, L"%s_bak", browser->dataPath);
        
        // 如果备份目录已存在，先删除它
        if (PathFileExistsW(dataPathBak)) {
            DeleteDirectory(dataPathBak, NULL);
        }
        
        if (!MoveFileW(browser->dataPath, dataPathBak)) {
            DWORD err = GetLastError();
            swprintf_s(debugMsg, 1024, L"无法备份原数据目录（改名失败）!\n路径: %s\n错误码: %lu", browser->dataPath, err);
            MessageBoxW(g_hMainWindow, debugMsg, lang->errorTitle, MB_OK | MB_ICONERROR);
            return FALSE;
        }
        
        MessageBoxW(g_hMainWindow, L"[DEBUG] 原数据目录已改名为 _bak 备份", L"DEBUG", MB_OK);

        // 3. 创建符号链接
        MessageBoxW(g_hMainWindow, L"[DEBUG] 开始为用户数据创建Junction...", L"DEBUG", MB_OK);
        SetStatusProgress(step + 1, 10);
        SetStatusText(lang->creatingSymlink);
        
        if (!CreateJunction(browser->dataPath, browser->dataTargetPath)) {
            DWORD err = GetLastError();
            // 失败了，尝试把备份改名回来
            MoveFileW(dataPathBak, browser->dataPath);
            
            swprintf_s(debugMsg, 1024, L"创建Junction失败!\n错误码: %lu\n原始目录已恢复。", err);
            MessageBoxW(g_hMainWindow, debugMsg, L"DEBUG - CreateJunction失败", MB_OK | MB_ICONERROR);
            return FALSE;
        }
        MessageBoxW(g_hMainWindow, L"[DEBUG] 用户数据Junction创建成功!", L"DEBUG", MB_OK);
        
        browser->dataMoved = TRUE;
        step += 2;
    }
    
    MessageBoxW(g_hMainWindow, L"[DEBUG] 文件移动阶段完成，开始修复注册表...", L"DEBUG", MB_OK);
    
    // 4. 修复注册表
    if (g_config.fixShortcuts) {
        MessageBoxW(g_hMainWindow, L"[DEBUG] 开始修复注册表...", L"DEBUG", MB_OK);
        SetStatusProgress(step, 10);
        SetStatusText(lang->fixingRegistry);
        
        FixAllRegistryPaths(browser);
        MessageBoxW(g_hMainWindow, L"[DEBUG] 注册表修复完成!", L"DEBUG", MB_OK);
        step++;
    }
    
    // 5. 修复快捷方式
    if (g_config.fixShortcuts) {
        MessageBoxW(g_hMainWindow, L"[DEBUG] 开始修复快捷方式...", L"DEBUG", MB_OK);
        SetStatusProgress(step, 10);
        SetStatusText(lang->fixingShortcuts);
        
        FindAndFixShortcuts(browser);
        MessageBoxW(g_hMainWindow, L"[DEBUG] 快捷方式修复完成!", L"DEBUG", MB_OK);
        step++;
    }
    
    // 6. 清理备份目录（仅在所有核心步骤完成后）
    MessageBoxW(g_hMainWindow, L"[DEBUG] 所有修复步骤已完成，准备清理备份目录...", L"DEBUG", MB_OK);
    
    if (g_config.moveApp) {
        WCHAR appPathBak[MAX_PATH];
        swprintf_s(appPathBak, MAX_PATH, L"%s_bak", browser->appPath);
        if (PathFileExistsW(appPathBak)) {
            DeleteDirectory(appPathBak, NULL);
        }
    }
    
    if (g_config.moveData) {
        WCHAR dataPathBak[MAX_PATH];
        swprintf_s(dataPathBak, MAX_PATH, L"%s_bak", browser->dataPath);
        if (PathFileExistsW(dataPathBak)) {
            DeleteDirectory(dataPathBak, NULL);
        }
    }
    
    // 7. 完成
    MessageBoxW(g_hMainWindow, L"[DEBUG] 所有步骤完成，准备显示成功消息...", L"DEBUG", MB_OK);
    SetStatusProgress(10, 10);
    SetStatusText(lang->completed);
    
    MessageBoxW(g_hMainWindow, L"[DEBUG] ExecuteMoveOperation 返回 TRUE", L"DEBUG", MB_OK);
    return TRUE;
}

// 执行还原操作
static BOOL ExecuteRestoreOperation(BrowserInfo* browser)
{
    const LanguageStrings* lang = GetCurrentLanguageStrings();
    WCHAR debugMsg[1024];
    int step = 1;
    
    // 1. 还原应用程序目录
    if (browser->appMoved) {
        SetStatusProgress(step, 6);
        SetStatusText(lang->restoringApp);
        
        // 删除符号链接
        if (!DeleteJunction(browser->appPath)) {
            // Junction可能不存在，继续尝试
        }
        
        // 移回文件
        if (!MoveDirectory(browser->appTargetPath, browser->appPath, MoveProgressCallback)) {
            DWORD err = GetLastError();
            swprintf_s(debugMsg, 1024, 
                L"[DEBUG] 还原应用程序目录失败!\n\n"
                L"源: %s\n"
                L"目标: %s\n"
                L"错误码: %lu",
                browser->appTargetPath, browser->appPath, err);
            MessageBoxW(g_hMainWindow, debugMsg, L"DEBUG - 还原失败", MB_OK | MB_ICONERROR);
            return FALSE;
        }
        
        browser->appMoved = FALSE;
        step++;
    }
    
    // 2. 还原用户数据
    if (browser->dataMoved) {
        SetStatusProgress(step, 6);
        SetStatusText(lang->restoringData);
        
        // 删除符号链接
        if (!DeleteJunction(browser->dataPath)) {
            // Junction可能不存在，继续尝试
        }
        
        // 移回文件
        if (!MoveDirectory(browser->dataTargetPath, browser->dataPath, MoveProgressCallback)) {
            DWORD err = GetLastError();
            swprintf_s(debugMsg, 1024, 
                L"[DEBUG] 还原用户数据目录失败!\n\n"
                L"源: %s\n"
                L"目标: %s\n"
                L"错误码: %lu",
                browser->dataTargetPath, browser->dataPath, err);
            MessageBoxW(g_hMainWindow, debugMsg, L"DEBUG - 还原失败", MB_OK | MB_ICONERROR);
            return FALSE;
        }
        
        browser->dataMoved = FALSE;
        step++;
    }
    
    // 3. 还原注册表（将新路径改回原路径）
    SetStatusProgress(step, 6);
    SetStatusText(lang->fixingRegistry);
    
    // 应用程序路径注册表还原
    if (wcslen(browser->appTargetPath) > 0) {
        FixBrowserRegistry(browser->type, browser->appTargetPath, browser->defaultAppPath);
    }
    // 用户数据路径注册表还原
    if (wcslen(browser->dataTargetPath) > 0) {
        FixBrowserRegistry(browser->type, browser->dataTargetPath, browser->defaultDataPath);
    }
    
    SetStatusProgress(6, 6);
    SetStatusText(lang->restoreCompleted);
    return TRUE;
}

// 搬家按钮点击事件
void OnMoveButtonClicked()
{
    const LanguageStrings* lang = GetCurrentLanguageStrings();
    BrowserInfo* browser = &g_browsers[g_config.selectedBrowser];
    
    if (!browser->isInstalled) {
        MessageBoxW(g_hMainWindow, lang->notInstalled, L"Error", MB_OK | MB_ICONWARNING);
        return;
    }
    
    // 从编辑框获取最新的目标路径
    GetWindowTextW(g_ui.pathSelector.hAppTargetPathEdit, browser->appTargetPath, MAX_PATH);
    GetWindowTextW(g_ui.pathSelector.hDataTargetPathEdit, browser->dataTargetPath, MAX_PATH);
    
    // 弹出确认框：提示需要关闭浏览器
    int result = MessageBoxW(g_hMainWindow, 
        lang->confirmMoveMessage,
        lang->confirmTitle, MB_YESNO | MB_ICONQUESTION);
    
    if (result != IDYES) {
        return;  // 用户取消，终止执行
    }
    
    // 检查磁盘空间（使用应用程序目标路径的磁盘）
    ULONGLONG requiredSpace = 0;
    WCHAR checkPath[MAX_PATH] = {0};
    
    if (g_config.moveApp && !browser->appMoved) {
        requiredSpace += browser->appSize;
        wcscpy_s(checkPath, MAX_PATH, browser->appTargetPath);
    }
    if (g_config.moveData && !browser->dataMoved) {
        requiredSpace += browser->dataSize;
        if (wcslen(checkPath) == 0) {
            wcscpy_s(checkPath, MAX_PATH, browser->dataTargetPath);
        }
    }
    
    if (requiredSpace > 0 && wcslen(checkPath) >= 2) {
        WCHAR drivePath[4] = {checkPath[0], checkPath[1], L'\\', L'\0'};
        if (!HasEnoughDiskSpace(drivePath, requiredSpace)) {
            MessageBoxW(g_hMainWindow, lang->diskSpaceInsufficient, lang->errorTitle, MB_OK | MB_ICONERROR);
            return;
        }
    }
    
    // 操作前准备（共用）
    if (!PrepareOperation(browser, lang, 10)) {
        FinishOperation(browser, FALSE, lang, NULL);
        return;
    }
    
    // 执行搬家
    BOOL success = ExecuteMoveOperation(browser);
    
    // 操作后清理（共用）
    FinishOperation(browser, success, lang, lang->moveCompleted);
}

// 还原按钮点击事件
void OnRestoreButtonClicked()
{
    const LanguageStrings* lang = GetCurrentLanguageStrings();
    BrowserInfo* browser = &g_browsers[g_config.selectedBrowser];
    
    if (!browser->isInstalled || (!browser->appMoved && !browser->dataMoved)) {
        MessageBoxW(g_hMainWindow, lang->nothingToRestore, lang->infoTitle, MB_OK | MB_ICONINFORMATION);
        return;
    }
    
    // 检查原路径磁盘空间
    ULONGLONG requiredSpace = 0;
    WCHAR checkPath[MAX_PATH] = {0};
    
    if (browser->appMoved) {
        requiredSpace += browser->appSize;
        wcscpy_s(checkPath, MAX_PATH, browser->defaultAppPath);
    }
    if (browser->dataMoved) {
        requiredSpace += browser->dataSize;
        if (wcslen(checkPath) == 0) {
            wcscpy_s(checkPath, MAX_PATH, browser->defaultDataPath);
        }
    }
    
    if (requiredSpace > 0 && wcslen(checkPath) >= 2) {
        WCHAR drivePath[4] = {checkPath[0], checkPath[1], L'\\', L'\0'};
        if (!HasEnoughDiskSpace(drivePath, requiredSpace)) {
            MessageBoxW(g_hMainWindow, lang->diskSpaceInsufficient, lang->errorTitle, MB_OK | MB_ICONERROR);
            return;
        }
    }
    
    // 弹出确认框：提示需要关闭浏览器
    int result = MessageBoxW(g_hMainWindow, 
        lang->confirmRestoreMessage,
        lang->confirmTitle, MB_YESNO | MB_ICONQUESTION);
    
    if (result != IDYES) {
        return;  // 用户取消，终止执行
    }
    
    // 操作前准备（共用）
    if (!PrepareOperation(browser, lang, 6)) {
        FinishOperation(browser, FALSE, lang, NULL);
        return;
    }
    
    // 执行还原
    BOOL success = ExecuteRestoreOperation(browser);
    
    // 操作后清理（共用）
    FinishOperation(browser, success, lang, lang->restoreCompleted);
}

// 路径浏览按钮点击事件
void OnPathBrowseClicked(HWND hEdit, BOOL isTarget, BOOL isApp)
{
    BrowserInfo* browser = &g_browsers[g_config.selectedBrowser];
    WCHAR selectedPath[MAX_PATH];
    LPCWSTR title;
    
    // 设置初始路径
    if (isTarget) {
        // 从当前编辑框获取路径作为初始路径
        GetWindowTextW(hEdit, selectedPath, MAX_PATH);
        if (wcslen(selectedPath) == 0) {
            wcscpy_s(selectedPath, MAX_PATH, g_config.targetBasePath);
        }
        title = isApp ? L"Select Application Target Folder" : L"Select Data Target Folder";
    } else {
        wcscpy_s(selectedPath, MAX_PATH, isApp ? browser->appPath : browser->dataPath);
        title = isApp ? L"Select Application Source Folder" : L"Select Data Source Folder";
    }
    
    // 打开文件夹选择对话框
    if (BrowseForFolder(g_hMainWindow, title, selectedPath, MAX_PATH)) {
        // 更新编辑框
        SetWindowTextW(hEdit, selectedPath);
        
        // EN_CHANGE事件会自动更新磁盘空间显示
    }
}