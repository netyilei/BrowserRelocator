#include "../../include/ui/path_selector.h"
#include "../../include/ui/main_window.h"
#include "../../include/utils/disk_utils.h"
#include "../../include/utils/string_utils.h"
#include "../../include/language.h"

// 控件ID
#define ID_EDIT_APP_PATH 3001
#define ID_EDIT_DATA_PATH 3002
#define ID_EDIT_APP_TARGET 3003
#define ID_EDIT_DATA_TARGET 3004
#define ID_BUTTON_BROWSE_APP_TARGET 4003
#define ID_BUTTON_BROWSE_DATA_TARGET 4004
#define ID_LABEL_APP_SIZE 3005
#define ID_LABEL_DATA_SIZE 3006
#define ID_LABEL_APP_STATUS 3007
#define ID_LABEL_DATA_STATUS 3008

// 浏览文件夹回调
int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    if (uMsg == BFFM_INITIALIZED)
    {
        SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
    }
    return 0;
}

// 浏览文件夹
BOOL BrowseForFolder(HWND hParent, LPCWSTR title, WCHAR *selectedPath, int pathSize)
{
    BROWSEINFOW bi = {0};
    bi.hwndOwner = hParent;
    bi.lpszTitle = title;
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_VALIDATE;
    bi.lpfn = BrowseCallbackProc;
    bi.lParam = (LPARAM)selectedPath;

    LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
    if (pidl)
    {
        if (SHGetPathFromIDListW(pidl, selectedPath))
        {
            CoTaskMemFree(pidl);
            return TRUE;
        }
        CoTaskMemFree(pidl);
    }
    return FALSE;
}

// 创建路径选择器UI
void CreatePathSelectorUI(HWND hParent, PathSelectorUI *ui, int x, int y)
{
    const LanguageStrings* lang = GetCurrentLanguageStrings();
    
    // 字体 - 增大到22px
    HFONT hFont = CreateFontW(22, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                              GB2312_CHARSET, OUT_DEFAULT_PRECIS,
                              CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                              DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei UI");
    HFONT hFontBold = CreateFontW(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                  GB2312_CHARSET, OUT_DEFAULT_PRECIS,
                                  CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                  DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei UI");
    HFONT hFontTitle = CreateFontW(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                  GB2312_CHARSET, OUT_DEFAULT_PRECIS,
                                  CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                  DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei UI");
    
    int editWidth = 480;
    int labelWidth = 80;
    int btnWidth = 65;
    int groupWidth = 680;
    int editHeight = 26;
    int panelHeight = 105;
    
    // ========== 应用程序目录区域 ==========
    // 标题在边框上方
    ui->hAppTitle = CreateWindowExW(0, L"STATIC", lang->appDirectory,
        WS_CHILD | WS_VISIBLE,
        x + 5, y, 120, 24, hParent, NULL, g_hInstance, NULL);
    SendMessageW(ui->hAppTitle, WM_SETFONT, (WPARAM)hFontTitle, TRUE);
    
    // 边框面板（使用STATIC + SS_ETCHEDFRAME）
    ui->hAppPanel = CreateWindowExW(0, L"STATIC", NULL,
        WS_CHILD | WS_VISIBLE | SS_ETCHEDFRAME,
        x, y + 20, groupWidth, panelHeight, hParent, NULL, g_hInstance, NULL);

    // 当前路径标签
    ui->hAppPathLabel = CreateWindowExW(0, L"STATIC", lang->currentPath,
                    WS_CHILD | WS_VISIBLE, x + 15, y + 35, labelWidth, 24, hParent, NULL, g_hInstance, NULL);
    SendMessageW(ui->hAppPathLabel, WM_SETFONT, (WPARAM)hFont, TRUE);

    // 当前路径编辑框
    ui->hAppPathEdit = CreateWindowExW(
        WS_EX_CLIENTEDGE, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | ES_READONLY | ES_AUTOHSCROLL,
        x + labelWidth + 20, y + 33, editWidth, editHeight, hParent, (HMENU)ID_EDIT_APP_PATH, g_hInstance, NULL);
    SendMessageW(ui->hAppPathEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

    // 目标路径标签
    ui->hAppTargetLabel = CreateWindowExW(0, L"STATIC", lang->targetPath,
                    WS_CHILD | WS_VISIBLE, x + 15, y + 68, labelWidth, 24, hParent, NULL, g_hInstance, NULL);
    SendMessageW(ui->hAppTargetLabel, WM_SETFONT, (WPARAM)hFont, TRUE);

    // 目标路径编辑框
    ui->hAppTargetPathEdit = CreateWindowExW(
        WS_EX_CLIENTEDGE, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        x + labelWidth + 20, y + 66, editWidth - btnWidth - 10, editHeight, hParent, (HMENU)ID_EDIT_APP_TARGET, g_hInstance, NULL);
    SendMessageW(ui->hAppTargetPathEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

    // 浏览按钮
    ui->hAppBrowseBtn = CreateWindowExW(0, L"BUTTON", lang->browse,
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    x + labelWidth + 20 + editWidth - btnWidth - 5, y + 65, btnWidth, 28, hParent, (HMENU)ID_BUTTON_BROWSE_APP_TARGET, g_hInstance, NULL);
    SendMessageW(ui->hAppBrowseBtn, WM_SETFONT, (WPARAM)hFont, TRUE);

    // 应用程序目标磁盘可用空间
    ui->hAppSpaceLabel = CreateWindowExW(0, L"STATIC", L"",
                                        WS_CHILD | WS_VISIBLE, x + 430, y + 100, 240, 22, hParent, NULL, g_hInstance, NULL);
    SendMessageW(ui->hAppSpaceLabel, WM_SETFONT, (WPARAM)hFont, TRUE);

    // 应用程序状态
    ui->hAppStatusLabel = CreateWindowExW(0, L"STATIC", L"",
                                          WS_CHILD | WS_VISIBLE, x + 15, y + 100, 220, 22, hParent, (HMENU)ID_LABEL_APP_STATUS, g_hInstance, NULL);
    SendMessageW(ui->hAppStatusLabel, WM_SETFONT, (WPARAM)hFontBold, TRUE);

    ui->hAppSizeLabel = CreateWindowExW(0, L"STATIC", L"",
                                        WS_CHILD | WS_VISIBLE, x + 250, y + 100, 160, 22, hParent, (HMENU)ID_LABEL_APP_SIZE, g_hInstance, NULL);
    SendMessageW(ui->hAppSizeLabel, WM_SETFONT, (WPARAM)hFont, TRUE);

    // ========== 用户数据目录区域 ==========
    y += 135;
    
    // 标题在边框上方
    ui->hDataTitle = CreateWindowExW(0, L"STATIC", lang->dataDirectory,
        WS_CHILD | WS_VISIBLE,
        x + 5, y, 120, 24, hParent, NULL, g_hInstance, NULL);
    SendMessageW(ui->hDataTitle, WM_SETFONT, (WPARAM)hFontTitle, TRUE);

    // 边框面板
    ui->hDataPanel = CreateWindowExW(0, L"STATIC", NULL,
        WS_CHILD | WS_VISIBLE | SS_ETCHEDFRAME,
        x, y + 20, groupWidth, panelHeight, hParent, NULL, g_hInstance, NULL);

    // 当前路径标签
    ui->hDataPathLabel = CreateWindowExW(0, L"STATIC", lang->currentPath,
                    WS_CHILD | WS_VISIBLE, x + 15, y + 35, labelWidth, 24, hParent, NULL, g_hInstance, NULL);
    SendMessageW(ui->hDataPathLabel, WM_SETFONT, (WPARAM)hFont, TRUE);

    // 当前路径编辑框
    ui->hDataPathEdit = CreateWindowExW(
        WS_EX_CLIENTEDGE, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | ES_READONLY | ES_AUTOHSCROLL,
        x + labelWidth + 20, y + 33, editWidth, editHeight, hParent, (HMENU)ID_EDIT_DATA_PATH, g_hInstance, NULL);
    SendMessageW(ui->hDataPathEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

    // 目标路径标签
    ui->hDataTargetLabel = CreateWindowExW(0, L"STATIC", lang->targetPath,
                    WS_CHILD | WS_VISIBLE, x + 15, y + 68, labelWidth, 24, hParent, NULL, g_hInstance, NULL);
    SendMessageW(ui->hDataTargetLabel, WM_SETFONT, (WPARAM)hFont, TRUE);

    // 目标路径编辑框
    ui->hDataTargetPathEdit = CreateWindowExW(
        WS_EX_CLIENTEDGE, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        x + labelWidth + 20, y + 66, editWidth - btnWidth - 10, editHeight, hParent, (HMENU)ID_EDIT_DATA_TARGET, g_hInstance, NULL);
    SendMessageW(ui->hDataTargetPathEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

    // 浏览按钮
    ui->hDataBrowseBtn = CreateWindowExW(0, L"BUTTON", lang->browse,
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    x + labelWidth + 20 + editWidth - btnWidth - 5, y + 65, btnWidth, 28, hParent, (HMENU)ID_BUTTON_BROWSE_DATA_TARGET, g_hInstance, NULL);
    SendMessageW(ui->hDataBrowseBtn, WM_SETFONT, (WPARAM)hFont, TRUE);

    // 用户数据目标磁盘可用空间
    ui->hDataSpaceLabel = CreateWindowExW(0, L"STATIC", L"",
                                         WS_CHILD | WS_VISIBLE, x + 430, y + 100, 240, 22, hParent, NULL, g_hInstance, NULL);
    SendMessageW(ui->hDataSpaceLabel, WM_SETFONT, (WPARAM)hFont, TRUE);

    // 用户数据状态
    ui->hDataStatusLabel = CreateWindowExW(0, L"STATIC", L"",
                                           WS_CHILD | WS_VISIBLE, x + 15, y + 100, 220, 22, hParent, (HMENU)ID_LABEL_DATA_STATUS, g_hInstance, NULL);
    SendMessageW(ui->hDataStatusLabel, WM_SETFONT, (WPARAM)hFontBold, TRUE);

    ui->hDataSizeLabel = CreateWindowExW(0, L"STATIC", L"",
                                         WS_CHILD | WS_VISIBLE, x + 250, y + 100, 160, 22, hParent, (HMENU)ID_LABEL_DATA_SIZE, g_hInstance, NULL);
    SendMessageW(ui->hDataSizeLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
}

// 移动窗口位置的辅助函数
static void MoveWindowY(HWND hwnd, int deltaY)
{
    if (!hwnd) return;
    RECT rc;
    GetWindowRect(hwnd, &rc);
    HWND hParent = GetParent(hwnd);
    POINT pt = {rc.left, rc.top};
    ScreenToClient(hParent, &pt);
    SetWindowPos(hwnd, NULL, pt.x, pt.y + deltaY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

// 显示/隐藏应用程序目录区域
void ShowAppPathSection(PathSelectorUI *ui, BOOL show)
{
    int cmd = show ? SW_SHOW : SW_HIDE;
    ShowWindow(ui->hAppTitle, cmd);
    ShowWindow(ui->hAppPanel, cmd);
    ShowWindow(ui->hAppPathLabel, cmd);
    ShowWindow(ui->hAppPathEdit, cmd);
    ShowWindow(ui->hAppTargetLabel, cmd);
    ShowWindow(ui->hAppTargetPathEdit, cmd);
    ShowWindow(ui->hAppBrowseBtn, cmd);
    ShowWindow(ui->hAppStatusLabel, cmd);
    ShowWindow(ui->hAppSizeLabel, cmd);
    ShowWindow(ui->hAppSpaceLabel, cmd);
    
    // 计算偏移量（应用程序区域高度约135px）
    int deltaY = show ? 0 : -135;
    static int lastDelta = 0;
    int actualDelta = deltaY - lastDelta;
    lastDelta = deltaY;
    
    if (actualDelta != 0) {
        // 移动用户数据目录区域
        MoveWindowY(ui->hDataTitle, actualDelta);
        MoveWindowY(ui->hDataPanel, actualDelta);
        MoveWindowY(ui->hDataPathLabel, actualDelta);
        MoveWindowY(ui->hDataPathEdit, actualDelta);
        MoveWindowY(ui->hDataTargetLabel, actualDelta);
        MoveWindowY(ui->hDataTargetPathEdit, actualDelta);
        MoveWindowY(ui->hDataBrowseBtn, actualDelta);
        MoveWindowY(ui->hDataStatusLabel, actualDelta);
        MoveWindowY(ui->hDataSizeLabel, actualDelta);
        MoveWindowY(ui->hDataSpaceLabel, actualDelta);
    }
}

// 更新路径选择器UI
void UpdatePathSelectorUI(PathSelectorUI *ui, BrowserType browserType)
{
    BrowserInfo *browser = &g_browsers[browserType];

    // 更新应用程序路径
    if (browser->isInstalled) {
        SetWindowTextW(ui->hAppPathEdit, browser->appPath);
        SetWindowTextW(ui->hDataPathEdit, browser->dataPath);
    } else {
        // 未安装时显示默认路径提示
        if (browserType == BROWSER_CHROME) {
            SetWindowTextW(ui->hAppPathEdit, L"C:\\Program Files\\Google\\Chrome\\Application");
            
            WCHAR dataPath[MAX_PATH];
            ExpandEnvironmentStringsW(L"%LOCALAPPDATA%\\Google\\Chrome\\User Data", dataPath, MAX_PATH);
            SetWindowTextW(ui->hDataPathEdit, dataPath);
        } else {
            SetWindowTextW(ui->hAppPathEdit, L"C:\\Program Files (x86)\\Microsoft\\Edge\\Application");
            
            WCHAR dataPath[MAX_PATH];
            ExpandEnvironmentStringsW(L"%LOCALAPPDATA%\\Microsoft\\Edge\\User Data", dataPath, MAX_PATH);
            SetWindowTextW(ui->hDataPathEdit, dataPath);
        }
    }

    // 生成目标路径
    if (wcslen(g_config.targetBasePath) > 0)
    {
        WCHAR targetPath[MAX_PATH];
        wcscpy_s(targetPath, MAX_PATH, g_config.targetBasePath);
        wcscat_s(targetPath, MAX_PATH, L"\\");
        wcscat_s(targetPath, MAX_PATH, browser->name);

        wcscpy_s(browser->appTargetPath, MAX_PATH, targetPath);
        wcscat_s(browser->appTargetPath, MAX_PATH, L"\\App");

        wcscpy_s(browser->dataTargetPath, MAX_PATH, targetPath);
        wcscat_s(browser->dataTargetPath, MAX_PATH, L"\\User Data");
    }

    SetWindowTextW(ui->hAppTargetPathEdit, browser->appTargetPath);
    SetWindowTextW(ui->hDataTargetPathEdit, browser->dataTargetPath);

    // 更新路径信息显示
    UpdatePathInfoDisplay(ui, browser);
}

// 更新路径信息显示
void UpdatePathInfoDisplay(PathSelectorUI *ui, BrowserInfo *browser)
{
    const LanguageStrings *lang = GetCurrentLanguageStrings();
    WCHAR sizeText[64];
    WCHAR statusText[128];
    WCHAR spaceText[128];

    // 应用程序信息
    FormatSize(browser->appSize, sizeText, 64);

    if (browser->appMoved)
    {
        swprintf_s(statusText, 128, lang->statusMoved, g_config.targetBasePath);
    }
    else
    {
        wcscpy_s(statusText, 128, lang->statusNotMoved);
    }

    SetWindowTextW(ui->hAppSizeLabel, sizeText);
    SetWindowTextW(ui->hAppStatusLabel, statusText);
    
    // 应用程序目标磁盘可用空间
    if (ui->hAppSpaceLabel) {
        WCHAR appTargetPath[MAX_PATH];
        GetWindowTextW(ui->hAppTargetPathEdit, appTargetPath, MAX_PATH);
        if (wcslen(appTargetPath) >= 2) {
            WCHAR drivePath[4] = {0};
            drivePath[0] = appTargetPath[0];
            drivePath[1] = appTargetPath[1];
            drivePath[2] = L'\\';
            drivePath[3] = L'\0';
            
            ULONGLONG freeSpace = GetDiskAvailableSpace(drivePath);
            FormatDiskSpace(freeSpace, sizeText, 64);
            swprintf_s(spaceText, 128, L"%s: %s", lang->availableSpace, sizeText);
            SetWindowTextW(ui->hAppSpaceLabel, spaceText);
        }
    }

    // 用户数据信息
    FormatSize(browser->dataSize, sizeText, 64);

    if (browser->dataMoved)
    {
        swprintf_s(statusText, 128, lang->statusMoved, g_config.targetBasePath);
    }
    else
    {
        wcscpy_s(statusText, 128, lang->statusNotMoved);
    }

    SetWindowTextW(ui->hDataSizeLabel, sizeText);
    SetWindowTextW(ui->hDataStatusLabel, statusText);
    
    // 用户数据目标磁盘可用空间
    if (ui->hDataSpaceLabel) {
        WCHAR dataTargetPath[MAX_PATH];
        GetWindowTextW(ui->hDataTargetPathEdit, dataTargetPath, MAX_PATH);
        if (wcslen(dataTargetPath) >= 2) {
            WCHAR drivePath[4] = {0};
            drivePath[0] = dataTargetPath[0];
            drivePath[1] = dataTargetPath[1];
            drivePath[2] = L'\\';
            drivePath[3] = L'\0';
            
            ULONGLONG freeSpace = GetDiskAvailableSpace(drivePath);
            FormatDiskSpace(freeSpace, sizeText, 64);
            swprintf_s(spaceText, 128, L"%s: %s", lang->availableSpace, sizeText);
            SetWindowTextW(ui->hDataSpaceLabel, spaceText);
        }
    }
}
