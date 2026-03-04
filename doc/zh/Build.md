# inip 构建文档

## 目录

- [项目概述](#项目概述)
- [构建要求](#构建要求)
- [快速开始](#快速开始)
- [构建选项](#构建选项)
- [Unix-like 系统构建](#unix-like-系统构建)
- [Windows 系统构建](#windows-系统构建)
- [交叉编译](#交叉编译)
- [使用构建脚本](#使用构建脚本)
- [常见问题](#常见问题)

## 项目概述

inip 是一个 INI 文件解析库，支持：

- C++11 标准
- 单元测试 (GoogleTest)
- 示例程序 (依赖 CLI11 和 replxx)
- 静态库和动态库构建
- 交叉编译支持
- 多平台支持 (Linux, Windows)

## 构建要求

### 基础工具
| 工具 | 版本要求 | 说明 |
|------|----------|------|
| CMake | ≥ 3.10 | 构建系统生成 |
| C++编译器 | C++11支持 | GCC ≥ 4.8, Clang ≥ 3.3, MSVC ≥ 2015 |
| Make | ≥ 3.8 | Unix-like 系统 |
| Ninja | 可选 | 替代 Make |

### 可选依赖
| 依赖库 | 用途 | 默认状态 |
|--------|------|----------|
| GoogleTest | 单元测试 | 启用 |
| CLI11 | 命令行解析 (Demo) | 启用 |
| replxx | 交互式REPL (Demo) | 启用 |

## 快速开始

### 使用构建脚本(需要多平台交叉编译和CI/CD环境)
```bash
# 1. 克隆项目并检出子模块
git clone https://github.com/APersonWithNoName/inip.git
cd inip
git submodule update --init --recursive

# 2. 创建配置文件 build.ini
cat > build.ini << EOF
[project]
dirs.src = $(pwd)
dirs.dist = %(dirs.src)s/build/dist
name = inip
version = v1.0.0
other_info = release

[target.native]
name = $(uname -m)-$(uname -s | tr '[:upper:]' '[:lower:]')
toolchain.file =
toolchain.internal = yes
EOF

# 3. 一键构建
python3 build.py --configfile build.ini
```
注: 关于此文构建脚本工具(`build.ini`和`build.py`), 参见<./BuildTool.md>

### 手动构建
```bash
# 创建构建目录
mkdir build && cd build

# 配置
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local

# 编译
make -j$(nproc)

# 测试（可选）
make test

# 安装
make install
```

## 构建选项
### 基础选项
| CMake 选项 | 说明 | 默认值 |
|------------|------|--------|
| `CMAKE_INSTALL_PREFIX` | 安装路径 | `/usr/local` |
| `CMAKE_BUILD_TYPE` | 构建类型 (Debug/Release) | 空 |
| `BUILD_SHARED_LIBS` | 构建动态库 | OFF |

### 项目选项
| 选项 | 说明 | 默认值 |
|------|------|--------|
| `INIP_ENABLE_GOOGLETEST` | 启用 GoogleTest 测试 | ON |
| `INIP_ENABLE_DEMO` | 构建示例程序 | ON |

### 第三方库路径
| 选项 | 说明 | 默认值 |
|------|------|--------|
| `INIP_GOOGLETEST_PATH` | GTest 源码路径 | `${CMAKE_SOURCE_DIR}/3rdlib/googletest` |
| `INIP_CLI11_PATH` | CLI11 源码路径 | `${CMAKE_SOURCE_DIR}/3rdlib/CLI11` |
| `INIP_REPLXX_PATH` | replxx 源码路径 | `${CMAKE_SOURCE_DIR}/3rdlib/replxx` |

## Unix-like 系统构建

### Linux (Ubuntu/Debian)
```bash
# 安装依赖
sudo apt-get update
sudo apt-get install build-essential cmake

# 如需单元测试
sudo apt-get install libgtest-dev

# 构建
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install
```
### Linux (CentOS/RHEL)
```bash
# 安装依赖
sudo yum groupinstall "Development Tools"
sudo yum install cmake3

# 构建
mkdir build && cd build
cmake3 .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install
```
### macOS
```bash
# 安装依赖 (使用 Homebrew)
brew install cmake

# 构建
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(sysctl -n hw.ncpu)
sudo make install
```

## Windows 系统构建

### 使用 MinGW (推荐)
```bash
# 安装 MinGW-w64

# 构建
mkdir build && cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
mingw32-make -j%NUMBER_OF_PROCESSORS%
mingw32-make install
```
### 使用 Visual Studio
```batch
:: 打开 Visual Studio 开发者命令提示符
mkdir build
cd build

:: 生成 Visual Studio 解决方案
cmake .. -G "Visual Studio 17 2022" -A x64

:: 构建 Release 版本
cmake --build . --config Release

:: 安装
cmake --install . --config Release
```
### 使用 MSYS2
```bash
# 在 MSYS2 终端中
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake

# 构建
mkdir build && cd build
cmake .. -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
make install
```

## 交叉编译

### Linux 主机交叉编译到不同架构
#### 配置文件方式 (推荐)

```ini
# build.ini
[project]
dirs.src = /home/user/inip
dirs.dist = %(dirs.src)s/build/dist
name = inip
version = v1.0.0
other_info = release

# ARM64 目标
[target.aarch64-linux]
name = aarch64-linux
toolchain.file = aarch64-linux-toolchain.cmake
toolchain.internal = yes

# x86_64 目标
[target.x86_64-linux]
name = x86_64-linux
toolchain.file = x86_64-linux-toolchain.cmake
toolchain.internal = yes

# Windows MinGW 目标
[target.x86_64-mingw]
name = x86_64-windows
toolchain.file = x86_64-mingw-toolchain.cmake
toolchain.internal = yes
```
```bash
# 一键构建所有目标
python3 build.py --configfile build.ini
```
#### 手动交叉编译
**ARM64 (aarch64)**
```bash
# 安装交叉编译器
sudo apt-get install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu

# 构建
mkdir build-aarch64 && cd build-aarch64
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=cmake/aarch64-linux-toolchain.cmake \
    -DINIP_ENABLE_GOOGLETEST=OFF
make -j$(nproc)
```
**x86_64**
```bash
# 安装交叉编译器
sudo apt-get install gcc-x86-64-linux-gnu g++-x86-64-linux-gnu

# 构建
mkdir build-x86_64 && cd build-x86_64
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=cmake/x86_64-linux-toolchain.cmake \
    -DINIP_ENABLE_GOOGLETEST=OFF
make -j$(nproc)
```
**Windows (MinGW)**
```bash
# 安装 MinGW 交叉编译器
sudo apt-get install gcc-mingw-w64-x86-64 g++-mingw-w64-x86-64

# 构建
mkdir build-mingw && cd build-mingw
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=cmake/x86_64-mingw-toolchain.cmake \
    -DINIP_ENABLE_GOOGLETEST=OFF
make -j$(nproc)
```

## 使用构建脚本
参见<./BuildTool.md>

## 常见问题

### 找不到 GoogleTest

**问题**: `Could NOT find GTest`
**解决方案**:
```bash
# 禁用测试
cmake .. -DINIP_ENABLE_GOOGLETEST=OFF

# 或安装 GTest
sudo apt-get install libgtest-dev  # Ubuntu
cd /usr/src/gtest && sudo cmake . && sudo make && sudo cp *.a /usr/lib
```

### 交叉编译时找不到库

**问题**: `fatal error: features.h: No such file`
**解决方案**:
```bash
# 安装交叉编译库
sudo apt-get install libc6-dev-${ARCH}-cross
# 例如: libc6-dev-arm64-cross, libc6-dev-amd64-cross
```

### replxx char8_t 警告

**问题**: `warning: identifier ‘char8_t’ is a keyword in C++20`
**解决方案**:
```bash
# 在 CMake 命令行添加
cmake .. -DCMAKE_CXX_FLAGS="-Wno-c++20-compat"
```

### 构建 Windows 目标时找不到 pthread

**问题**: `undefined reference to pthread_create`
**解决方案**:
```cmake
# 在 CMakeLists.txt 中添加
if(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()
```

