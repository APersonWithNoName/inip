# inip
使用C++11标准的ini配置文件解析库
--- 

## 目录
- [第一节：简介](#第一节简介)
- [第二节：构建](#第二节构建)
- [第三节：使用](#第三节使用)
- [第四节：接口](#第四节接口)

--- 
## 第一节：简介
使用C++11标准的ini配置文件解析库。
基于 [GPL v3.0 License](./LICENSE) 开源。


--- 
## 第二节：构建

### 先决条件
- cmake 可用，版本 __3.10__ 以上
- c++ 编译器可用，且支持 __C++11__ 标准
### 编译
#### Windows
```shell
mkdir build
cd build
cmake ..
```
#### Unix
```shell
mkdir build && cd build
cmake ..
```
#### 其他平台
请参见 [CMake文档](https://cmake.org/documentation/)。
#### 可选配置
关于 CMake 构建时的 `-D type=value` 选项，请参见 [源码](./src/config/config.h.in) 或 [项目文档](./doc/build.md)。

--- 
## 第三节：使用
- 在源码中引入 `inip/inip.hpp`
- 链接 `inip` 库
- __注：__ 仅 __`inip/inip.hpp`__ 头文件可以被直接包含！
### 最小示例
``` cpp
#include "inip/inip.hpp"

#include <string>

int main(int argc, char **argv)
{
    std::string file_name = "config.ini"

    inip::ini(file_name) ini; // 构造 ini 对象
    ini.load_file(); // 加载 ini 文件
    ini.get("secname", "key");
    ini.get_def("secname", "key", "def_value");

    return 0;
}
```


--- 
## 第四节：接口
