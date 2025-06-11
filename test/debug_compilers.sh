#!/bin/bash

echo "=== 编译器调试信息 ==="

echo "1. 检查Homebrew GCC安装:"
if [ -d "/opt/homebrew/Cellar/gcc" ]; then
    echo "✅ 找到Homebrew GCC安装目录:"
    ls -la /opt/homebrew/Cellar/gcc/

    # 查找GCC版本
    GCC_VERSION=$(ls /opt/homebrew/Cellar/gcc/ | head -1)
    echo "GCC版本: $GCC_VERSION"

    # 检查二进制文件
    if [ -d "/opt/homebrew/Cellar/gcc/$GCC_VERSION/bin" ]; then
        echo "二进制文件目录:"
        ls -la /opt/homebrew/Cellar/gcc/$GCC_VERSION/bin/gcc*
        ls -la /opt/homebrew/Cellar/gcc/$GCC_VERSION/bin/g++*
    fi
else
    echo "❌ 未找到Homebrew GCC安装"
fi

echo ""
echo "2. 检查/opt/homebrew/bin中的GCC链接:"
ls -la /opt/homebrew/bin/gcc* 2>/dev/null || echo "未找到gcc*"
ls -la /opt/homebrew/bin/g++* 2>/dev/null || echo "未找到g++*"

echo ""
echo "3. 检查PATH中的编译器:"
echo "gfortran: $(which gfortran)"
echo "gcc: $(which gcc)"
echo "g++: $(which g++)"

echo ""
echo "4. 尝试使用不同的GCC命令:"
for cmd in gcc-15 gcc-14 gcc-13 gcc; do
    if command -v $cmd >/dev/null 2>&1; then
        echo "✅ $cmd: $(which $cmd)"
        echo "   版本: $($cmd --version | head -1)"
    else
        echo "❌ $cmd: 未找到"
    fi
done

echo ""
echo "5. 尝试使用不同的G++命令:"
for cmd in g++-15 g++-14 g++-13 g++; do
    if command -v $cmd >/dev/null 2>&1; then
        echo "✅ $cmd: $(which $cmd)"
        echo "   版本: $($cmd --version | head -1)"
    else
        echo "❌ $cmd: 未找到"
    fi
done

echo ""
echo "6. 建议的PATH设置:"
echo "请将以下命令添加到你的 ~/.zshrc 或 ~/.bash_profile:"
echo "export PATH=\"/opt/homebrew/bin:\$PATH\""

echo ""
echo "7. 测试简单编译:"

# 测试C编译
echo "测试C编译..."
cat > test_c.c << 'EOF'
#include <stdio.h>
int main() {
    printf("C compilation works!\n");
    return 0;
}
EOF

# 尝试不同的编译器
for compiler in gcc-15 gcc-14 gcc; do
    if command -v $compiler >/dev/null 2>&1; then
        echo "尝试使用 $compiler:"
        if $compiler -o test_c test_c.c 2>/dev/null; then
            echo "✅ $compiler 编译成功"
            ./test_c
            rm -f test_c
            break
        else
            echo "❌ $compiler 编译失败"
        fi
    fi
done

rm -f test_c.c

# 测试C++编译
echo "测试C++编译..."
cat > test_cpp.cpp << 'EOF'
#include <iostream>
int main() {
    std::cout << "C++ compilation works!" << std::endl;
    return 0;
}
EOF

for compiler in g++-15 g++-14 g++; do
    if command -v $compiler >/dev/null 2>&1; then
        echo "尝试使用 $compiler:"
        if $compiler -o test_cpp test_cpp.cpp 2>/dev/null; then
            echo "✅ $compiler 编译成功"
            ./test_cpp
            rm -f test_cpp
            break
        else
            echo "❌ $compiler 编译失败"
        fi
    fi
done

rm -f test_cpp.cpp
