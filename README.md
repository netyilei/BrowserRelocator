# BrowserGarage

[English](#english) | [中文](#chinese)

---

<a name="english"></a>

# BrowserGarage

A lightweight Windows Chrome/Edge browser migration tool.

## Features

- 🚀 Move browser application directory and user data
- 🔧 Automatically fix registry paths
- 📋 Automatically fix desktop and taskbar shortcuts
- 💡 Simple and intuitive GUI
- 🎨 Modern flat design
- 📊 Real-time disk space and migration progress
- 🌍 Multi-language support (English/Chinese)

## Tech Stack

- **Language**: C (C99)
- **GUI**: Win32 API + GDI+
- **Build**: MinGW-w64 / MSVC
- **Platform**: Windows 7+

## Building

### Using MinGW-w64

```bash
cd BrowserGarage
make
```

### Using MSVC

```cmd
cd BrowserGarage
build.bat
```

## Usage

1. Select the browser to move
2. Check the content you want to move
3. Select target location
4. Click "Start Move"

## Notes

- The program requires administrator privileges
- Please close the browser before migration
- Ensure sufficient disk space on the target drive

## Language

Default language is **English**. You can switch to **Chinese** from the menu:

```
Language / 语言 → English / 中文
```

## License

MIT License

---

<a name="chinese"></a>

# BrowserGarage

一款轻量级的 Windows Chrome/Edge 浏览器搬家工具。

## 功能特性

- 🚀 搬家应用程序目录和用户数据
- 🔧 自动修复注册表路径
- 📋 自动修复桌面和任务栏快捷方式
- 💡 简单直观的图形界面
- 🎨 现代化扁平化设计
- 📊 实时显示磁盘空间和搬家进度
- 🌍 多语言支持（英文/中文）

## 技术栈

- **语言**: C (C99)
- **GUI**: Win32 API + GDI+
- **构建**: MinGW-w64 / MSVC
- **平台**: Windows 7+

## 编译

### 使用 MinGW-w64

```bash
cd BrowserGarage
make
```

### 使用 MSVC

```cmd
cd BrowserGarage
build.bat
```

## 使用说明

1. 选择要搬家的浏览器
2. 勾选需要搬家的内容
3. 选择目标位置
4. 点击"开始搬家"

## 注意事项

- 程序需要管理员权限运行
- 搬家前请关闭浏览器
- 确保目标磁盘有足够空间

## 语言切换

默认语言为**英文**。你可以从菜单切换到**中文**：

```
Language / 语言 → English / 中文
```

## 许可证

MIT License
