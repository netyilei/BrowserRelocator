#ifndef BROWSER_DETECTOR_H
#define BROWSER_DETECTOR_H

#include "../common.h"
#include "../browser_types.h"

BOOL IsBrowserInstalled(BrowserType type, BrowserInfo* info);
void DetectUserDataPath(BrowserType type, BrowserInfo* info);
BOOL GetBrowserVersion(BrowserType type, WCHAR* version, int size);
BOOL GetBrowserAppPath(BrowserType type, WCHAR* path, int size);
void InitializeBrowserInfo(BrowserInfo* chrome, BrowserInfo* edge);
void RefreshBrowserStatus(BrowserInfo* info);

#endif // BROWSER_DETECTOR_H