# BrowserGarage Makefile for MinGW-w64

# 编译器
CC = gcc
WINDRES = windres

# 编译选项
CFLAGS = -Wall -O2 -mwindows -DUNICODE -D_UNICODE
LDFLAGS = -mwindows

# 链接库
LIBS = -lgdiplus -lcomctl32 -lshlwapi -lshell32 -lole32 -luuid -lversion

# 源文件
SRCDIR = src
INCDIR = include
OBJDIR = obj
RESDIR = res
TARGETDIR = bin

# 源文件列表
SOURCES = $(SRCDIR)/main.c \
          $(SRCDIR)/ui/main_window.c \
          $(SRCDIR)/ui/browser_selector.c \
          $(SRCDIR)/ui/path_selector.c \
          $(SRCDIR)/ui/progress_dialog.c \
          $(SRCDIR)/ui/move_handler.c \
          $(SRCDIR)/core/browser_detector.c \
          $(SRCDIR)/core/file_mover.c \
          $(SRCDIR)/core/symlink.c \
          $(SRCDIR)/core/registry_fixer.c \
          $(SRCDIR)/core/shortcut_fixer.c \
          $(SRCDIR)/utils/string_utils.c \
          $(SRCDIR)/utils/disk_utils.c \
          $(SRCDIR)/utils/process_utils.c \
          $(SRCDIR)/utils/language.c

# 资源文件
RESOURCE = $(RESDIR)/resource.rc

# 目标文件
TARGET = $(TARGETDIR)/BrowserGarage.exe

# 目标文件列表
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o) $(OBJDIR)/resource.o

# 包含路径
INCLUDES = -I$(INCDIR)

# 默认目标
all: directories $(TARGET)

# 创建必要的目录
directories:
	@mkdir -p $(OBJDIR)/ui $(OBJDIR)/core $(OBJDIR)/utils $(TARGETDIR)

# 编译资源
$(OBJDIR)/resource.o: $(RESOURCE)
	$(WINDRES) -i $(RESOURCE) -o $@

# 编译C文件
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# 链接
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS) $(LIBS)
	@echo "Build complete: $(TARGET)"

# 清理
clean:
	rm -rf $(OBJDIR) $(TARGETDIR)

# 清理并重新构建
rebuild: clean all

# 安装（复制到指定位置）
install: $(TARGET)
	@echo "Installing BrowserGarage..."
	@cp $(TARGET) /c/Program\ Files/BrowserGarage/

# 运行
run: $(TARGET)
	$(TARGET)

.PHONY: all clean rebuild install run directories