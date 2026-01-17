#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include "../common.h"

BOOL ReplaceString(WCHAR* str, LPCWSTR oldStr, LPCWSTR newStr, WCHAR* outStr, int outSize);
BOOL ReplacePathInString(WCHAR* str, LPCWSTR oldPath, LPCWSTR newPath, WCHAR* outStr, int outSize);
void Trim(WCHAR* str);
BOOL EndsWith(LPCWSTR str, LPCWSTR suffix);
BOOL StartsWith(LPCWSTR str, LPCWSTR prefix);
WCHAR* ExtractFileName(LPCWSTR path);
WCHAR* ExtractFilePath(LPCWSTR path);
void CombinePath(LPCWSTR basePath, LPCWSTR relativePath, WCHAR* result, int resultSize);

#endif // STRING_UTILS_H