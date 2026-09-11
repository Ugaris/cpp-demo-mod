cmake_minimum_required(VERSION 3.16)
project(ugaris-cpp-demo-mod CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output name
set(MOD_OUTPUT_NAME "cpp_demo_mod")

# Create shared library
add_library(${MOD_OUTPUT_NAME} SHARED
    src/main.cpp
)

# Platform-specific settings
if(WIN32)
    set_target_properties(${MOD_OUTPUT_NAME} PROPERTIES
        PREFIX ""
        SUFFIX ".dll"
    )
    # Windows: link the client's import library (lib/moac.lib or moac.a from
    # the client release's mod-sdk.zip). A DLL built with unresolved client
    # symbols (/FORCE:UNRESOLVED) loads, but the first client API call jumps
    # to a null pointer and crashes the game.
    if(EXISTS "${CMAKE_SOURCE_DIR}/lib/moac.a" AND NOT MSVC)
        target_link_libraries(${MOD_OUTPUT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/lib/moac.a")
    elseif(EXISTS "${CMAKE_SOURCE_DIR}/lib/moac.lib")
        target_link_libraries(${MOD_OUTPUT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/lib/moac.lib")
    else()
        message(FATAL_ERROR "Windows builds need lib/moac.lib (or lib/moac.a) from the client mod-sdk.zip - see README")
    endif()
elseif(APPLE)
    set_target_properties(${MOD_OUTPUT_NAME} PROPERTIES
        PREFIX ""
        SUFFIX ".dylib"
    )
    if(CMAKE_OSX_ARCHITECTURES STREQUAL "")
        set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for macOS")
    endif()
    # macOS: Allow undefined symbols (resolved at runtime by host application)
    target_link_options(${MOD_OUTPUT_NAME} PRIVATE -undefined dynamic_lookup)
else()
    set_target_properties(${MOD_OUTPUT_NAME} PROPERTIES
        PREFIX ""
        SUFFIX ".so"
    )
    target_compile_options(${MOD_OUTPUT_NAME} PRIVATE -fvisibility=hidden)
    # Linux: Allow undefined symbols (default, but be explicit)
    target_link_options(${MOD_OUTPUT_NAME} PRIVATE -Wl,--allow-shlib-undefined)
endif()

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${MOD_OUTPUT_NAME} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

# Release optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${MOD_OUTPUT_NAME} PRIVATE -O2)
    endif()
endif()

install(TARGETS ${MOD_OUTPUT_NAME}
    LIBRARY DESTINATION .
    RUNTIME DESTINATION .
)
