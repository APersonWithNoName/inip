# inip 多平台构建工具文档

## 目录

- [概述](#概述)
- [安装](#安装)
- [配置文件格式](#配置文件格式)
- [命令行使用](#命令行使用)
- [构建流程](#构建流程)
- [高级配置](#高级配置)
- [故障排除](#故障排除)
- [参考信息](#参考信息)

## 概述

`build.py` 是一个通用的 CMake 项目多平台构建工具, 专为 CI/CD 环境设计.它可以:

- **自动检测主机架构**(x86_64,aarch64,i386,riscv64 等)
- **自动判断交叉编译**:当目标架构与主机不同时自动使用工具链文件
- **并行构建**:自动利用多核 CPU
- **多平台构建**:通过配置文件同时构建多个目标

## 安装

### 方式一:直接下载
```bash
# 下载构建脚本
wget https://raw.githubusercontent.com/your-repo/inip/main/build.py
chmod +x build.py
# 安装依赖(Python 3.6+ 内置模块, 无需额外安装)
```
### 方式二:作为项目一部分
```bash
# 脚本已包含在项目中
cd your-project
ls -l build.py
```

## 配置文件格式

配置文件使用标准的 INI 格式, 包含 `[project]` 和多个 `[target.*]` 部分.

### 完整示例

```ini
# build.ini
[project]
# 项目源码目录(必填)
dirs.src = /home/user/projects/inip

# 输出目录(可选, 默认:%(dirs.src)s/dist)
dirs.dist = %(dirs.src)s/build/dist

# 项目名称(必填, 用于生成包名)
name = inip

# 版本号(必填, 用于生成包名)
version = v1.0.0

# 其他信息(可选, 用于生成包名, 如 debug/release/commit-id)
other_info = release

# 工具路径(可选, 默认使用系统路径)
path.make = /usr/bin/make
path.cmake = /usr/bin/cmake

# 通用编译标志(可选)
flags.cmake = -DCMAKE_BUILD_TYPE=Release -DINIP_ENABLE_GOOGLETEST=OFF
flags.make = -j4

# 是否强制使用工具链(即使目标与主机相同)
toolchain.force = false

# Linux ARM64 目标
[target.aarch64-linux]
# 目标名称(用于输出目录和包名)
name = aarch64-linux
# 工具链文件名(相对于项目 cmake/ 目录)
toolchain.file = aarch64-linux-toolchain.cmake
# 是否使用内部工具链(true: 相对于项目 cmake/, false: 绝对路径)
toolchain.internal = true
# 是否强制使用工具链(覆盖 project 设置)
toolchain.force = false
# 可覆盖全局设置
path.make = /usr/bin/make
path.cmake = /usr/bin/cmake
flags.cmake = -DCMAKE_BUILD_TYPE=Release
flags.make = -j4

# Linux x86_64 目标
[target.x86_64-linux]
name = x86_64-linux
toolchain.file = x86_64-linux-toolchain.cmake
toolchain.internal = true

# Windows MinGW 目标
[target.x86_64-windows]
name = x86_64-windows
toolchain.file = x86_64-mingw-toolchain.cmake
toolchain.internal = true

# 本地编译目标(不使用工具链)
[target.native]
name = x86_64-linux
toolchain.file = 
toolchain.internal = true
toolchain.force = false  # 不强制使用工具链, 自动本地编译
```
注: 变量替换规则同Python configparser库, 参见<https://docs.pythonlang.cn/3/library/configparser.html>

### 配置文件参数详解

#### [project] 部分

| 参数 | 必填 | 默认值 | 说明 |
|------|------|--------|------|
| `dirs.src` | YES | 无 | 项目源码绝对路径 |
| `dirs.dist` | NO | `%(dirs.src)s/dist` | 输出目录, 支持变量替换 |
| `name` | YES | 无 | 项目名称 |
| `version` | YES | 无 | 版本号 |
| `other_info` | NO | 空 | 附加信息(如 debug/release) |
| `path.make` | NO | `/usr/bin/make` | make 命令路径 |
| `path.cmake` | NO | `/usr/bin/cmake` | cmake 命令路径 |
| `flags.cmake` | NO | 空 | 全局 CMake 标志 |
| `flags.make` | NO | 空 | 全局 Make 标志 |
| `toolchain.force` | NO | `false` | 是否强制使用工具链 |

#### [target.*] 部分

| 参数 | 必填 | 默认值 | 说明 |
|------|------|--------|------|
| `name` | YES | 无 | 目标名称(用于目录和包名) |
| `toolchain.file` | NO | 空 | 工具链文件名, 空表示本地编译 |
| `toolchain.internal` | NO | `true` | `true`: 相对于 `cmake/` 目录, `false`: 绝对路径 |
| `toolchain.force` | NO | 继承 `project` | 是否强制使用工具链 |
| `path.make` | NO | 继承 `project` | 覆盖 make 路径 |
| `path.cmake` | NO | 继承 `project` | 覆盖 cmake 路径 |
| `flags.cmake` | NO | 继承 `project` | 附加 CMake 标志 |
| `flags.make` | NO | 继承 `project` | 附加 Make 标志 |

## 命令行使用

### 基本命令

```bash
# 使用配置文件构建
python3 build.py --configfile build.ini

# 使用默认配置文件(build.ini)
python3 build.py --configfile build.ini

# 显示帮助
python3 build.py --help

# 指定构建目录
python3 build.py -C build
```

### 构建过程输出

运行脚本后, 您会看到彩色日志输出:

```
Host target: x86_64-linux.
Use build configure: build.ini
Add Target:aarch64-linux Toolchain:/home/user/inip/cmake/aarch64-linux-toolchain.cmake.
Add Target:x86_64-linux Toolchain:/home/user/inip/cmake/x86_64-linux-toolchain.cmake HostCompile:True.
Add Target:x86_64-windows Toolchain:/home/user/inip/cmake/x86_64-mingw-toolchain.cmake.

[ LOGOUT ] 2026-03-02 15:30:00,123 INFO at build.py:245:prepare_all  Preparing: 1/3 Target:aarch64-linux --- Start Preparing.
[ LOGOUT ] 2026-03-02 15:30:00,124 INFO at build.py:XX:prepare  Target:aarch64-linux Using toolchain: /home/user/inip/cmake/aarch64-linux-toolchain.cmake
[ LOGOUT ] 2026-03-02 15:30:00,125 INFO at build.py:XX:prepare  Target:aarch64-linux --- End Preparing.

[  EXEC   ] Executing: ['/usr/bin/cmake', '-B', 'aarch64-linux.build', '-S', '/home/user/inip', '-DCMAKE_TOOLCHAIN_FILE=/home/user/inip/cmake/aarch64-linux-toolchain.cmake', '-DCMAKE_INSTALL_PREFIX=aarch64-linux.install']
[ STDOUT  ] -- The C compiler identification is GNU 14.2.0
[ STDOUT  ] -- Configuring done
[ STDOUT  ] -- Generating done
...

[ LOGOUT ] 2026-03-02 15:30:30,456 INFO at build.py:260:package_all  Packaging: 3/3 Target:x86_64-windows --- End Packaging.
```
### 输出文件
每个构建目标会生成独立的安装包, 格式为:
```
{project}-{version}-{other_info}-{target.name}.tar.gz
```
例如:
- `inip-v1.0.0-release-aarch64-linux.tar.gz`
- `inip-v1.0.0-release-x86_64-linux.tar.gz`
- `inip-v1.0.0-debug-x86_64-windows.tar.gz`
每个包包含:
- `lib/` - 库文件(.so, .a, .dll 等)
- `include/` - 头文件
- `LICENSE` - 许可证文件
- `README.md` - 说明文档
- `doc/` - 文档目录

## 构建流程

脚本按以下顺序处理每个目标:

### 1. 准备阶段 (`prepare`)
- 检测主机架构和系统
- 创建构建目录 (`{target.name}.build`)
- 创建安装目录 (`{target.name}.install`)
- 检查工具链文件(如果需要交叉编译)
- 验证 CMake 和 Make 可用性
### 2. 配置阶段 (`configure`)
```python
cmake -B {target}.build -S {src_dir} \
      {cmake_flags} \
      [-DCMAKE_TOOLCHAIN_FILE={toolchain}] \
      -DCMAKE_INSTALL_PREFIX={install_dir}
```
### 3. 编译阶段 (`compile`)
```python
make -j{nproc} -C {build_dir} {make_flags}
```
### 4. 安装阶段 (`install`)
```python
make install -C {build_dir}
cp -r LICENSE README.md doc/ {build_dir}/
```
### 5. 打包阶段 (`package`)
```python
tar -czf {dist_dir}/{package_name}.tar.gz {install_dir}/
```


## 高级配置

### 多版本构建
```ini
[project]
dirs.src = /home/user/inip
dirs.dist = %(dirs.src)s/build/dist
name = inip
version = v1.0.0
other_info = $(date +%Y%m%d)

# Debug 版本
[target.aarch64-linux-debug]
name = aarch64-linux-debug
toolchain.file = aarch64-linux-toolchain.cmake
flags.cmake = -DCMAKE_BUILD_TYPE=Debug

# Release 版本
[target.aarch64-linux-release]
name = aarch64-linux-release
toolchain.file = aarch64-linux-toolchain.cmake
flags.cmake = -DCMAKE_BUILD_TYPE=Release
```
### 条件编译
```ini
[project]
# ... 基础配置 ...

# 带测试的构建
[target.x86_64-linux-with-tests]
name = x86_64-linux-tests
toolchain.file = 
flags.cmake = -DINIP_ENABLE_GOOGLETEST=ON

# 不带测试的构建(最小化)
[target.x86_64-linux-minimal]
name = x86_64-linux-minimal
toolchain.file = 
flags.cmake = -DINIP_ENABLE_GOOGLETEST=OFF -DINIP_ENABLE_DEMO=OFF
```
### 使用绝对路径的工具链

```ini
[target.custom-toolchain]
name = arm-linux-gnueabihf
toolchain.file = /opt/toolchains/arm-linux-gnueabihf.cmake
toolchain.internal = false  # 使用绝对路径
```
### 强制交叉编译
```ini
[project]
# 即使目标与主机相同, 也强制使用工具链
toolchain.force = true

[target.x86_64-linux]
name = x86_64-linux
toolchain.file = x86_64-linux-toolchain.cmake
# 会强制使用工具链, 而不是本地编译
```

## 故障排除

### 找不到工具链文件

**错误信息**:
```
CRITICAL: Toolchain file does not exist: /path/to/cmake/xxx-toolchain.cmake
```

**解决方案**:
```bash
# 检查文件是否存在
ls -la /path/to/project/cmake/

# 确认 toolchain.internal 设置正确
# true: 相对于项目 cmake/ 目录
# false: 使用绝对路径
```

### CMake 版本过低

**错误信息**:
```
CMake Error: Unknown argument -B
```

**解决方案**:
```ini
[project]
# 指定高版本 CMake
path.cmake = /usr/local/bin/cmake
```

### 交叉编译时找不到库

**错误信息**:
```
fatal error: features.h: No such file or directory
```

**解决方案**:
```bash
# 安装交叉编译库
sudo apt-get update
sudo apt-get install libc6-dev-${ARCH}-cross
# 例如: libc6-dev-arm64-cross, libc6-dev-amd64-cross
```

### GTest 找不到

**错误信息**:
```
Could NOT find GTest (missing: GTEST_LIBRARY GTEST_MAIN_LIBRARY)
```

**解决方案**:
```ini
[project]
# 禁用测试
flags.cmake = -DINIP_ENABLE_GOOGLETEST=OFF
```

### 权限错误

**错误信息**:
```
Permission denied: /path/to/build
```

**解决方案**:
```bash
# 清理并重新创建目录
rm -rf *.build *.install
python3 build.py --configfile build.ini
```

### 彩色日志不显示

**问题**:日志没有颜色

**解决方案**:
- 确保终端支持 ANSI 颜色
- 检查是否设置了 `NO_COLOR` 环境变量
- 在 CI 环境中可能需要设置 `TERM=xterm-256color`

## 参考信息

### 脚本文件结构
```
build.py
├── Color                  # ANSI 颜色定义
├── exec()                 # 命令执行函数(带彩色输出)
├── CMakeTargetBuilder     # 构建器类
│   ├── prepare()          # 准备阶段
│   ├── configure()        # 配置阶段
│   ├── compile()          # 编译阶段
│   ├── install()          # 安装阶段
│   └── package()          # 打包阶段
├── RunBuild               # 旧版(已弃用)
└── RunBuild1              # 新版主控制类
    ├── add_targets()      # 从配置文件添加目标
    ├── prepare_all()      # 执行所有准备
    ├── configure_all()    # 执行所有配置
    ├── compile_all()      # 执行所有编译
    ├── install_all()      # 执行所有安装
    └── package_all()      # 执行所有打包
```
### 依赖关系
- Python 3.6+
- 标准库:`os`, `subprocess`, `logging`, `tarfile`, `argparse`, `platform`, `threading`, `configparser`, `dataclasses`, `warnings`
- 外部工具:CMake (≥3.10), Make
### 彩色输出说明
| 颜色 | 用途 |
|------|------|
| 绿色 | 正常信息,成功提示 |
| 蓝色 | 进度指示(如 1/3) |
| 黄色 | STDOUT 输出 |
| 红色 | STDERR 输出,错误信息 |
| 青色 | 执行的命令 |
### 输出规范
每个安装包遵循以下目录结构:
```
package.tar.gz
├── lib/
│   ├── libinip.so          # 动态库 (Unix)
│   ├── libinip.a           # 静态库
│   └── libinip.dll         # 动态库 (Windows)
├── include/
│   ├── inip/
│   │   ├── Exception.hpp
│   │   ├── IniMgr.hpp
│   │   ├── Node.hpp
│   │   └── Section.hpp
│   └── inip.h
├── LICENSE
├── README.md
└── doc/
    └── ... (文档文件)
```
