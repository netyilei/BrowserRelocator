#include "../../include/ui/progress_dialog.h"
#include "../../include/ui/main_window.h"

HWND g_hProgressDialog = NULL;
HWND g_hProgressBar = NULL;
HWND g_hStatusText = NULL;
HWND g_hDetailText = NULL;
HWND g_hCancelButton = NULL;

// 创建进度对话框
HWND CreateProgressDialog(HWND hParent)
{
    if (g_hProgressDialog) {
        SetForegroundWindow(g_hProgressDialog);
        return g_hProgressDialog;
    }
    
    // 计算窗口位置
    RECT parentRect;
    GetWindowRect(hParent, &parentRect);
    int width = 400;
    int height = 200;
    int x = (parentRect.right + parentRect.left - width) / 2;
    int y = (parentRect.bottom + parentRect.top - height) / 2;
    
    // 创建无边框窗口
    g_hProgressDialog = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_DLGMODALFRAME,
        L"STATIC", L"BrowserGarage",
        WS_POPUP | WS_CAPTION | WS_VISIBLE,
        x, y, width, height,
        hParent, NULL, g_hInstance, NULL
    );
    
    // 创建UI
    HFONT hFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                             DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei UI");
    
    // 状态标签
    g_hStatusText = CreateWindowExW(
        0, L"STATIC", L"准备中...",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        20, 20, 360, 25, g_hProgressDialog, NULL, g_hInstance, NULL
    );
    SendMessageW(g_hStatusText, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // 详情标签
    g_hDetailText = CreateWindowExW(
        0, L"STATIC", L"",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        20, 50, 360, 20, g_hProgressDialog, NULL, g_hInstance, NULL
    );
    SendMessageW(g_hDetailText, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // 进度条
    g_hProgressBar = CreateWindowExW(
        0, PROGRESS_CLASSW, L"",
        WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
        20, 90, 360, 25, g_hProgressDialog, NULL, g_hInstance, NULL
    );
    SendMessageW(g_hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
    
    // 取消按钮
    g_hCancelButton = CreateWindowExW(
        0, L"BUTTON", L"取消",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        150, 140, 100, 30, g_hProgressDialog, NULL, g_hInstance, NULL
    );
    SendMessageW(g_hCancelButton, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    ShowWindow(g_hProgressDialog, SW_SHOW);
    EnableWindow(hParent, FALSE);
    
    return g_hProgressDialog;
}

// 进度对话框过程
LRESULT CALLBACK ProgressDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_COMMAND:
            if (LOWORD(wParam) == IDCANCEL) {
                // 取消操作（待实现）
                CloseProgressDialog();
            }
            break;
            
        case WM_CLOSE:
            return 0;
            
        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// 更新进度
void UpdateProgress(int current, int total, LPCWSTR status)
{
    if (!g_hProgressDialog) return;
    
    int percent = 0;
    if (total > 0) {
        percent = (int)((double)current / total * 100);
    }
    
    SendMessageW(g_hProgressBar, PBM_SETPOS, percent, 0);
    SetWindowTextW(g_hStatusText, status);
    
    // 更新窗口
    UpdateWindow(g_hProgressDialog);
}

// 设置进度详情
void SetProgressDetail(LPCWSTR detail)
{
    if (!g_hProgressDialog) return;
    
    SetWindowTextW(g_hDetailText, detail);
    UpdateWindow(g_hProgressDialog);
}

// 关闭进度对话框
void CloseProgressDialog()
{
    if (g_hProgressDialog) {
        EnableWindow(g_hMainWindow, TRUE);
        DestroyWindow(g_hProgressDialog);
        g_hProgressDialog = NULL;
        SetForegroundWindow(g_hMainWindow);
    }
}