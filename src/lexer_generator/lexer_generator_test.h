#ifndef SUBCC__LEXER_GENERATOR_TEST_H
#define SUBCC__LEXER_GENERATOR_TEST_H

#include "lexer_generator.h"
#include "fcntl.h" // for open, read
//#include "unistd.h" // for constants

namespace subcc
{
void lexerTest();
void lexerTest(std::string const &inputText);
void printToken(NfaLexerGenerator<>::Token const &);
} // namespace subcc

#endif