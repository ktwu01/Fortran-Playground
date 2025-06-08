# Fortran ZIP炸弹项目 Makefile
# 支持 macOS 和 Linux

# 编译器设置
FC = gfortran
CC = gcc
CXX = g++

# 编译标志
FFLAGS = -std=f2008 -Wall -Wextra -g -O2
CFLAGS = -Wall -Wextra -g -O2 -fPIC
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O2 -fPIC

# 目录设置
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

# 源文件
FSRC = $(SRCDIR)/main.f90 $(SRCDIR)/interfaces.f90
CSRC = $(SRCDIR)/utils.c
CXXSRC = $(SRCDIR)/zipbomb.cpp

# 目标文件
FOBJ = $(FSRC:$(SRCDIR)/%.f90=$(OBJDIR)/%.o)
COBJ = $(CSRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
CXXOBJ = $(CXXSRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# 主目标
TARGET = $(BINDIR)/zipbomb

# 默认目标
.PHONY: all clean install help

all: $(TARGET)

# 创建目录
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# 编译规则
$(OBJDIR)/%.o: $(SRCDIR)/%.f90 | $(OBJDIR)
	$(FC) $(FFLAGS) -I$(INCDIR) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

# 链接主程序
$(TARGET): $(FOBJ) $(COBJ) $(CXXOBJ) | $(BINDIR)
	$(FC) $(FFLAGS) -o $@ $^ -lstdc++ -lz

# 清理
clean:
	rm -rf $(OBJDIR) $(BINDIR)
	rm -f *.mod *.zip

# 安装（复制到系统路径）
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# 帮助信息
help:
	@echo "可用目标："
	@echo "  all      - 编译所有文件"
	@echo "  clean    - 清理编译文件"
	@echo "  install  - 安装到系统路径"
	@echo "  help     - 显示此帮助"

# 测试目标
test: $(TARGET)
	@echo "运行测试..."
	@mkdir -p test
	@cd test && ../$(TARGET)
	@echo "测试完成，检查 test/ 目录中的文件"

# 依赖关系
$(OBJDIR)/main.o: $(OBJDIR)/interfaces.o
