#include <iostream>
#include <fstream>
#include <streambuf>
#include "test.h"
#include "compiler.h"
#include "symbol_table.h"
#include "type.h"
#include "syntax_tree.h"

void subcc::subccTest()
{
	using namespace subcc;
	using std::cout;
	using std::string;
	std::cout << "---------------- Beginning subcc test -----------------\n\n";

	vector<string> testFiles = {"a1", "a2", "test"};
	//std::string inputFileName = "../subcc/subc_code/a1.c";
	//std::string inputFileName = "../subcc/subc_code/a2.c";
	std::string inputFileName = "../subcc/subc_code/test.c";

	for (auto testFile : testFiles)
	{
		auto inputFileName = "../subcc/subc_code/" + testFile + ".c";
		std::ifstream stream(inputFileName);
		if (!stream)
		{
			cout << "subccTest error: cannot open input file\n";
			exit(0);
		}
		string sourceText;
		stream.seekg(std::ios::end);
		sourceText.reserve(static_cast<size_t>(stream.tellg()));
		stream.seekg(std::ios::beg);
		sourceText.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
		stream.close();

		cout << "input size is: " << sourceText.size() << "\n";
		string header("void printStr( char s[80] ); \n void printInt ( int a ); \n\
      void printNewLine(); \n");
		string inputToCompiler = header + sourceText;
		cout << "\n";

		/* Uncomment below line to save output to a file (which can be run in subcc_run project) */
		std::ofstream outFile("../subcc/output/" + testFile + ".c");
		std::ostream &file = std::cout;

		//while(true) { // loop to check memory leak
		compile(inputToCompiler, outFile);
		//}

		SymbolTable::countObjects();
		GlobalSymbolTable::countObjects();
		Type::countObjects();
		SyntaxTreeNode::countObjects();
	}

	std::cout << "---------------- Subcc test finished -----------------\n\n\n";
}
