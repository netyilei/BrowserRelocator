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
#define ID_BUTTON_BROWSE_APP 4001
#define ID_BUTTON_BROWSE_DATA 4002
#define ID_BUTTON_BROWSE_APP_TARGET 4003
#define ID_BUTTON_BROWSE_DATA_TARGET 4004
#define ID_LABEL_APP_SIZE 3005
#define ID_LABEL_DATA_SIZE 3006
#define ID_LABEL_APP_STATUS 3007
#define ID_LABEL_DATA_STATUS 3008
#define ID_LABEL_AVAILABLE_SPACE 3009

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
    HFONT hFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                              DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                              CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                              DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei UI");
    HFONT hFontBold = CreateFontW(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                  CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                  DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei UI");

    // 应用程序路径组
    CreateWindowExW(0, L"STATIC", L"应用程序目录:",
                    WS_CHILD | WS_VISIBLE, x, y, 100, 20, hParent, NULL, g_hInstance, NULL);

    ui->hAppPathEdit = CreateWindowExW(
        WS_EX_CLIENTEDGE, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | ES_READONLY,
        x + 100, y, 400, 22, hParent, (HMENU)ID_EDIT_APP_PATH, g_hInstance, NULL);

    CreateWindowExW(0, L"BUTTON", L"浏览...",
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    x + 510, y, 70, 24, hParent, (HMENU)ID_BUTTON_BROWSE_APP, g_hInstance, NULL);

    // 应用程序目标路径
    CreateWindowExW(0, L"STATIC", L"目标位置:",
                    WS_CHILD | WS_VISIBLE, x, y + 30, 100, 20, hParent, NULL, g_hInstance, NULL);

    ui->hAppTargetPathEdit = CreateWindowExW(
        WS_EX_CLIENTEDGE, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE,
        x + 100, y + 30, 400, 22, hParent, (HMENU)ID_EDIT_APP_TARGET, g_hInstance, NULL);

    CreateWindowExW(0, L"BUTTON", L"浏览...",
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    x + 510, y + 30, 70, 24, hParent, (HMENU)ID_BUTTON_BROWSE_APP_TARGET, g_hInstance, NULL);

    // 应用程序状态
    ui->hAppStatusLabel = CreateWindowExW(0, L"STATIC", L"",
                                          WS_CHILD | WS_VISIBLE, x, y + 60, 200, 20, hParent, (HMENU)ID_LABEL_APP_STATUS, g_hInstance, NULL);

    ui->hAppSizeLabel = CreateWindowExW(0, L"STATIC", L"",
                                        WS_CHILD | WS_VISIBLE, x + 210, y + 60, 150, 20, hParent, (HMENU)ID_LABEL_APP_SIZE, g_hInstance, NULL);

    // 用户数据路径组
    y += 90;
    CreateWindowExW(0, L"STATIC", L"用户数据:",
                    WS_CHILD | WS_VISIBLE, x, y, 100, 20, hParent, NULL, g_hInstance, NULL);

    ui->hDataPathEdit = CreateWindowExW(
        WS_EX_CLIENTEDGE, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | ES_READONLY,
        x + 100, y, 400, 22, hParent, (HMENU)ID_EDIT_DATA_PATH, g_hInstance, NULL);

    CreateWindowExW(0, L"BUTTON", L"浏览...",
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    x + 510, y, 70, 24, hParent, (HMENU)ID_BUTTON_BROWSE_DATA, g_hInstance, NULL);

    // 用户数据目标路径
    CreateWindowExW(0, L"STATIC", L"目标位置:",
                    WS_CHILD | WS_VISIBLE, x, y + 30, 100, 20, hParent, NULL, g_hInstance, NULL);

    ui->hDataTargetPathEdit = CreateWindowExW(
        WS_EX_CLIENTEDGE, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE,
        x + 100, y + 30, 400, 22, hParent, (HMENU)ID_EDIT_DATA_TARGET, g_hInstance, NULL);

    CreateWindowExW(0, L"BUTTON", L"浏览...",
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    x + 510, y + 30, 70, 24, hParent, (HMENU)ID_BUTTON_BROWSE_DATA_TARGET, g_hInstance, NULL);

    // 用户数据状态
    ui->hDataStatusLabel = CreateWindowExW(0, L"STATIC", L"",
                                           WS_CHILD | WS_VISIBLE, x, y + 60, 200, 20, hParent, (HMENU)ID_LABEL_DATA_STATUS, g_hInstance, NULL);

    ui->hDataSizeLabel = CreateWindowExW(0, L"STATIC", L"",
                                         WS_CHILD | WS_VISIBLE, x + 210, y + 60, 150, 20, hParent, (HMENU)ID_LABEL_DATA_SIZE, g_hInstance, NULL);

    // 可用空间
    y += 90;
    CreateWindowExW(0, L"STATIC", L"目标磁盘可用空间:",
                    WS_CHILD | WS_VISIBLE, x, y, 120, 20, hParent, NULL, g_hInstance, NULL);

    ui->hAvailableSpaceLabel = CreateWindowExW(0, L"STATIC", L"计算中...",
                                               WS_CHILD | WS_VISIBLE, x + 125, y, 200, 20, hParent, (HMENU)ID_LABEL_AVAILABLE_SPACE, g_hInstance, NULL);

    // 设置字体
    SendMessageW(ui->hAppPathEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(ui->hAppTargetPathEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(ui->hDataPathEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(ui->hDataTargetPathEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(ui->hAppStatusLabel, WM_SETFONT, (WPARAM)hFontBold, TRUE);
    SendMessageW(ui->hDataStatusLabel, WM_SETFONT, (WPARAM)hFontBold, TRUE);
    SendMessageW(ui->hAppSizeLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(ui->hDataSizeLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(ui->hAvailableSpaceLabel, WM_SETFONT, (WPARAM)hFontBold, TRUE);
}

// 更新路径选择器UI
void UpdatePathSelectorUI(PathSelectorUI *ui, BrowserType browserType)
{
    BrowserInfo *browser = &g_browsers[browserType];

    // 更新应用程序路径
    SetWindowTextW(ui->hAppPathEdit, browser->appPath);

    // 更新用户数据路径
    SetWindowTextW(ui->hDataPathEdit, browser->dataPath);

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

    // 更新可用空间
    ULONGLONG freeSpace = GetDiskFreeSpace(g_config.targetBasePath);
    FormatDiskSpace(freeSpace, sizeText, 64);
    SetWindowTextW(ui->hAvailableSpaceLabel, sizeText);
}