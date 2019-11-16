#ifndef LIB_CALVIN__LEXER__LEXER_TEST_H
#define LIB_CALVIN__LEXER__LEXER_TEST_H

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