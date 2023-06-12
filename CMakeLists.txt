cmake_minimum_required(VERSION 3.17)
project(CC-Core)

set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE CC_CORE_SOURCES "src/*.c")
file(GLOB_RECURSE CC_CORE_HEADERS "include/*.h")

add_library(CC-Core STATIC ${CC_CORE_SOURCES} ${CC_CORE_HEADERS})

if(MSVC)
    target_compile_options(CC-Core PRIVATE /W4 /WX)
else()
    target_compile_options(CC-Core PRIVATE -Wall -Wextra -Wpedantic -Werror -Wno-aggressive-loop-optimizations)
endif()

target_include_directories(CC-Core PUBLIC include/)