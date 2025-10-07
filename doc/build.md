# 构建
---

## 编译
### 先决条件
参见 [README.md](./../README.md)
### 编译方式
``` shell
# 可选
mkdir build
cd build

cmake .. -G <type> -D<options>
```
其中 `options` 在 `src/config/config.h.in` 中定义，具体意义参见源码文件或以下表格。
### 可选参数 `options`
| 宏名 | 类型 | 值 | 默认值 | 作用范围 | 作用 | 函数默认行为 | 依赖 | 其他 |
|---|---|---|---|---|---|---|---|---|
| INIP_ALLOW_DUPLICATE_SECTIONS | BOOL | ON/OFF | OFF | "inip::*::add*"和"inip::IniMgr::parse_str" | 是否允许出现同名Section | 不允许，报错SEC_EXISTS | 无 |  |
| INIP_COVER_SECTION_IF_EXIST | BOOL | ON/OFF | OFF | "inip::*::add*"和"inip::IniMgr::parse_str" | 出现同名Section时，覆盖 | 追加 | INIP_ALLOW_DUPLICATE_SECTIONS | 自动启用INIP_ALLOW_DUPLICATE_SECTIONS |
| INIP_ALLOW_DUPLICATE_KEYS | BOOL | ON/OFF | OFF | "inip::*::add*"和"inip::IniMgr::parse_str" | 是否允许出现同名Node | 不允许，报错KEY_EXISTS | 无 |  |
| INIP_COVER_KEY_IF_EXIST | BOOL | ON/OFF | OFF | "inip::*::add*"和"inip::IniMgr::parse_str" | 出现同名Node时，覆盖 | 无操作，保持原状 | INIP_ALLOW_DUPLICATE_KEYS | 自动启用INIP_ALLOW_DUPLICATE_KEYS |
| INIP_THROW_EXP_IF_ERROR | BOOL | ON/OFF | OFF | "inip::IniMgr::parse_str" | 出现解析错误时，抛出异常 | 返回错误码 | 无 |  |
| INIP_SECTION_CLEAR_NAME和 | BOOL | ON/OFF | OFF | "inip::*::clear" | 调用clear()时是否清除Name | 清Data，保留Name | 无 |  |
| INIP_USE_IMPORT | BOOL | ON/OFF | OFF | "inip::IniMgr::parse_str" | 启用__import__私有扩展 | 不启用 | 无 |  |
| INIP_IMPORT_COVER_SECTION_IF_EXIST | BOOL | ON/OFF | OFF | "inip::IniMgr::parse_str" | 导入文件时，以本文件内容覆盖导入内容 | 保持原状 | 无 |  |

