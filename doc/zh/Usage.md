# inip 库使用文档

## 目录

- [简介](#简介)
- [安装](#安装)
- [快速开始](#快速开始)
- [核心类](#核心类)
- [解析器参数详解](#解析器参数详解)
- [基本操作](#基本操作)
- [数据类型操作](#数据类型操作)
- [高级功能](#高级功能)
- [错误处理](#错误处理)
- [API 参考](#api-参考)
- [示例代码](#示例代码)

## 简介
inip 是一个功能强大的 INI 文件解析库，支持 C++11 标准。它提供了完整的 INI 文件读写功能，包括节（Section）、键（Key）、值（Value）的管理，以及丰富的数据类型转换功能。
### 特性
- 完整的 INI 格式支持
- 支持节（Section）和键（Key）的增删改查
- 内置多种数据类型转换（bool、int、double 等）
- 模板支持，可扩展自定义类型
- 详细的错误处理

## 安装
### 手动编译
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install
```

## 快速开始
### 基本示例
```cpp
#include <iostream>
#include <inip/inip.hpp>

int main() {
    // 创建 ini 对象
    inip::ini config;
    
    // 加载配置文件
    std::vector<inip::err::Errors> errors;
    if (config.load_file("config.ini", errors) == inip::err::Errors::OK) {
        // 读取配置
        std::string host = config.get("database", "host");
        int port = config.get_int("database", "port");
        
        std::cout << "Host: " << host << std::endl;
        std::cout << "Port: " << port << std::endl;
        
        // 修改配置
        config.set("database", "port", "3307");
        
        // 保存文件
        config.write("config_new.ini");
    }
    
    return 0;
}
```
### 配置文件格式
```ini
; 这是注释
[global]
debug = true
version = 1.0.0

[database]
host = localhost
port = 3306
username = root
password = 123456

[logging]
level = info
file = /var/log/app.log
```

## 核心类
### inip::ini
主类，提供所有 INI 文件操作接口，封装了 `IniMgr` 的复杂参数。
### inip::IniMgr
底层解析器，提供 8 个布尔参数精确控制解析行为。
### inip::Section
表示一个配置节，包含多个键值对。
```cpp
class Section {
    std::string name;
    std::map<std::string, Node> nodes;
    unsigned int line;  // 在文件中的行号
};
```
### inip::Node
表示一个键值对节点。
```cpp
class Node {
    std::string key;
    std::string value;
    unsigned int line;  // 在文件中的行号
};
```
### inip::err::Errors
错误码枚举，用于错误处理。

## 解析器参数详解
`IniMgr::parse_str()` 和 `IniMgr::parse_file()` 方法提供 8 个布尔参数，精确控制解析行为。
### 参数列表
| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `failed_skip` | bool | `false` | 是否跳过错误继续解析 |
| `import_cover_sec_if_exist` | bool | `true` | 导入时同名节是否完全覆盖 |
| `allow_duplicate_sec` | bool | `true` | 是否允许重复节 |
| `cover_sec_if_exist` | bool | `true` | 同名节是否完全覆盖 |
| `allow_duplicate_keys` | bool | `true` | 是否允许重复键 |
| `cover_key_if_exist` | bool | `true` | 同名键是否覆盖 |
### 参数详解
#### `failed_skip` - 错误跳过模式
```cpp
// 遇到错误立即停止（严格模式）
config.load_file("config.ini", errors, false);

// 遇到错误跳过继续解析（宽松模式）
config.load_file("config.ini", errors, true);
```
| 值 | 行为 |
|----|------|
| `false` | 遇到第一个错误立即停止，返回错误 |
| `true` | 跳过错误行继续解析，错误记录到 `err_list` |
#### `import_cover_sec_if_exist` - 导入覆盖模式
| 值 | 行为 |
|----|------|
| `true` | 导入时同名节完全覆盖（清空原有数据） |
| `false` | 导入时同名节合并（保留原有数据） |
#### `allow_duplicate_sec` - 重复节控制
```ini
[section]  ; 第一次出现
key1 = value1

[section]  ; 第二次出现（重复节）
key2 = value2
```
| 值 | 行为 |
|----|------|
| `true` | 允许重复节，根据 `cover_sec_if_exist` 决定处理方式 |
| `false` | 禁止重复节，遇到重复节返回错误 |
#### `cover_sec_if_exist` - 节覆盖模式
当 `allow_duplicate_sec = true` 时：
| 值 | 行为 |
|----|------|
| `true` | 完全覆盖：清空原有节，使用新数据 |
| `false` | 合并模式：保留原有数据，追加新键值对 |
#### `allow_duplicate_keys` - 重复键控制
```ini
[section]
key = value1  ; 第一次出现
key = value2  ; 第二次出现（重复键）
```
| 值 | 行为 |
|----|------|
| `true` | 允许重复键，根据 `cover_key_if_exist` 决定处理方式 |
| `false` | 禁止重复键，遇到重复键返回错误 |
#### `cover_key_if_exist` - 键覆盖模式
当 `allow_duplicate_keys = true` 时：
| 值 | 行为 |
|----|------|
| `true` | 覆盖：新值替换旧值 |
| `false` | 保留：保持原值不变，忽略新值 |
### 参数组合场景
#### 严格模式（配置管理）
```cpp
// 严格的配置文件，不允许任何重复
config.load_file("config.ini", errors, 
    false,                    // failed_skip: 遇到错误立即停止
    true,                     // use_import: 允许导入
    true,                     // import_cover_sec_if_exist: 导入覆盖
    false,                    // allow_duplicate_sec: 禁止重复节
    true,                     // cover_sec_if_exist: (无意义)
    false,                    // allow_duplicate_keys: 禁止重复键
    true                      // cover_key_if_exist: (无意义)
);
```
#### 宽松模式（日志合并）
```cpp
// 宽松模式，合并多个配置
config.load_file("config.ini", errors,
    true,                     // failed_skip: 跳过错误
    true,                     // use_import: 允许导入
    false,                    // import_cover_sec_if_exist: 导入合并
    true,                     // allow_duplicate_sec: 允许重复节
    false,                    // cover_sec_if_exist: 节合并
    true,                     // allow_duplicate_keys: 允许重复键
    false                     // cover_key_if_exist: 保留原值
);
```
#### 默认配置 + 用户覆盖
```cpp
// 先加载默认配置（允许覆盖）
config.load_file("default.ini", errors, 
    false, false, true, true, true, true, true);

// 再加载用户配置（覆盖默认）
config.load_file("user.ini", errors,
    false, false, true, true, true, true, true);
```
#### 只读模式
```cpp
// 只读模式：保留原始数据，不允许修改
config.load_file("config.ini", errors,
    true,                     // 跳过错误
    true,                     // 允许导入
    false,                    // 导入合并
    true,                     // 允许重复节
    false,                    // 节合并
    true,                     // 允许重复键
    false                     // 保留原值
);
```

## 基本操作
### 1. 创建和初始化
```cpp
// 创建空配置
inip::ini config;

// 从文件加载
inip::ini config("config.ini");

// 创建后加载
inip::ini config;
config.load_file("config.ini", errors);
```
### 2. 加载配置
```cpp
std::vector<inip::err::Errors> errors;

// 从文件加载（使用默认参数）
auto err = config.load_file("config.ini", errors);

// 从文件加载（自定义参数）
err = config.load_file("config.ini", errors, 
    true,   // failed_skip: 跳过错误
    true,   // use_import: 允许导入
    true,   // import_cover_sec_if_exist: 导入覆盖
    false,  // allow_duplicate_sec: 禁止重复节
    true,   // cover_sec_if_exist: (无意义)
    false,  // allow_duplicate_keys: 禁止重复键
    true    // cover_key_if_exist: (无意义)
);

// 从字符串加载
std::string content = "[sec]\nkey=value";
err = config.load_str(content, errors, 
    false, true, true, true, true, true, true);

// 从字符串流加载
std::stringstream ss(content);
err = config.load_sstr(ss, errors);
```
### 3. 读取配置
```cpp
// 检查是否存在
if (config.contains("database")) {
    // 节存在
}

if (config.exist_key("database", "host")) {
    // 键存在
}

if (config.exist_key("debug")) {  // 无节名，查找根键
    // 根键存在
}

// 读取字符串值
std::string host = config.get("database", "host");
std::string host2 = config.get_default("database", "host", "localhost");

// 读取根键
std::string debug = config.get("debug");

// 使用操作符
std::string host3 = config["database"]["host"];
```
### 4. 修改配置
```cpp
// 添加节
config.set("database");

// 添加键值对
config.set("database", "host", "127.0.0.1");
config.set("database", "port", "3306");

// 添加根键
config.set("debug", "true");

// 使用操作符
config["database"]["user"] = "admin";

// 批量设置
Section& db = config["database"];
db.set("host", "localhost");
db.set("port", "3306");
```
### 5. 删除配置
```cpp
// 删除键
config["database"].remove("password");

// 删除节
config["database"].clear();  // 清空节内容
// 注：要删除整个节，需要直接操作 inimgr
```
### 6. 保存配置
```cpp
// 保存到原文件
config.write();

// 保存到新文件
config.write("new_config.ini");

// 导出为字符串
std::string content = config.to_string();
```

## 数据类型操作
### 布尔类型
```cpp
// 设置
config.set_bool("global", "debug", true);

// 读取
bool debug = config.get_bool("global", "debug");
bool auto_save = config.get_bool_def("global", "auto_save", false);
```
### 整数类型
```cpp
// 设置
config.set_int("server", "port", 8080);
config.set_uint("server", "timeout", 30);
config.set_long("server", "max_connections", 1000L);

// 读取
int port = config.get_int("server", "port");
unsigned int timeout = config.get_uint("server", "timeout");
long max_conn = config.get_long_def("server", "max_connections", 100L);
```
### 浮点类型
```cpp
// 设置
config.set_float("math", "pi", 3.14159f);
config.set_double("math", "e", 2.71828);

// 读取
float pi = config.get_float("math", "pi");
double e = config.get_double_def("math", "e", 2.7);
```
### 自定义类型（模板）
```cpp
// 自定义类型转换函数
std::string int_to_str(int value) {
    return std::to_string(value);
}

int str_to_int(const std::string& str) {
    return std::stoi(str);
}

// 使用模板设置
config.set_value<int>("section", "key", 42, int_to_str);

// 使用模板读取
int value = config.get_value<int>("section", "key", str_to_int);

// 带默认值的读取
int safe_value = config.get_value_def<int>("section", "key", str_to_int, 0);
```

## 高级功能
### 遍历节
```cpp
// 获取所有节名
auto names = config.section_names();
for (const auto& name : names) {
    std::cout << "Section: " << name << std::endl;
}

// 跳过空节
auto names_no_empty = config.section_names<std::vector<std::string>>(true);

// 获取节引用（避免拷贝）
auto sections = config.sections();
for (const auto& sec_ref : sections) {
    const Section& sec = sec_ref.get();
    std::cout << "Section: " << sec.name() << " (line: " << sec.line() << ")" << std::endl;
}

// 获取节拷贝
auto sections_copy = config.sections_copy();
```
### 操作节
```cpp
// 获取节引用
Section& db = config["database"];

// 获取节信息
std::string name = db.name();
unsigned int line = db.line();  // 节在文件中的行号

// 遍历节内所有键
for (const auto& pair : db) {
    const Node& node = pair.second;
    std::cout << node.key() << " = " << node.value() 
              << " (line: " << node.line() << ")" << std::endl;
}

// 获取节大小
size_t size = db.size();

// 检查节是否为空
if (db.empty()) {
    std::cout << "Section is empty" << std::endl;
}
```
### 获取行号信息
```cpp
// 获取节的行号
unsigned int sec_line = config["database"].line();

// 获取键的行号
unsigned int key_line = config["database"]["host"].line();

// 直接使用 IniMgr 获取
unsigned int line = config.inimgr.get_line("database");
unsigned int key_line2 = config.inimgr.get_line("database", "host");
```
### 清空和重置
```cpp
// 清空所有数据
config.clear();

// 清空数据但保留文件名
config.clear(false);

// 完全重置（包括文件名）
config.clear(true);
```

## 错误处理
### 错误码枚举
```cpp
namespace inip::err {
    enum class Errors {
        OK = 0,              // 成功
        FILE_NOT_FOUND,      // 文件不存在
        PARSE_ERROR,         // 解析错误
        DUPLICATE_SECTION,   // 重复节
        DUPLICATE_KEY,       // 重复键
        IMPORT_ERROR,        // 导入错误
        // ... 其他错误码
    };
}
```
### 错误处理示例
```cpp
std::vector<inip::err::Errors> errors;
auto result = config.load_file("config.ini", errors, true);  // failed_skip = true

if (result != inip::err::Errors::OK) {
    std::cerr << "Failed to load config, errors: " << errors.size() << std::endl;
    
    for (const auto& err : errors) {
        switch (err) {
            case inip::err::Errors::FILE_NOT_FOUND:
                std::cerr << "  - File not found" << std::endl;
                break;
            case inip::err::Errors::PARSE_ERROR:
                std::cerr << "  - Parse error" << std::endl;
                break;
            case inip::err::Errors::DUPLICATE_SECTION:
                std::cerr << "  - Duplicate section" << std::endl;
                break;
            case inip::err::Errors::DUPLICATE_KEY:
                std::cerr << "  - Duplicate key" << std::endl;
                break;
            default:
                std::cerr << "  - Error code: " << static_cast<int>(err) << std::endl;
        }
    }
}
```

## API 参考
### ini 类
#### 构造函数
```cpp
ini();                                   // 创建空配置
ini(const std::string &file_name);       // 从文件创建
```
#### 加载方法
```cpp
err::Errors load_file(const std::string &file_name,
    std::vector<err::Errors> &err_list,
    bool failed_skip = false,
    bool use_import = true,
    bool import_cover_sec_if_exist = true,
    bool allow_duplicate_sec = true,
    bool cover_sec_if_exist = true,
    bool allow_duplicate_keys = true,
    bool cover_key_if_exist = true);

err::Errors load_str(const std::string &str,
    std::vector<err::Errors> &err_list,
    ...);  // 参数同上

err::Errors load_sstr(std::stringstream &str,
    std::vector<err::Errors> &err_list,
    ...);  // 参数同上
```
#### 查询方法
```cpp
bool contains(const std::string &secname) const;
bool exist_key(const std::string &secname, const std::string &key) const;
bool exist_key(const std::string &key) const;
size_t size() const;
size_t max_size() const;
```
#### 设置方法
```cpp
void set(const std::string &secname);
void set(const std::string &key, const std::string &value);
void set(const std::string &secname, const std::string &key, const std::string &value);
```
#### 获取方法
```cpp
Section get_section(const std::string &secname);
std::string get(const std::string &key) const;
std::string get(const std::string &secname, const std::string &key) const;
std::string get_default(const std::string &key, const std::string &def) const;
std::string get_default(const std::string &secname, const std::string &key, 
                        const std::string &def) const;
```
#### 操作符重载
```cpp
inip::Section& operator[] (const std::string &sec);
inip::Section& at(const std::string &sec);
const inip::Section& at(const std::string &sec) const;
ini& operator=(const ini &ini_obj);
```
### IniMgr 类
```cpp
class IniMgr {
public:
    // 解析字符串
    err::Errors parse_str(std::istringstream &iss,
        std::vector<inip::err::Errors> &err_list,
        bool failed_skip = false,
        bool use_import = true,
        bool import_cover_sec_if_exist = true,
        bool allow_duplicate_sec = true,
        bool cover_sec_if_exist = true,
        bool allow_duplicate_keys = true,
        bool cover_key_if_exist = true);
    
    // 解析文件
    err::Errors parse_file(std::vector<inip::err::Errors> &err_list, ...);
    err::Errors parse_file(const std::string &file_name,
        std::vector<inip::err::Errors> &err_list, ...);
    
    // 获取行号
    unsigned int get_line(const std::string &secname) const;
    unsigned int get_line(const std::string &secname, const std::string &key) const;
};
```

