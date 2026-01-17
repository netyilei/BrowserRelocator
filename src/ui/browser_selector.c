#include "../../include/ui/browser_selector.h"
#include "../../include/ui/main_window.h"

// 控件ID
#define ID_RADIO_CHROME 1001
#define ID_RADIO_EDGE 1002
#define ID_LABEL_CHROME_STATUS 1003
#define ID_LABEL_EDGE_STATUS 1004
#define ID_LABEL_CHROME_VERSION 1005
#define ID_LABEL_EDGE_VERSION 1006

// 创建浏览器选择器UI
void CreateBrowserSelectorUI(HWND hParent, BrowserSelectorUI* ui, int x, int y)
{
    // 创建Chrome单选框
    ui->hChromeRadio = CreateWindowExW(
        0, L"BUTTON", L"Chrome",
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
        x + 50, y + 30, 80, 20, hParent, (HMENU)ID_RADIO_CHROME, g_hInstance, NULL
    );
    
    // Chrome状态标签
    ui->hChromeStatusLabel = CreateWindowExW(
        0, L"STATIC", L"未安装",
        WS_CHILD | WS_VISIBLE,
        x + 140, y + 30, 150, 20, hParent, (HMENU)ID_LABEL_CHROME_STATUS, g_hInstance, NULL
    );
    
    // Chrome版本标签
    ui->hChromeVersionLabel = CreateWindowExW(
        0, L"STATIC", L"",
        WS_CHILD | WS_VISIBLE,
        x + 300, y + 30, 250, 20, hParent, (HMENU)ID_LABEL_CHROME_VERSION, g_hInstance, NULL
    );
    
    // 创建Edge单选框
    ui->hEdgeRadio = CreateWindowExW(
        0, L"BUTTON", L"Edge",
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        x + 50, y + 60, 80, 20, hParent, (HMENU)ID_RADIO_EDGE, g_hInstance, NULL
    );
    
    // Edge状态标签
    ui->hEdgeStatusLabel = CreateWindowExW(
        0, L"STATIC", L"未安装",
        WS_CHILD | WS_VISIBLE,
        x + 140, y + 60, 150, 20, hParent, (HMENU)ID_LABEL_EDGE_STATUS, g_hInstance, NULL
    );
    
    // Edge版本标签
    ui->hEdgeVersionLabel = CreateWindowExW(
        0, L"STATIC", L"",
        WS_CHILD | WS_VISIBLE,
        x + 300, y + 60, 250, 20, hParent, (HMENU)ID_LABEL_EDGE_VERSION, g_hInstance, NULL
    );
    
    // 设置字体
    HFONT hFont = CreateFontW(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                             DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei UI");
    
    SendMessageW(ui->hChromeRadio, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(ui->hEdgeRadio, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(ui->hChromeStatusLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(ui->hEdgeStatusLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(ui->hChromeVersionLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(ui->hEdgeVersionLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // 设置默认选择Chrome
    SendMessageW(ui->hChromeRadio, BM_SETCHECK, BST_CHECKED, 0);
    
    // 更新UI
    UpdateBrowserSelectorUI(ui);
}

// 更新浏览器选择器UI
void UpdateBrowserSelectorUI(BrowserSelectorUI* ui)
{
    // 更新Chrome信息
    if (g_browsers[BROWSER_CHROME].isInstalled) {
        SetWindowTextW(ui->hChromeStatusLabel, L"已安装");
        SetWindowTextW(ui->hChromeVersionLabel, g_browsers[BROWSER_CHROME].version);
    } else {
        SetWindowTextW(ui->hChromeStatusLabel, L"未安装");
        SetWindowTextW(ui->hChromeVersionLabel, L"");
    }
    
    // 更新Edge信息
    if (g_browsers[BROWSER_EDGE].isInstalled) {
        SetWindowTextW(ui->hEdgeStatusLabel, L"已安装");
        SetWindowTextW(ui->hEdgeVersionLabel, g_browsers[BROWSER_EDGE].version);
    } else {
        SetWindowTextW(ui->hEdgeStatusLabel, L"未安装");
        SetWindowTextW(ui->hEdgeVersionLabel, L"");
    }
}