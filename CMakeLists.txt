cmake_minimum_required(VERSION 3.10)
project(
  inip
  VERSION 0.1.0
  DESCRIPTION "Tiny INI parser by C++ 11"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_policy(VERSION ${CMAKE_VERSION})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(INIP_ENABLE_GOOGLETEST "Build GoogleTest" ON)
option(INIP_ENABLE_DOXYGEN "Build Doxygen Documents" ON)
option(INIP_ENABLE_CMAKE_CONFIG "Build CMake Config Files" ON)
option(INSTALL_GTEST "Install Gtest libraries" OFF)
option(INIP_ENABLE_PKGCONFIG "Build pkg-config .pc configure" ON)

set(INIP_GOOGLETEST_PATH ${CMAKE_SOURCE_DIR}/3rdlib/googletest)
if(INIP_ENABLE_GOOGLETEST)
  add_subdirectory(${INIP_GOOGLETEST_PATH})
  enable_testing()
endif()

set(LIB_NAME
    ${PROJECT_NAME}
    CACHE STRING "Name of the dynamic library")
set(ORIGINAL_STATIC_LIB_NAME
    ${LIB_NAME}_static
    CACHE STRING "Name of the static library")

include_directories(include)
add_subdirectory(src/library)
if(INIP_ENABLE_GOOGLETEST)
  add_subdirectory(src/test)
endif()

# cmake-lint: disable=E1122
install(
  DIRECTORY include/
  DESTINATION include
  FILES_MATCHING
  PATTERN "*.h"
  PATTERN "*.hpp")

if(INIP_ENABLE_DOXYGEN)
  find_package(Doxygen REQUIRED)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
                 ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

  add_custom_target(
    docs
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)
endif()

if(INIP_ENABLE_PKGCONFIG)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.pc.in
                 ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)

  install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.pc
          DESTINATION lib/pkgconfig/)
endif()

# Sre https://cmake.org/cmake/help/latest/module/CMakePackageConfigHelpers.html
if(INIP_ENABLE_CMAKE_CONFIG)
  install(
    EXPORT ${PROJECT_NAME}
    FILE "${PROJECT_NAME}.cmake"
    DESTINATION "lib/cmake/${PROJECT_NAME}")

  set(INCLUDE_DIRS include)
  set(LIBRARIES ${LIB_NAME})
  set(LIB_DIR lib)

  include(CMakePackageConfigHelpers)

  write_basic_package_version_file(
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    COMPATIBILITY SameMajorVersion
    VERSION ${PROJECT_VERSION})

  configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake.in
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
    PATH_VARS INCLUDE_DIRS LIBRARIES LIB_DIR
    INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

  install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
                ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
          DESTINATION lib/cmake/${PROJECT_NAME})
endif()
