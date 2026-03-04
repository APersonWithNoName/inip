# 构建测试
function(run_googletest) 
    # 查找 GoogleTest 包
    find_package(GTest REQUIRED)
    
    # 递归收集目录下所有 test_*.cpp 文件
    file(GLOB_RECURSE TEST_FILES "test_*.cpp")
    
    # 为每个测试文件创建独立的可执行文件
    foreach (TEST_FILE ${TEST_FILES})
        # 获取不包含扩展名的文件名
        get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
        
        # 创建仅包含当前测试文件的可执行文件
        add_executable(${TEST_NAME} ${TEST_FILE})
        
        # 添加连接库
        target_link_libraries(${TEST_NAME} PRIVATE
            GTest::gtest_main
            ${ORIGINAL_STATIC_LIB_NAME}
        )
        
        # 添加测试
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
        
        message(STATUS "Create Test : ${TEST_NAME} (From ${TEST_FILE})")
    endforeach()
endfunction()

if (INIP_ENABLE_GOOGLETEST)
    message(STATUS "GoogleTest : ON")
    run_googletest()
endif()
