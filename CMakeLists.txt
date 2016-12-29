cmake_minimum_required(VERSION 3.6)
project(json)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        src/json.h
        src/characters.c
        src/buffer.c src/buffer_internal.h
        src/errors.c src/errors_internal.h
        src/tokenizer.c src/tokenizer_internal.h
        src/main.c)

add_executable(json ${SOURCE_FILES})