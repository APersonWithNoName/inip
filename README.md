# inip (INI Parser)
Tiny INI parser by c++ 11.

## Build & Install
### Setup Environment
- CMake >= 3.10
- GCC ≥ 4.8 || Clang ≥ 3.3 || MSVC ≥ 2015 (C++ 11 Support)
- Doxygen (Optional)

### Compile Project
#### Pull Git Repository
```shell
git clone https://github.com/APersonWithNoName/inip.git
cd inip
git submodule update --init --recursive
# if you would like to build googletest
```

#### Run Build
```shell
mkdir build && cd build
cmake .. && make -j4
```

### Available CMake Options
- `INIP_ENABLE_GOOGLETEST` Whether build googletest.
  
  For this, git submodules should be enabled. Or specify
 `INIP_GOOGLETEST_PATH` to a googletest directory. Default is `ON`.
- `INIP_ENABLE_DOXYGEN` Whether build the Doxygen documents.
  
  The documents can be found in the `BUILD_DIR/doxybuild` directory. And only `html` format will be generated. Default is `ON`.
- `INIP_ENABLE_CMAKE_CONFIG` Whether build CMake configure `inipConfig.cmake` file for development. Default is `ON`.
- `INIP_ENABLE_PKGCONFIG` Whether build pkg-config configure `inip.pc` for development. Default is `ON`.
- `INSTALL_GTEST` Whether install googletest to system.
  
  Exactly, this is not a inip option. Instead, it is a googletest option, but exposed to inip configure. Default is `OFF`.

> After CMake configure, run `make docs` to build doxygen files.


## Quick Start
### Basic Example
```cpp
#include <inip/inip.hpp>

/*
[db]
user = root
port = 22
*/

int main(int argc, char **argv)
{
	inip::err::ErrList errlist;

	inip::ini ini("config.ini");
	auto result = ini.load_file(errlist);
	if (!result.valid()) {
		return 0;
	}

	ini.get("db", "user"); // root
	ini.get_int("db", "port"); // 22

	ini.set("db", "user", "admin");
	ini.set_int("db", "port", 2222);

	ini.write();

	return 0;
}
```

### Other Examples
- See [inip_base.cpp](./src/examples/inip_base.cpp) for more basic interfaces.
- See [inip_advance.cpp](./src/examples/inip_base.cpp) for some advanced interfaces.
  
If you need more interface introduction or detailed documents, please build Doxygen documents.

### Use In Different Build-Systems
#### CMake
```cmake
find_package(inip REQUIRED)
target_link_libraries(app inip)
```

#### pkg-config
```shell
g++ $(pkg-config --libs --cflags inip)
```

#### Use Directly
```shell
CFLAGS="-I/usr/local/include"
LDFLAGS="-linip"
```

## INI Format
### Base Format & Internal Structure
#### Comments
Comments starts with `;` or `#`

#### Node
Node is the basic unit of a INI configure. 
```ini
key = value
```
Every Node requires a `=` to split key and value.
Space and whitespace chars in the head or tail of key or value will be neglected.
But the space inside key or walue will be reserved. e.g.
```ini
   key  with space   = value with   space  
```
will be parsed to 
```
key = "key  with space"
value = "value with   space"
```
Internal class `inip::Node` stores the Node **Value** and **LineNum** information.

#### Section
Section is a collection of Node. Every section usually has a name in a pair of `[]`.
```ini
[Section]
Key = value
Key = value
```
Unless the keys in the global section, every key should be under a section.
> Note
> 
> At the top of a INI file, you may see
> ```ini
> global_key = value
> [FirstSection]
> ```
> This global key are in a section named `""` (Empty string) in this library.
> 
> If you would like to visit this section, use `""`.

A section name is strictly in `[]`, and only whitespace can be put in front of `[`,
any other char out of `[]` will cause an error.

Internal class `inip::Section` use `std::map<std::string, inip::Node>` to restore section data, with keys and Nodes.

#### INI
INI is a collection of Sections. Optionally start with a golbal section with name.
```ini
global_key = value

[FirstSection]

[SecondSection]

```

#### Andance Grammar
> **Note** 
> This advanced grammars can be only loaded in this library.
> 
> Other INI parser may parse this format with a completely wrong result.

- Keywords
  
The following chars can be viewed as keywords.
| Char | Usage |
| ---- | ------|
| `[`  | Begin of a section name |
| `]`  | End of a section name |
| `=`  | Delimiter of key and value in a node |
| `;`  | Comment |
| `#`  | Comment |

- Escape Chars (`\`)
  
Escape char (`\`) can be only use before a keyword to avoid being parsed as keyword.
A string made up of a escape char and a keyword can be called escape sequence.

Keyword behind `\` will be seen as common text. As a consequences, only the following
escape sequences are valid:
| `\[`  | `\]`  |  `\;` | `\#`  | `\=`  |
| ----- | ----- | ----- | ----- | ----- |

If they are in keys or values, the escape char will be removed,
you can use raw chars directly. e.g.
```
key\=With\=Equal = Value with \;\# Comment ; real comment

Key = "key=With=Equal"
Value = "Value with ;# Comment"
```

Currently, `\` itself should use double `\\`, but we **DO NOT** parse it to a signle `\`,
we only parse the escape sequence we understand. Even `\n`, `\t` will be reserved originally.

- Long String

If a value is too long to be written in one line, use `\` at the end of line to use next line. A escaped `\` (like `\\`) or anything else at the end of line will stop this. 
Every new line will not be trimed. e.g.
```ini
[Section]
LongString = This is \
a very \
   long string \\ ; valid "This is a very    long string \\"

LongString = This is \
a very \\ ; invalid, stop here
   long string
```
