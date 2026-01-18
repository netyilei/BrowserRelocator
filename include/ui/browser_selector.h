#ifndef BROWSER_SELECTOR_H
#define BROWSER_SELECTOR_H

#include "../common.h"
#include "../browser_types.h"

extern HINSTANCE g_hInstance;

void CreateBrowserSelectorUI(HWND hParent, BrowserSelectorUI* ui, int x, int y);
LRESULT CALLBACK BrowserSelectorProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
void UpdateBrowserSelectorUI(BrowserSelectorUI* ui);

#endif // BROWSER_SELECTOR_H