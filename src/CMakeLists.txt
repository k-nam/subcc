cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_FLAGS "")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project (LexerGenerator)

include_directories ("${PROJECT_SOURCE_DIR}/lexer_generator/")
include_directories ("${PROJECT_SOURCE_DIR}/parser_generator/")
include_directories ("${PROJECT_SOURCE_DIR}/subcc/")

add_library (LexerGeneratorTest lexer_generator/lexer_generator_test)

add_executable (LexerGenerator lexer_generator/lexer_generator_main)

target_link_libraries (LexerGenerator  LexerGeneratorTest)


add_definitions(-Wfatal-errors)