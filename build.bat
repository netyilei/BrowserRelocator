@echo off
REM BrowserGarage 构建脚本 (MSVC)

echo ============================================
echo  BrowserGarage 构建脚本 (MSVC)
echo ============================================
echo.

REM 设置环境
set "VCVARS=C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
if not exist "%VCVARS%" (
    set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
)

if exist "%VCVARS%" (
    call "%VCVARS%"
) else (
    echo 未找到 Visual Studio 环境变量脚本
    echo 请确保已安装 Visual Studio 或 Build Tools
    pause
    exit /b 1
)

echo 开始编译...
echo.

REM 创建输出目录
if not exist bin mkdir bin
if not exist obj mkdir obj

REM 编译资源文件
rc /fo obj\resource.res res\resource.rc
if %errorlevel% neq 0 (
    echo 资源文件编译失败！
    pause
    exit /b 1
)

REM 编译源文件
cl.exe /nologo /O2 /DUNICODE /D_UNICODE /Iinclude src\main.c src\ui\*.c src\core\*.c src\utils\*.c /link obj\resource.res gdiplus.lib comctl32.lib shlwapi.lib shell32.lib ole32.lib uuid.lib version.lib /SUBSYSTEM:WINDOWS /OUT:bin\BrowserGarage.exe
if %errorlevel% neq 0 (
    echo 编译失败！
    pause
    exit /b 1
)

echo.
echo ============================================
echo  编译成功！
echo ============================================
echo 输出文件: bin\BrowserGarage.exe
echo.

pause