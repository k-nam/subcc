#ifndef LIB_CALVIN__SUBCC__COMPILER
#define LIB_CALVIN__SUBCC__COMPILER

#include <string>

namespace subcc 
{
	// Compiler and send output to stdout
	void compile(std::string sourceCode, std::ostream &file);
}

#endif