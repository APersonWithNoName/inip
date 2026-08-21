# Function to run googletest
function(run_googletest)
  file(GLOB_RECURSE test_files "test_*.cpp")

  foreach(test_file ${test_files})
    get_filename_component(TEST_NAME ${test_file} NAME_WE)

    add_executable(${TEST_NAME} ${test_file})

    target_link_libraries(${TEST_NAME} PRIVATE GTest::gtest_main
                                               ${ORIGINAL_STATIC_LIB_NAME})

    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    message(STATUS "Create Test : ${TEST_NAME} (From ${test_file})")
  endforeach()
endfunction()

if(INIP_ENABLE_GOOGLETEST)
  message(STATUS "GoogleTest : ON")
  run_googletest()
endif()
