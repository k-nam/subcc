#pragma once

#include <string>

namespace subcc 
{
	// Compiler and send output to stdout
	void compile(std::string sourceCode, std::ostream &file);
}
