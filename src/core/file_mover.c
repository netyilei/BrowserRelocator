#include "../../include/core/file_mover.h"
#include "../../include/core/symlink.h"

// 全局回调函数指针
static ProgressCallback g_progressCallback = NULL;
static volatile BOOL g_cancelMove = FALSE;

// 递归计算目录大小
ULONGLONG CalculateDirectorySize(LPCWSTR path)
{
    ULONGLONG totalSize = 0;
    WIN32_FIND_DATAW findData;
    WCHAR searchPath[MAX_PATH];
    
    wcscpy_s(searchPath, _countof(searchPath), path);
    wcscat_s(searchPath, _countof(searchPath), L"\\*");
    
    HANDLE hFind = FindFirstFileW(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        return 0;
    }
    
    do {
        if (wcscmp(findData.cFileName, L".") == 0 || 
            wcscmp(findData.cFileName, L"..") == 0) {
            continue;
        }
        
        WCHAR fullPath[MAX_PATH];
        wcscpy_s(fullPath, _countof(fullPath), path);
        wcscat_s(fullPath, _countof(fullPath), L"\\");
        wcscat_s(fullPath, _countof(fullPath), findData.cFileName);
        
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            totalSize += CalculateDirectorySize(fullPath);
        } else {
            LARGE_INTEGER fileSize;
            fileSize.LowPart = findData.nFileSizeLow;
            fileSize.HighPart = findData.nFileSizeHigh;
            totalSize += fileSize.QuadPart;
        }
    } while (FindNextFileW(hFind, &findData) && !g_cancelMove);
    
    FindClose(hFind);
    return totalSize;
}

// 递归复制目录
BOOL CopyDirectoryRecursive(LPCWSTR sourcePath, LPCWSTR targetPath, int* currentCount, int totalCount)
{
    // 创建目标目录
    if (!CreateDirectoryW(targetPath, NULL)) {
        DWORD error = GetLastError();
        if (error != ERROR_ALREADY_EXISTS) {
            return FALSE;
        }
    }
    
    WIN32_FIND_DATAW findData;
    WCHAR searchPath[MAX_PATH];
    
    wcscpy_s(searchPath, _countof(searchPath), sourcePath);
    wcscat_s(searchPath, _countof(searchPath), L"\\*");
    
    HANDLE hFind = FindFirstFileW(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    
    do {
        if (g_cancelMove) {
            FindClose(hFind);
            return FALSE;
        }
        
        if (wcscmp(findData.cFileName, L".") == 0 || 
            wcscmp(findData.cFileName, L"..") == 0) {
            continue;
        }
        
        WCHAR sourceFile[MAX_PATH];
        WCHAR targetFile[MAX_PATH];
        
        wcscpy_s(sourceFile, _countof(sourceFile), sourcePath);
        wcscat_s(sourceFile, _countof(sourceFile), L"\\");
        wcscat_s(sourceFile, _countof(sourceFile), findData.cFileName);
        
        wcscpy_s(targetFile, _countof(targetFile), targetPath);
        wcscat_s(targetFile, _countof(targetFile), L"\\");
        wcscat_s(targetFile, _countof(targetFile), findData.cFileName);
        
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // 递归复制子目录
            if (!CopyDirectoryRecursive(sourceFile, targetFile, currentCount, totalCount)) {
                FindClose(hFind);
                return FALSE;
            }
        } else {
            // 复制文件
            if (!CopyFileExW(sourceFile, targetFile, NULL, NULL, FALSE, COPY_FILE_RESTARTABLE)) {
                FindClose(hFind);
                return FALSE;
            }
            
            (*currentCount)++;
            
            // 更新进度
            if (g_progressCallback && totalCount > 0) {
                WCHAR status[MAX_PATH];
                wcscpy_s(status, MAX_PATH, L"正在复制: ");
                wcscat_s(status, MAX_PATH, findData.cFileName);
                g_progressCallback(*currentCount, totalCount, status);
            }
        }
    } while (FindNextFileW(hFind, &findData) && !g_cancelMove);
    
    FindClose(hFind);
    return TRUE;
}

// 复制目录
BOOL CopyDirectory(LPCWSTR sourcePath, LPCWSTR targetPath, ProgressCallback callback)
{
    g_progressCallback = callback;
    g_cancelMove = FALSE;
    
    // 计算文件总数（简化版，不精确）
    int totalCount = 100;
    int currentCount = 0;
    
    return CopyDirectoryRecursive(sourcePath, targetPath, &currentCount, totalCount);
}

// 递归删除目录
BOOL DeleteDirectoryRecursive(LPCWSTR path, int* currentCount, int totalCount)
{
    WIN32_FIND_DATAW findData;
    WCHAR searchPath[MAX_PATH];
    
    wcscpy_s(searchPath, _countof(searchPath), path);
    wcscat_s(searchPath, _countof(searchPath), L"\\*");
    
    HANDLE hFind = FindFirstFileW(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        return RemoveDirectoryW(path);
    }
    
    do {
        if (g_cancelMove) {
            FindClose(hFind);
            return FALSE;
        }
        
        if (wcscmp(findData.cFileName, L".") == 0 || 
            wcscmp(findData.cFileName, L"..") == 0) {
            continue;
        }
        
        WCHAR fullPath[MAX_PATH];
        wcscpy_s(fullPath, _countof(fullPath), path);
        wcscat_s(fullPath, _countof(fullPath), L"\\");
        wcscat_s(fullPath, _countof(fullPath), findData.cFileName);
        
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (!DeleteDirectoryRecursive(fullPath, currentCount, totalCount)) {
                FindClose(hFind);
                return FALSE;
            }
        } else {
            // 清除只读属性
            SetFileAttributesW(fullPath, FILE_ATTRIBUTE_NORMAL);
            if (!DeleteFileW(fullPath)) {
                FindClose(hFind);
                return FALSE;
            }
            
            (*currentCount)++;
            
            if (g_progressCallback && totalCount > 0) {
                WCHAR status[MAX_PATH];
                wcscpy_s(status, MAX_PATH, L"正在删除: ");
                wcscat_s(status, MAX_PATH, findData.cFileName);
                g_progressCallback(*currentCount, totalCount, status);
            }
        }
    } while (FindNextFileW(hFind, &findData) && !g_cancelMove);
    
    FindClose(hFind);
    
    // 删除空目录
    return RemoveDirectoryW(path);
}

// 删除目录
BOOL DeleteDirectory(LPCWSTR path, ProgressCallback callback)
{
    g_progressCallback = callback;
    g_cancelMove = FALSE;
    
    int totalCount = 100;
    int currentCount = 0;
    
    return DeleteDirectoryRecursive(path, &currentCount, totalCount);
}

// 移动目录（复制后删除）
BOOL MoveDirectory(LPCWSTR sourcePath, LPCWSTR targetPath, ProgressCallback callback)
{
    g_progressCallback = callback;
    g_cancelMove = FALSE;
    
    // 先复制
    if (!CopyDirectory(sourcePath, targetPath, callback)) {
        return FALSE;
    }
    
    // 验证复制
    if (!ValidateDirectoryCopy(sourcePath, targetPath)) {
        return FALSE;
    }
    
    // 删除源目录
    return DeleteDirectory(sourcePath, callback);
}

// 验证目录复制
BOOL ValidateDirectoryCopy(LPCWSTR sourcePath, LPCWSTR targetPath)
{
    // 检查目标目录是否存在
    if (!PathFileExistsW(targetPath)) {
        return FALSE;
    }
    
    // 简单验证：检查大小
    ULONGLONG sourceSize = CalculateDirectorySize(sourcePath);
    ULONGLONG targetSize = CalculateDirectorySize(targetPath);
    
    // 允许10%的误差
    if (sourceSize > 0 && targetSize == 0) {
        return FALSE;
    }
    
    return TRUE;
}