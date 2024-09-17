cmake_minimum_required(VERSION 3.23)

project(
    CppCon2024_StdFn
    VERSION 0.0.1
    DESCRIPTION "Some examples to show limitation of std::function and proposed workarounds"
    HOMEPAGE_URL "https://github.com/adchawla/CppCon2024_StdFn")

if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

add_subdirectory(./src)

if(PROJECT_IS_TOP_LEVEL)
#    add_subdirectory(common)
    add_subdirectory(tests)
    if (INCLUDE_BENCHMARKS)
        add_subdirectory(benchmarks)
    endif()
endif()
