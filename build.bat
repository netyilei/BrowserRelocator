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

REM Compiler flags (optimized for size)
set CFLAGS=-Wall -Os -mwindows -DUNICODE -D_UNICODE -I%INCDIR% -finput-charset=UTF-8 -fexec-charset=GBK -ffunction-sections -fdata-sections
set LDFLAGS=-mwindows -s -Wl,--gc-sections
set LIBS=-lgdiplus -lcomctl32 -lshlwapi -lshell32 -lole32 -luuid -lversion

REM Create output directories
echo [INFO] Cleaning old build files...
if exist %OBJDIR% rmdir /s /q %OBJDIR%
if exist %TARGETDIR% rmdir /s /q %TARGETDIR%

echo [INFO] Creating directories...
if not exist %OBJDIR% mkdir %OBJDIR%
if not exist %OBJDIR%\ui mkdir %OBJDIR%\ui
if not exist %OBJDIR%\core mkdir %OBJDIR%\core
if not exist %OBJDIR%\utils mkdir %OBJDIR%\utils
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

REM Compile source files
echo [INFO] Compiling source files...

echo   Compiling: main.c
gcc %CFLAGS% -c %SRCDIR%\main.c -o %OBJDIR%\main.o
if %errorlevel% neq 0 goto compile_error

echo   Compiling: ui\main_window.c
gcc %CFLAGS% -c %SRCDIR%\ui\main_window.c -o %OBJDIR%\ui\main_window.o
if %errorlevel% neq 0 goto compile_error

echo   Compiling: ui\browser_selector.c
gcc %CFLAGS% -c %SRCDIR%\ui\browser_selector.c -o %OBJDIR%\ui\browser_selector.o
if %errorlevel% neq 0 goto compile_error

echo   Compiling: ui\path_selector.c
gcc %CFLAGS% -c %SRCDIR%\ui\path_selector.c -o %OBJDIR%\ui\path_selector.o
if %errorlevel% neq 0 goto compile_error

echo   Compiling: ui\progress_dialog.c
gcc %CFLAGS% -c %SRCDIR%\ui\progress_dialog.c -o %OBJDIR%\ui\progress_dialog.o
if %errorlevel% neq 0 goto compile_error

echo   Compiling: ui\move_handler.c
gcc %CFLAGS% -c %SRCDIR%\ui\move_handler.c -o %OBJDIR%\ui\move_handler.o
if %errorlevel% neq 0 goto compile_error

echo   Compiling: core\browser_detector.c
gcc %CFLAGS% -c %SRCDIR%\core\browser_detector.c -o %OBJDIR%\core\browser_detector.o
if %errorlevel% neq 0 goto compile_error

echo   Compiling: core\file_mover.c
gcc %CFLAGS% -c %SRCDIR%\core\file_mover.c -o %OBJDIR%\core\file_mover.o
if %errorlevel% neq 0 goto compile_error

echo   Compiling: core\symlink.c
gcc %CFLAGS% -c %SRCDIR%\core\symlink.c -o %OBJDIR%\core\symlink.o
if %errorlevel% neq 0 goto compile_error

echo   Compiling: core\registry_fixer.c
gcc %CFLAGS% -c %SRCDIR%\core\registry_fixer.c -o %OBJDIR%\core\registry_fixer.o
if %errorlevel% neq 0 goto compile_error

echo   Compiling: core\shortcut_fixer.c
gcc %CFLAGS% -c %SRCDIR%\core\shortcut_fixer.c -o %OBJDIR%\core\shortcut_fixer.o
if %errorlevel% neq 0 goto compile_error

echo   Compiling: utils\string_utils.c
gcc %CFLAGS% -c %SRCDIR%\utils\string_utils.c -o %OBJDIR%\utils\string_utils.o
if %errorlevel% neq 0 goto compile_error

echo   Compiling: utils\disk_utils.c
gcc %CFLAGS% -c %SRCDIR%\utils\disk_utils.c -o %OBJDIR%\utils\disk_utils.o
if %errorlevel% neq 0 goto compile_error

echo   Compiling: utils\process_utils.c
gcc %CFLAGS% -c %SRCDIR%\utils\process_utils.c -o %OBJDIR%\utils\process_utils.o
if %errorlevel% neq 0 goto compile_error

echo   Compiling: utils\language.c
gcc %CFLAGS% -c %SRCDIR%\utils\language.c -o %OBJDIR%\utils\language.o
if %errorlevel% neq 0 goto compile_error

echo [OK] All source files compiled
echo.

REM Link
echo [INFO] Linking...
set OBJECTS=%OBJDIR%\main.o
set OBJECTS=%OBJECTS% %OBJDIR%\ui\main_window.o
set OBJECTS=%OBJECTS% %OBJDIR%\ui\browser_selector.o
set OBJECTS=%OBJECTS% %OBJDIR%\ui\path_selector.o
set OBJECTS=%OBJECTS% %OBJDIR%\ui\progress_dialog.o
set OBJECTS=%OBJECTS% %OBJDIR%\ui\move_handler.o
set OBJECTS=%OBJECTS% %OBJDIR%\core\browser_detector.o
set OBJECTS=%OBJECTS% %OBJDIR%\core\file_mover.o
set OBJECTS=%OBJECTS% %OBJDIR%\core\symlink.o
set OBJECTS=%OBJECTS% %OBJDIR%\core\registry_fixer.o
set OBJECTS=%OBJECTS% %OBJDIR%\core\shortcut_fixer.o
set OBJECTS=%OBJECTS% %OBJDIR%\utils\string_utils.o
set OBJECTS=%OBJECTS% %OBJDIR%\utils\disk_utils.o
set OBJECTS=%OBJECTS% %OBJDIR%\utils\process_utils.o
set OBJECTS=%OBJECTS% %OBJDIR%\utils\language.o
set OBJECTS=%OBJECTS% %OBJDIR%\resource.o

gcc %OBJECTS% -o %TARGETDIR%\BrowserGarage.exe %LDFLAGS% %LIBS%
if %errorlevel% neq 0 (
    echo [ERROR] Linking failed!
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
