#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "../common.h"
#include "../browser_types.h"
#include "../language.h"

extern HWND g_hMainWindow;
extern MainWindowUI g_ui;
extern Config g_config;
extern BrowserInfo g_browsers[2];

// 窗口创建和消息处理
HWND CreateMainWindow(HINSTANCE hInstance);
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// UI 创建函数（前向声明）
void CreateMainMenu(HWND hwnd);
void CreateMainWindowUI(HWND hwnd);
void CreateOptionsUI(HWND hwnd, OptionsUI* ui, int x, int y);
void CreateActionButtons(HWND hwnd, MainWindowUI* ui, int x, int y);

// 事件处理函数
void UpdateUIState();
void RefreshBrowserInfo();
void OnBrowserSelected(BrowserType type);
void OnMoveButtonClicked();
void OnRestoreButtonClicked();
void OnPathBrowseClicked(HWND hEdit, BOOL isTarget, BOOL isApp);
void OnLanguageChanged(LanguageType lang);

// 辅助函数
void FormatSize(ULONGLONG size, WCHAR *buffer, int bufferSize);
void UpdateWindowStrings();

// 状态栏函数
void CreateStatusBar(HWND hwnd);
void SetStatusText(LPCWSTR text);
void SetStatusProgress(int current, int total);
void ShowStatusProgress(BOOL show);
void ResetStatusBar();
void UpdateDiskSpaceDisplay();

#endif // MAIN_WINDOW_H
