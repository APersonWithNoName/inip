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

| 参数 | 类型 | 值 | 默认 | 作用范围 | 作用 | 函数默认行为 | 依赖 |
| --- | ---  | --- | --- | --- | --- | --- | --- |
| `INIP_DISABLE_RRADD_NODE_KEY` | BOOL | ON/OFF | OFF | 所有`inip::Section::add*` 函数 | 若启用，禁止在同一个 Section 中添加两个相同 Key 的 Node | 不变 | 无 |
| `INIP_DISABLE_APPEND_SECTION` | BOOL | ON/OFF | OFF | `inip::IniMgr::parse_file` `inip::ini::add(const std::string &)` | 若启用，禁止在同一个 ini 对象中添加两个相同 Name 的 Section | 追加 | 无 |
| `INIP_COVER_KEY_IF_EXIST` | BOOL | ON/OFF | OFF | `inip::IniMgr::parse_file` `inip::Section::add*` | 若启用，同一个 Section 相同 Key 的 Node 会被覆盖 | 追加 | `INIP_DISABLE_RRADD_NODE_KEY=OFF` |
| `INIP_COVER_SECTION_IF_EXIST` | BOOL | ON/OFF | OFF | `inip::IniMgr::parse_file` | 若启用，同一个 ini 对象中出现相同 Name 的 Section 时，覆盖 | 追加 | `INIP_DISABLE_APPEND_SECTION=OFF` |
| `INIP_THROW_EXP_IF_ERROR` | BOOL | ON/OFF | OFF | `inip::IniMgr::parse_file` | 若启用，解析文件遇到错误时，将抛出异常而非返回错误码 | 返回错误码 | 无 |
| `INIP_SECTION_CLEAR_NAME` | BOOL | ON/OFF | OFF | `inip::Section::clear` | 若启用，`inip::Section::clear` 函数同时清除 Name | 不清除 | 无 |
| `INIP_INI_CLEAR_NAME` | BOOL | ON/OFF | OFF | `inip::ini::clear` | 若启用， `inip::ini::clear` 函数同时清除 FileName | 不清除 | 无 |
| `INIP_USE_IMPORT` | BOOL | ON/OFF | OFF | `inip::IniMgr::parse_file` | 若启用，则启用 `__import__` 关键字私有扩展，参见 [import 扩展文档](./import_ex.md) | 不启用 | 无 |

