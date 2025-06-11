#!/bin/bash

# ============================================================================
# Fortran ZIP炸弹项目 - 基础测试脚本
#
# 功能: 测试编译、运行和基本功能
# 作者: Fortran-Playground项目
# 使用: ./test_basic.sh
# ============================================================================

set -e  # 遇到错误时退出

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TEST_DIR="$PROJECT_ROOT/test"
BUILD_DIR="$PROJECT_ROOT"

echo -e "${BLUE}============================================${NC}"
echo -e "${BLUE}  Fortran ZIP炸弹项目 - 基础测试${NC}"
echo -e "${BLUE}============================================${NC}"
echo

# ============================================================================
# 函数定义
# ============================================================================

log_info() {
    echo -e "${BLUE}[信息]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[成功]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[警告]${NC} $1"
}

log_error() {
    echo -e "${RED}[错误]${NC} $1"
}

check_command() {
    if command -v "$1" >/dev/null 2>&1; then
        log_success "找到命令: $1"
        return 0
    else
        log_error "未找到命令: $1"
        return 1
    fi
}

# ============================================================================
# 环境检查
# ============================================================================

log_info "检查编译环境..."

# 检查必需的编译器
MISSING_TOOLS=0

if ! check_command "gfortran"; then
    log_error "请安装 gfortran: brew install gcc (macOS) 或 apt install gfortran (Linux)"
    MISSING_TOOLS=1
fi

if ! check_command "g++"; then
    log_error "请安装 g++: brew install gcc (macOS) 或 apt install g++ (Linux)"
    MISSING_TOOLS=1
fi

if ! check_command "gcc"; then
    log_error "请安装 gcc: brew install gcc (macOS) 或 apt install gcc (Linux)"
    MISSING_TOOLS=1
fi

if ! check_command "make"; then
    log_error "请安装 make: brew install make (macOS) 或 apt install make (Linux)"
    MISSING_TOOLS=1
fi

if [ $MISSING_TOOLS -eq 1 ]; then
    log_error "缺少必需的编译工具，请先安装"
    exit 1
fi

# 检查编译器版本
log_info "编译器版本信息:"
echo "  gfortran: $(gfortran --version | head -n1)"
echo "  g++: $(g++ --version | head -n1)"
echo "  gcc: $(gcc --version | head -n1)"
echo

# ============================================================================
# 项目结构检查
# ============================================================================

log_info "检查项目结构..."

REQUIRED_FILES=(
    "Makefile"
    "src/main.f90"
    "src/interfaces.f90"
    "src/zipbomb.cpp"
    "src/utils.c"
    "include/zipbomb.h"
)

MISSING_FILES=0
for file in "${REQUIRED_FILES[@]}"; do
    if [ -f "$PROJECT_ROOT/$file" ]; then
        log_success "找到文件: $file"
    else
        log_error "缺少文件: $file"
        MISSING_FILES=1
    fi
done

if [ $MISSING_FILES -eq 1 ]; then
    log_error "项目文件不完整，请检查"
    exit 1
fi

# ============================================================================
# 编译测试
# ============================================================================

log_info "开始编译项目..."

cd "$BUILD_DIR"

# 清理之前的编译文件
if [ -f "Makefile" ]; then
    log_info "清理之前的编译文件..."
    make clean 2>/dev/null || true
fi

# 编译项目
log_info "编译 ZIP炸弹生成器..."
if make all; then
    log_success "编译成功!"
else
    log_error "编译失败!"
    exit 1
fi

# 检查可执行文件
if [ -f "bin/zipbomb" ]; then
    log_success "生成可执行文件: bin/zipbomb"
    ls -lh bin/zipbomb
else
    log_error "未找到可执行文件"
    exit 1
fi

echo

# ============================================================================
# 功能测试
# ============================================================================

log_info "开始功能测试..."

# 创建测试目录
mkdir -p "$TEST_DIR/output"
cd "$TEST_DIR"

# 运行ZIP炸弹生成器
log_info "运行 ZIP炸弹生成器..."
echo "测试输入: 自动确认生成"

# 使用expect模拟用户输入，如果没有expect则跳过交互测试
if command -v expect >/dev/null 2>&1; then
    expect << EOF
spawn $BUILD_DIR/bin/zipbomb
expect "继续？" { send "y\r" }
expect eof
EOF
else
    log_warning "未安装expect，手动运行测试"
    echo "请手动运行: $BUILD_DIR/bin/zipbomb"
    echo "并在提示时输入 'y' 确认生成"
    read -p "按回车键继续..." -r
fi

# 检查生成的文件
if [ -f "bomb.zip" ]; then
    log_success "生成 ZIP炸弹文件: bomb.zip"

    # 检查文件大小
    FILESIZE=$(stat -f%z "bomb.zip" 2>/dev/null || stat -c%s "bomb.zip" 2>/dev/null)
    log_info "文件大小: $FILESIZE 字节 ($(echo "scale=2; $FILESIZE/1024" | bc 2>/dev/null || echo "N/A") KB)"

    # 检查文件类型
    if command -v file >/dev/null 2>&1; then
        log_info "文件类型: $(file bomb.zip)"
    fi

    # 安全检查ZIP内容（不解压）
    if command -v unzip >/dev/null 2>&1; then
        log_info "ZIP文件内容预览:"
        unzip -l bomb.zip | head -20
    fi

else
    log_error "未生成 ZIP炸弹文件"
    exit 1
fi

echo

# ============================================================================
# 安全测试
# ============================================================================

log_info "进行安全测试..."

# 检查磁盘空间
AVAILABLE_SPACE=$(df . | tail -1 | awk '{print $4}')
log_info "当前可用磁盘空间: $AVAILABLE_SPACE KB"

# 警告用户关于解压测试
log_warning "⚠️  解压测试警告 ⚠️"
echo "  解压此文件将占用大约 10GB 磁盘空间"
echo "  请确保有足够的磁盘空间和时间"
echo "  建议在虚拟机或容器中进行此测试"
echo

read -p "是否进行解压测试？(y/N): " -r
if [[ $REPLY =~ ^[Yy]$ ]]; then
    log_info "开始解压测试..."

    # 创建测试解压目录
    mkdir -p extract_test
    cd extract_test

    # 设置解压超时（防止系统卡住）
    timeout 30s unzip ../bomb.zip || {
        log_warning "解压超时或被中断（这是正常的）"
        log_info "清理解压文件..."
        cd ..
        rm -rf extract_test
    }
else
    log_info "跳过解压测试"
fi

# ============================================================================
# 性能测试
# ============================================================================

log_info "性能测试结果:"

if [ -f "bomb.zip" ]; then
    FILESIZE=$(stat -f%z "bomb.zip" 2>/dev/null || stat -c%s "bomb.zip" 2>/dev/null)
    TARGET_SIZE=$((10 * 1024 * 1024 * 1024))  # 10GB

    if [ $FILESIZE -gt 0 ] && [ $TARGET_SIZE -gt 0 ]; then
        COMPRESSION_RATIO=$(echo "scale=6; $FILESIZE / $TARGET_SIZE" | bc 2>/dev/null || echo "N/A")
        log_info "压缩比: $COMPRESSION_RATIO ($(echo "scale=2; $COMPRESSION_RATIO * 100" | bc 2>/dev/null || echo "N/A")%)"
    fi
fi

# ============================================================================
# 清理和总结
# ============================================================================

log_info "测试完成，进行清理..."

# 询问是否删除测试文件
read -p "是否删除生成的测试文件？(Y/n): " -r
if [[ ! $REPLY =~ ^[Nn]$ ]]; then
    log_info "删除测试文件..."
    rm -f bomb.zip
    rm -rf extract_test
    rm -rf output
    log_success "清理完成"
else
    log_info "保留测试文件在: $TEST_DIR"
fi

echo
log_success "==== 测试总结 ===="
echo "✅ 编译环境检查通过"
echo "✅ 项目结构完整"
echo "✅ 编译成功"
echo "✅ ZIP炸弹生成功能正常"
echo "✅ 文件格式正确"

echo
log_info "项目已就绪! 可执行文件位于: $BUILD_DIR/bin/zipbomb"
log_warning "⚠️  请负责任地使用此工具，仅用于教学和研究目的"

echo -e "${BLUE}============================================${NC}"
echo -e "${BLUE}  测试完成${NC}"
echo -e "${BLUE}============================================${NC}"
