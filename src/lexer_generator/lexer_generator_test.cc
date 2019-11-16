#include "lexer_generator_test.h"
#include <string>

namespace subcc
{

void lexerTest()
{
	std::cout << "------------- Beginning lexer test ---------------\n\n";
	lexerTest("int main(int argc, char *argv[]){\n return 1;} ");
	std::cout << "------------- Lexer test finished ---------------\n\n\n";
}

void lexerTest(std::string const &inputText)
{
	typedef RegularExpression<>::ParseTreeNode Node;
	typedef NfaLexerGenerator<> Nfa;
	typedef DfaLexerGenerator<> Dfa;
	typedef NfaLexerGenerator<>::Token Token;

	/*
		// 10MB will be enough for a source file
		int const MAX_FILE_SIZE = 10000000;

		char *buffer = new char[MAX_FILE_SIZE]; // store entire file
		int fileSize = 0;
		if (argc != 3) {
		cout << "usage: subcc <input-file> <output-file>\n";
		return 0;
		}
		int readFd, writeFd;
		if ((readFd = open(argv[1], O_RDONLY)) < 0) {
		cout << "could not read input file\n";
		return 0;
		}
		if ((writeFd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0) {
		cout << "could not create output file\n";
		return 0;
		}
		dup2(writeFd, STDOUT_FILENO);
		if ((fileSize = read(readFd, buffer, MAX_FILE_SIZE)) == MAX_FILE_SIZE) {
		cout << "too big file...please use smaller source file ^^\n";
		return 0;
		}
		string sourceFile(buffer, fileSize); // entire input
		*/

	string sourceFile(inputText);

	/******** Configuring Lexer Generator *******/
	Nfa nfa; // create an instance of Nfa simulator for regex
	Dfa dfa;
	// As I don't have regex parser, I need to do some manual setting
	// prepare intermediate expressions
	string letterString("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	string digitString("0123456789");
	string underscoreString("_");
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
	shared_ptr<Node> letterNode, digitNode;
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
	idNode = CAT(OR(letterNode, LITERAL(underscoreString)),
				 STAR(OR(letterNode, digitNode)));
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
	decimalNode = CAT(ONEORZERO(ONEOF(string("+-"))),
					  CAT(PLUS(digitNode), ONEORZERO(CAT(LITERAL(string(".")),
														 PLUS(digitNode)))));
	hexNode = CAT(ONEORZERO(ONEOF(string("+-"))),
				  CAT(LITERAL(string("0x")),
					  PLUS(OR(digitNode, ONEOF(string("abcdefABCDEF"))))));

	// Insert each regex into Nfa simulator
	// First entered regex has higher priority than later entered ones
	nfa.addRegularExpression(-5, whitespaceNode);
	nfa.addRegularExpression(15, commentsNode);
	nfa.addRegularExpression(0, LITERAL(string("void")));
	nfa.addRegularExpression(1, LITERAL(string("short")));
	nfa.addRegularExpression(2, LITERAL(string("int")));
	nfa.addRegularExpression(3, LITERAL(string("struct")));
	nfa.addRegularExpression(4, LITERAL(string("sizeof")));
	nfa.addRegularExpression(5, LITERAL(string("typedef")));
	nfa.addRegularExpression(6, LITERAL(string("if")));
	nfa.addRegularExpression(7, LITERAL(string("else")));
	nfa.addRegularExpression(8, LITERAL(string("while")));
	nfa.addRegularExpression(9, LITERAL(string("switch")));
	nfa.addRegularExpression(10, LITERAL(string("case")));
	nfa.addRegularExpression(11, LITERAL(string("default")));
	nfa.addRegularExpression(12, LITERAL(string("break")));
	nfa.addRegularExpression(13, LITERAL(string("return")));
	nfa.addRegularExpression(14, LITERAL(string("char")));
	nfa.addRegularExpression(100, idNode);
	nfa.addRegularExpression(16, charLiteralNode);
	nfa.addRegularExpression(17, stringLiteralNode);
	nfa.addRegularExpression(18, decimalNode);
	nfa.addRegularExpression(19, hexNode);
	nfa.addRegularExpression(20, LITERAL(string("+")));
	nfa.addRegularExpression(21, LITERAL(string("++")));
	nfa.addRegularExpression(22, LITERAL(string("-")));
	nfa.addRegularExpression(23, LITERAL(string("--")));
	nfa.addRegularExpression(24, LITERAL(string("*")));
	nfa.addRegularExpression(25, LITERAL(string("/")));
	nfa.addRegularExpression(26, LITERAL(string("%")));
	nfa.addRegularExpression(27, LITERAL(string("&")));
	nfa.addRegularExpression(28, LITERAL(string("|")));
	nfa.addRegularExpression(29, LITERAL(string("^")));
	nfa.addRegularExpression(30, LITERAL(string("~")));
	nfa.addRegularExpression(31, LITERAL(string("&&")));
	nfa.addRegularExpression(32, LITERAL(string("||")));
	nfa.addRegularExpression(33, LITERAL(string("!")));
	nfa.addRegularExpression(34, LITERAL(string("==")));
	nfa.addRegularExpression(35, LITERAL(string(">")));
	nfa.addRegularExpression(36, LITERAL(string("<")));
	nfa.addRegularExpression(37, LITERAL(string(">=")));
	nfa.addRegularExpression(38, LITERAL(string("<=")));
	nfa.addRegularExpression(39, LITERAL(string("!=")));
	nfa.addRegularExpression(40, LITERAL(string("=")));
	nfa.addRegularExpression(41, LITERAL(string("+=")));
	nfa.addRegularExpression(42, LITERAL(string("-=")));
	nfa.addRegularExpression(43, LITERAL(string("*=")));
	nfa.addRegularExpression(44, LITERAL(string("/=")));
	nfa.addRegularExpression(45, LITERAL(string("%=")));
	nfa.addRegularExpression(46, LITERAL(string("^=")));
	nfa.addRegularExpression(47, LITERAL(string("&=")));
	nfa.addRegularExpression(48, LITERAL(string("|=")));
	nfa.addRegularExpression(49, LITERAL(string(".")));
	nfa.addRegularExpression(50, LITERAL(string(",")));
	nfa.addRegularExpression(51, LITERAL(string(":")));
	nfa.addRegularExpression(52, LITERAL(string(";")));
	nfa.addRegularExpression(53, LITERAL(string("(")));
	nfa.addRegularExpression(54, LITERAL(string(")")));
	nfa.addRegularExpression(55, LITERAL(string("{")));
	nfa.addRegularExpression(56, LITERAL(string("}")));
	nfa.addRegularExpression(57, LITERAL(string("[")));
	nfa.addRegularExpression(58, LITERAL(string("]")));

	// Let the Nfa do the Thompson construction
	nfa.buildAll();
	cout << "Thompson contruction : " << endl;
	// Let the Dfa to the subset construction
	dfa.convertFrom(nfa);
	cout << "Subset contruction : " << endl;
	// Start lexicalizing with given source file
	Token myToken;
	int index = 0, lineNum = 0; // index: current pointer to buffer
	cout << "Output:\n";
	while (index < static_cast<int>(sourceFile.size()))
	{
		nfa.getMatch(sourceFile, index, myToken); // get next Token
												  //if (myToken.key == 15)
												  //cout << "comment\t";
		if (myToken.key == -1)
		{ // lexical error
			cout << lineNum << ": ";
			cout << "lexical error: " << sourceFile[index] << endl;
			index++;
			continue;
		}
		if (myToken.key != -5 && myToken.key != 15)
		{							 // ingnore ws and comments
			cout << lineNum << ": "; // print line number
			for (int i = index; i < index + myToken.length; ++i)
				cout << sourceFile[i]; // print lexeme
			printToken(myToken);	   // print Token name with ( and )
			cout << endl;
		}
		for (int i = index; i < index + myToken.length; ++i)
		{
			if (sourceFile[i] == '\n') // count lines
				++lineNum;
		}
		index += myToken.length; // move pointer
	}
	cout << "Lexing : " << endl;
	// job done; clean up
	// close(readFd);
	//close(writeFd);
	//delete buffer;
	cout << "\n";
}

// Trivial function to print Token name
void printToken(NfaLexerGenerator<>::Token const &inToken)
{
	if (inToken.key == -1)
	{
		cout << "lexical error detected: ";
		exit(0);
	}
	if (inToken.key == -5)
	{
		//cout << "Whitespace\t";
		return;
	}
	switch (inToken.key)
	{
	case 0:
		cout << "(VOID)";
		return;
	case 1:
		cout << "(SHORT)";
		return;
	case 2:
		cout << "(INT)";
		return;
	case 3:
		cout << "(STRUCT)";
		return;
	case 4:
		cout << "(SIZEOF)";
		return;
	case 5:
		cout << "(TYPEDEF)";
		return;
	case 6:
		cout << "(IF)";
		return;
	case 7:
		cout << "(ELSE)";
		return;
	case 8:
		cout << "(WHILE)";
		return;
	case 9:
		cout << "(SWITCH)";
		return;
	case 10:
		cout << "(CASE)";
		return;
	case 11:
		cout << "(DEFAULT)";
		return;
	case 12:
		cout << "(BREAK)";
		return;
	case 13:
		cout << "(RETURN)";
		return;
	case 14:
		cout << "(CHAR)";
		return;
	case 15:
		cout << "(COMMENT)";
		return;
	case 16:
		cout << "(CHAR_LITERAL)";
		return;
	case 17:
		cout << "(STRING_LITERAL)";
		return;
	case 18:
		cout << "(DECIMAL_LITERAL)";
		return;
	case 19:
		cout << "(HEXADECIMAL_LITERAL)";
		return;
	case 100:
		cout << "(ID)";
		return;
	}
	if (inToken.key >= 20 && inToken.key <= 48)
	{
		//cout << "Operator\t";
		return;
	}
	if (inToken.key >= 49 && inToken.key <= 58)
	{
		//cout << "Punctuation\t";
		return;
	}
}

} // namespace subcc
