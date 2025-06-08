# Fortran ZIP炸弹教学项目

## ⚠️ 安全警告
**本项目仅用于教学和研究目的！**
- 禁止用于恶意攻击网站或系统
- 使用前请确保在安全的测试环境中运行
- 生成的文件可能消耗大量磁盘空间和内存
- 请遵守相关法律法规和道德规范

## 项目简介

这是一个多语言协作的教学演示项目，展示如何使用 **Fortran + C++ + C** 创建一个ZIP炸弹生成器。项目演示了：

- 现代Fortran与C/C++的混合编程
- 文件压缩算法的实现原理
- 系统级编程概念
- 网络安全中的压缩炸弹攻击原理

## 工作原理

```
小文件 (几KB) → 解压后 → 巨大文件 (10GB+)
```

通过创建高度重复的数据模式，利用压缩算法的特性，实现极高的压缩比。当目标系统解压时，会消耗大量资源。

## 环境要求

### 必需软件
- **VSCode** + Modern Fortran扩展
- **gfortran** (GNU Fortran编译器)
- **g++** (GNU C++编译器)
- **gcc** (GNU C编译器)
- **make** 构建工具

### 推荐配置
```bash
# macOS (使用Homebrew)
brew install gcc make

# Linux (Ubuntu/Debian)
sudo apt install gfortran g++ gcc make

# 验证安装
gfortran --version
g++ --version
gcc --version
```

## 快速开始

### 1. 克隆仓库
```bash
git clone https://github.com/ktwu01/Fortran-Playground.git
cd Fortran-Playground
```

### 2. 编译项目
```bash
make zipbomb
```

### 3. 运行程序
```bash
./zipbomb
```

程序将生成一个名为 `bomb.zip` 的文件（约5KB），解压后大小约10GB。

## 项目结构

```
Fortran-Playground/
├── src/
│   ├── main.f90           # Fortran主程序
│   ├── zipbomb.cpp        # C++ ZIP生成模块
│   ├── utils.c            # C 系统工具函数
│   └── interfaces.f90     # Fortran-C接口定义
├── include/
│   └── zipbomb.h          # C/C++头文件
├── Makefile               # 构建脚本
└── README.md             # 项目说明
```

## 核心代码解析

### Fortran主控制器 (main.f90)
```fortran
program zipbomb_generator
    use iso_c_binding
    implicit none
    
    ! 调用C++函数生成ZIP炸弹
    call create_zipbomb(c_char_"bomb.zip"//c_null_char)
    
    write(*,*) '⚠️  ZIP炸弹已生成: bomb.zip'
    write(*,*) '文件大小: ~5KB'
    write(*,*) '解压后大小: ~10GB'
    write(*,*) '请在安全环境中测试！'
end program
```

### C++压缩引擎 (zipbomb.cpp)
```cpp
#include <iostream>
#include <fstream>
#include <string>

extern "C" {
    void create_zipbomb(const char* filename);
}

void create_zipbomb(const char* filename) {
    // 生成高重复性数据
    std::string pattern(1024*1024, 'A'); // 1MB重复字符
    
    // 创建ZIP文件结构
    // 利用ZIP格式的重复数据压缩特性
    // 实现小文件→大解压的效果
}
```

### C系统接口 (utils.c)
```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

// 获取文件大小
long get_file_size(const char* filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        return st.st_size;
    }
    return -1;
}
```

## 编译选项详解

```makefile
# Makefile核心配置
FC = gfortran
CC = gcc  
CXX = g++

# 编译标志
FFLAGS = -std=f2008 -Wall -Wextra -g
CFLAGS = -Wall -Wextra -g
CXXFLAGS = -std=c++17 -Wall -Wextra -g

# 链接多语言目标文件
zipbomb: main.o zipbomb.o utils.o
	$(FC) $(FFLAGS) -o $@ $^ -lstdc++
```

## 学习要点

### 1. 多语言互操作
- Fortran通过 `iso_c_binding` 调用C函数
- C++提供高级文件处理功能
- C处理底层系统调用

### 2. 压缩算法原理
- 重复数据检测
- 字典压缩技术
- 压缩比优化策略

### 3. 系统编程概念
- 文件I/O操作
- 内存管理
- 跨平台兼容性

## 测试和验证

### 安全测试环境
```bash
# 创建测试目录
mkdir test_env
cd test_env

# 生成ZIP炸弹
../zipbomb

# 检查文件大小
ls -lh bomb.zip

# ⚠️ 谨慎解压测试
# unzip bomb.zip  # 注意：会生成大文件！
```

### 性能分析
- 压缩比计算
- 生成时间测量
- 内存使用监控

## 故障排除

### 常见问题

**编译错误：找不到gfortran**
```bash
# 检查编译器路径
which gfortran
# 如果没有，重新安装gcc套件
```

**链接错误：undefined reference**
```bash
# 检查库链接顺序
# 确保-lstdc++在最后
```

**运行时错误：权限不足**
```bash
# 确保有写入权限
chmod +w .
```

## 扩展功能

### 进阶实现
- 支持不同压缩格式（7z, RAR）
- 可配置压缩参数
- 批量生成功能
- 图形界面

### 性能优化
- 并行压缩算法
- 内存映射文件
- 流式处理

## 相关资源

### 学习材料
- [Modern Fortran教程](https://fortran-lang.org/)
- [C++17标准文档](https://en.cppreference.com/)
- [ZIP文件格式规范](https://pkware.cachefly.net/webdocs/casestudies/APPNOTE.TXT)

### 开发工具
- [VSCode Fortran扩展](https://marketplace.visualstudio.com/items?itemName=fortls.fortls)
- [GNU编译器套件](https://gcc.gnu.org/)

## 贡献指南

欢迎提交Pull Request和Issue！

### 开发规范
- 代码注释使用中文
- 遵循各语言的标准风格
- 添加适当的错误处理
- 编写测试用例

## 许可证

本项目仅用于教育目的，请勿用于非法用途。

---

**再次提醒：本工具仅供学习研究，请在安全环境中使用！**
