#include "../../include/utils/disk_utils.h"
#include <lmcons.h>

// 获取磁盘可用空间
ULONGLONG GetDiskFreeSpace(LPCWSTR path)
{
    ULARGE_INTEGER freeBytesAvailable;
    ULARGE_INTEGER totalBytes;
    ULARGE_INTEGER totalFreeBytes;
    
    if (GetDiskFreeSpaceExW(path, &freeBytesAvailable, &totalBytes, &totalFreeBytes)) {
        return freeBytesAvailable.QuadPart;
    }
    
    return 0;
}

// 获取磁盘总空间
ULONGLONG GetDiskTotalSpace(LPCWSTR path)
{
    ULARGE_INTEGER freeBytesAvailable;
    ULARGE_INTEGER totalBytes;
    ULARGE_INTEGER totalFreeBytes;
    
    if (GetDiskFreeSpaceExW(path, &freeBytesAvailable, &totalBytes, &totalFreeBytes)) {
        return totalBytes.QuadPart;
    }
    
    return 0;
}

// 格式化磁盘空间显示
void FormatDiskSpace(ULONGLONG bytes, WCHAR* buffer, int bufferSize)
{
    const WCHAR* units[] = {L"B", L"KB", L"MB", L"GB", L"TB"};
    int unitIndex = 0;
    double dBytes = (double)bytes;
    
    while (dBytes >= 1024.0 && unitIndex < 4) {
        dBytes /= 1024.0;
        unitIndex++;
    }
    
    swprintf_s(buffer, bufferSize, L"%.2f %s", dBytes, units[unitIndex]);
}

// 检查磁盘是否有足够空间
BOOL HasEnoughDiskSpace(LPCWSTR path, ULONGLONG requiredBytes)
{
    ULONGLONG freeSpace = GetDiskFreeSpace(path);
    return freeSpace >= requiredBytes;
}