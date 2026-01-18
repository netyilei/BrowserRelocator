#ifndef DISK_UTILS_H
#define DISK_UTILS_H

#include "../common.h"

ULONGLONG GetDiskAvailableSpace(LPCWSTR path);
ULONGLONG GetDiskTotalSpaceBytes(LPCWSTR path);
void FormatDiskSpace(ULONGLONG bytes, WCHAR* buffer, int bufferSize);
BOOL HasEnoughDiskSpace(LPCWSTR path, ULONGLONG requiredBytes);

#endif // DISK_UTILS_H
