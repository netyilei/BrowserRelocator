#include "../../include/ui/browser_selector.h"
#include "../../include/ui/main_window.h"
#include "../../include/ui/path_selector.h"
#include "../../include/language.h"

// 控件ID
#define ID_RADIO_CHROME 1001
#define ID_RADIO_EDGE 1002
#define ID_LABEL_CHROME_STATUS 1003
#define ID_LABEL_EDGE_STATUS 1004
#define ID_LABEL_CHROME_VERSION 1005
#define ID_LABEL_EDGE_VERSION 1006

// 图标ID
#define IDI_CHROME_ICON 102
#define IDI_EDGE_ICON 103

// 创建浏览器选择器UI
void CreateBrowserSelectorUI(HWND hParent, BrowserSelectorUI* ui, int x, int y)
{
    const LanguageStrings* lang = GetCurrentLanguageStrings();
    
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
    
    // 标题在边框上方
    HWND hTitle = CreateWindowExW(0, L"STATIC", lang->selectBrowser,
        WS_CHILD | WS_VISIBLE,
        x + 5, y, 100, 24, hParent, NULL, g_hInstance, NULL);
    SendMessageW(hTitle, WM_SETFONT, (WPARAM)hFontTitle, TRUE);
    
    // 边框面板
    HWND hPanel = CreateWindowExW(0, L"STATIC", NULL,
        WS_CHILD | WS_VISIBLE | SS_ETCHEDFRAME,
        x, y + 20, 680, 55, hParent, NULL, g_hInstance, NULL);
    (void)hPanel;
    
    // 加载Chrome图标
    HICON hChromeIcon = (HICON)LoadImageW(g_hInstance, MAKEINTRESOURCEW(IDI_CHROME_ICON), IMAGE_ICON, 24, 24, LR_DEFAULTCOLOR | LR_SHARED);
    ui->hChromeIcon = CreateWindowExW(WS_EX_TRANSPARENT, L"STATIC", NULL,
        WS_CHILD | WS_VISIBLE | SS_ICON | SS_REALSIZECONTROL,
        x + 20, y + 35, 24, 24, hParent, NULL, g_hInstance, NULL);
    SendMessageW(ui->hChromeIcon, STM_SETICON, (WPARAM)hChromeIcon, 0);
    
    // Chrome单选框
    ui->hChromeRadio = CreateWindowExW(
        0, L"BUTTON", L"Chrome",
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
        x + 58, y + 37, 90, 26, hParent, (HMENU)ID_RADIO_CHROME, g_hInstance, NULL
    );
    SendMessageW(ui->hChromeRadio, WM_SETFONT, (WPARAM)hFontBold, TRUE);
    
    // Chrome状态标签
    ui->hChromeStatusLabel = CreateWindowExW(
        0, L"STATIC", lang->notInstalled,
        WS_CHILD | WS_VISIBLE,
        x + 155, y + 38, 75, 24, hParent, (HMENU)ID_LABEL_CHROME_STATUS, g_hInstance, NULL
    );
    SendMessageW(ui->hChromeStatusLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // Chrome版本标签
    ui->hChromeVersionLabel = CreateWindowExW(
        0, L"STATIC", L"",
        WS_CHILD | WS_VISIBLE,
        x + 235, y + 38, 120, 24, hParent, (HMENU)ID_LABEL_CHROME_VERSION, g_hInstance, NULL
    );
    SendMessageW(ui->hChromeVersionLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // 加载Edge图标 - 位置在Chrome版本标签之后，避免重叠
    HICON hEdgeIcon = (HICON)LoadImageW(g_hInstance, MAKEINTRESOURCEW(IDI_EDGE_ICON), IMAGE_ICON, 24, 24, LR_DEFAULTCOLOR | LR_SHARED);
    ui->hEdgeIcon = CreateWindowExW(WS_EX_TRANSPARENT, L"STATIC", NULL,
        WS_CHILD | WS_VISIBLE | SS_ICON | SS_REALSIZECONTROL,
        x + 370, y + 35, 24, 24, hParent, NULL, g_hInstance, NULL);
    SendMessageW(ui->hEdgeIcon, STM_SETICON, (WPARAM)hEdgeIcon, 0);
    
    // Edge单选框
    ui->hEdgeRadio = CreateWindowExW(
        0, L"BUTTON", L"Edge",
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        x + 405, y + 37, 70, 26, hParent, (HMENU)ID_RADIO_EDGE, g_hInstance, NULL
    );
    SendMessageW(ui->hEdgeRadio, WM_SETFONT, (WPARAM)hFontBold, TRUE);
    
    // Edge状态标签
    ui->hEdgeStatusLabel = CreateWindowExW(
        0, L"STATIC", lang->notInstalled,
        WS_CHILD | WS_VISIBLE,
        x + 480, y + 38, 80, 24, hParent, (HMENU)ID_LABEL_EDGE_STATUS, g_hInstance, NULL
    );
    SendMessageW(ui->hEdgeStatusLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // Edge版本标签
    ui->hEdgeVersionLabel = CreateWindowExW(
        0, L"STATIC", L"",
        WS_CHILD | WS_VISIBLE,
        x + 565, y + 38, 110, 24, hParent, (HMENU)ID_LABEL_EDGE_VERSION, g_hInstance, NULL
    );
    SendMessageW(ui->hEdgeVersionLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // 智能选择浏览器
    BOOL chromeInstalled = g_browsers[BROWSER_CHROME].isInstalled;
    BOOL edgeInstalled = g_browsers[BROWSER_EDGE].isInstalled;
    
    if (!chromeInstalled && !edgeInstalled) {
        // 都不存在，弹出提示
        MessageBoxW(hParent, lang->noBrowserDetected, lang->noBrowserTitle, MB_OK | MB_ICONWARNING);
        SendMessageW(ui->hChromeRadio, BM_SETCHECK, BST_CHECKED, 0);
        g_config.selectedBrowser = BROWSER_CHROME;
    } else if (!chromeInstalled && edgeInstalled) {
        SendMessageW(ui->hEdgeRadio, BM_SETCHECK, BST_CHECKED, 0);
        g_config.selectedBrowser = BROWSER_EDGE;
        // 更新路径选择器显示Edge路径
        UpdatePathSelectorUI(&g_ui.pathSelector, BROWSER_EDGE);
    } else {
        SendMessageW(ui->hChromeRadio, BM_SETCHECK, BST_CHECKED, 0);
        g_config.selectedBrowser = BROWSER_CHROME;
        // 更新路径选择器显示Chrome路径
        UpdatePathSelectorUI(&g_ui.pathSelector, BROWSER_CHROME);
    }
    
    UpdateBrowserSelectorUI(ui);
}

// 更新浏览器选择器UI
void UpdateBrowserSelectorUI(BrowserSelectorUI* ui)
{
    const LanguageStrings* lang = GetCurrentLanguageStrings();
    
    // 更新Chrome信息
    if (g_browsers[BROWSER_CHROME].isInstalled) {
        SetWindowTextW(ui->hChromeStatusLabel, lang->installed);
        SetWindowTextW(ui->hChromeVersionLabel, g_browsers[BROWSER_CHROME].version);
    } else {
        SetWindowTextW(ui->hChromeStatusLabel, lang->notInstalled);
        SetWindowTextW(ui->hChromeVersionLabel, L"");
    }
    
    // 更新Edge信息
    if (g_browsers[BROWSER_EDGE].isInstalled) {
        SetWindowTextW(ui->hEdgeStatusLabel, lang->installed);
        SetWindowTextW(ui->hEdgeVersionLabel, g_browsers[BROWSER_EDGE].version);
    } else {
        SetWindowTextW(ui->hEdgeStatusLabel, lang->notInstalled);
        SetWindowTextW(ui->hEdgeVersionLabel, L"");
    }
}