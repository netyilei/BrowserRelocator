# BrowserGarage 构建说明

## 系统要求

### 开发环境
- **MinGW-w64** (推荐) 或
- **Microsoft Visual Studio 2019/2022** (带 C++ 构建工具)

### 运行环境
- Windows 7 或更高版本
- 管理员权限（程序会自动请求）

## 编译步骤

### 方法一：使用 MinGW-w64

1. 安装 MinGW-w64
   - 下载：https://www.mingw-w64.org/
   - 或使用 MSYS2：`pacman -S mingw-w64-x86_64-gcc`

2. 编译
   ```bash
   cd BrowserGarage
   make clean
   make
   ```

3. 运行
   ```bash
   make run
   ```

### 方法二：使用 MSVC

1. 安装 Visual Studio 或 Build Tools
   - 下载：https://visualstudio.microsoft.com/downloads/
   - 安装时勾选"使用C++的桌面开发"

2. 双击运行 `build.bat`

3. 运行生成的可执行文件
   ```cmd
   bin\BrowserGarage.exe
   ```

## 图标准备

在首次编译前，需要将图标文件放入 `res/icons/` 目录：

- `app_icon.ico` - 主程序图标
- `chrome.ico` - Chrome 浏览器图标
- `edge.ico` - Edge 浏览器图标
- `folder.ico` - 文件夹图标

图标可以从浏览器安装目录提取或使用在线图标资源。

**临时跳过图标（仅测试编译）：**

1. 编辑 `res/resource.rc`
2. 注释掉或删除图标引用行
3. 编译

## 项目结构

```
BrowserGarage/
├── bin/                # 编译输出目录
│   └── BrowserGarage.exe
├── obj/                # 中间文件目录
├── src/                # 源代码
│   ├── main.c
│   ├── ui/            # UI 组件
│   ├── core/          # 核心功能
│   └── utils/         # 工具函数
├── include/            # 头文件
├── res/                # 资源文件
│   ├── icons/         # 图标
│   ├── resource.rc    # 资源脚本
│   └── manifest.xml   # UAC 清单
├── Makefile           # MinGW 构建脚本
├── build.bat          # MSVC 构建脚本
└── README.md
```

## 编译选项

### Debug 模式

```bash
# MinGW
gcc -g -O0 -mwindows -DUNICODE -D_UNICODE ...

# MSVC
cl /Od /Zi /DUNICODE /D_UNICODE ...
```

### Release 模式

```bash
# MinGW
gcc -O2 -mwindows -DUNICODE -D_UNICODE ...

# MSVC
cl /O2 /DUNICODE /D_UNICODE ...
```

## 常见问题

### 1. 找不到 gdiplus.h

确保包含的库路径正确：
```bash
# MinGW
-L/path/to/mingw/lib

# MSVC
/I/path/to/sdk/include
```

### 2. 资源编译失败

检查 `res/icons/` 目录是否有所需的图标文件。

### 3. 链接错误

确保所有依赖库都已链接：
- gdiplus
- comctl32
- shlwapi
- shell32
- ole32
- uuid
- version

### 4. 权限问题

程序需要管理员权限才能创建符号链接。UAC 清单已配置，首次运行时会自动请求。

## 优化建议

1. **减小体积**：使用 `strip` 命令
   ```bash
   strip bin/BrowserGarage.exe
   ```

2. **UPX 压缩**：进一步减小可执行文件大小
   ```bash
   upx --best bin/BrowserGarage.exe
   ```

3. **静态链接**：避免依赖 DLL
   ```bash
   gcc -static ... (会增加文件大小)
   ```

## 发布清单

发布时需要包含：
- `BrowserGarage.exe`
- `README.md`
- `LICENSE`（如果适用）

**注意**：程序是纯 Win32 应用，不需要额外的运行时库（如果使用静态链接或系统自带库）。