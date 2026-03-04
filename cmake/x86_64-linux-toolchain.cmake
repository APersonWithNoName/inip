# x86_64-linux-toolchain.cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER /usr/bin/x86_64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/x86_64-linux-gnu-g++)

set(CMAKE_AR /usr/bin/x86_64-linux-gnu-ar)
set(CMAKE_LINKER /usr/bin/x86_64-linux-gnu-ld)
set(CMAKE_OBJCOPY /usr/bin/x86_64-linux-gnu-objcopy)
set(CMAKE_STRIP /usr/bin/x86_64-linux-gnu-strip)
set(CMAKE_RANLIB /usr/bin/x86_64-linux-gnu-ranlib)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_C_FLAGS_INIT "-march=x86-64-v2")
set(CMAKE_CXX_FLAGS_INIT "${CMAKE_C_FLAGS_INIT}")
