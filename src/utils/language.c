#include "../../include/language.h"

// 全局语言状态
static LanguageType g_currentLanguage = LANG_EN;
static LanguageStrings g_strings;

// 英文字符串
static const LanguageStrings g_english = {
    // Window
    L"BrowserGarage v2.1",
    
    // Browser Selection
    L"Chrome",
    L"Edge",
    L"Installed",
    L"Not Installed",
    L"v",
    
    // Options
    L"Move Application Directory",
    L"Move User Data",
    L"Close Browser Before Move",
    L"Create Backup After Move",
    L"Fix Shortcuts",
    
    // Paths
    L"Application Directory:",
    L"User Data:",
    L"Target Location:",
    L"Browse...",
    
    // Status
    L"Status:",
    L"Status: Not Moved",
    L"Status: Moved to %s",
    L"Size:",
    L"Target Disk Free Space:",
    
    // Buttons
    L"Start Move",
    L"Restore",
    L"Help",
    L"Cancel",
    
    // Progress
    L"Preparing...",
    L"Closing browser...",
    L"Moving files...",
    L"Creating symlink...",
    L"Fixing registry...",
    L"Fixing shortcuts...",
    L"Completed successfully!",
    
    // Menu
    L"Language",
    L"English",
    L"中文",
};

// 中文字符串
static const LanguageStrings g_chinese = {
    // Window
    L"BrowserGarage v2.1",
    
    // Browser Selection
    L"Chrome",
    L"Edge",
    L"已安装",
    L"未安装",
    L"v",
    
    // Options
    L"搬家应用程序目录",
    L"搬家用户数据",
    L"搬家前自动关闭浏览器",
    L"搬家后创建备份",
    L"修复快捷方式",
    
    // Paths
    L"应用程序目录:",
    L"用户数据:",
    L"目标位置:",
    L"浏览...",
    
    // Status
    L"状态:",
    L"状态: 未移动",
    L"状态: 已移动到 %s",
    L"大小:",
    L"目标磁盘可用空间:",
    
    // Buttons
    L"开始搬家",
    L"还原",
    L"帮助",
    L"取消",
    
    // Progress
    L"准备中...",
    L"正在关闭浏览器...",
    L"正在移动文件...",
    L"正在创建符号链接...",
    L"正在修复注册表...",
    L"正在修复快捷方式...",
    L"操作完成！",
    
    // Menu
    L"语言",
    L"English",
    L"中文",
};

// 设置语言
void SetLanguage(LanguageType lang)
{
    g_currentLanguage = lang;
    
    if (lang == LANG_EN) {
        g_strings = g_english;
    } else {
        g_strings = g_chinese;
    }
}

// 获取当前语言
LanguageType GetLanguage()
{
    return g_currentLanguage;
}

// 获取本地化字符串
LPCWSTR GetLocalizedString(LPCWSTR key)
{
    // 简单实现：直接返回当前语言的对应字符串
    // 实际使用时通过 UpdateAllUIStrings() 更新
    return key;
}

// 获取当前语言字符串表
const LanguageStrings* GetCurrentLanguageStrings()
{
    return &g_strings;
}

// 更新所有UI字符串
void UpdateAllUIStrings()
{
    // 这个函数在 main_window.c 中实现
    extern void UpdateWindowStrings();
    UpdateWindowStrings();
}