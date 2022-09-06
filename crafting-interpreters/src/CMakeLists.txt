cmake_minimum_required(VERSION 2.8.12.2)
project(clox C)

set(CMAKE_C_FLAGS "-std=c99")
set(CMAKE_C_STANDARD 99)

add_executable(clox main.c common.h chunk.h chunk.c memory.h memory.c debug.c debug.h
        value.c value.h vm.h vm.c compiler.c compiler.h scanner.c scanner.h object.c object.h table.c table.h)