@echo off
REM ============================================
REM  BrowserGarage Clean Script
REM ============================================

echo ============================================
echo  BrowserGarage Clean Script
echo ============================================
echo.

if exist obj (
    echo [INFO] Removing obj directory...
    rmdir /s /q obj
    echo [OK] obj directory removed
) else (
    echo [INFO] obj directory does not exist
)

if exist bin (
    echo [INFO] Removing bin directory...
    rmdir /s /q bin
    echo [OK] bin directory removed
) else (
    echo [INFO] bin directory does not exist
)

echo.
echo ============================================
echo  Clean Complete!
echo ============================================
echo.
pause
