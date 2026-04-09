cmake_minimum_required(VERSION 3.20)
project(nvc CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Build Runtime Library
add_library(nvc_runtime STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/runtime.cpp)

# Build Compiler
set(SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Lexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CodeGen.cpp)
add_executable(nvc ${SRC_FILES})

# Link LLVM directly
target_link_libraries(nvc PRIVATE LLVM)
