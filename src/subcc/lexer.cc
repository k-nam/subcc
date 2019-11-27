/**  SubccLexer.cc **/
#include "lexer.h"

#define CAT CAT<char>
#define OR OR<char>
#define STAR STAR<char>
#define PLUS PLUS<char>
#define ONEORZERO ONEORZERO<char>

namespace subcc
{

// Build lexical analyzer for subcc
void Lexer::build()
{
	/******** Configuring Lexer Generator *******/

	// As I don't have regex parser, I need to do some manual setting
	// prepare intermediate expressions
	string letterString("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	string digitString("0123456789");
	string nonZeroDigitString("123456789");
	string underScoreString("_");
	string esc_a("\\a");
	string esc_b("\\b");
	string esc_f("\\f");
	string esc_n("\\n");
	string esc_r("\\r");
	string esc_v("\\v");
	string esc_sq("\\\'"); // single quote
	string esc_dq("\\\""); // double quote
	string esc_bs("\\\\"); // backslash
	string esc_null("\\0");

	shared_ptr<Node> whitespaceNode;
	shared_ptr<Node> letterNode, digitNode, nonZeroDigitNode;
	shared_ptr<Node> idNode;
	// both // and /* */
	shared_ptr<Node> cCommentNode, cppCommentNode, commentsNode;
	// ' can not be inside a char literal! ( \' is fine)
	shared_ptr<Node> escapeCharNode, charLiteralNode;
	// " or newline can not be inside a string literal! ( \" is fine)
	shared_ptr<Node> stringLiteralNode;
	shared_ptr<Node> decimalNode, hexNode;

	whitespaceNode = PLUS(ONEOF(string(" \t\n")));
	letterNode = ONEOF(letterString);
	digitNode = ONEOF(digitString);
	nonZeroDigitNode = ONEOF(nonZeroDigitString);
	idNode = CAT(OR(letterNode, LITERAL(underScoreString)),
				 STAR(OR(OR(letterNode, digitNode), LITERAL(underScoreString))));
	cppCommentNode = CAT(LITERAL(string("//")),
						 CAT(STAR(NOTOF(string("\n"))),
							 LITERAL(string("\n"))));
	// \/*([^*]|(*)+[^*/])*(*)+\/
	cCommentNode = CAT(LITERAL(string("/*")),
					   CAT(STAR(OR(NOTOF(string("*")),
								   CAT(PLUS(LITERAL(string("*"))), NOTOF(string("/*"))))),
						   CAT(PLUS(LITERAL(string("*"))), LITERAL(string("/")))));
	commentsNode = OR(cCommentNode, cppCommentNode);
	escapeCharNode = OR(LITERAL(esc_a), OR(LITERAL(esc_b), OR(LITERAL(esc_f),
															  OR(LITERAL(esc_n), OR(LITERAL(esc_r), OR(LITERAL(esc_v),
																									   OR(LITERAL(esc_sq), OR(LITERAL(esc_dq),
																															  OR(LITERAL(esc_bs),
																																 LITERAL(esc_null))))))))));
	charLiteralNode = CAT(LITERAL(string("\'")),
						  CAT(OR(NOTOF(string("\'")), escapeCharNode),
							  LITERAL(string("\'"))));
	//  \"([^\"\n]|[\t\n\r...]|(\\\n))*\"
	stringLiteralNode = CAT(LITERAL(string("\"")),
							CAT(STAR(OR(OR(NOTOF(string("\"\n")), escapeCharNode),
										LITERAL(string("\\\n")))),
								LITERAL(string("\""))));
	decimalNode = OR(CAT(nonZeroDigitNode,
						 CAT(STAR(digitNode), ONEORZERO(CAT(LITERAL(string(".")),
															PLUS(digitNode))))),
					 LITERAL(string("0")));
	hexNode = CAT(ONEORZERO(ONEOF(string("+-"))),
				  CAT(LITERAL(string("0x")),
					  PLUS(OR(digitNode, ONEOF(string("abcdefABCDEF"))))));

	// Insert each regex into NFA simulator
	// First entered regex has higher priority than later entered ones
	nfa.addRegularExpression(WHITESPACE, whitespaceNode);
	nfa.addRegularExpression(COMMENT, commentsNode);
	nfa.addRegularExpression(SHORT, LITERAL(string("short")));
	nfa.addRegularExpression(INT, LITERAL(string("int")));
	nfa.addRegularExpression(STRUCT, LITERAL(string("struct")));
	nfa.addRegularExpression(SIZEOF, LITERAL(string("sizeof")));
	nfa.addRegularExpression(TYPEDEF, LITERAL(string("typedef")));
	nfa.addRegularExpression(IF, LITERAL(string("if")));
	nfa.addRegularExpression(ELSE, LITERAL(string("else")));
	nfa.addRegularExpression(WHILE, LITERAL(string("while")));
	nfa.addRegularExpression(SWITCH, LITERAL(string("switch")));
	nfa.addRegularExpression(CASE, LITERAL(string("case")));
	nfa.addRegularExpression(DEFAULT, LITERAL(string("default")));
	nfa.addRegularExpression(BREAK, LITERAL(string("break")));
	nfa.addRegularExpression(CONTINUE, LITERAL(string("continue")));
	nfa.addRegularExpression(RETURN, LITERAL(string("return")));
	nfa.addRegularExpression(CHAR, LITERAL(string("char")));
	nfa.addRegularExpression(VOID_, LITERAL(string("void")));
	nfa.addRegularExpression(ID, idNode);
	nfa.addRegularExpression(CHAR_LITERAL, charLiteralNode);
	nfa.addRegularExpression(STRING_LITERAL, stringLiteralNode);
	nfa.addRegularExpression(DECIMAL_LITERAL, decimalNode);
	nfa.addRegularExpression(HEXADECIMAL_LITERAL, hexNode);
	nfa.addRegularExpression(ARITH_PLUS, LITERAL(string("+")));
	nfa.addRegularExpression(ARITH_INCREMENT, LITERAL(string("++")));
	nfa.addRegularExpression(ARITH_MINUS, LITERAL(string("-")));
	nfa.addRegularExpression(ARITH_DECREMENT, LITERAL(string("--")));
	nfa.addRegularExpression(ARITH_MULTI, LITERAL(string("*")));
	nfa.addRegularExpression(ARITH_DIVIDE, LITERAL(string("/")));
	nfa.addRegularExpression(ARITH_MOD, LITERAL(string("%")));
	nfa.addRegularExpression(LOGIC_BIT_AND, LITERAL(string("&")));
	nfa.addRegularExpression(LOGIC_BIT_OR, LITERAL(string("|")));
	nfa.addRegularExpression(LOGIC_BIT_XOR, LITERAL(string("^")));
	nfa.addRegularExpression(LOGIC_BIT_NOT, LITERAL(string("~")));
	nfa.addRegularExpression(LOGIC_AND, LITERAL(string("&&")));
	nfa.addRegularExpression(LOGIC_OR, LITERAL(string("||")));
	nfa.addRegularExpression(LOGIC_NOT, LITERAL(string("!")));
	nfa.addRegularExpression(COMP_EQ, LITERAL(string("==")));
	nfa.addRegularExpression(COMP_GT, LITERAL(string(">")));
	nfa.addRegularExpression(COMP_LT, LITERAL(string("<")));
	nfa.addRegularExpression(COMP_GE, LITERAL(string(">=")));
	nfa.addRegularExpression(COMP_LE, LITERAL(string("<=")));
	nfa.addRegularExpression(COMP_NE, LITERAL(string("!=")));
	nfa.addRegularExpression(ASSIGN, LITERAL(string("=")));
	nfa.addRegularExpression(ASSIGN_PLUS, LITERAL(string("+=")));
	nfa.addRegularExpression(ASSIGN_MINUS, LITERAL(string("-=")));
	nfa.addRegularExpression(ASSIGN_MULTI, LITERAL(string("*=")));
	nfa.addRegularExpression(ASSIGN_DIVIDE, LITERAL(string("/=")));
	nfa.addRegularExpression(ASSIGN_MOD, LITERAL(string("%=")));
	nfa.addRegularExpression(ASSIGN_BIT_XOR, LITERAL(string("^=")));
	nfa.addRegularExpression(ASSIGN_BIT_AND, LITERAL(string("&=")));
	nfa.addRegularExpression(ASSIGN_BIT_OR, LITERAL(string("|=")));
	nfa.addRegularExpression(PUNC_PERIOD, LITERAL(string(".")));
	nfa.addRegularExpression(PUNC_COMMA, LITERAL(string(",")));
	nfa.addRegularExpression(PUNC_COLON, LITERAL(string(":")));
	nfa.addRegularExpression(PUNC_SEMICOLON, LITERAL(string(";")));
	nfa.addRegularExpression(DELIM_PAREN_OPEN, LITERAL(string("(")));
	nfa.addRegularExpression(DELIM_PAREN_CLOSE, LITERAL(string(")")));
	nfa.addRegularExpression(DELIM_BRACKET_OPEN, LITERAL(string("[")));
	nfa.addRegularExpression(DELIM_BRACKET_CLOSE, LITERAL(string("]")));
	nfa.addRegularExpression(DELIM_BRACE_OPEN, LITERAL(string("{")));
	nfa.addRegularExpression(DELIM_BRACE_CLOSE, LITERAL(string("}")));
	nfa.addRegularExpression(DEREFERENCE, LITERAL(string("*")));
	nfa.addRegularExpression(AMPERSAND, LITERAL(string("&")));
	nfa.buildAll();
	dfa.convertFrom(nfa);
}

// return -1 on no match (error),
// return 0 on finish ( $ = 0 )
int Lexer::getNextToken()
{
	// Start lexicalizing with given source file
	Token myToken;

	while (index_ < static_cast<int>(text_.size()))
	{
		nfa.getMatch(text_, index_, myToken); // get next Token
		//cout << "TOken key: " << myToken.key << " , " << "len: " << myToken.length << "\n";
		for (int i = index_; i < index_ + myToken.length; ++i)
		{
			if (text_[i] == '\n') // count lines
				++lineNum_;
		}
		index_ += myToken.length; // move pointer
		if (myToken.key == -1)
		{ // lexical error: length is 1
			std::cout << lineNum_ << ": ";
			std::cout << "lexical error: " << text_[index_] << std::endl;
			continue; // continue until finding legal lexeme
		}
		if (myToken.key == 200 || myToken.key == 201)
		{ // ingnore ws and comments
			continue;
		}
		else
		{
			// Store currently matched lexeme
			curLexeme_ = text_.substr(index_ - myToken.length, myToken.length);
			return myToken.key;
		}
	}
	// At the end of text

	return 0;
}
} // end namespace subcc