# 生成全局配置头文件

string(TIMESTAMP TIMESTAMP "%Y-%m-%d %H:%M:%S")

configure_file(config.h.in ${CMAKE_BINARY_DIR}/include/config.h @ONLY)

configure_file(version.h.in ${CMAKE_BINARY_DIR}/include/version.h @ONLY)
