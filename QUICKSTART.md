# BrowserGarage 快速开始指南

## 第一次编译？

### 步骤 1：准备图标（可跳过）

**快速开始**：你可以先跳过图标，编译程序功能测试！

编辑 `res/resource.rc`，注释掉这些行：
```rc
// IDI_APP_ICON     ICON "icons/app_icon.ico"
// IDI_CHROME_ICON  ICON "icons/chrome.ico"
// IDI_EDGE_ICON    ICON "icons/edge.ico"
// IDI_FOLDER_ICON  ICON "icons/folder.ico"
```

### 步骤 2：编译

#### Windows + MinGW-w64:
```bash
make
```

#### Windows + Visual Studio:
```cmd
build.bat
```

### 步骤 3：运行

```cmd
bin\BrowserGarage.exe
```

程序会自动请求管理员权限，点击"是"即可。

## 项目完成情况

### ✅ 已完成
- [x] 项目结构初始化
- [x] 所有头文件定义
- [x] 主程序入口和窗口框架
- [x] 浏览器检测器
- [x] 路径选择器UI
- [x] 浏览器选择器UI
- [x] 进度对话框
- [x] 文件移动和复制功能
- [x] 符号链接（Junction）创建
- [x] 注册表路径修复
- [x] 快捷方式修复
- [x] 磁盘空间查询
- [x] 进程管理（关闭浏览器）
- [x] 构建脚本（Makefile + build.bat）
- [x] 资源文件和清单

### ⚠️ 待完成（核心功能已就绪，需整合）

以下功能需要将各模块整合到主窗口的事件处理中：

1. **搬家按钮点击事件** (`src/ui/main_window.c`)
   - 关闭浏览器进程
   - 调用文件移动函数
   - 创建符号链接
   - 修复注册表
   - 修复快捷方式
   - 更新进度显示

2. **还原按钮点击事件**
   - 删除符号链接
   - 移回数据
   - 还原注册表

3. **路径浏览按钮事件**
   - 打开文件夹选择对话框
   - 更新路径编辑框

## 测试清单

编译成功后，可以测试以下功能：

- [ ] 程序启动并请求管理员权限
- [ ] 检测到已安装的 Chrome/Edge
- [ ] 显示浏览器版本信息
- [ ] 选择浏览器后自动填充路径
- [ ] 点击"浏览"可以打开文件夹选择
- [ ] 路径信息正确显示大小
- [ ] 复选框可以正常勾选/取消

## 开发建议

### 1. 先测试基本UI

注释掉 `main_window.c` 中未实现的功能调用，确保窗口能正常显示。

### 2. 逐个实现功能

按照以下顺序实现：
1. 路径浏览功能（最简单）
2. 浏览器检测和显示（已完成）
3. 进度对话框（已完成）
4. 文件移动（已完成）
5. 符号链接（已完成）
6. 注册表修复（已完成）
7. 搬家主流程（整合以上功能）

### 3. 调试技巧

在 `main_window.c` 的 `OnMoveButtonClicked()` 函数中添加断点或日志输出。

## 获取图标

如果需要正式版本的图标：

### Chrome 图标
```cmd
# 从 Chrome 安装目录复制
复制: C:\Program Files\Google\Chrome\Application\chrome.exe
到临时文件夹，右键 -> 属性 -> 更改图标 -> 提取图标
```

### Edge 图标
```cmd
# 从 Edge 安装目录复制
复制: C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe
到临时文件夹，右键 -> 属性 -> 更改图标 -> 提取图标
```

### 在线下载
- https://www.iconfinder.com/
- https://www.flaticon.com/

搜索："browser icon", "chrome icon", "edge icon"

下载后重命名为对应的文件名放入 `res/icons/` 目录。

## 下一步？

1. **编译测试**：运行 `make` 或 `build.bat`
2. **UI 调试**：确保窗口正常显示
3. **功能实现**：按照测试清单逐步实现功能
4. **图标准备**：准备正式版本的图标
5. **测试发布**：完整测试后打包发布

## 需要帮助？

查看详细文档：
- `README.md` - 项目概述
- `BUILD.md` - 构建说明

或者直接修改代码测试！