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

add_executable (LexerGeneratorMain lexer_generator/main)
add_library (LexerGeneratorTest lexer_generator/lexer_generator_test)
target_link_libraries (LexerGeneratorMain  LexerGeneratorTest)

add_executable (ParserGeneratorMain parser_generator/main)
add_library (ParserGeneratorTest parser_generator/parser_generator_test)
add_library (ParserGenerator parser_generator/parser_generator)
target_link_libraries (ParserGeneratorMain ParserGeneratorTest ParserGenerator)

add_executable (SubccMain subcc/main)
add_library (SubccCodeGenerator subcc/code_generator)
add_library (SubccCompiler subcc/compiler)
add_library (SubccLexer subcc/lexer)
add_library (SubccParser subcc/parser)
add_library (SubccSymbolTable subcc/symbol_table)
add_library (SubccSyntaxTree subcc/syntax_tree)
add_library (SubccTest subcc/test)
add_library (SubccThreeAddress subcc/three_address)
add_library (SubccType subcc/type)
add_library (SubccUtility subcc/utility)

target_link_libraries (SubccMain SubccTest SubccCompiler SubccCodeGenerator SubccParser
  SubccLexer SubccSymbolTable SubccSyntaxTree SubccThreeAddress SubccType SubccUtility
  ParserGenerator)

add_executable (InterpreterMain interpreter/main)
add_library (InterpreterTest interpreter/interpreter_test)
add_library (Interpreter interpreter/interpreter)
add_library (InterpreterAssembly interpreter/assembly)
target_link_libraries (InterpreterMain InterpreterTest Interpreter InterpreterAssembly)


add_definitions(-Wfatal-errors)