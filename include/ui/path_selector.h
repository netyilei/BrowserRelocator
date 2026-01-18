#ifndef PATH_SELECTOR_H
#define PATH_SELECTOR_H

#include "../common.h"
#include "../browser_types.h"

extern HINSTANCE g_hInstance;

void CreatePathSelectorUI(HWND hParent, PathSelectorUI* ui, int x, int y);
void UpdatePathSelectorUI(PathSelectorUI* ui, BrowserType browserType);
void UpdatePathInfoDisplay(PathSelectorUI* ui, BrowserInfo* browser);
void ShowAppPathSection(PathSelectorUI* ui, BOOL show);
void BrowseForPath(HWND hParent, HWND hEdit, BOOL isFolder);
BOOL BrowseForFolder(HWND hParent, LPCWSTR title, WCHAR* selectedPath, int pathSize);

#endif // PATH_SELECTOR_H