#ifndef SYMLINK_H
#define SYMLINK_H

#include "../common.h"

BOOL CreateJunction(LPCWSTR linkPath, LPCWSTR targetPath);
BOOL DeleteJunction(LPCWSTR linkPath);
BOOL IsJunction(LPCWSTR path);
BOOL ResolveJunction(LPCWSTR linkPath, WCHAR* targetPath, int pathSize);
BOOL CreateSymlink(LPCWSTR linkPath, LPCWSTR targetPath);
BOOL DeleteSymlink(LPCWSTR linkPath);

#endif // SYMLINK_H