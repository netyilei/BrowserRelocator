@echo off
REM ============================================
REM  BrowserGarage Build Script (MinGW-w64)
REM ============================================
setlocal enabledelayedexpansion

echo ============================================
echo  BrowserGarage Build Script (MinGW-w64)
echo ============================================
echo.

REM Check if gcc is available
where gcc >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] gcc not found in PATH
    echo Please install MinGW-w64 and add bin directory to PATH
    echo.
    echo Download: https://www.mingw-w64.org/
    echo Or MSYS2: https://www.msys2.org/
    echo.
    pause
    exit /b 1
)

echo [INFO] Found gcc:
gcc --version | findstr /i "gcc"
echo.

REM Set directories
set SRCDIR=src
set INCDIR=include
set OBJDIR=obj
set RESDIR=res
set TARGETDIR=bin

REM Compiler flags (Extreme Size Optimization)
set CFLAGS=-Wall -Os -mwindows -DUNICODE -D_UNICODE -I%INCDIR% -finput-charset=UTF-8 -fexec-charset=GBK -ffunction-sections -fdata-sections -flto -fno-ident -fno-asynchronous-unwind-tables
set LDFLAGS=-mwindows -s -Wl,--gc-sections -flto
set LIBS=-lgdiplus -lcomctl32 -lshlwapi -lshell32 -lole32 -luuid -lversion

REM Create output directories
echo [INFO] Cleaning old build files...
if exist %OBJDIR% rmdir /s /q %OBJDIR%
if exist %TARGETDIR% rmdir /s /q %TARGETDIR%

echo [INFO] Creating directories...
if not exist %OBJDIR% mkdir %OBJDIR%
if not exist %TARGETDIR% mkdir %TARGETDIR%

REM Compile resource file
echo [INFO] Compiling resources...
windres -i %RESDIR%\resource.rc -o %OBJDIR%\resource.o
if %errorlevel% neq 0 (
    echo [ERROR] Failed to compile resources!
    pause
    exit /b 1
)
echo [OK] Resources compiled

REM Single-Unit Build (Combining all C files for maximum optimization)
echo [INFO] Compiling and Linking (Single-Unit + LTO)...

set SOURCES=%SRCDIR%\main.c
set SOURCES=%SOURCES% %SRCDIR%\ui\main_window.c
set SOURCES=%SOURCES% %SRCDIR%\ui\browser_selector.c
set SOURCES=%SOURCES% %SRCDIR%\ui\path_selector.c
set SOURCES=%SOURCES% %SRCDIR%\ui\move_handler.c
set SOURCES=%SOURCES% %SRCDIR%\core\browser_detector.c
set SOURCES=%SOURCES% %SRCDIR%\core\file_mover.c
set SOURCES=%SOURCES% %SRCDIR%\core\symlink.c
set SOURCES=%SOURCES% %SRCDIR%\core\registry_fixer.c
set SOURCES=%SOURCES% %SRCDIR%\utils\string_utils.c
set SOURCES=%SOURCES% %SRCDIR%\utils\disk_utils.c
set SOURCES=%SOURCES% %SRCDIR%\utils\process_utils.c
set SOURCES=%SOURCES% %SRCDIR%\utils\language.c

gcc %CFLAGS% %SOURCES% %OBJDIR%\resource.o -o %TARGETDIR%\BrowserGarage.exe %LDFLAGS% %LIBS%

if %errorlevel% neq 0 (
    echo [ERROR] Build failed!
    pause
    exit /b 1
)

echo.
echo ============================================
echo  Build Successful!
echo ============================================
echo Output: %TARGETDIR%\BrowserGarage.exe
echo.

REM Show file size
for %%A in (%TARGETDIR%\BrowserGarage.exe) do (
    echo File size: %%~zA bytes
)
echo.
pause
exit /b 0

:compile_error
echo [ERROR] Compilation failed!
pause
exit /b 1
