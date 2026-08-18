cmake_minimum_required(VERSION 3.20)
project(nvc CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT EMSCRIPTEN)
    # Robust LLVM discovery across distros (Ubuntu/MSYS2/macOS).
    # Users may pass -DLLVM_DIR=<path-to-LLVMConfig.cmake>.
    if(NOT DEFINED LLVM_DIR AND DEFINED ENV{LLVM_DIR})
        set(LLVM_DIR $ENV{LLVM_DIR})
    endif()

    find_package(LLVM REQUIRED CONFIG)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

    include_directories(${LLVM_INCLUDE_DIRS})
    separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
    add_definitions(${LLVM_DEFINITIONS_LIST})

    # Map the LLVM components NVC actually exercises. Keep the list
    # conservative so it works with standard distro packages and MSYS2.
    llvm_map_components_to_libnames(LLVM_LIBS
        Core
        Support
        IRReader
        Analysis
        TransformUtils
        Passes
        Target
        X86
        native
        AsmParser
        AsmPrinter
        CodeGen
        MC
        MCParser
        Object
        BitReader
        BitWriter
        MCJIT
        Option
        ProfileData
        TargetParser
        BinaryFormat
    )
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Build Runtime Library
add_library(nvc_runtime STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/runtime.cpp)

# Build Compiler
if (EMSCRIPTEN)
    set(SRC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Lexer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Parser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Grammar.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Interpreter.cpp)
    add_executable(nvc ${SRC_FILES})
    
    # Export compileSanskrit to JS and allow cwrap/UTF8ToString
    set_target_properties(nvc PROPERTIES LINK_FLAGS "-fexceptions -s DISABLE_EXCEPTION_CATCHING=0 -s ASSERTIONS=1 -s EXPORTED_RUNTIME_METHODS=['cwrap','UTF8ToString'] -s EXPORTED_FUNCTIONS=['_compileSanskrit'] -s ALLOW_MEMORY_GROWTH=1 -s MODULARIZE=1 -s EXPORT_NAME='NavaCompiler' -s ENVIRONMENT='web,worker,node'")
else()
    set(SRC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Lexer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Parser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Grammar.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/CodeGen.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SandhiPass.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Interpreter.cpp)
    add_executable(nvc ${SRC_FILES})
    target_link_libraries(nvc PRIVATE ${LLVM_LIBS})
endif()
