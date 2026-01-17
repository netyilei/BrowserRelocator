#ifndef COMMON_H
#define COMMON_H

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <shellapi.h>
#include <gdiplus.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")

#define MAX_PATH_LENGTH 1024
#define APP_NAME L"BrowserGarage"
#define APP_VERSION L"2.1.0"

#define IDI_APP_ICON 101
#define IDI_CHROME_ICON 102
#define IDI_EDGE_ICON 103
#define IDI_FOLDER_ICON 104

// 颜色定义
#define COLOR_BG RGB(255, 255, 255)
#define COLOR_CARD_BG RGB(250, 250, 250)
#define COLOR_ACCENT RGB(0, 120, 215)
#define COLOR_TEXT RGB(51, 51, 51)
#define COLOR_TEXT_LIGHT RGB(128, 128, 128)
#define COLOR_BORDER RGB(220, 220, 220)
#define COLOR_SUCCESS RGB(76, 175, 80)
#define COLOR_ERROR RGB(244, 67, 54)
#define COLOR_WARNING RGB(255, 152, 0)

#endif // COMMON_H