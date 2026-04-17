cmake_minimum_required(VERSION 3.20)
project(encoding VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(encoding
    src/utf_convert.cpp
    src/platform_convert.cpp
    src/convert.cpp
)

target_include_directories(encoding PUBLIC include)
target_include_directories(encoding PRIVATE src)

if(MSVC)
    target_compile_options(encoding PRIVATE /utf-8)
endif()

if(UNIX)
    # iconv通常随libc提供，macOS需要显式链接
    if(APPLE)
        target_link_libraries(encoding PRIVATE iconv)
    endif()
endif()

# 测试
option(ENCODING_BUILD_TESTS "Build tests" ON)
if(ENCODING_BUILD_TESTS)
    add_executable(encoding_test test/test_main.cpp)
    target_link_libraries(encoding_test PRIVATE encoding)
    if(MSVC)
        target_compile_options(encoding_test PRIVATE /utf-8)
    endif()
endif()
