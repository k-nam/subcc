#include "parser_generator_test.h"

void subcc::parserTest()
{
	std::cout << "------------- Beginning parser test --------------\n\n";
	ContextFreeLanguage lang(250, 500);
	ContextFreeLanguage lang2(250, 500);
	ContextFreeLanguage lang3(250, 500);
	vector<int> productionKey;
	// 0: E -> E + T
	lang.add(1, ExpTokens_EXPR, ExpTokens_EXPR, ExpTokens_PLUS, ExpTokens_TERM, kEndMarker);
	// 1: E -> E - T
	lang.add(2, ExpTokens_EXPR, ExpTokens_EXPR, ExpTokens_MINUS, ExpTokens_TERM, kEndMarker);
	// 2: E -> T
	lang.add(3, ExpTokens_EXPR, ExpTokens_TERM, kEndMarker);
	// 3: T -> T * F
	lang.add(4, ExpTokens_TERM, ExpTokens_TERM, ExpTokens_MULTI, ExpTokens_FACTOR, kEndMarker);
	// 4: T -> F
	lang.add(5, ExpTokens_TERM, ExpTokens_FACTOR, kEndMarker);
	// 5: F -> ID
	lang.add(6, ExpTokens_FACTOR, ExpTokens_ID, kEndMarker);
	// 6: F -> ( E )
	lang.add(7, ExpTokens_FACTOR, ExpTokens_OPENPAREN, ExpTokens_EXPR, expTokens_CLOSEPAREN, kEndMarker);

	// 0. S -> L = R
	lang2.add(1, ValueTokens_S, ValueTokens_L, ValueTokens_EQUAL, ValueTokens_R, kEndMarker);
	// 1. S -> R
	lang2.add(2, ValueTokens_S, ValueTokens_R, kEndMarker);
	// 2. R -> L
	lang2.add(3, ValueTokens_R, ValueTokens_L, kEndMarker);
	// 3. L -> id
	lang2.add(4, ValueTokens_L, ValueTokens_ID, kEndMarker);
	// 4. L -> *R
	lang2.add(5, ValueTokens_L, ValueTokens_STAR, ValueTokens_R, kEndMarker);

	lang3.add(1, IfTokens_STMT, IfTokens_IF, IfTokens_STMT, IfTokens_ELSESTMT, kEndMarker);
	lang3.add(2, IfTokens_ELSESTMT, kEndMarker);
	lang3.add(3, IfTokens_ELSESTMT, IfTokens_ELSE, IfTokens_STMT, kEndMarker);

	lang.finalize();
	lang2.finalize();
	lang3.finalize();
	cout << "\n";

	SlrParserGenerator slr_parser(lang);
	slr_parser.build();
	Lr1ParserGenerator lr1_parser(lang);
	lr1_parser.build();
	LalrParserGenerator lalr_parser(lang);
	lalr_parser.build();
	LalrParserGenerator lalr_parser2(lang);
	lalr_parser2.buildFast();
	cout << "\n";

	slr_parser.parse(ExpTokens_ID, ExpTokens_MULTI, ExpTokens_ID, ExpTokens_PLUS,
					 ExpTokens_OPENPAREN, ExpTokens_ID, ExpTokens_MULTI, ExpTokens_ID,
					 ExpTokens_PLUS, ExpTokens_ID, expTokens_CLOSEPAREN, ExpTokens_MULTI,
					 ExpTokens_ID, kEndMarker);
	lr1_parser.parse(ExpTokens_ID, ExpTokens_MULTI, ExpTokens_ID, ExpTokens_PLUS,
					 ExpTokens_OPENPAREN, ExpTokens_ID, ExpTokens_MULTI, ExpTokens_ID,
					 ExpTokens_PLUS, ExpTokens_ID, expTokens_CLOSEPAREN, ExpTokens_MULTI,
					 ExpTokens_ID, kEndMarker);
	lalr_parser.parse(ExpTokens_ID, ExpTokens_MULTI, ExpTokens_ID, ExpTokens_PLUS,
					  ExpTokens_OPENPAREN, ExpTokens_ID, ExpTokens_MULTI, ExpTokens_ID,
					  ExpTokens_PLUS, ExpTokens_ID, expTokens_CLOSEPAREN, ExpTokens_MULTI,
					  ExpTokens_ID, kEndMarker);
	lalr_parser2.parse(ExpTokens_ID, ExpTokens_MULTI, ExpTokens_ID, ExpTokens_PLUS,
					   ExpTokens_OPENPAREN, ExpTokens_ID, ExpTokens_MULTI, ExpTokens_ID,
					   ExpTokens_PLUS, ExpTokens_ID, expTokens_CLOSEPAREN, ExpTokens_MULTI,
					   ExpTokens_ID, kEndMarker);
	/*
  lr1_parser.parse(ValueTokens_STAR, ValueTokens_STAR, ValueTokens_STAR, 
		ValueTokens_ID, ValueTokens_EQUAL, ValueTokens_STAR, ValueTokens_STAR, 
		ValueTokens_ID, kEndMarker);
  lalr_parser.parse(ValueTokens_STAR, ValueTokens_STAR, ValueTokens_STAR, 
		ValueTokens_ID, ValueTokens_EQUAL, ValueTokens_STAR, ValueTokens_STAR, 
		ValueTokens_ID, kEndMarker);
  lalr_parser2.parse(ValueTokens_STAR, ValueTokens_STAR, ValueTokens_STAR, 
		ValueTokens_ID, ValueTokens_EQUAL, ValueTokens_STAR, ValueTokens_STAR, 
		ValueTokens_ID, kEndMarker);
		*/
	std::cout << "------------- Parser test finished --------------\n\n\n";
}