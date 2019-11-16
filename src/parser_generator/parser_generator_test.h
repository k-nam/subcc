#ifndef SUBCC__PARSER_GENERATOR_TEST_H
#define SUBCC__PARSER_GENERATOR_TEST_H

#include "parser_generator.h"
#include "fcntl.h" // for open, read
//#include "unistd.h" // for constants

#include <iostream>

namespace subcc
{
using std::cout;
using std::endl;

enum ExpTokens
{
  ExpTokens_ID = 3,
  ExpTokens_PLUS = 4,
  ExpTokens_MINUS = 5,
  ExpTokens_MULTI = 6,
  ExpTokens_OPENPAREN = 7,
  expTokens_CLOSEPAREN = 8,
  ExpTokens_EXPR = 251,
  ExpTokens_TERM = 252,
  ExpTokens_FACTOR = 253
};

enum ValueTokens
{
  ValueTokens_ID = 1,
  ValueTokens_STAR = 2,
  ValueTokens_EQUAL = 3,
  ValueTokens_L = 251,
  ValueTokens_R = 252,
  ValueTokens_S = 253
};

enum IfTokens
{
  IfTokens_IF = 2,
  IfTokens_ELSE = 3,
  IfTokens_STMT = 251,
  IfTokens_ELSESTMT = 252
};
} // namespace subcc

namespace subcc
{
void parserTest();
}

#endif