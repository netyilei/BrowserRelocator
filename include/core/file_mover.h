#ifndef FILE_MOVER_H
#define FILE_MOVER_H

#include "../common.h"
#include "../browser_types.h"

typedef void (*ProgressCallback)(int current, int total, LPCWSTR status);

BOOL MoveDirectory(LPCWSTR sourcePath, LPCWSTR targetPath, ProgressCallback callback);
BOOL CopyDirectory(LPCWSTR sourcePath, LPCWSTR targetPath, ProgressCallback callback);
BOOL DeleteDirectory(LPCWSTR path, ProgressCallback callback);
BOOL ValidateDirectoryCopy(LPCWSTR sourcePath, LPCWSTR targetPath);
ULONGLONG CalculateDirectorySize(LPCWSTR path);

#endif // FILE_MOVER_H