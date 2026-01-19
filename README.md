# BrowserGarage

[English](#english) | [中文](#chinese)

---

<a name="english"></a>

# BrowserGarage

> A lightweight Windows browser migration tool that helped me save my C drive! 🎉

## 📖 The Story

I created this tool because I faced a critical problem: **my C drive was running out of space** (less than 1GB free!) due to Chrome and Edge browsers taking up massive amounts of storage.

After researching various solutions, I realized that browsers store everything - cache, user data, extensions, and more - on system drive. Traditional solutions were either too complex, required manual file copying, or didn't properly handle symlinks and registry updates.

So I decided to create **BrowserGarage** - a simple, one-click solution that:

- Moves browser data to another drive
- Creates transparent symbolic links
- Automatically fixes registry paths
- Updates shortcuts
- All with a clean, intuitive interface

If you're also struggling with C drive space issues like I was, I hope this tool can help you! 💪

## ✨ Features

- 🚀 **One-Click Migration** - Move application directory and user data with a single click
- 💡 **Clean GUI** - Modern flat design with a dedicated real-time progress bar
- 🔗 **Safety-First Strategy** - "Copy-Backup-Link" workflow with strict error halting and auto-rollback
- 🌍 **Multi-language** - English and Chinese support
- 🛡️ **Permission Handling** - Automatically handles system-protected directories in Program Files
- ⚡ **Zero Config Change** - Uses transparent Junctions; no need to touch registry or shortcuts
- 📦 **Extreme Lightweight** - Optimized to only ~111KB using Single-Unit + LTO build
- 🔄 **Easy Restore** - One-click restore to original location

## 💻 Tech Stack

- **Language**: C (C99)
- **GUI**: Win32 API + GDI+
- **Optimization**: Single-Unit Build + LTO (Link Time Optimization)
- **Build**: MinGW-w64
- **Platform**: Windows 7+

## 🏗️ Building

### Prerequisites

- MinGW-w64 (gcc, windres)
- Windows 7 or later

### Method 1: Using build.bat (Recommended)

```cmd
cd BrowserGarage
build.bat
```

### Method 2: Using Make

```bash
cd BrowserGarage
make clean
make
```

### Clean Build Artifacts

```cmd
# Using clean.bat
clean.bat

# Or using Make
make clean
```

## 📖 Usage

1. **Select Browser** - Choose Chrome or Edge to migrate
2. **Choose Options** - Select what to move (app directory, user data)
3. **Pick Destination** - Select target drive/location with enough space
4. **Click "Start Move"** - Sit back and let BrowserGarage do the work!
5. **Done!** - Your browser now runs from the new location

## ⚠️ Important Notes

- ✅ **Administrator privileges required** - Automatically requests UAC elevation for registry and permission modifications
- ✅ **Service Management** - Temporarily disables update services (e.g., EdgeUpdate) to prevent file locking
- ✅ **Data Safety** - Renames original folders to `_bak` instead of deleting them, only cleaning up after full success
- ✅ **Sufficient Disk Space** - Ensure target drive has enough space for migration
- ✅ **Backup Recommended** - While the tool has built-in safety mechanisms, backups are always recommended

## 🌍 Language

Default language is **English**. You can switch to **Chinese** from menu:

```
Language / 语言 → English / 中文
```

## 🎯 How It Works

BrowserGarage uses **Junctions** (directory symbolic links) to create transparent redirects:

```
Before:
C:\Program Files\Chrome\ (Actual files)
C:\Users\You\AppData\Local\Chrome\User Data\ (Actual data)

After Moving to D:\BrowserData\:
C:\Program Files\Chrome\ → D:\BrowserData\Chrome\App\ (Junction link)
C:\Users\You\AppData\Local\Chrome\User Data\ → D:\BrowserData\Chrome\User Data\ (Junction link)
```

Browsers still think they're in the original location, but the data is actually stored on your target drive!

## 💾 What Gets Moved

| Item | Description | Optional? |
|------|-------------|-----------|
| Application Directory | Browser executable and program files | ✅ Yes |
| User Data | Cache, bookmarks, extensions, history | ✅ Yes |
| Registry Paths | Transparently handled via Junction (No change needed) | ✅ Auto |
| Shortcuts | Desktop, taskbar, start menu (No change needed) | ✅ Auto |

## 🐛 Troubleshooting

### Browser won't start after migration

- Check if junction links were created correctly
- Try restoring to original location and re-migrating
- Ensure target drive is accessible

### Migration failed partway

- Don't worry! BrowserGarage only moves files after successful copying
- You can try restoring any partial migration
- Check disk space on target drive

### Need to restore?

- Simply click the "Restore" button in BrowserGarage
- All data will be moved back to the original location

## 🤝 Contributing

This is a personal project that helped me solve my C drive space issue. If you find it useful or want to contribute:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

Suggestions and bug reports are welcome!

## 📄 License

MIT License

## 🙏 Acknowledgments

- Created because my C drive was literally crying for help 😂
- Tested on Windows 10/11 with Chrome and Edge
- Built with love (and desperation) for the community

---

<a name="chinese"></a>

# BrowserGarage

> 一款帮我从C盘告急中拯救出来的浏览器搬家工具！🎉

## 📖 故事由来

我创建这个工具是因为遇到了一个紧迫的问题：**C盘空间不足**（剩余空间不到1GB！）导致Chrome和Edge浏览器占用了大量的存储空间。

经过研究各种解决方案后，我发现浏览器将所有内容 - 缓存、用户数据、扩展程序等 - 都存储在系统盘上。传统的解决方案要么太复杂，需要手动复制文件，要么没有正确处理符号链接和注册表更新。

所以我决定创建 **BrowserGarage** - 一个简单的一键解决方案，它可以：

- 将浏览器数据移动到其他磁盘
- 创建透明的符号链接
- 自动修复注册表路径
- 更新快捷方式
- 所有操作都有一个简洁直观的界面

如果你也像我一样被C盘空间问题困扰，希望这个工具能帮到你！💪

## ✨ 功能特性

- 🚀 **一键搬家** - 单击即可移动应用程序目录和用户数据
- 💡 **简洁GUI** - 现代扁平化设计，配备独立的实时进度条
- 🔗 **安全搬家策略** - 采用“复制-备份-链接”流程，失败自动回滚，确保数据万无一失
- 🌍 **多语言支持** - 支持英文和中文
- 🛡️ **权限突破** - 自动处理 Program Files 目录下受系统保护的 Application 目录
- ⚡ **零配置修改** - 纯 Junction 映射技术，无需修改注册表或快捷方式，完美兼容系统更新
- 📦 **极致轻量** - 采用单单元构建 + LTO 优化，程序体量仅约 111KB
- 🔄 **轻松还原** - 一键还原到原始位置

## 💻 技术栈

- **语言**: C (C99)
- **GUI**: Win32 API + GDI+
- **构建**: MinGW-w64
- **平台**: Windows 7+

## 🏗️ 编译

### 前置要求

- MinGW-w64 (gcc, windres)
- Windows 7 或更高版本

### 方法 1: 使用 build.bat（推荐）

```cmd
cd BrowserGarage
build.bat
```

### 方法 2: 使用 Make

```bash
cd BrowserGarage
make clean
make
```

### 清理编译产物

```cmd
# 使用 clean.bat
clean.bat

# 或使用 Make
make clean
```

## 📖 使用说明

1. **选择浏览器** - 选择要搬家的Chrome或Edge
2. **选择选项** - 勾选要搬家的内容（应用程序目录、用户数据）
3. **选择目标位置** - 选择有足够空间的目标磁盘/位置
4. **点击"开始搬家"** - 放心让BrowserGarage自动完成！
5. **完成！** - 浏览器现在从新位置运行了

## ⚠️ 重要提示

- ✅ **需要管理员权限** - 程序会自动请求UAC提升，用于修改注册表和文件权限
- ✅ **自动禁用更新服务** - 搬家过程中会自动暂时禁用 Edge 等更新服务，防止文件被锁定
- ✅ **数据安全策略** - 原目录会被重命名为 `_bak` 而非直接删除，只有在搬家彻底完成后才清理备份
- ✅ **确保目标磁盘有足够空间** - 目标驱动器必须有足够空间存储浏览器数据
- ✅ **建议备份** - 虽然工具内置了多重保护和熔断机制，但备份重要数据总是明智的选择

## 🌍 语言切换

默认语言为**英文**。你可以从菜单切换到**中文**：

```
Language / 语言 → English / 中文
```

## 🎯 工作原理

BrowserGarage使用**Junctions**（目录符号链接）创建透明的重定向：

```
搬家前：
C:\Program Files\Chrome\ (实际文件)
C:\Users\You\AppData\Local\Chrome\User Data\ (实际数据)

搬家到 D:\BrowserData\ 后：
C:\Program Files\Chrome\ → D:\BrowserData\Chrome\App\ (Junction链接)
C:\Users\You\AppData\Local\Chrome\User Data\ → D:\BrowserData\Chrome\User Data\ (Junction链接)
```

浏览器仍然认为它们在原始位置，但数据实际上存储在目标驱动器上！

## 💾 搬家内容

| 项目 | 说明 | 可选？ |
|------|------|--------|
| 应用程序目录 | 浏览器可执行文件和程序文件 | ✅ 是 |
| 用户数据 | 缓存、书签、扩展程序、历史记录 | ✅ 是 |
| 注册表路径 | 通过 Junction 透明处理 (无需修改) | ✅ 自动 |
| 快捷方式 | 桌面、任务栏、开始菜单 (无需修改) | ✅ 自动 |

## 🐛 故障排除

### 搬家后浏览器无法启动

- 检查Junction链接是否创建正确
- 尝试还原到原始位置并重新搬家
- 确保目标驱动器可访问

### 搬家过程中失败

- 别担心！BrowserGarage只会在复制成功后才移动文件
- 你可以尝试还原部分迁移的数据
- 检查目标驱动器的磁盘空间

### 需要还原？

- 只需在BrowserGarage中点击"还原"按钮
- 所有数据将被移回原始位置

## 🤝 贡献

这是一个个人项目，帮助我解决了C盘空间问题。如果你觉得有用或想贡献：

1. Fork本仓库
2. 创建功能分支
3. 进行修改
4. 提交Pull Request

欢迎建议和bug报告！

## 📄 许可证

MIT License

## 🙏 致谢

- 因为我的C盘空间告急才创建的 😂
- 在Windows 10/11上使用Chrome和Edge测试
- 为社区用爱心（和绝望）构建

---

**希望能帮到同样被C盘空间困扰的你！** 🎉

*Created with love, from one C drive survivor to another ❤️*
