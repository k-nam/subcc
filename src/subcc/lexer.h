#ifndef LIB_CALVIN__SUBCC__SUBCC_LEXER_H
#define LIB_CALVIN__SUBCC__SUBCC_LEXER_H

#include "lexer_generator.h"
#include <string>
#include <iostream>

namespace subcc
{

class Lexer
{
public:
	typedef typename RegularExpression<>::ParseTreeNode Node;
	typedef NfaLexerGenerator<> NFA;
	typedef DfaLexerGenerator<> DFA;
	typedef NfaLexerGenerator<>::Token Token;
	Lexer(string const &inText) : text_(inText), index_(0), lineNum_(1)
	{
		std::cout << text_;
		build();
		std::cout << "Lexer generating took " << "sec\n";
	}
	void initialize()
	{ // start from the beginning of text again
		index_ = 0;
		lineNum_ = 1;
	}
	int getNextToken(); // return Token number
	int getLineNum() const
	{ // return current line number
		return lineNum_;
	}
	string getLexeme() const
	{ // return matched string
		return curLexeme_;
	}

private:
	void build();
	NFA nfa;
	DFA dfa;
	string const &text_; // input source code to lex
	int index_;			   // current position of reading
	int lineNum_;		   // count current line number from start
	string curLexeme_;   // store currently matched string
};

enum Tokens
{
	WHITESPACE = 200,
	COMMENT = 201,
	SHORT = 1,
	INT = 2,
	STRUCT = 3,
	SIZEOF = 4,
	TYPEDEF = 5,
	IF = 6,
	ELSE = 7,
	WHILE = 8,
	SWITCH = 9,
	CASE = 10,
	DEFAULT = 11,
	BREAK = 12,
	CONTINUE = 16,
	RETURN = 13,
	CHAR = 14,
	VOID_ = 15,
	ID = 100,
	CHAR_LITERAL = 150,
	STRING_LITERAL = 151,
	DECIMAL_LITERAL = 152,
	HEXADECIMAL_LITERAL = 153,
	ARITH_PLUS = 20,
	ARITH_INCREMENT = 21,
	ARITH_MINUS = 22,
	ARITH_DECREMENT = 23,
	ARITH_MULTI = 24,
	ARITH_DIVIDE = 25,
	ARITH_MOD = 26,
	LOGIC_BIT_AND = 30,
	LOGIC_BIT_OR = 31,
	LOGIC_BIT_XOR = 32,
	LOGIC_BIT_NOT = 33,
	LOGIC_AND = 34,
	LOGIC_OR = 35,
	LOGIC_NOT = 36,
	COMP_EQ = 40,
	COMP_GT = 41,
	COMP_LT = 42,
	COMP_GE = 43,
	COMP_LE = 44,
	COMP_NE = 45,
	ASSIGN = 50,
	ASSIGN_PLUS = 51,
	ASSIGN_MINUS = 52,
	ASSIGN_MULTI = 53,
	ASSIGN_DIVIDE = 54,
	ASSIGN_MOD = 55,
	ASSIGN_BIT_XOR = 56,
	ASSIGN_BIT_AND = 57,
	ASSIGN_BIT_OR = 58,
	PUNC_PERIOD = 70,
	PUNC_COMMA = 71,
	PUNC_COLON = 72,
	PUNC_SEMICOLON = 73,
	DELIM_PAREN_OPEN = 80,
	DELIM_PAREN_CLOSE = 81,
	DELIM_BRACKET_OPEN = 82,
	DELIM_BRACKET_CLOSE = 83,
	DELIM_BRACE_OPEN = 84,
	DELIM_BRACE_CLOSE = 85,

	DEREFERENCE = 90,
	AMPERSAND = 91,

	USERDEFTYPE = 160
};

} // namespace subcc

#endif
