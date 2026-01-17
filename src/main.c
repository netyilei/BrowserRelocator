#include "../include/common.h"
#include "../include/browser_types.h"
#include "../include/ui/main_window.h"
#include "../include/ui/progress_dialog.h"

// 全局变量
HINSTANCE g_hInstance = NULL;
HWND g_hMainWindow = NULL;
MainWindowUI g_ui;
Config g_config;
BrowserInfo g_browsers[2];
Gdiplus::GdiplusStartupInput g_gdiplusStartupInput;
ULONG_PTR g_gdiplusToken = 0;

// 初始化GDI+
BOOL InitializeGDIPlus()
{
    Gdiplus::Status status = Gdiplus::GdiplusStartup(&g_gdiplusToken, &g_gdiplusStartupInput, NULL);
    return status == Gdiplus::Ok;
}

// 清理GDI+
void CleanupGDIPlus()
{
    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
    }
}

// 初始化配置
void InitializeConfig()
{
    ZeroMemory(&g_config, sizeof(Config));
    g_config.selectedBrowser = BROWSER_CHROME;
    g_config.moveApp = TRUE;
    g_config.moveData = TRUE;
    g_config.closeBrowserBeforeMove = TRUE;
    g_config.createBackup = FALSE;
    g_config.fixShortcuts = TRUE;
    wcscpy_s(g_config.targetBasePath, MAX_PATH, L"D:\\BrowserData");
}

// 检查管理员权限
BOOL IsRunningAsAdmin()
{
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;
    SID_IDENTIFIER_AUTHORITY authority = SECURITY_NT_AUTHORITY;
    
    if (AllocateAndInitializeSid(&authority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup)) {
        if (!CheckTokenMembership(NULL, adminGroup, &isAdmin)) {
            isAdmin = FALSE;
        }
        FreeSid(adminGroup);
    }
    
    return isAdmin;
}

// 请求管理员权限
void RunAsAdmin()
{
    WCHAR szPath[MAX_PATH];
    GetModuleFileNameW(NULL, szPath, MAX_PATH);
    
    SHELLEXECUTEINFOW sei = {0};
    sei.cbSize = sizeof(SHELLEXECUTEINFOW);
    sei.lpVerb = L"runas";
    sei.lpFile = szPath;
    sei.hwnd = NULL;
    sei.nShow = SW_NORMAL;
    
    if (!ShellExecuteExW(&sei)) {
        DWORD dwError = GetLastError();
        if (dwError == ERROR_CANCELLED) {
            MessageBoxW(NULL, L"需要管理员权限才能运行此程序", APP_NAME, MB_OK | MB_ICONWARNING);
        }
    }
}

// WinMain 入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 检查管理员权限
    if (!IsRunningAsAdmin()) {
        RunAsAdmin();
        return 0;
    }
    
    g_hInstance = hInstance;
    
    // 初始化GDI+
    if (!InitializeGDIPlus()) {
        MessageBoxW(NULL, L"无法初始化GDI+", APP_NAME, MB_OK | MB_ICONERROR);
        return 1;
    }
    
    // 初始化通用控件
    INITCOMMONCONTROLSEX iccex = {0};
    iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    iccex.dwICC = ICC_STANDARD_CLASSES | ICC_PROGRESS_CLASS;
    InitCommonControlsEx(&iccex);
    
    // 初始化配置
    InitializeConfig();
    
    // 创建主窗口
    g_hMainWindow = CreateMainWindow(hInstance);
    if (!g_hMainWindow) {
        CleanupGDIPlus();
        return 1;
    }
    
    // 显示窗口
    ShowWindow(g_hMainWindow, nCmdShow);
    UpdateWindow(g_hMainWindow);
    
    // 消息循环
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    
    // 清理
    CleanupGDIPlus();
    
    return (int)msg.wParam;
}