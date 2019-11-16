#include <iostream>
#include "interpreter.h"
#include "interpreter_test.h"

void subcc_interpreter::subccInterpreterTest() {
	std::cout << "---------- Beginning subcc_interpreter test -----------\n\n";
	subcc_interpreter::test();
	std::cout << "---------- subcc_interpreter test finished -----------\n\n\n";

}

void subcc_interpreter::test()
{
	/* initialize stack pointer (SP) */
	SP = STACK_SIZE;
	FP = STACK_SIZE;
	memset(STACK, 0, STACK_SIZE);

	SP = SP - 4;
	_main();
}