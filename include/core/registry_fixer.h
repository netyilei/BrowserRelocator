#ifndef REGISTRY_FIXER_H
#define REGISTRY_FIXER_H

#include "../common.h"
#include "../browser_types.h"

// 更新浏览器相关的注册表路径
// browserType: 浏览器类型 (BROWSER_CHROME 或 BROWSER_EDGE)
// oldPath: 旧路径
// newPath: 新路径
// 返回: 成功返回 TRUE，失败返回 FALSE
BOOL FixBrowserRegistryPaths(BrowserType browserType, LPCWSTR oldPath, LPCWSTR newPath);

// 更新 App Paths 注册表
BOOL FixAppPathsRegistry(BrowserType browserType, LPCWSTR newPath);

// 更新浏览器关联 (HTML, HTTP, HTTPS 等)
BOOL FixBrowserAssociations(BrowserType browserType, LPCWSTR oldPath, LPCWSTR newPath);

// 更新默认图标路径
BOOL FixDefaultIconPaths(BrowserType browserType, LPCWSTR oldPath, LPCWSTR newPath);

// 检查并更新特定的注册表值
BOOL UpdateRegistryValueIfMatches(HKEY hRootKey, LPCWSTR subKey, LPCWSTR valueName, 
                                   LPCWSTR oldPath, LPCWSTR newPath);

#endif // REGISTRY_FIXER_H