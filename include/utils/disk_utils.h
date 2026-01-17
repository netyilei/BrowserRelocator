#ifndef DISK_UTILS_H
#define DISK_UTILS_H

#include "../common.h"

ULONGLONG GetDiskFreeSpace(LPCWSTR path);
ULONGLONG GetDiskTotalSpace(LPCWSTR path);
void FormatDiskSpace(ULONGLONG bytes, WCHAR* buffer, int bufferSize);
BOOL HasEnoughDiskSpace(LPCWSTR path, ULONGLONG requiredBytes);

#endif // DISK_UTILS_H