#ifndef PATH_SELECTOR_H
#define PATH_SELECTOR_H

#include "../common.h"
#include "../browser_types.h"

void CreatePathSelectorUI(HWND hParent, PathSelectorUI* ui, int x, int y);
void UpdatePathSelectorUI(PathSelectorUI* ui, BrowserType browserType);
void UpdatePathInfoDisplay(PathSelectorUI* ui, BrowserInfo* browser);
void BrowseForPath(HWND hParent, HWND hEdit, BOOL isFolder);

#endif // PATH_SELECTOR_H