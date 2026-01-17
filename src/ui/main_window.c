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
#define ID_CHECK_MOVE_DATA 2002
#define ID_CHECK_CLOSE_BROWSER 2003
#define ID_CHECK_CREATE_BACKUP 2004
#define ID_CHECK_FIX_SHORTCUTS 2005
#define ID_EDIT_APP_PATH 3001
#define ID_EDIT_DATA_PATH 3002
#define ID_EDIT_APP_TARGET 3003
#define ID_EDIT_DATA_TARGET 3004
#define ID_BUTTON_BROWSE_APP 4001
#define ID_BUTTON_BROWSE_DATA 4002
#define ID_BUTTON_BROWSE_APP_TARGET 4003
#define ID_BUTTON_BROWSE_DATA_TARGET 4004
#define ID_BUTTON_MOVE 5001
#define ID_BUTTON_RESTORE 5002
#define ID_BUTTON_HELP 5003
#define ID_MENU_LANGUAGE 6001
#define ID_MENU_ENGLISH 6002
#define ID_MENU_CHINESE 6003

// 绘制圆角矩形
void DrawRoundRect(HDC hdc, int x, int y, int width, int height, int radius)
{
    HPEN hPen = CreatePen(PS_SOLID, 1, COLOR_BORDER);
    HBRUSH hBrush = CreateSolidBrush(COLOR_CARD_BG);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    
    RoundRect(hdc, x, y, x + width, y + height, radius, radius);
    
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
    DeleteObject(hBrush);
}

// 绘制分组框
void DrawGroupBox(HWND hwnd, HDC hdc, LPCWSTR title, RECT rect)
{
    HPEN hPen = CreatePen(PS_SOLID, 1, COLOR_BORDER);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    
    // 绘制边框
    Rectangle(hdc, rect.left, rect.top + 8, rect.right, rect.bottom);
    
    // 绘制标题背景
    HBRUSH hBgBrush = CreateSolidBrush(COLOR_BG);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBgBrush);
    RECT titleRect = {rect.left + 10, rect.top - 2, rect.left + 120, rect.top + 14};
    FillRect(hdc, &titleRect, hBgBrush);
    DeleteObject(hBgBrush);
    
    // 绘制标题
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(100, 100, 100));
    HFONT hFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                             DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei UI");
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
    TextOutW(hdc, rect.left + 15, rect.top - 2, title, (int)wcslen(title));
    
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldFont);
    DeleteObject(hPen);
    DeleteObject(hFont);
}

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
    int windowWidth = 680;
    int windowHeight = 720;
    
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
    HMENU hMenuBar = CreateMenu();
    HMENU hLanguageMenu = CreatePopupMenu();
    
    AppendMenuW(hLanguageMenu, MF_STRING, ID_MENU_ENGLISH, L"English");
    AppendMenuW(hLanguageMenu, MF_STRING, ID_MENU_CHINESE, L"中文");
    
    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hLanguageMenu, L"Language / 语言");
    
    SetMenu(hwnd, hMenuBar);
}

// 创建主窗口UI
void CreateMainWindowUI(HWND hwnd)
{
    int yOffset = 20;
    
    // 浏览器选择器
    CreateBrowserSelectorUI(hwnd, &g_ui.browserSelector, 20, yOffset);
    yOffset += 110;
    
    // 搬家选项
    CreateOptionsUI(hwnd, &g_ui.options, 20, yOffset);
    yOffset += 100;
    
    // 路径选择器
    CreatePathSelectorUI(hwnd, &g_ui.pathSelector, 20, yOffset);
    yOffset += 200;
    
    // 操作按钮
    CreateActionButtons(hwnd, &g_ui, 20, yOffset);
}

// 创建选项UI
void CreateOptionsUI(HWND hwnd, OptionsUI* ui, int x, int y)
{
    RECT groupRect = {x, y, x + 640, y + 85};
    
    // 创建复选框
    ui->hMoveAppCheckbox = CreateWindowExW(
        0, L"BUTTON", L"搬家应用程序目录",
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        x + 20, y + 25, 200, 20, hwnd, (HMENU)ID_CHECK_MOVE_APP, g_hInstance, NULL
    );
    SendMessageW(ui->hMoveAppCheckbox, BM_SETCHECK, BST_CHECKED, 0);
    
    ui->hMoveDataCheckbox = CreateWindowExW(
        0, L"BUTTON", L"搬家用户数据",
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        x + 240, y + 25, 200, 20, hwnd, (HMENU)ID_CHECK_MOVE_DATA, g_hInstance, NULL
    );
    SendMessageW(ui->hMoveDataCheckbox, BM_SETCHECK, BST_CHECKED, 0);
    
    ui->hCloseBrowserCheckbox = CreateWindowExW(
        0, L"BUTTON", L"搬家前自动关闭浏览器",
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        x + 20, y + 55, 200, 20, hwnd, (HMENU)ID_CHECK_CLOSE_BROWSER, g_hInstance, NULL
    );
    SendMessageW(ui->hCloseBrowserCheckbox, BM_SETCHECK, BST_CHECKED, 0);
    
    ui->hCreateBackupCheckbox = CreateWindowExW(
        0, L"BUTTON", L"搬家后创建备份",
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        x + 240, y + 55, 200, 20, hwnd, (HMENU)ID_CHECK_CREATE_BACKUP, g_hInstance, NULL
    );
    
    ui->hFixShortcutsCheckbox = CreateWindowExW(
        0, L"BUTTON", L"修复快捷方式",
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        x + 440, y + 55, 160, 20, hwnd, (HMENU)ID_CHECK_FIX_SHORTCUTS, g_hInstance, NULL
    );
    SendMessageW(ui->hFixShortcutsCheckbox, BM_SETCHECK, BST_CHECKED, 0);
}

// 创建操作按钮
void CreateActionButtons(HWND hwnd, MainWindowUI* ui, int x, int y)
{
    // 移动按钮
    ui->hMoveButton = CreateWindowExW(
        0, L"BUTTON", L"开始搬家",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        x, y, 200, 45, hwnd, (HMENU)ID_BUTTON_MOVE, g_hInstance, NULL
    );
    
    // 还原按钮
    ui->hRestoreButton = CreateWindowExW(
        0, L"BUTTON", L"还原",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        x + 220, y, 200, 45, hwnd, (HMENU)ID_BUTTON_RESTORE, g_hInstance, NULL
    );
    
    // 帮助按钮
    ui->hHelpButton = CreateWindowExW(
        0, L"BUTTON", L"帮助",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        x + 440, y, 200, 45, hwnd, (HMENU)ID_BUTTON_HELP, g_hInstance, NULL
    );
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
                    MessageBoxW(hwnd, 
                        L"BrowserGarage 使用说明:\n\n"
                        L"1. 选择要搬家的浏览器\n"
                        L"2. 勾选需要搬家的内容\n"
                        L"3. 选择目标位置\n"
                        L"4. 点击\"开始搬家\"\n\n"
                        L"注意: 搬家前请关闭浏览器",
                        L"帮助", MB_OK | MB_ICONINFORMATION);
                    break;
                    
                case ID_BUTTON_BROWSE_APP:
                    OnPathBrowseClicked(g_ui.pathSelector.hAppPathEdit, FALSE, TRUE);
                    break;
                    
                case ID_BUTTON_BROWSE_DATA:
                    OnPathBrowseClicked(g_ui.pathSelector.hDataPathEdit, FALSE, FALSE);
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
            }
            }
            break;
            
        case WM_CTLCOLORSTATIC:
            // 设置静态文本颜色
            HDC hdcStatic = (HDC)wParam;
            SetBkColor(hdcStatic, COLOR_BG);
            SetTextColor(hdcStatic, COLOR_TEXT);
            return (INT_PTR)GetStockObject(NULL_BRUSH);
            
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
    
    // 更新复选框
    SetWindowTextW(g_ui.options.hMoveAppCheckbox, lang->moveApp);
    SetWindowTextW(g_ui.options.hMoveDataCheckbox, lang->moveData);
    SetWindowTextW(g_ui.options.hCloseBrowserCheckbox, lang->closeBrowser);
    SetWindowTextW(g_ui.options.hCreateBackupCheckbox, lang->createBackup);
    SetWindowTextW(g_ui.options.hFixShortcutsCheckbox, lang->fixShortcuts);
    
    // 更新按钮
    SetWindowTextW(g_ui.hMoveButton, lang->moveButton);
    SetWindowTextW(g_ui.hRestoreButton, lang->restoreButton);
    SetWindowTextW(g_ui.hHelpButton, lang->helpButton);
    
    // 更新路径选择器中的标签
    // Note: Static labels need to be recreated or have IDs
    
    // 更新浏览器选择器
    UpdateBrowserSelectorUI(&g_ui.browserSelector);
    UpdatePathSelectorUI(&g_ui.pathSelector, g_config.selectedBrowser);
}

// 语言切换事件
void OnLanguageChanged(LanguageType lang)
{
    SetLanguage(lang);
    g_language = lang;
    UpdateWindowStrings();
    
    // 显示确认消息
    const LanguageStrings* langStrings = GetCurrentLanguageStrings();
    MessageBoxW(g_hMainWindow, 
        lang == LANG_EN ? L"Language changed to English" : L"语言已切换为中文", 
        L"Language / 语言", MB_OK | MB_ICONINFORMATION);
}