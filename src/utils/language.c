#include "../../include/language.h"

static LanguageType g_currentLanguage = LANG_ZH;
static LanguageStrings g_strings;

// 英文字符串
static const LanguageStrings g_english = {
    // Window
    L"BrowserGarage v2.1",
    
    // Browser Selection
    L"Select Browser",
    L"Chrome",
    L"Edge",
    L"Installed",
    L"Not Installed",
    L"v",
    
    // Options
    L"Move Options",
    L"Application Directory",
    L"User Data",
    L"Close Browser Before Move",
    L"Fix Shortcuts",
    
    // Paths
    L"Application Directory",
    L"User Data Directory",
    L"Current Path:",
    L"Target Path:",
    L"Browse",
    
    // Status
    L"Status:",
    L"Status: Not Moved",
    L"Status: Moved to %s",
    L"Size:",
    L"Target Disk Free Space:",
    L"Ready",
    
    // Buttons
    L"Start Move",
    L"Restore",
    L"Help",
    L"Cancel",
    
    // Progress - Move
    L"Preparing...",
    L"Closing browser...",
    L"Moving files...",
    L"Creating symlink...",
    L"Fixing registry...",
    L"Fixing shortcuts...",
    L"Completed!",
    L"Browser relocation completed!",
    
    // Progress - Restore
    L"Restoring application...",
    L"Restoring user data...",
    L"Restore completed!",
    L"Preparing restore...",
    
    // Menu
    L"Language",
    L"English",
    L"中文",
    
    // Dialog titles
    L"Help",
    L"BrowserGarage Usage:\n\n"
    L"1. Select the browser to relocate\n"
    L"2. Check the items you want to move\n"
    L"3. Set the target location\n"
    L"4. Click \"Start Move\"\n\n"
    L"Note: Please close the browser before moving.",
    L"Confirm",
    L"Moving requires closing the browser.\n\n"
    L"The program will automatically close running browser processes.\n"
    L"Please make sure all important data is saved.\n\n"
    L"Continue?",
    L"Restoring requires closing the browser.\n\n"
    L"The program will automatically close running browser processes.\n"
    L"Please make sure all important data is saved.\n\n"
    L"Continue?",
    L"Error",
    L"Success",
    L"Info",
    
    // Error messages
    L"Administrator privileges required to run this program",
    L"Failed to initialize GDI+",
    L"No browser detected!\n\n"
    L"Please fill in browser directories manually.\n\n"
    L"Chrome default paths:\n"
    L"  App: C:\\Program Files\\Google\\Chrome\\Application\n"
    L"  Data: %LOCALAPPDATA%\\Google\\Chrome\\User Data\n\n"
    L"Edge default paths:\n"
    L"  App: C:\\Program Files (x86)\\Microsoft\\Edge\\Application\n"
    L"  Data: %LOCALAPPDATA%\\Microsoft\\Edge\\User Data",
    L"No Browser Detected",
    L"Insufficient disk space on target drive",
    L"Nothing to restore",
    
    // File operations
    L"Copying: ",
    L"Deleting: ",
};

// 中文字符串
static const LanguageStrings g_chinese = {
    // Window
    L"BrowserGarage v2.1",
    
    // Browser Selection
    L"选择浏览器",
    L"Chrome",
    L"Edge",
    L"已安装",
    L"未安装",
    L"v",
    
    // Options
    L"搬家选项",
    L"应用程序目录",
    L"用户数据",
    L"搬家前自动关闭浏览器",
    L"修复快捷方式",
    
    // Paths
    L"应用程序目录",
    L"用户数据目录",
    L"当前路径:",
    L"目标路径:",
    L"浏览",
    
    // Status
    L"状态:",
    L"状态: 未移动",
    L"状态: 已移动到 %s",
    L"大小:",
    L"目标磁盘可用空间:",
    L"就绪",
    
    // Buttons
    L"开始搬家",
    L"还原",
    L"帮助",
    L"取消",
    
    // Progress - Move
    L"准备中...",
    L"正在关闭浏览器...",
    L"正在移动文件...",
    L"正在创建符号链接...",
    L"正在修复注册表...",
    L"正在修复快捷方式...",
    L"操作完成！",
    L"浏览器搬家完成！",
    
    // Progress - Restore
    L"正在还原应用程序...",
    L"正在还原用户数据...",
    L"还原完成！",
    L"准备还原...",
    
    // Menu
    L"语言",
    L"English",
    L"中文",
    
    // Dialog titles
    L"帮助",
    L"BrowserGarage 使用说明:\n\n"
    L"1. 选择要搬家的浏览器\n"
    L"2. 勾选需要搬家的内容\n"
    L"3. 选择目标位置\n"
    L"4. 点击\"开始搬家\"\n\n"
    L"注意: 搬家前请关闭浏览器",
    L"确认操作",
    L"执行搬家操作前需要关闭浏览器\n\n"
    L"程序将自动关闭正在运行的浏览器进程\n"
    L"请确保已保存所有重要数据\n\n"
    L"是否继续？",
    L"执行还原操作前需要关闭浏览器\n\n"
    L"程序将自动关闭正在运行的浏览器进程\n"
    L"请确保已保存所有重要数据\n\n"
    L"是否继续？",
    L"错误",
    L"成功",
    L"提示",
    
    // Error messages
    L"需要管理员权限才能运行此程序",
    L"无法初始化GDI+",
    L"未检测到已安装的浏览器！\n\n"
    L"请手动填写浏览器相关目录。\n\n"
    L"Chrome 默认路径:\n"
    L"  应用: C:\\Program Files\\Google\\Chrome\\Application\n"
    L"  数据: %LOCALAPPDATA%\\Google\\Chrome\\User Data\n\n"
    L"Edge 默认路径:\n"
    L"  应用: C:\\Program Files (x86)\\Microsoft\\Edge\\Application\n"
    L"  数据: %LOCALAPPDATA%\\Microsoft\\Edge\\User Data",
    L"未检测到浏览器",
    L"目标磁盘空间不足",
    L"没有需要还原的内容",
    
    // File operations
    L"正在复制: ",
    L"正在删除: ",
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

// 获取当前语言字符串表
const LanguageStrings* GetCurrentLanguageStrings()
{
    return &g_strings;
}

