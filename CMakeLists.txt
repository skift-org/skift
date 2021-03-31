cmake_minimum_required(VERSION 3.10)
project(skift CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(HOSTED_ENV)
add_compile_definitions(__CONFIG_IS_RELEASE__=0)

add_subdirectory(libraries)
add_subdirectory(tests)