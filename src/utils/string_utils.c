#include "../../include/utils/string_utils.h"
#include <string.h>

// 替换字符串
BOOL ReplaceString(WCHAR* str, LPCWSTR oldStr, LPCWSTR newStr, WCHAR* outStr, int outSize)
{
    if (!str || !oldStr || !newStr || !outStr) {
        return FALSE;
    }
    
    if (wcsstr(str, oldStr) == NULL) {
        wcscpy_s(outStr, outSize, str);
        return FALSE;
    }
    
    size_t oldLen = wcslen(oldStr);
    size_t newLen = wcslen(newStr);
    size_t strLen = wcslen(str);
    
    // 计算结果字符串最大长度
    size_t maxResultLen = strLen + newLen * 10; // 简化估算
    if (maxResultLen >= (size_t)outSize) {
        return FALSE;
    }
    
    // 临时缓冲区
    WCHAR* temp = (WCHAR*)malloc((maxResultLen + 1) * sizeof(WCHAR));
    if (!temp) {
        return FALSE;
    }
    
    wcscpy_s(temp, maxResultLen + 1, str);
    
    WCHAR* pos = temp;
    WCHAR result[MAX_PATH * 4] = L"";
    size_t resultLen = 0;
    
    while ((pos = wcsstr(pos, oldStr)) != NULL) {
        // 添加旧字符串前的内容
        size_t prefixLen = pos - temp;
        if (resultLen + prefixLen < MAX_PATH * 4) {
            wcsncat_s(result, MAX_PATH * 4, temp, prefixLen);
            resultLen += prefixLen;
        }
        
        // 添加新字符串
        if (resultLen + newLen < MAX_PATH * 4) {
            wcscat_s(result, MAX_PATH * 4, newStr);
            resultLen += newLen;
        }
        
        pos += oldLen;
        temp = pos;
    }
    
    // 添加剩余部分
    wcscat_s(result, MAX_PATH * 4, temp);
    
    wcscpy_s(outStr, outSize, result);
    
    free(temp);
    return TRUE;
}

// 替换路径字符串
BOOL ReplacePathInString(WCHAR* str, LPCWSTR oldPath, LPCWSTR newPath, WCHAR* outStr, int outSize)
{
    if (!str || !oldPath || !newPath || !outStr) {
        return FALSE;
    }
    
    // 尝试直接替换
    if (wcsstr(str, oldPath) != NULL) {
        return ReplaceString(str, oldPath, newPath, outStr, outSize);
    }
    
    // 尝试替换带引号的路径
    WCHAR quotedOldPath[MAX_PATH * 2];
    WCHAR quotedNewPath[MAX_PATH * 2];
    
    swprintf_s(quotedOldPath, MAX_PATH * 2, L"\"%s\"", oldPath);
    swprintf_s(quotedNewPath, MAX_PATH * 2, L"\"%s\"", newPath);
    
    if (wcsstr(str, quotedOldPath) != NULL) {
        return ReplaceString(str, quotedOldPath, quotedNewPath, outStr, outSize);
    }
    
    // 如果没有找到，直接复制
    wcscpy_s(outStr, outSize, str);
    return FALSE;
}

// 去除首尾空格
void Trim(WCHAR* str)
{
    if (!str) return;
    
    WCHAR* start = str;
    while (*start == L' ' || *start == L'\t' || *start == L'\n' || *start == L'\r') {
        start++;
    }
    
    if (start != str) {
        memmove(str, start, (wcslen(start) + 1) * sizeof(WCHAR));
    }
    
    WCHAR* end = str + wcslen(str) - 1;
    while (end >= str && (*end == L' ' || *end == L'\t' || *end == L'\n' || *end == L'\r')) {
        end--;
    }
    *(end + 1) = L'\0';
}

// 检查字符串是否以指定后缀结尾
BOOL EndsWith(LPCWSTR str, LPCWSTR suffix)
{
    if (!str || !suffix) return FALSE;
    
    size_t strLen = wcslen(str);
    size_t suffixLen = wcslen(suffix);
    
    if (suffixLen > strLen) return FALSE;
    
    return wcscmp(str + strLen - suffixLen, suffix) == 0;
}

// 检查字符串是否以指定前缀开头
BOOL StartsWith(LPCWSTR str, LPCWSTR prefix)
{
    if (!str || !prefix) return FALSE;
    
    return wcsncmp(str, prefix, wcslen(prefix)) == 0;
}

// 提取文件名
WCHAR* ExtractFileName(LPCWSTR path)
{
    if (!path) return NULL;
    
    WCHAR* lastSlash = wcsrchr(path, L'\\');
    if (lastSlash) {
        return lastSlash + 1;
    }
    
    WCHAR* lastSlash2 = wcsrchr(path, L'/');
    if (lastSlash2) {
        return lastSlash2 + 1;
    }
    
    return (WCHAR*)path;
}

// 提取文件路径
WCHAR* ExtractFilePath(LPCWSTR path)
{
    static WCHAR result[MAX_PATH];
    
    if (!path) {
        wcscpy_s(result, MAX_PATH, L"");
        return result;
    }
    
    wcscpy_s(result, MAX_PATH, path);
    
    WCHAR* lastSlash = wcsrchr(result, L'\\');
    if (lastSlash) {
        *lastSlash = L'\0';
    }
    
    return result;
}

// 组合路径
void CombinePath(LPCWSTR basePath, LPCWSTR relativePath, WCHAR* result, int resultSize)
{
    if (!basePath || !relativePath || !result) {
        if (result) wcscpy_s(result, resultSize, L"");
        return;
    }
    
    wcscpy_s(result, resultSize, basePath);
    
    // 移除结尾的反斜杠
    size_t len = wcslen(result);
    while (len > 0 && result[len - 1] == L'\\') {
        result[len - 1] = L'\0';
        len--;
    }
    
    // 跳过相对路径开头的反斜杠
    while (*relativePath == L'\\') {
        relativePath++;
    }
    
    wcscat_s(result, resultSize, L"\\");
    wcscat_s(result, resultSize, relativePath);
}