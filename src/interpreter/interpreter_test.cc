#include <iostream>
#include "interpreter.h"
#include "interpreter_test.h"

void subcc::interpreterTest()
{
	std::cout << "---------- Beginning interpreter test -----------\n\n";
	subcc::test();
	std::cout << "---------- interpreter test finished -----------\n\n\n";
}

void subcc::test()
{
	/* initialize stack pointer (SP) */
	SP = STACK_SIZE;
	FP = STACK_SIZE;
	memset(STACK, 0, STACK_SIZE);

	SP = SP - 4;
	_main();
}