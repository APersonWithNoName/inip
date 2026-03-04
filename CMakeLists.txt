cmake_minimum_required(VERSION 3.10)
project(inip)

# =============== 项目基本配置 ===============
## 设置 c++ 版本
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
## 设置使用策略为最新
cmake_policy(VERSION ${CMAKE_VERSION})
## 输出 compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ===============   构建选项   ===============
## 是否启用GoogleTest
option(INIP_ENABLE_GOOGLETEST
	"Build GoogleTest."
	ON
)
## 是否构建Demo
option(INIP_ENABLE_DEMO
	"Build demo."
	ON	
)

# =============== 指定第三方库选项 ===============
set(INIP_GOOGLETEST_PATH ${CMAKE_SOURCE_DIR}/3rdlib/googletest)  # 参数相关
set(INIP_CLI11_PATH      ${CMAKE_SOURCE_DIR}/3rdlib/CLI11)       # Demo依赖
set(INIP_REPLXX_PATH     ${CMAKE_SOURCE_DIR}/3rdlib/replxx)      # Demo依赖
if(INIP_ENABLE_GOOGLETEST)
	add_subdirectory(${INIP_GOOGLETEST_PATH} )
	enable_testing()
endif()
if(INIP_ENABLE_DEMO)
	add_subdirectory(${INIP_CLI11_PATH})
	add_subdirectory(${INIP_REPLXX_PATH} EXCLUDE_FROM_ALL)
endif()

# =============== 库构建相关 ===============
# 设置库名称
set(DEMO_NAME
    ${PROJECT_NAME}_demo
    CACHE STRING "Name of inip demo"
)
set(LIB_NAME
    ${PROJECT_NAME}
    CACHE STRING "Name of the dynamic library"
)
set(ORIGINAL_STATIC_LIB_NAME
    ${LIB_NAME}_static
    CACHE STRING "Name of the static library"
)
# 源文件
include_directories(include)
include_directories(${CMAKE_BINARY_DIR}/include)
# 添加子目录
add_subdirectory(src/library)  # 核心库
if(INIP_ENABLE_GOOGLETEST)
	add_subdirectory(src/test) # 测试
endif()
if(INIP_ENABLE_DEMO)
	add_subdirectory(src/demo) # Demo
endif()
# 内部检查宏
add_definitions(
    -DINIP_COMPILATION
)
add_compile_definitions(INIP_COMPILATION)


# =============== 安装 ===============
# 安装头文件
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
