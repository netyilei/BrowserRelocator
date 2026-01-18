#include "../../include/ui/main_window.h"
#include "../../include/ui/browser_selector.h"
#include "../../include/ui/path_selector.h"
#include "../../include/core/browser_detector.h"
#include "../../include/utils/disk_utils.h"
#include "../../include/language.h"

// 控件ID定义
#define ID_RADIO_CHROME 1001
#define ID_RADIO_EDGE 1002
#define ID_CHECK_MOVE_APP 2001
#define ID_CHECK_FIX_SHORTCUTS 2005
#define ID_BUTTON_BROWSE_APP_TARGET 4003
#define ID_BUTTON_BROWSE_DATA_TARGET 4004
#define ID_BUTTON_MOVE 5001
#define ID_BUTTON_RESTORE 5002
#define ID_BUTTON_HELP 5003
#define ID_MENU_ENGLISH 6002
#define ID_MENU_CHINESE 6003
#define ID_MENU_GITHUB 6004

// 创建主窗口
HWND CreateMainWindow(HINSTANCE hInstance)
{
    // 注册窗口类
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"BrowserGarageMainWindow";
    
    HICON hIcon = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_APP_ICON));
    wc.hIcon = hIcon;
    wc.hIconSm = hIcon;
    
    if (!RegisterClassExW(&wc)) {
        return NULL;
    }
    
    // 计算窗口大小
    int windowWidth = 720;
    int windowHeight = 665;
    
    RECT rect;
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &rect, 0);
    int x = (rect.right - rect.left - windowWidth) / 2;
    int y = (rect.bottom - rect.top - windowHeight) / 2;
    
    // 创建窗口
    HWND hwnd = CreateWindowExW(
        0,
        L"BrowserGarageMainWindow",
        L"BrowserGarage v2.1",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        x, y, windowWidth, windowHeight,
        NULL, NULL, hInstance, NULL
    );
    
    if (!hwnd) {
        return NULL;
    }
    
    // 初始化浏览器信息
    InitializeBrowserInfo(&g_browsers[0], &g_browsers[1]);
    
    // 创建UI控件
    CreateMainWindowUI(hwnd);
    
    // 创建菜单
    CreateMainMenu(hwnd);
    
    return hwnd;
}

// 创建主菜单
void CreateMainMenu(HWND hwnd)
{
    const LanguageStrings* lang = GetCurrentLanguageStrings();
    HMENU hMenuBar = CreateMenu();
    HMENU hLanguageMenu = CreatePopupMenu();
    
    // 语言菜单 - 默认中文有对号
    AppendMenuW(hLanguageMenu, MF_STRING, ID_MENU_ENGLISH, lang->menuEnglish);
    AppendMenuW(hLanguageMenu, MF_STRING | MF_CHECKED, ID_MENU_CHINESE, lang->menuChinese);
    
    // 显示当前语言
    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hLanguageMenu, lang->menuLanguage);
    
    // GitHub菜单
    AppendMenuW(hMenuBar, MF_STRING, ID_MENU_GITHUB, L"GitHub");
    
    SetMenu(hwnd, hMenuBar);
}

// 创建主窗口UI
void CreateMainWindowUI(HWND hwnd)
{
    int yOffset = 15;
    
    // 浏览器选择器
    CreateBrowserSelectorUI(hwnd, &g_ui.browserSelector, 15, yOffset);
    yOffset += 90;
    
    // 搬家选项
    CreateOptionsUI(hwnd, &g_ui.options, 15, yOffset);
    yOffset += 85;
    
    // 路径选择器
    CreatePathSelectorUI(hwnd, &g_ui.pathSelector, 15, yOffset);
    yOffset += 305;
    
    // 操作按钮
    CreateActionButtons(hwnd, &g_ui, 15, yOffset);
    
    // 创建状态UI（进度条和文本）
    CreateStatusBar(hwnd);
    
    // 初始化路径显示（必须在所有UI创建完成后）
    UpdatePathSelectorUI(&g_ui.pathSelector, g_config.selectedBrowser);
}

// 创建选项UI
void CreateOptionsUI(HWND hwnd, OptionsUI* ui, int x, int y)
{
    const LanguageStrings* lang = GetCurrentLanguageStrings();
    HFONT hFont = CreateFontW(22, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             GB2312_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                             DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei UI");
    HFONT hFontTitle = CreateFontW(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                             GB2312_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                             DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei UI");
    
    // 标题在边框上方
    HWND hTitle = CreateWindowExW(0, L"STATIC", lang->moveOptions,
        WS_CHILD | WS_VISIBLE,
        x + 5, y, 85, 24, hwnd, NULL, g_hInstance, NULL);
    SendMessageW(hTitle, WM_SETFONT, (WPARAM)hFontTitle, TRUE);
    
    // 边框面板
    HWND hPanel = CreateWindowExW(0, L"STATIC", NULL,
        WS_CHILD | WS_VISIBLE | SS_ETCHEDFRAME,
        x, y + 20, 680, 50, hwnd, NULL, g_hInstance, NULL);
    (void)hPanel;
    
    int checkWidth = 140;
    int checkHeight = 26;
    int startY = y + 32;
    
    // 复选框（用户数据迁移为默认必选，不显示）
    ui->hMoveAppCheckbox = CreateWindowExW(
        0, L"BUTTON", lang->moveApp,
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        x + 15, startY, checkWidth, checkHeight, hwnd, (HMENU)ID_CHECK_MOVE_APP, g_hInstance, NULL
    );
    SendMessageW(ui->hMoveAppCheckbox, BM_SETCHECK, BST_CHECKED, 0);
    SendMessageW(ui->hMoveAppCheckbox, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // 用户数据迁移是核心功能，默认启用，不显示选项
    ui->hMoveDataCheckbox = NULL;
    
    // 关闭浏览器改为弹窗确认，不显示复选框
    ui->hCloseBrowserCheckbox = NULL;
    
    ui->hFixShortcutsCheckbox = CreateWindowExW(
        0, L"BUTTON", lang->fixShortcuts,
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        x + 165, startY, 150, checkHeight, hwnd, (HMENU)ID_CHECK_FIX_SHORTCUTS, g_hInstance, NULL
    );
    SendMessageW(ui->hFixShortcutsCheckbox, BM_SETCHECK, BST_CHECKED, 0);
    SendMessageW(ui->hFixShortcutsCheckbox, WM_SETFONT, (WPARAM)hFont, TRUE);
}

// 创建操作按钮
void CreateActionButtons(HWND hwnd, MainWindowUI* ui, int x, int y)
{
    const LanguageStrings* lang = GetCurrentLanguageStrings();
    HFONT hFont = CreateFontW(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                             GB2312_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                             DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei UI");
    
    int btnWidth = 210;
    int btnHeight = 40;
    int spacing = 20;
    
    // 移动按钮
    ui->hMoveButton = CreateWindowExW(
        0, L"BUTTON", lang->moveButton,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        x, y, btnWidth, btnHeight, hwnd, (HMENU)ID_BUTTON_MOVE, g_hInstance, NULL
    );
    SendMessageW(ui->hMoveButton, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // 还原按钮
    ui->hRestoreButton = CreateWindowExW(
        0, L"BUTTON", lang->restoreButton,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        x + btnWidth + spacing, y, btnWidth, btnHeight, hwnd, (HMENU)ID_BUTTON_RESTORE, g_hInstance, NULL
    );
    SendMessageW(ui->hRestoreButton, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // 帮助按钮
    ui->hHelpButton = CreateWindowExW(
        0, L"BUTTON", lang->helpButton,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        x + (btnWidth + spacing) * 2, y, btnWidth, btnHeight, hwnd, (HMENU)ID_BUTTON_HELP, g_hInstance, NULL
    );
    SendMessageW(ui->hHelpButton, WM_SETFONT, (WPARAM)hFont, TRUE);
}

// 主窗口过程
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_CREATE:
            // 初始化已完成，在CreateMainWindow中处理
            break;
            
        case WM_COMMAND:
            {
            WORD controlId = LOWORD(wParam);
            
            switch (controlId) {
                case ID_RADIO_CHROME:
                    OnBrowserSelected(BROWSER_CHROME);
                    break;
                    
                case ID_RADIO_EDGE:
                    OnBrowserSelected(BROWSER_EDGE);
                    break;
                    
                case ID_BUTTON_MOVE:
                    OnMoveButtonClicked();
                    break;
                    
                case ID_BUTTON_RESTORE:
                    OnRestoreButtonClicked();
                    break;
                    
                case ID_BUTTON_HELP:
                    {
                    const LanguageStrings* lang = GetCurrentLanguageStrings();
                    MessageBoxW(hwnd, lang->helpContent, lang->helpTitle, MB_OK | MB_ICONINFORMATION);
                    }
                    break;
                    
                case ID_BUTTON_BROWSE_APP_TARGET:
                    OnPathBrowseClicked(g_ui.pathSelector.hAppTargetPathEdit, TRUE, TRUE);
                    break;
                    
                case ID_BUTTON_BROWSE_DATA_TARGET:
                    OnPathBrowseClicked(g_ui.pathSelector.hDataTargetPathEdit, TRUE, FALSE);
                    break;
                    
                case ID_MENU_ENGLISH:
                    OnLanguageChanged(LANG_EN);
                    break;
                    
                case ID_MENU_CHINESE:
                    OnLanguageChanged(LANG_ZH);
                    break;
                    
                case ID_MENU_GITHUB:
                    ShellExecuteW(NULL, L"open", L"https://github.com/netyilei/BrowserRelocator", NULL, NULL, SW_SHOWNORMAL);
                    break;
                    
                case ID_CHECK_MOVE_APP:
                    {
                    // 应用程序目录复选框状态变化
                    BOOL checked = (SendMessageW(g_ui.options.hMoveAppCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED);
                    g_config.moveApp = checked;
                    ShowAppPathSection(&g_ui.pathSelector, checked);
                    
                    // 动态调整窗口高度
                    RECT rcWindow;
                    GetWindowRect(hwnd, &rcWindow);
                    int newHeight = checked ? 665 : 530;
                    SetWindowPos(hwnd, NULL, 0, 0, 
                        rcWindow.right - rcWindow.left, newHeight,
                        SWP_NOMOVE | SWP_NOZORDER);
                    
                    // 移动按钮位置
                    int btnY = checked ? 490 : 355;  // 按钮的y坐标
                    SetWindowPos(g_ui.hMoveButton, NULL, 15, btnY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                    SetWindowPos(g_ui.hRestoreButton, NULL, 245, btnY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                    SetWindowPos(g_ui.hHelpButton, NULL, 475, btnY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                    
                    InvalidateRect(hwnd, NULL, TRUE);
                    }
                    break;
            }
            // 处理编辑框内容变化或失去焦点
            if (HIWORD(wParam) == EN_CHANGE || HIWORD(wParam) == EN_KILLFOCUS) {
                HWND hCtrl = (HWND)lParam;
                const LanguageStrings* lang = GetCurrentLanguageStrings();
                WCHAR newPath[MAX_PATH];
                WCHAR sizeText[64], spaceText[128];
                
                // 应用程序目标路径变化 - 更新应用程序可用空间
                if (hCtrl == g_ui.pathSelector.hAppTargetPathEdit) {
                    GetWindowTextW(g_ui.pathSelector.hAppTargetPathEdit, newPath, MAX_PATH);
                    if (wcslen(newPath) >= 2) {
                        WCHAR drivePath[4] = {newPath[0], newPath[1], L'\\', L'\0'};
                        ULONGLONG freeSpace = GetDiskAvailableSpace(drivePath);
                        FormatDiskSpace(freeSpace, sizeText, 64);
                        swprintf_s(spaceText, 128, L"%s: %s", lang->availableSpace, sizeText);
                        SetWindowTextW(g_ui.pathSelector.hAppSpaceLabel, spaceText);
                    }
                }
                // 用户数据目标路径变化 - 更新用户数据可用空间
                else if (hCtrl == g_ui.pathSelector.hDataTargetPathEdit) {
                    GetWindowTextW(g_ui.pathSelector.hDataTargetPathEdit, newPath, MAX_PATH);
                    if (wcslen(newPath) >= 2) {
                        WCHAR drivePath[4] = {newPath[0], newPath[1], L'\\', L'\0'};
                        ULONGLONG freeSpace = GetDiskAvailableSpace(drivePath);
                        FormatDiskSpace(freeSpace, sizeText, 64);
                        swprintf_s(spaceText, 128, L"%s: %s", lang->availableSpace, sizeText);
                        SetWindowTextW(g_ui.pathSelector.hDataSpaceLabel, spaceText);
                    }
                }
            }
            }
            break;
            
        case WM_CTLCOLORSTATIC:
            {
            // 设置静态文本/图标透明背景
            HDC hdcStatic = (HDC)wParam;
            HWND hCtrl = (HWND)lParam;
            
            // 设置透明模式和背景色
            SetBkMode(hdcStatic, TRANSPARENT);
            
            // 浏览器状态标签显示不同颜色
            if (hCtrl == g_ui.browserSelector.hChromeStatusLabel) {
                if (g_browsers[BROWSER_CHROME].isInstalled) {
                    SetTextColor(hdcStatic, COLOR_SUCCESS);  // 绿色
                } else {
                    SetTextColor(hdcStatic, COLOR_ERROR);    // 红色
                }
            } else if (hCtrl == g_ui.browserSelector.hEdgeStatusLabel) {
                if (g_browsers[BROWSER_EDGE].isInstalled) {
                    SetTextColor(hdcStatic, COLOR_SUCCESS);  // 绿色
                } else {
                    SetTextColor(hdcStatic, COLOR_ERROR);    // 红色
                }
            } else {
                SetTextColor(hdcStatic, COLOR_TEXT);
            }
            
            // 只读编辑框显示灰色背景
            static HBRUSH hReadOnlyBrush = NULL;
            if (!hReadOnlyBrush) {
                hReadOnlyBrush = CreateSolidBrush(RGB(240, 240, 240));  // 灰色背景
            }
            
            if (hCtrl == g_ui.pathSelector.hAppPathEdit ||
                hCtrl == g_ui.pathSelector.hDataPathEdit) {
                // 只读路径编辑框使用灰色背景
                SetBkColor(hdcStatic, RGB(240, 240, 240));
                return (INT_PTR)hReadOnlyBrush;
            }
            
            // 返回窗口背景色画刷（解决图标黑色背景问题）
            static HBRUSH hBgBrush = NULL;
            if (!hBgBrush) {
                hBgBrush = CreateSolidBrush(COLOR_BG);
            }
            return (INT_PTR)hBgBrush;
            }
            
        case WM_PAINT:
            {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // 绘制背景
            HBRUSH hBgBrush = CreateSolidBrush(COLOR_BG);
            FillRect(hdc, &ps.rcPaint, hBgBrush);
            DeleteObject(hBgBrush);
            
            EndPaint(hwnd, &ps);
            }
            break;
            
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    
    return 0;
}

// 浏览器选择事件
void OnBrowserSelected(BrowserType type)
{
    g_config.selectedBrowser = type;
    UpdateUIState();
    UpdatePathSelectorUI(&g_ui.pathSelector, type);
}

// 更新UI状态
void UpdateUIState()
{
    // 更新浏览器选择器
    UpdateBrowserSelectorUI(&g_ui.browserSelector);
    // 更新磁盘空间显示
    UpdateDiskSpaceDisplay();
}

// 刷新浏览器信息
void RefreshBrowserInfo()
{
    InitializeBrowserInfo(&g_browsers[0], &g_browsers[1]);
    UpdateUIState();
}

// 格式化大小显示
void FormatSize(ULONGLONG size, WCHAR* buffer, int bufferSize)
{
    const WCHAR* units[] = {L"B", L"KB", L"MB", L"GB", L"TB"};
    int unitIndex = 0;
    double dSize = (double)size;
    
    while (dSize >= 1024.0 && unitIndex < 4) {
        dSize /= 1024.0;
        unitIndex++;
    }
    
    swprintf_s(buffer, bufferSize, L"%.2f %s", dSize, units[unitIndex]);
}

// 更新窗口所有文本
void UpdateWindowStrings()
{
    const LanguageStrings* lang = GetCurrentLanguageStrings();
    
    // 更新窗口标题
    SetWindowTextW(g_hMainWindow, lang->windowTitle);
    
    // 更新复选框（跳过NULL指针）
    if (g_ui.options.hMoveAppCheckbox) SetWindowTextW(g_ui.options.hMoveAppCheckbox, lang->moveApp);
    if (g_ui.options.hFixShortcutsCheckbox) SetWindowTextW(g_ui.options.hFixShortcutsCheckbox, lang->fixShortcuts);
    
    // 更新按钮
    SetWindowTextW(g_ui.hMoveButton, lang->moveButton);
    SetWindowTextW(g_ui.hRestoreButton, lang->restoreButton);
    SetWindowTextW(g_ui.hHelpButton, lang->helpButton);
    
    // 更新浏览器选择器和路径选择器
    UpdateBrowserSelectorUI(&g_ui.browserSelector);
    UpdatePathSelectorUI(&g_ui.pathSelector, g_config.selectedBrowser);
}

// 语言切换事件
void OnLanguageChanged(LanguageType langType)
{
    SetLanguage(langType);
    UpdateWindowStrings();
    
    // 更新菜单对号
    HMENU hMenu = GetMenu(g_hMainWindow);
    HMENU hLangMenu = GetSubMenu(hMenu, 0);
    const LanguageStrings* lang = GetCurrentLanguageStrings();
    
    CheckMenuItem(hLangMenu, ID_MENU_ENGLISH, langType == LANG_EN ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hLangMenu, ID_MENU_CHINESE, langType == LANG_ZH ? MF_CHECKED : MF_UNCHECKED);
    ModifyMenuW(hMenu, 0, MF_BYPOSITION | MF_POPUP, (UINT_PTR)hLangMenu, lang->menuLanguage);
    DrawMenuBar(g_hMainWindow);
    
    // 刷新整个窗口以清除文字重叠
    InvalidateRect(g_hMainWindow, NULL, TRUE);
    UpdateWindow(g_hMainWindow);
}

// 创建状态UI
void CreateStatusBar(HWND hwnd)
{
    // 获取窗口大小以确定位置
    RECT rc;
    GetClientRect(hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    
    // 字体
    HFONT hFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             GB2312_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                             DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei UI");

    // 1. 创建状态文本标签
    g_ui.hStatusLabel = CreateWindowExW(
        0, L"STATIC", GetCurrentLanguageStrings()->ready,
        WS_CHILD | WS_VISIBLE,
        15, height - 55, width - 30, 20,
        hwnd, NULL, g_hInstance, NULL
    );
    SendMessageW(g_ui.hStatusLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // 2. 创建进度条（直接作为主窗口的子窗口，不再嵌入状态栏）
    g_ui.hProgressBar = CreateWindowExW(
        0, PROGRESS_CLASSW, NULL,
        WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
        15, height - 30, width - 30, 18,
        hwnd, NULL, g_hInstance, NULL
    );
    
    // 设置进度条范围
    SendMessageW(g_ui.hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
    SendMessageW(g_ui.hProgressBar, PBM_SETPOS, 0, 0);
    
    // 初始隐藏进度条
    ShowWindow(g_ui.hProgressBar, SW_HIDE);
}

// 设置状态文本
void SetStatusText(LPCWSTR text)
{
    if (g_ui.hStatusLabel) {
        SetWindowTextW(g_ui.hStatusLabel, text);
    }
}

// 更新磁盘空间显示
void UpdateDiskSpaceDisplay()
{
    BrowserInfo* browser = &g_browsers[g_config.selectedBrowser];
    UpdatePathInfoDisplay(&g_ui.pathSelector, browser);
}

// 设置进度
void SetStatusProgress(int current, int total)
{
    if (g_ui.hProgressBar && total > 0) {
        int percent = (current * 100) / total;
        SendMessageW(g_ui.hProgressBar, PBM_SETPOS, percent, 0);
    }
}

// 显示/隐藏进度条
void ShowStatusProgress(BOOL show)
{
    if (g_ui.hProgressBar) {
        ShowWindow(g_ui.hProgressBar, show ? SW_SHOW : SW_HIDE);
    }
}

// 重置状态栏
void ResetStatusBar()
{
    SetStatusText(GetCurrentLanguageStrings()->ready);
    SetStatusProgress(0, 100);
    ShowStatusProgress(FALSE);
}