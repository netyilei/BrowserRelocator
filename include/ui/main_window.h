#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "../common.h"
#include "../browser_types.h"
#include "../language.h"

extern HWND g_hMainWindow;
extern MainWindowUI g_ui;
extern Config g_config;
extern BrowserInfo g_browsers[2];
extern LanguageType g_language;

HWND CreateMainWindow(HINSTANCE hInstance);
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void UpdateUIState();
void RefreshBrowserInfo();
void OnBrowserSelected(BrowserType type);
void OnMoveButtonClicked();
void OnRestoreButtonClicked();
void OnPathBrowseClicked(HWND hEdit, BOOL isTarget, BOOL isApp);
void FormatSize(ULONGLONG size, WCHAR* buffer, int bufferSize);
void UpdateWindowStrings();
void OnLanguageChanged(LanguageType lang);

#endif // MAIN_WINDOW_H