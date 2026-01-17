@echo off
REM 创建简单的图标占位符
REM 这个脚本会创建最基本的图标文件，用于测试编译

echo Creating placeholder icons...

cd res\icons

REM 创建临时文本文件来模拟图标（仅用于编译测试）
echo. > app_icon.ico
echo. > chrome.ico
echo. > edge.ico
echo. > folder.ico

echo.
echo Placeholder icons created!
echo Note: These are NOT real icon files.
echo They only allow compilation to succeed.
echo.
echo For production, replace with real icon files.
echo.

cd ..\..

pause