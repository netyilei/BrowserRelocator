#include "../../include/core/symlink.h"
#include <winioctl.h>

// Junction 重解析点结构
typedef struct _REPARSE_DATA_BUFFER {
    ULONG ReparseTag;
    USHORT ReparseDataLength;
    USHORT Reserved;
    union {
        struct {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            ULONG Flags;
            WCHAR PathBuffer[1];
        } SymbolicLinkReparseBuffer;
        struct {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            WCHAR PathBuffer[1];
        } MountPointReparseBuffer;
        struct {
            UCHAR DataBuffer[1];
        } GenericReparseBuffer;
    };
} REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;

#define REPARSE_DATA_BUFFER_HEADER_SIZE FIELD_OFFSET(REPARSE_DATA_BUFFER, GenericReparseBuffer)

// 创建 Junction 链接
BOOL CreateJunction(LPCWSTR linkPath, LPCWSTR targetPath)
{
    HANDLE hDir;
    BYTE reparseBuffer[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
    PREPARSE_DATA_BUFFER pReparseData = (PREPARSE_DATA_BUFFER)reparseBuffer;
    DWORD bytesReturned;
    
    // 格式化目标路径（需要 \??\ 前缀）
    WCHAR targetPathFormatted[MAX_PATH * 2];
    swprintf_s(targetPathFormatted, MAX_PATH * 2, L"\\??\\%s", targetPath);
    
    size_t targetPathLen = wcslen(targetPathFormatted) * sizeof(WCHAR);
    size_t printNameLen = wcslen(targetPath) * sizeof(WCHAR);
    
    // 创建空目录（如果不存在）
    if (!PathFileExistsW(linkPath)) {
        if (!CreateDirectoryW(linkPath, NULL)) {
            DWORD error = GetLastError();
            if (error != ERROR_ALREADY_EXISTS) {
                return FALSE;
            }
        }
    }
    
    // 打开目录句柄
    hDir = CreateFileW(
        linkPath,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
        NULL
    );
    
    if (hDir == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    
    // 填充重解析点数据
    ZeroMemory(reparseBuffer, sizeof(reparseBuffer));
    pReparseData->ReparseTag = IO_REPARSE_TAG_MOUNT_POINT;
    pReparseData->MountPointReparseBuffer.SubstituteNameOffset = 0;
    pReparseData->MountPointReparseBuffer.SubstituteNameLength = (USHORT)targetPathLen;
    pReparseData->MountPointReparseBuffer.PrintNameOffset = (USHORT)(targetPathLen + sizeof(WCHAR));
    pReparseData->MountPointReparseBuffer.PrintNameLength = (USHORT)printNameLen;
    
    memcpy(pReparseData->MountPointReparseBuffer.PathBuffer, targetPathFormatted, targetPathLen + sizeof(WCHAR));
    memcpy((BYTE*)pReparseData->MountPointReparseBuffer.PathBuffer + targetPathLen + sizeof(WCHAR), targetPath, printNameLen + sizeof(WCHAR));
    
    pReparseData->ReparseDataLength = (USHORT)(
        FIELD_OFFSET(REPARSE_DATA_BUFFER, MountPointReparseBuffer.PathBuffer) - REPARSE_DATA_BUFFER_HEADER_SIZE +
        targetPathLen + sizeof(WCHAR) + printNameLen + sizeof(WCHAR)
    );
    
    // 设置重解析点
    BOOL result = DeviceIoControl(
        hDir,
        FSCTL_SET_REPARSE_POINT,
        pReparseData,
        REPARSE_DATA_BUFFER_HEADER_SIZE + pReparseData->ReparseDataLength,
        NULL,
        0,
        &bytesReturned,
        NULL
    );
    
    CloseHandle(hDir);
    return result;
}

// 删除 Junction 链接
BOOL DeleteJunction(LPCWSTR linkPath)
{
    HANDLE hDir;
    BYTE reparseBuffer[REPARSE_DATA_BUFFER_HEADER_SIZE];
    PREPARSE_DATA_BUFFER pReparseData = (PREPARSE_DATA_BUFFER)reparseBuffer;
    DWORD bytesReturned;
    
    // 打开目录句柄
    hDir = CreateFileW(
        linkPath,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
        NULL
    );
    
    if (hDir == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    
    // 填充删除重解析点数据
    ZeroMemory(reparseBuffer, sizeof(reparseBuffer));
    pReparseData->ReparseTag = IO_REPARSE_TAG_MOUNT_POINT;
    pReparseData->ReparseDataLength = 0;
    
    // 删除重解析点
    BOOL result = DeviceIoControl(
        hDir,
        FSCTL_DELETE_REPARSE_POINT,
        pReparseData,
        REPARSE_DATA_BUFFER_HEADER_SIZE,
        NULL,
        0,
        &bytesReturned,
        NULL
    );
    
    CloseHandle(hDir);
    
    // 删除空目录
    if (result) {
        RemoveDirectoryW(linkPath);
    }
    
    return result;
}

// 检查路径是否是 Junction
BOOL IsJunction(LPCWSTR path)
{
    DWORD attributes = GetFileAttributesW(path);
    
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        return FALSE;
    }
    
    if (!(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        return FALSE;
    }
    
    if (!(attributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
        return FALSE;
    }
    
    // 进一步检查重解析点类型
    WIN32_FIND_DATAW findData;
    HANDLE hFind = FindFirstFileW(path, &findData);
    
    if (hFind == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    
    FindClose(hFind);
    
    // 检查是否是 Junction (IO_REPARSE_TAG_MOUNT_POINT)
    if (findData.dwReserved0 == IO_REPARSE_TAG_MOUNT_POINT) {
        return TRUE;
    }
    
    return FALSE;
}

// 解析 Junction 目标路径
BOOL ResolveJunction(LPCWSTR linkPath, WCHAR* targetPath, int pathSize)
{
    HANDLE hDir;
    BYTE reparseBuffer[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
    PREPARSE_DATA_BUFFER pReparseData = (PREPARSE_DATA_BUFFER)reparseBuffer;
    DWORD bytesReturned;
    
    // 打开目录句柄
    hDir = CreateFileW(
        linkPath,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
        NULL
    );
    
    if (hDir == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    
    // 获取重解析点数据
    ZeroMemory(reparseBuffer, sizeof(reparseBuffer));
    
    BOOL result = DeviceIoControl(
        hDir,
        FSCTL_GET_REPARSE_POINT,
        NULL,
        0,
        pReparseData,
        sizeof(reparseBuffer),
        &bytesReturned,
        NULL
    );
    
    CloseHandle(hDir);
    
    if (!result) {
        return FALSE;
    }
    
    // 验证重解析点类型
    if (pReparseData->ReparseTag != IO_REPARSE_TAG_MOUNT_POINT) {
        return FALSE;
    }
    
    // 提取目标路径（跳过 \??\ 前缀）
    WCHAR* printName = pReparseData->MountPointReparseBuffer.PathBuffer +
        (pReparseData->MountPointReparseBuffer.PrintNameOffset / sizeof(WCHAR));
    int printNameLen = pReparseData->MountPointReparseBuffer.PrintNameLength / sizeof(WCHAR);
    
    if (printNameLen > 0 && printNameLen < pathSize) {
        wcsncpy_s(targetPath, pathSize, printName, printNameLen);
        targetPath[printNameLen] = L'\0';
        return TRUE;
    }
    
    // 如果 PrintName 为空，尝试从 SubstituteName 提取（去除 \??\ 前缀）
    WCHAR* substituteName = pReparseData->MountPointReparseBuffer.PathBuffer +
        (pReparseData->MountPointReparseBuffer.SubstituteNameOffset / sizeof(WCHAR));
    int substituteNameLen = pReparseData->MountPointReparseBuffer.SubstituteNameLength / sizeof(WCHAR);
    
    // 跳过 \??\ 前缀（4个字符）
    if (substituteNameLen > 4 && wcsncmp(substituteName, L"\\??\\", 4) == 0) {
        substituteName += 4;
        substituteNameLen -= 4;
    }
    
    if (substituteNameLen > 0 && substituteNameLen < pathSize) {
        wcsncpy_s(targetPath, pathSize, substituteName, substituteNameLen);
        targetPath[substituteNameLen] = L'\0';
        return TRUE;
    }
    
    return FALSE;
}

// 创建符号链接（需要管理员权限或开发者模式）
BOOL CreateSymlink(LPCWSTR linkPath, LPCWSTR targetPath)
{
    // 使用 CreateSymbolicLinkW 创建符号链接
    // 注意：目录需要 SYMBOLIC_LINK_FLAG_DIRECTORY 标志
    DWORD flags = SYMBOLIC_LINK_FLAG_DIRECTORY;
    
    // Windows 10 1703+ 支持不需要管理员权限创建符号链接
    // 需要启用开发者模式
    flags |= SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE;
    
    return CreateSymbolicLinkW(linkPath, targetPath, flags);
}

// 删除符号链接
BOOL DeleteSymlink(LPCWSTR linkPath)
{
    // 符号链接本质上是目录，使用 RemoveDirectory 删除
    return RemoveDirectoryW(linkPath);
}
