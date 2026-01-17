#ifndef SHORTCUT_FIXER_H
#define SHORTCUT_FIXER_H

#include "../common.h"
#include "../browser_types.h"

BOOL FixShortcutPath(LPCWSTR shortcutPath, LPCWSTR oldPath, LPCWSTR newPath);
BOOL FixAllShortcuts(BrowserType type, LPCWSTR oldPath, LPCWSTR newPath);
BOOL FindAndFixShortcuts(BrowserInfo* browser);
BOOL FindShortcutsInFolder(LPCWSTR folderPath, BrowserType type, LPCWSTR oldPath, LPCWSTR newPath);

#endif // SHORTCUT_FIXER_H