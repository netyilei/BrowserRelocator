# BrowserGarage 快速测试指南

## 🚀 快速开始

### 第一步：准备图标（3分钟）

#### 方案A：使用图标占位符（最快）
```cmd
cd BrowserGarage
create_placeholder_icons.bat
```

#### 方案B：使用真实图标（推荐）

1. 获取图标文件：
   - `app_icon.ico` - 主程序图标
   - `chrome.ico` - Chrome图标
   - `edge.ico` - Edge图标
   - `folder.ico` - 文件夹图标

2. 放入目录：
   ```
   BrowserGarage/res/icons/
   ```

### 第二步：编译

#### MinGW-w64:
```bash
cd BrowserGarage
make clean
make
```

#### MSVC:
```cmd
cd BrowserGarage
build.bat
```

### 第三步：运行

```cmd
cd BrowserGarage
bin\BrowserGarage.exe
```

程序会自动请求管理员权限，点击"是"即可。

---

## 🧪 功能测试清单

### 基础功能测试

- [ ] 程序启动并显示窗口
- [ ] 窗口标题显示 "BrowserGarage v2.1"
- [ ] 菜单栏显示 "Language / 语言"
- [ ] 浏览器选择框显示 Chrome 和 Edge
- [ ] 搬家选项复选框可以勾选/取消
- [ ] 路径编辑框显示当前路径

### 语言切换测试

1. **默认语言测试**：
   - [ ] 程序启动后默认显示英文
   - [ ] 按钮文字为 "Start Move"、"Restore"、"Help"
   - [ ] 复选框文字为 "Move Application Directory" 等

2. **切换到中文**：
   - 点击菜单 → "Language / 语言" → "中文"
   - [ ] 界面立即切换为中文
   - [ ] 按钮文字变为 "开始搬家"、"还原"、"帮助"
   - [ ] 复选框文字变为 "搬家应用程序目录" 等

3. **切换回英文**：
   - 点击菜单 → "Language / 语言" → "English"
   - [ ] 界面立即切换回英文

### 浏览器选择测试

1. **Chrome 测试**：
   - [ ] 选中 Chrome 单选框
   - [ ] 显示 Chrome 安装路径
   - [ ] 显示 Chrome 用户数据路径
   - [ ] 如果已安装，显示版本号和 "Installed"
   - [ ] 如果未安装，显示 "Not Installed"

2. **Edge 测试**：
   - [ ] 选中 Edge 单选框
   - [ ] 显示 Edge 安装路径
   - [ ] 显示 Edge 用户数据路径
   - [ ] 如果已安装，显示版本号和 "已安装"
   - [ ] 如果未安装，显示 "未安装"

### 路径浏览测试

1. **源路径浏览**：
   - 点击应用程序的 "Browse..." 按钮
   - [ ] 打开文件夹选择对话框
   - [ ] 选择新路径后更新编辑框
   - [ ] 点击用户数据的 "Browse..." 按钮
   - [ ] 打开文件夹选择对话框
   - [ ] 选择新路径后更新编辑框

2. **目标路径浏览**：
   - 点击应用程序目标位置的 "Browse..." 按钮
   - [ ] 打开文件夹选择对话框
   - [ ] 选择目标路径后更新编辑框
   - [ ] 点击用户数据目标位置的 "Browse..." 按钮
   - [ ] 打开文件夹选择对话框
   - [ ] 选择目标路径后更新编辑框

### 搬家功能测试（⚠️ 谨慎测试）

**警告**：此操作会实际移动文件，请先备份数据！

**测试建议**：
1. 创建一个测试文件夹
2. 修改代码将目标路径指向测试文件夹
3. 或使用虚拟机测试

测试步骤：

1. **准备阶段**：
   - [ ] 关闭要搬家的浏览器
   - [ ] 确保目标磁盘有足够空间
   - [ ] 勾选 "搬家应用程序目录"（可选）
   - [ ] 勾选 "搬家用户数据"（推荐）
   - [ ] 勾选 "搬家前自动关闭浏览器"
   - [ ] 勾选 "修复快捷方式"

2. **执行搬家**：
   - 点击 "Start Move" 按钮
   - [ ] 显示进度对话框
   - [ ] 进度条从 0% 增加到 100%
   - [ ] 状态文字实时更新
   - [ ] "Preparing..." → "Closing browser..." → "Moving files..." → "Creating symlink..." → "Fixing registry..." → "Fixing shortcuts..." → "Completed successfully!"
   - [ ] 显示成功提示消息

3. **验证搬家**：
   - [ ] 原路径变为符号链接（Junction）
   - [ ] 目标路径包含实际文件
   - [ ] 浏览器可以正常启动
   - [ ] 书签、扩展等数据完整
   - [ ] 状态显示 "Status: Moved to [target path]"

### 还原功能测试

1. **执行还原**：
   - 点击 "Restore" 按钮
   - [ ] 显示进度对话框
   - [ ] 进度条从 0% 增加到 100%
   - [ ] "Restoring application..." → "Restoring user data..." → "Restore completed!"
   - [ ] 显示成功提示消息

2. **验证还原**：
   - [ ] 符号链接被删除
   - [ ] 文件移回原位置
   - [ ] 浏览器可以正常启动
   - [ ] 数据完整
   - [ ] 状态显示 "Status: Not Moved"

---

## 🐛 常见问题

### 编译失败

**问题**：找不到图标文件
```bat
Error: can't open icon file 'icons/app_icon.ico'
```

**解决**：
```cmd
# 方案1：使用占位符
cd BrowserGarage
create_placeholder_icons.bat

# 方案2：注释掉图标引用
# 编辑 res/resource.rc，注释掉这些行：
# IDI_APP_ICON     ICON "icons/app_icon.ico"
# IDI_CHROME_ICON  ICON "icons/chrome.ico"
# IDI_EDGE_ICON    ICON "icons/edge.ico"
# IDI_FOLDER_ICON  ICON "icons/folder.ico"
```

### 链接错误

**问题**：undefined reference to ...

**解决**：
```bash
# 确保 Makefile 包含所有源文件
# 检查 src/ui/move_handler.c 是否在 SOURCES 列表中
# 检查 src/utils/language.c 是否在 SOURCES 列表中
```

### 权限问题

**问题**：创建符号链接失败

**解决**：
- 确保以管理员权限运行
- 检查 `res/manifest.xml` 中是否设置了 `requireAdministrator`

### 浏览器检测失败

**问题**：显示 "Not Installed" 但浏览器已安装

**解决**：
- 检查浏览器安装路径是否为默认路径
- 如果是自定义路径，使用"浏览"按钮手动选择

### 搬家失败

**问题**：搬家过程中失败

**检查清单**：
- [ ] 浏览器是否已关闭
- [ ] 目标磁盘是否有足够空间
- [ ] 是否有足够的权限
- [ ] 是否有文件被其他程序占用

---

## 📊 性能测试

### 大文件测试

测试不同大小的数据搬家速度：

| 数据大小 | 预计时间 | 备注 |
|----------|----------|------|
| 1GB | 2-5分钟 | SSD最快 |
| 5GB | 5-15分钟 | 取决于磁盘速度 |
| 10GB | 10-30分钟 | HDD可能更慢 |
| 50GB | 30-60分钟 | 大型配置文件 |

### 并发测试

- [ ] 同时搬家 Chrome 和 Edge
- [ ] 多次连续搬家
- [ ] 搬家过程中切换语言

---

## ✅ 成功标志

当以下所有项目都完成时，说明测试成功：

1. ✅ 程序可以编译和运行
2. ✅ 英文界面显示正确
3. ✅ 中文界面显示正确
4. ✅ 语言切换流畅无错误
5. ✅ 浏览器检测准确
6. ✅ 路径浏览功能正常
7. ✅ 搬家功能正常（实际测试）
8. ✅ 还原功能正常（实际测试）
9. ✅ 符号链接创建正确
10. ✅ 注册表修复成功
11. ✅ 快捷方式修复成功
12. ✅ 浏览器可以正常启动和使用

---

## 🎯 测试完成后续

测试成功后，你可以：

1. **准备正式图标**：获取高质量的图标文件
2. **优化编译**：使用 `strip` 减小文件大小
3. **打包发布**：创建安装程序或压缩包
4. **分发测试**：让其他用户测试
5. **收集反馈**：收集用户意见并改进

---

**祝你测试顺利！** 🚀

如有任何问题，请查看：
- `README.md` - 项目说明
- `BUILD.md` - 编译指南
- `UPDATE_LOG.md` - 更新日志
- `IMPLEMENTATION_SUMMARY.md` - 实现细节