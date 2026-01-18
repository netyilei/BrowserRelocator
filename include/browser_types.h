#ifndef BROWSER_TYPES_H
#define BROWSER_TYPES_H

typedef enum {
    BROWSER_CHROME = 0,
    BROWSER_EDGE = 1,
    BROWSER_NONE = -1
} BrowserType;

typedef struct {
    BrowserType type;
    WCHAR name[32];
    WCHAR displayName[64];
    WCHAR defaultAppPath[MAX_PATH];
    WCHAR defaultDataPath[MAX_PATH];
    WCHAR appPath[MAX_PATH];
    WCHAR dataPath[MAX_PATH];
    WCHAR exeName[32];
    BOOL isInstalled;
    WCHAR version[64];
    
    BOOL appMoved;
    BOOL dataMoved;
    ULONGLONG appSize;
    ULONGLONG dataSize;
    
    WCHAR appTargetPath[MAX_PATH];
    WCHAR dataTargetPath[MAX_PATH];
} BrowserInfo;

typedef struct {
    BrowserType selectedBrowser;
    BOOL moveApp;
    BOOL moveData;
    BOOL closeBrowserBeforeMove;
    BOOL fixShortcuts;
    WCHAR targetBasePath[MAX_PATH];
    
    int totalSteps;
    int currentStep;
    WCHAR currentOperation[256];
} Config;

typedef struct {
    // 应用程序目录区域
    HWND hAppTitle;
    HWND hAppPanel;
    HWND hAppPathLabel;
    HWND hAppPathEdit;
    HWND hAppTargetLabel;
    HWND hAppTargetPathEdit;
    HWND hAppBrowseBtn;
    HWND hAppSizeLabel;
    HWND hAppStatusLabel;
    HWND hAppSpaceLabel;
    
    // 用户数据目录区域
    HWND hDataTitle;
    HWND hDataPanel;
    HWND hDataPathLabel;
    HWND hDataPathEdit;
    HWND hDataTargetLabel;
    HWND hDataTargetPathEdit;
    HWND hDataBrowseBtn;
    HWND hDataSizeLabel;
    HWND hDataStatusLabel;
    HWND hDataSpaceLabel;
} PathSelectorUI;

typedef struct {
    HWND hChromeRadio;
    HWND hEdgeRadio;
    HWND hChromeStatusLabel;
    HWND hEdgeStatusLabel;
    HWND hChromeVersionLabel;
    HWND hEdgeVersionLabel;
    HWND hChromeIcon;
    HWND hEdgeIcon;
} BrowserSelectorUI;

typedef struct {
    HWND hMoveAppCheckbox;
    HWND hMoveDataCheckbox;
    HWND hCloseBrowserCheckbox;
    HWND hFixShortcutsCheckbox;
} OptionsUI;

typedef struct {
    BrowserSelectorUI browserSelector;
    PathSelectorUI pathSelector;
    OptionsUI options;
    HWND hMoveButton;
    HWND hRestoreButton;
    HWND hHelpButton;
    HWND hStatusBar;
    HWND hProgressBar;
} MainWindowUI;

#endif // BROWSER_TYPES_H