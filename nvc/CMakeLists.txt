cmake_minimum_required(VERSION 3.20)
project(nvc CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Core Library
add_library(nava_core STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/Lexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/Parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/Grammar.cpp

)

if (EMSCRIPTEN)
    # WebAssembly Build
    message(STATUS "Building for WebAssembly (WASM)")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s WASM=1 -s EXPORT_ALL=1")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android" OR CMAKE_SYSTEM_NAME STREQUAL "iOS")
    message(STATUS "Building for Mobile (${CMAKE_SYSTEM_NAME})")
    # Only the core library is built for mobile as requested. No LLVM linkage required.
else()
    # Native Desktop Build
    find_package(LLVM REQUIRED CONFIG)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

    include_directories(${LLVM_INCLUDE_DIRS})
    separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
    add_definitions(${LLVM_DEFINITIONS_LIST})

    # Build Runtime Library
    add_library(nvc_runtime STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/cli/runtime.cpp)

    # Build Compiler CLI
    add_executable(nvc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/cli/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/cli/CodeGen.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/SandhiPass.cpp
    )

    # Link Core and LLVM
    target_link_libraries(nvc PRIVATE nava_core LLVM)
endif()
