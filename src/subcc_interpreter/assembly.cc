#include <stdio.h> 
#include "interpreter.h"

namespace subcc_interpreter
{
	char DATA[1024];
	char STRING_SEGMENT[1024] = "Before sortingAfter sorting  <>1. Hello~ I was compiled by subcc.2. Calculated value of fibo(20) >> 3. Now testing insertion sort.4. Now testing nested loop.5. Finished all test.";
	int GPR0;
	int GPR1;
	int GPR2;
	int const ZERO = 0;
	char M0[256];

	void fibo() {

		STORE(FP, STACK, SP - 4, 4);
		SP = SP - 4;
		FP = SP;
		SP = SP - 32;
		LOAD(GPR0, STACK, FP + (4) + ZERO, 4);
		GPR1 = 1;
		GPR2 = GPR0 == GPR1;
		STORE(GPR2, STACK, FP + (-4) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-4) + ZERO, 4);
		IF(GPR0, L2);
		GOTO(L5);
	L5:
		LOAD(GPR0, STACK, FP + (4) + ZERO, 4);
		GPR1 = 2;
		GPR2 = GPR0 == GPR1;
		STORE(GPR2, STACK, FP + (-8) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-8) + ZERO, 4);
		IF(GPR0, L2);
		GOTO(L4);
	L2:
		*((int *)M0) = 1;
		SP = FP;
		LOAD(FP, STACK, SP, 4);
		SP = SP + 4;
		RETURN;
	L3:
		GOTO(L1);
	L4:
		LOAD(GPR0, STACK, FP + (4) + ZERO, 4);
		GPR1 = 2;
		GPR2 = GPR0 - GPR1;
		STORE(GPR2, STACK, FP + (-16) + ZERO, 4);
		LOAD(M0, STACK, FP + (-16) + ZERO, 4);
		STORE(M0, STACK, SP - 4, 4);
		SP = SP - 4;
		CALL(fibo);
		STORE(M0, STACK, FP + (-20) + ZERO, 4);
		SP = SP + 4;
		LOAD(GPR0, STACK, FP + (4) + ZERO, 4);
		GPR1 = 1;
		GPR2 = GPR0 - GPR1;
		STORE(GPR2, STACK, FP + (-24) + ZERO, 4);
		LOAD(M0, STACK, FP + (-24) + ZERO, 4);
		STORE(M0, STACK, SP - 4, 4);
		SP = SP - 4;
		CALL(fibo);
		STORE(M0, STACK, FP + (-28) + ZERO, 4);
		SP = SP + 4;
		LOAD(GPR0, STACK, FP + (-20) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-28) + ZERO, 4);
		GPR2 = GPR0 + GPR1;
		STORE(GPR2, STACK, FP + (-32) + ZERO, 4);
		LOAD(M0, STACK, FP + (-32) + ZERO, 4);
		SP = FP;
		LOAD(FP, STACK, SP, 4);
		SP = SP + 4;
		RETURN;
	L1:
		SP = FP;
		LOAD(FP, STACK, SP, 4);
		SP = SP + 4;
		RETURN;
	} // end of( fibo )

	void insertionSort() {

		STORE(FP, STACK, SP - 4, 4);
		SP = SP - 4;
		FP = SP;
		SP = SP - 200;
		LOAD(M0, STACK, FP + (4) + ZERO, 20);
		STORE(M0, STACK, FP + (-20) + ZERO, 20);
	L7:
		*((int *)M0) = 1;
		STORE(M0, STACK, FP + (-24) + ZERO, 4);
	L8:
	L10:
		LOAD(GPR0, STACK, FP + (-24) + ZERO, 4);
		GPR1 = 5;
		GPR2 = GPR0 < GPR1;
		STORE(GPR2, STACK, FP + (-36) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-36) + ZERO, 4);
		IF(GPR0, L11);
		GOTO(L9);
	L11:
		LOAD(M0, STACK, FP + (-24) + ZERO, 4);
		STORE(M0, STACK, FP + (-28) + ZERO, 4);
	L13:
		*((int *)M0) = 0;
		STORE(M0, STACK, FP + (-40) + ZERO, 4);
		GPR0 = 4;
		LOAD(GPR1, STACK, FP + (-28) + ZERO, 4);
		GPR2 = GPR0 * GPR1;
		STORE(GPR2, STACK, FP + (-68) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-40) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-68) + ZERO, 4);
		GPR2 = GPR0 + GPR1;
		STORE(GPR2, STACK, FP + (-64) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-64) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-20) + GPR1, 4);
		STORE(GPR0, STACK, FP + (-72) + ZERO, 4);
		LOAD(M0, STACK, FP + (-72) + ZERO, 4);
		STORE(M0, STACK, FP + (-32) + ZERO, 4);
	L14:
	L16:
		LOAD(GPR0, STACK, FP + (-28) + ZERO, 4);
		GPR1 = 0;
		GPR2 = GPR0 > GPR1;
		STORE(GPR2, STACK, FP + (-76) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-76) + ZERO, 4);
		IF(GPR0, L17);
		GOTO(L15);
	L17:
		*((int *)M0) = 0;
		STORE(M0, STACK, FP + (-80) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-28) + ZERO, 4);
		GPR1 = 1;
		GPR2 = GPR0 - GPR1;
		STORE(GPR2, STACK, FP + (-104) + ZERO, 4);
		GPR0 = 4;
		LOAD(GPR1, STACK, FP + (-104) + ZERO, 4);
		GPR2 = GPR0 * GPR1;
		STORE(GPR2, STACK, FP + (-112) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-80) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-112) + ZERO, 4);
		GPR2 = GPR0 + GPR1;
		STORE(GPR2, STACK, FP + (-108) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-108) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-20) + GPR1, 4);
		STORE(GPR0, STACK, FP + (-116) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-116) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-32) + ZERO, 4);
		GPR2 = GPR0 <= GPR1;
		STORE(GPR2, STACK, FP + (-120) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-120) + ZERO, 4);
		IF(GPR0, L19);
		GOTO(L21);
	L19:
		GOTO(L15);
	L20:
		GOTO(L18);
	L21:
		*((int *)M0) = 0;
		STORE(M0, STACK, FP + (-124) + ZERO, 4);
		GPR0 = 4;
		LOAD(GPR1, STACK, FP + (-28) + ZERO, 4);
		GPR2 = GPR0 * GPR1;
		STORE(GPR2, STACK, FP + (-152) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-124) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-152) + ZERO, 4);
		GPR2 = GPR0 + GPR1;
		STORE(GPR2, STACK, FP + (-148) + ZERO, 4);
		*((int *)M0) = 0;
		STORE(M0, STACK, FP + (-160) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-28) + ZERO, 4);
		GPR1 = 1;
		GPR2 = GPR0 - GPR1;
		STORE(GPR2, STACK, FP + (-184) + ZERO, 4);
		GPR0 = 4;
		LOAD(GPR1, STACK, FP + (-184) + ZERO, 4);
		GPR2 = GPR0 * GPR1;
		STORE(GPR2, STACK, FP + (-192) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-160) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-192) + ZERO, 4);
		GPR2 = GPR0 + GPR1;
		STORE(GPR2, STACK, FP + (-188) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-188) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-20) + GPR1, 4);
		STORE(GPR0, STACK, FP + (-196) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-148) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-196) + ZERO, 4);
		STORE(GPR0, STACK, FP + (-20) + GPR1, 4);
	L22:
		LOAD(GPR0, STACK, FP + (-28) + ZERO, 4);
		GPR1 = 1;
		GPR2 = GPR0 - GPR1;
		STORE(GPR2, STACK, FP + (-200) + ZERO, 4);
		LOAD(M0, STACK, FP + (-200) + ZERO, 4);
		STORE(M0, STACK, FP + (-28) + ZERO, 4);
	L18:
		GOTO(L16);
	L15:
		*((int *)M0) = 0;
		STORE(M0, STACK, FP + (-80) + ZERO, 4);
		GPR0 = 4;
		LOAD(GPR1, STACK, FP + (-28) + ZERO, 4);
		GPR2 = GPR0 * GPR1;
		STORE(GPR2, STACK, FP + (-108) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-80) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-108) + ZERO, 4);
		GPR2 = GPR0 + GPR1;
		STORE(GPR2, STACK, FP + (-104) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-104) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-32) + ZERO, 4);
		STORE(GPR0, STACK, FP + (-20) + GPR1, 4);
	L23:
		LOAD(GPR0, STACK, FP + (-24) + ZERO, 4);
		GPR1 = 1;
		GPR2 = GPR0 + GPR1;
		STORE(GPR2, STACK, FP + (-116) + ZERO, 4);
		LOAD(M0, STACK, FP + (-116) + ZERO, 4);
		STORE(M0, STACK, FP + (-24) + ZERO, 4);
	L12:
		GOTO(L10);
	L9:
		LOAD(M0, STACK, FP + (-20) + ZERO, 20);
		SP = FP;
		LOAD(FP, STACK, SP, 4);
		SP = SP + 4;
		RETURN;
	L6:
		SP = FP;
		LOAD(FP, STACK, SP, 4);
		SP = SP + 4;
		RETURN;
	} // end of( insertionSort )

	void sortTest() {

		STORE(FP, STACK, SP - 4, 4);
		SP = SP - 4;
		FP = SP;
		SP = SP - 560;
		LOAD(M0, STRING_SEGMENT, 0 + ZERO, 14);
		STORE(M0, STACK, FP + (-104) + ZERO, 14);
	L25:
		LOAD(M0, STRING_SEGMENT, 14 + ZERO, 13);
		STORE(M0, STACK, FP + (-184) + ZERO, 13);
	L26:
		LOAD(M0, STRING_SEGMENT, 27 + ZERO, 2);
		STORE(M0, STACK, FP + (-264) + ZERO, 2);
	L27:
		*((int *)M0) = 0;
		STORE(M0, STACK, FP + (-288) + ZERO, 4);
		GPR0 = 4;
		GPR1 = 0;
		GPR2 = GPR0 * GPR1;
		STORE(GPR2, STACK, FP + (-316) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-288) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-316) + ZERO, 4);
		GPR2 = GPR0 + GPR1;
		STORE(GPR2, STACK, FP + (-312) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-312) + ZERO, 4);
		GPR0 = 20;
		STORE(GPR0, STACK, FP + (-284) + GPR1, 4);
	L28:
		*((int *)M0) = 0;
		STORE(M0, STACK, FP + (-324) + ZERO, 4);
		GPR0 = 4;
		GPR1 = 1;
		GPR2 = GPR0 * GPR1;
		STORE(GPR2, STACK, FP + (-352) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-324) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-352) + ZERO, 4);
		GPR2 = GPR0 + GPR1;
		STORE(GPR2, STACK, FP + (-348) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-348) + ZERO, 4);
		GPR0 = 50;
		STORE(GPR0, STACK, FP + (-284) + GPR1, 4);
	L29:
		*((int *)M0) = 0;
		STORE(M0, STACK, FP + (-360) + ZERO, 4);
		GPR0 = 4;
		GPR1 = 2;
		GPR2 = GPR0 * GPR1;
		STORE(GPR2, STACK, FP + (-388) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-360) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-388) + ZERO, 4);
		GPR2 = GPR0 + GPR1;
		STORE(GPR2, STACK, FP + (-384) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-384) + ZERO, 4);
		GPR0 = 10;
		STORE(GPR0, STACK, FP + (-284) + GPR1, 4);
	L30:
		*((int *)M0) = 0;
		STORE(M0, STACK, FP + (-396) + ZERO, 4);
		GPR0 = 4;
		GPR1 = 3;
		GPR2 = GPR0 * GPR1;
		STORE(GPR2, STACK, FP + (-424) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-396) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-424) + ZERO, 4);
		GPR2 = GPR0 + GPR1;
		STORE(GPR2, STACK, FP + (-420) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-420) + ZERO, 4);
		GPR0 = 30;
		STORE(GPR0, STACK, FP + (-284) + GPR1, 4);
	L31:
		*((int *)M0) = 0;
		STORE(M0, STACK, FP + (-432) + ZERO, 4);
		GPR0 = 4;
		GPR1 = 4;
		GPR2 = GPR0 * GPR1;
		STORE(GPR2, STACK, FP + (-460) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-432) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-460) + ZERO, 4);
		GPR2 = GPR0 + GPR1;
		STORE(GPR2, STACK, FP + (-456) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-456) + ZERO, 4);
		GPR0 = 40;
		STORE(GPR0, STACK, FP + (-284) + GPR1, 4);
	L32:
		LOAD(M0, STACK, FP + (-104) + ZERO, 80);
		STORE(M0, STACK, SP - 80, 80);
		SP = SP - 80;
		CALL(printStr);
		SP = SP + 80;
	L33:
		CALL(printNewLine);
		SP = SP + 0;
	L34:
		*((int *)M0) = 0;
		STORE(M0, STACK, FP + (-24) + ZERO, 4);
	L35:
	L37:
		LOAD(GPR0, STACK, FP + (-24) + ZERO, 4);
		GPR1 = 5;
		GPR2 = GPR0 < GPR1;
		STORE(GPR2, STACK, FP + (-476) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-476) + ZERO, 4);
		IF(GPR0, L38);
		GOTO(L36);
	L38:
		*((int *)M0) = 0;
		STORE(M0, STACK, FP + (-480) + ZERO, 4);
		GPR0 = 4;
		LOAD(GPR1, STACK, FP + (-24) + ZERO, 4);
		GPR2 = GPR0 * GPR1;
		STORE(GPR2, STACK, FP + (-508) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-480) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-508) + ZERO, 4);
		GPR2 = GPR0 + GPR1;
		STORE(GPR2, STACK, FP + (-504) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-504) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-284) + GPR1, 4);
		STORE(GPR0, STACK, FP + (-512) + ZERO, 4);
		LOAD(M0, STACK, FP + (-512) + ZERO, 4);
		STORE(M0, STACK, SP - 4, 4);
		SP = SP - 4;
		CALL(printInt);
		SP = SP + 4;
	L40:
		LOAD(M0, STACK, FP + (-264) + ZERO, 80);
		STORE(M0, STACK, SP - 80, 80);
		SP = SP - 80;
		CALL(printStr);
		SP = SP + 80;
	L41:
		LOAD(GPR0, STACK, FP + (-24) + ZERO, 4);
		GPR1 = 1;
		GPR2 = GPR0 + GPR1;
		STORE(GPR2, STACK, FP + (-524) + ZERO, 4);
		LOAD(M0, STACK, FP + (-524) + ZERO, 4);
		STORE(M0, STACK, FP + (-24) + ZERO, 4);
	L39:
		GOTO(L37);
	L36:
		CALL(printNewLine);
		SP = SP + 0;
	L42:
		LOAD(M0, STACK, FP + (-184) + ZERO, 80);
		STORE(M0, STACK, SP - 80, 80);
		SP = SP - 80;
		CALL(printStr);
		SP = SP + 80;
	L43:
		CALL(printNewLine);
		SP = SP + 0;
	L44:
		LOAD(M0, STACK, FP + (-284) + ZERO, 20);
		STORE(M0, STACK, SP - 20, 20);
		SP = SP - 20;
		CALL(insertionSort);
		STORE(M0, STACK, FP + (-508) + ZERO, 20);
		SP = SP + 20;
		LOAD(M0, STACK, FP + (-508) + ZERO, 20);
		STORE(M0, STACK, FP + (-284) + ZERO, 20);
	L45:
		*((int *)M0) = 0;
		STORE(M0, STACK, FP + (-24) + ZERO, 4);
	L46:
	L47:
		LOAD(GPR0, STACK, FP + (-24) + ZERO, 4);
		GPR1 = 5;
		GPR2 = GPR0 < GPR1;
		STORE(GPR2, STACK, FP + (-512) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-512) + ZERO, 4);
		IF(GPR0, L48);
		GOTO(L24);
	L48:
		*((int *)M0) = 0;
		STORE(M0, STACK, FP + (-516) + ZERO, 4);
		GPR0 = 4;
		LOAD(GPR1, STACK, FP + (-24) + ZERO, 4);
		GPR2 = GPR0 * GPR1;
		STORE(GPR2, STACK, FP + (-544) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-516) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-544) + ZERO, 4);
		GPR2 = GPR0 + GPR1;
		STORE(GPR2, STACK, FP + (-540) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-540) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-284) + GPR1, 4);
		STORE(GPR0, STACK, FP + (-548) + ZERO, 4);
		LOAD(M0, STACK, FP + (-548) + ZERO, 4);
		STORE(M0, STACK, SP - 4, 4);
		SP = SP - 4;
		CALL(printInt);
		SP = SP + 4;
	L50:
		LOAD(M0, STACK, FP + (-264) + ZERO, 80);
		STORE(M0, STACK, SP - 80, 80);
		SP = SP - 80;
		CALL(printStr);
		SP = SP + 80;
	L51:
		LOAD(GPR0, STACK, FP + (-24) + ZERO, 4);
		GPR1 = 1;
		GPR2 = GPR0 + GPR1;
		STORE(GPR2, STACK, FP + (-560) + ZERO, 4);
		LOAD(M0, STACK, FP + (-560) + ZERO, 4);
		STORE(M0, STACK, FP + (-24) + ZERO, 4);
	L49:
		GOTO(L47);
	L24:
		SP = FP;
		LOAD(FP, STACK, SP, 4);
		SP = SP + 4;
		RETURN;
	} // end of( sortTest )

	void loopTest() {

		STORE(FP, STACK, SP - 4, 4);
		SP = SP - 4;
		FP = SP;
		SP = SP - 104;
		*((int *)M0) = 0;
		STORE(M0, STACK, FP + (-84) + ZERO, 4);
	L53:
		LOAD(M0, STRING_SEGMENT, 29 + ZERO, 2);
		STORE(M0, STACK, FP + (-80) + ZERO, 2);
	L54:
	L55:
		LOAD(GPR0, STACK, FP + (-84) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (4) + ZERO, 4);
		GPR2 = GPR0 < GPR1;
		STORE(GPR2, STACK, FP + (-92) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-92) + ZERO, 4);
		IF(GPR0, L56);
		GOTO(L52);
	L56:
		*((int *)M0) = 0;
		STORE(M0, STACK, FP + (-88) + ZERO, 4);
	L58:
	L60:
		LOAD(GPR0, STACK, FP + (-88) + ZERO, 4);
		LOAD(GPR1, STACK, FP + (-84) + ZERO, 4);
		GPR2 = GPR0 < GPR1;
		STORE(GPR2, STACK, FP + (-96) + ZERO, 4);
		LOAD(GPR0, STACK, FP + (-96) + ZERO, 4);
		IF(GPR0, L61);
		GOTO(L59);
	L61:
		LOAD(M0, STACK, FP + (-80) + ZERO, 80);
		STORE(M0, STACK, SP - 80, 80);
		SP = SP - 80;
		CALL(printStr);
		SP = SP + 80;
	L63:
		LOAD(GPR0, STACK, FP + (-88) + ZERO, 4);
		GPR1 = 1;
		GPR2 = GPR0 + GPR1;
		STORE(GPR2, STACK, FP + (-104) + ZERO, 4);
		LOAD(M0, STACK, FP + (-104) + ZERO, 4);
		STORE(M0, STACK, FP + (-88) + ZERO, 4);
	L62:
		GOTO(L60);
	L59:
		CALL(printNewLine);
		SP = SP + 0;
	L64:
		LOAD(GPR0, STACK, FP + (-84) + ZERO, 4);
		GPR1 = 1;
		GPR2 = GPR0 + GPR1;
		STORE(GPR2, STACK, FP + (-104) + ZERO, 4);
		LOAD(M0, STACK, FP + (-104) + ZERO, 4);
		STORE(M0, STACK, FP + (-84) + ZERO, 4);
	L57:
		GOTO(L55);
	L52:
		SP = FP;
		LOAD(FP, STACK, SP, 4);
		SP = SP + 4;
		RETURN;
	} // end of( loopTest )

	void _main() {

		STORE(FP, STACK, SP - 4, 4);
		SP = SP - 4;
		FP = SP;
		SP = SP - 476;
		LOAD(M0, STRING_SEGMENT, 31 + ZERO, 34);
		STORE(M0, STACK, FP + (-80) + ZERO, 34);
	L66:
		LOAD(M0, STACK, FP + (-80) + ZERO, 80);
		STORE(M0, STACK, SP - 80, 80);
		SP = SP - 80;
		CALL(printStr);
		SP = SP + 80;
	L67:
		CALL(printNewLine);
		SP = SP + 0;
	L68:
		CALL(printNewLine);
		SP = SP + 0;
	L69:
		LOAD(M0, STRING_SEGMENT, 65 + ZERO, 35);
		STORE(M0, STACK, FP + (-160) + ZERO, 35);
	L70:
		LOAD(M0, STACK, FP + (-160) + ZERO, 80);
		STORE(M0, STACK, SP - 80, 80);
		SP = SP - 80;
		CALL(printStr);
		SP = SP + 80;
	L71:
		*((int *)M0) = 10;
		STORE(M0, STACK, SP - 4, 4);
		SP = SP - 4;
		CALL(fibo);
		STORE(M0, STACK, FP + (-420) + ZERO, 4);
		SP = SP + 4;
		LOAD(M0, STACK, FP + (-420) + ZERO, 4);
		STORE(M0, STACK, SP - 4, 4);
		SP = SP - 4;
		CALL(printInt);
		SP = SP + 4;
	L72:
		CALL(printNewLine);
		SP = SP + 0;
	L73:
		CALL(printNewLine);
		SP = SP + 0;
	L74:
		LOAD(M0, STRING_SEGMENT, 100 + ZERO, 30);
		STORE(M0, STACK, FP + (-240) + ZERO, 30);
	L75:
		LOAD(M0, STACK, FP + (-240) + ZERO, 80);
		STORE(M0, STACK, SP - 80, 80);
		SP = SP - 80;
		CALL(printStr);
		SP = SP + 80;
	L76:
		CALL(printNewLine);
		SP = SP + 0;
	L77:
		CALL(sortTest);
		SP = SP + 0;
	L78:
		CALL(printNewLine);
		SP = SP + 0;
	L79:
		CALL(printNewLine);
		SP = SP + 0;
	L80:
		LOAD(M0, STRING_SEGMENT, 130 + ZERO, 27);
		STORE(M0, STACK, FP + (-320) + ZERO, 27);
	L81:
		LOAD(M0, STACK, FP + (-320) + ZERO, 80);
		STORE(M0, STACK, SP - 80, 80);
		SP = SP - 80;
		CALL(printStr);
		SP = SP + 80;
	L82:
		*((int *)M0) = 20;
		STORE(M0, STACK, SP - 4, 4);
		SP = SP - 4;
		CALL(loopTest);
		SP = SP + 4;
	L83:
		CALL(printNewLine);
		SP = SP + 0;
	L84:
		CALL(printNewLine);
		SP = SP + 0;
	L85:
		LOAD(M0, STRING_SEGMENT, 157 + ZERO, 21);
		STORE(M0, STACK, FP + (-400) + ZERO, 21);
	L86:
		LOAD(M0, STACK, FP + (-400) + ZERO, 80);
		STORE(M0, STACK, SP - 80, 80);
		SP = SP - 80;
		CALL(printStr);
		SP = SP + 80;
	L87:
		CALL(printNewLine);
		SP = SP + 0;
	L88:
		*((int *)M0) = 0;
		SP = FP;
		LOAD(FP, STACK, SP, 4);
		SP = SP + 4;
		RETURN;
	L65:
		SP = FP;
		LOAD(FP, STACK, SP, 4);
		SP = SP + 4;
		RETURN;
	} // end of( main )


}