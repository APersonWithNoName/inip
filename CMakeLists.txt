cmake_minimum_required(VERSION 3.10)
project(inip)

# 设置 c++ 版本
set(CMAKE_CXX_VERSION 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# 设置 buildtype 为 Debug
if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE
      "Debug"
      CACHE STRING "Build type")
endif()

# 输出 compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置库名称
set(DEMO_NAME
    ${PROJECT_NAME}_demo
    CACHE STRING "Name of inip demo")
set(LIB_NAME
    ${PROJECT_NAME}
    CACHE STRING "Name of the dynamic library")
set(ORIGINAL_STATIC_LIB_NAME
    ${LIB_NAME}_static
    CACHE STRING "Name of the static library")

# 源文件
include_directories(include)
include_directories(${CMAKE_BINARY_DIR}/include)

# 添加子目录
add_subdirectory(src/config)
add_subdirectory(src/library)
add_subdirectory(src/demo)

# 安装头文件
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
