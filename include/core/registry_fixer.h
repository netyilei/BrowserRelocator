#ifndef REGISTRY_FIXER_H
#define REGISTRY_FIXER_H

#include "../common.h"
#include "../browser_types.h"

BOOL FixChromeRegistry(LPCWSTR oldPath, LPCWSTR newPath);
BOOL FixEdgeRegistry(LPCWSTR oldPath, LPCWSTR newPath);
BOOL FixBrowserRegistry(BrowserType type, LPCWSTR oldPath, LPCWSTR newPath);
BOOL FixAllRegistryPaths(BrowserInfo* browser);
BOOL BackupRegistry(BrowserType type);
BOOL RestoreRegistryBackup(BrowserType type);

#endif // REGISTRY_FIXER_H