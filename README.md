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

