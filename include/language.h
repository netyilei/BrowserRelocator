#ifndef LANGUAGE_H
#define LANGUAGE_H

#include "../common.h"

typedef enum {
    LANG_EN = 0,
    LANG_ZH = 1
} LanguageType;

// UI字符串定义
typedef struct {
    // 窗口标题
    WCHAR windowTitle[128];
    
    // 浏览器选择
    WCHAR chrome[32];
    WCHAR edge[32];
    WCHAR installed[32];
    WCHAR notInstalled[32];
    WCHAR versionPrefix[32];
    
    // 搬家选项
    WCHAR moveApp[64];
    WCHAR moveData[64];
    WCHAR closeBrowser[64];
    WCHAR createBackup[64];
    WCHAR fixShortcuts[64];
    
    // 路径
    WCHAR appPath[32];
    WCHAR dataPath[32];
    WCHAR targetPath[32];
    WCHAR browse[32];
    
    // 状态
    WCHAR status[32];
    WCHAR statusNotMoved[64];
    WCHAR statusMoved[64];
    WCHAR size[32];
    WCHAR availableSpace[64];
    
    // 按钮
    WCHAR moveButton[32];
    WCHAR restoreButton[32];
    WCHAR helpButton[32];
    WCHAR cancelButton[32];
    
    // 进度
    WCHAR preparing[64];
    WCHAR closingBrowser[64];
    WCHAR movingFiles[64];
    WCHAR creatingSymlink[64];
    WCHAR fixingRegistry[64];
    WCHAR fixingShortcuts[64];
    WCHAR completed[64];
    
    // 菜单
    WCHAR menuLanguage[32];
    WCHAR menuEnglish[32];
    WCHAR menuChinese[32];
} LanguageStrings;

// 语言切换函数
void SetLanguage(LanguageType lang);
LanguageType GetLanguage();
LPCWSTR GetLocalizedString(LPCWSTR key);
void UpdateAllUIStrings();
const LanguageStrings* GetCurrentLanguageStrings();

#endif // LANGUAGE_H