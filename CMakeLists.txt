cmake_minimum_required(VERSION 3.5)
project(rockdbbench)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_subdirectory(rocksdb)

add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-std=c++14>)
add_compile_options(-g)
if(CLANG)
  if(APPLE)
    add_compile_options(-stdlib=libc++)
  endif()
endif()


set(SRCS main.cpp)
add_executable(rockbench ${SRCS})
target_include_directories(rockbench PUBLIC ${PROJECT_SOURCE_DIR}/rocksdb/include)
target_link_libraries(rockbench rocksdb)
