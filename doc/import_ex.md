# ` __import__ ` 关键字扩展
---

## 用途
导入指定文件的内容。

## 语法
在无名节中，使用：
``` ini
__import__=file_name
```
__注意__  
此语法仅限在 __无名节__ 中使用。

<font color=green>__正确__</font>
``` ini
__import__=config1.ini

[Section1]
sec1=key1
```
<font color=red>__错误__</font>
``` ini
[Section1]
sec1=key1
__import__=config1.ini  ;仅限无名节使用
```

## 错误处理
| 错误码 | 原因 |
| --- | --- |
| `IMPORT_INVALID_LOCATION` | 未在无名节中使用 |
| `IMPORT_INVALID` | 解析导入文件时出现错误 |