# x86_64-mingw-toolchain.cmake

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER /usr/bin/x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER /usr/bin/x86_64-w64-mingw32-windres)

set(CMAKE_AR /usr/bin/x86_64-w64-mingw32-ar)
set(CMAKE_LINKER /usr/bin/x86_64-w64-mingw32-ld)
set(CMAKE_OBJCOPY /usr/bin/x86_64-w64-mingw32-objcopy)
set(CMAKE_STRIP /usr/bin/x86_64-w64-mingw32-strip)
set(CMAKE_RANLIB /usr/bin/x86_64-w64-mingw32-ranlib)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_C_FLAGS_INIT "-D_WIN32_WINNT=0x0601")  # Windows 7 兼容
set(CMAKE_CXX_FLAGS_INIT "${CMAKE_C_FLAGS_INIT}")
