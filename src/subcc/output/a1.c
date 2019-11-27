#include <stdio.h> 
#include "interpreter.h"

namespace subcc
{
char DATA[1024];
char STRING_SEGMENT[1024] ="";
int GPR0;
int GPR1;
int GPR2;
int const ZERO = 0;
char M0[256];

void func3() {

	STORE ( FP, STACK, SP - 4, 4);
	SP = SP - 4;
	FP = SP;
	SP = SP - 24;
	*((int *)M0) = 0;
	STORE( M0 , STACK , FP + (-12) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-12) + ZERO , 4 );
	 GPR0  = 5;
	STORE( GPR0 , STACK , FP + (-8) + GPR1 , 4 );
L2:
	*((int *)M0) = 4;
	STORE( M0 , STACK , FP + (-20) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-20) + ZERO , 4 );
	 GPR0  = 6;
	STORE( GPR0 , STACK , FP + (-8) + GPR1 , 4 );
L3:
	LOAD( M0 , STACK , FP + (-8) + ZERO , 8 );
	SP = FP;
	LOAD ( FP, STACK, SP, 4);
	SP = SP + 4;
	RETURN;
L1:
	SP = FP;
	LOAD ( FP, STACK, SP, 4);
	SP = SP + 4;
	RETURN;
} // end of( func3 )

void func1() {

	STORE ( FP, STACK, SP - 4, 4);
	SP = SP - 4;
	FP = SP;
	SP = SP - 68;
	*((int *)M0) = 5;
	STORE( M0 , STACK , FP + (-4) + ZERO , 4 );
L5:
	LOAD( GPR0 , STACK , FP + (-4) + ZERO , 4 );
	 GPR1  = - GPR0 ;
	STORE( GPR1 , STACK , FP + (-12) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (-12) + ZERO , 4 );
	 GPR1  = ~ GPR0 ;
	STORE( GPR1 , STACK , FP + (-16) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-16) + ZERO , 4 );
	STORE( M0 , DATA , 0 +  ZERO , 4 );
L6:
	LOAD( GPR0 , DATA , 0 +  ZERO , 4 );
	IF ( GPR0 , L8 );
	GOTO (L7);
L8:
	*((int *)M0) = 4;
	STORE( M0 , STACK , FP + (-4) + ZERO , 4 );
L9:
	LOAD( M0 , STACK , FP + (-4) + ZERO , 4 );
	SP = FP;
	LOAD ( FP, STACK, SP, 4);
	SP = SP + 4;
	RETURN;
L7:
	LOAD( GPR0 , STACK , FP + (4) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (8) + ZERO , 4 );
	 GPR2  =  GPR0 > GPR1 ;
	STORE( GPR2 , STACK , FP + (-20) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (-20) + ZERO , 4 );
	IF ( GPR0 , L10 );
	GOTO (L12);
L10:
L14:
	LOAD( GPR0 , STACK , FP + (4) + ZERO , 4 );
	 GPR1  = 100;
	 GPR2  =  GPR0 < GPR1 ;
	STORE( GPR2 , STACK , FP + (-24) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (-24) + ZERO , 4 );
	IF ( GPR0 , L15 );
	GOTO (L13);
L15:
	LOAD( GPR0 , STACK , FP + (4) + ZERO , 4 );
	 GPR1  = 1;
	 GPR2  =  GPR0 + GPR1 ;
	STORE( GPR2 , STACK , FP + (-48) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-48) + ZERO , 4 );
	STORE( M0 , STACK , FP + (4) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-48) + ZERO , 4 );
	STORE( M0 , STACK , FP + (8) + ZERO , 4 );
L17:
	LOAD( GPR0 , STACK , FP + (8) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (4) + ZERO , 4 );
	 GPR2  =  GPR0 * GPR1 ;
	STORE( GPR2 , STACK , FP + (-52) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-52) + ZERO , 4 );
	STORE( M0 , STACK , FP + (8) + ZERO , 4 );
L18:
	LOAD( GPR0 , STACK , FP + (-4) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (4) + ZERO , 4 );
	 GPR2  =  GPR0 + GPR1 ;
	STORE( GPR2 , STACK , FP + (-56) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-56) + ZERO , 4 );
	STORE( M0 , DATA , 0 +  ZERO , 4 );
L19:
	*((int *)M0) = 111;
	STORE( M0 , STACK , FP + (-32) + ZERO , 4 );
L20:
	CALL( printNewLine);
	SP = SP + 0 ;
L21:
	LOAD( M0 , STACK , FP + (-32) + ZERO , 4 );
	STORE ( M0, STACK, SP - 4 ,4 );
	SP = SP - 4;
	CALL( printInt);
	SP = SP + 4 ;
L22:
	LOAD( GPR0 , STACK , FP + (4) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (8) + ZERO , 4 );
	 GPR2  =  GPR0 == GPR1 ;
	STORE( GPR2 , STACK , FP + (-68) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (-68) + ZERO , 4 );
	IF ( GPR0 , L23 );
	GOTO (L25);
L23:
	GOTO (L14);
L24:
	GOTO (L16);
L25:
	GOTO (L13);
L16:
	GOTO (L14);
L13:
	LOAD( M0 , STACK , FP + (4) + ZERO , 4 );
	SP = FP;
	LOAD ( FP, STACK, SP, 4);
	SP = SP + 4;
	RETURN;
L11:
	GOTO (L4);
L12:
	LOAD( GPR0 , STACK , FP + (4) + ZERO , 4 );
	 GPR1  = 1;
	IF ( GPR0 == GPR1 , L26 );
	LOAD( GPR0 , STACK , FP + (4) + ZERO , 4 );
	 GPR1  = 2;
	IF ( GPR0 == GPR1 , L27 );
	LOAD( GPR0 , STACK , FP + (4) + ZERO , 4 );
	 GPR1  = 3;
	IF ( GPR0 == GPR1 , L28 );
	GOTO (L29);
L26:
	LOAD( M0 , STACK , FP + (8) + ZERO , 4 );
	SP = FP;
	LOAD ( FP, STACK, SP, 4);
	SP = SP + 4;
	RETURN;
L27:
	*((int *)M0) = 3;
	SP = FP;
	LOAD ( FP, STACK, SP, 4);
	SP = SP + 4;
	RETURN;
L30:
	GOTO (L4);
L28:
	*((int *)M0) = 5;
	SP = FP;
	LOAD ( FP, STACK, SP, 4);
	SP = SP + 4;
	RETURN;
L31:
	GOTO (L4);
L29:
	LOAD( GPR0 , STACK , FP + (4) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (8) + ZERO , 4 );
	 GPR2  =  GPR0 + GPR1 ;
	STORE( GPR2 , STACK , FP + (-24) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-24) + ZERO , 4 );
	SP = FP;
	LOAD ( FP, STACK, SP, 4);
	SP = SP + 4;
	RETURN;
L4:
	SP = FP;
	LOAD ( FP, STACK, SP, 4);
	SP = SP + 4;
	RETURN;
} // end of( func1 )

void func2() {

	STORE ( FP, STACK, SP - 4, 4);
	SP = SP - 4;
	FP = SP;
	SP = SP - 12;
	LOAD( GPR0 , STACK , FP + (4) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (8) + ZERO , 4 );
	 GPR2  =  GPR0 + GPR1 ;
	STORE( GPR2 , STACK , FP + (-4) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (-4) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (12) + ZERO , 4 );
	 GPR2  =  GPR0 + GPR1 ;
	STORE( GPR2 , STACK , FP + (-8) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-8) + ZERO , 4 );
	SP = FP;
	LOAD ( FP, STACK, SP, 4);
	SP = SP + 4;
	RETURN;
L32:
	SP = FP;
	LOAD ( FP, STACK, SP, 4);
	SP = SP + 4;
	RETURN;
} // end of( func2 )

void fibo() {

	STORE ( FP, STACK, SP - 4, 4);
	SP = SP - 4;
	FP = SP;
	SP = SP - 24;
	LOAD( GPR0 , STACK , FP + (4) + ZERO , 4 );
	 GPR1  = 2;
	 GPR2  =  GPR0 < GPR1 ;
	STORE( GPR2 , STACK , FP + (-4) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (-4) + ZERO , 4 );
	IF ( GPR0 , L35 );
	GOTO (L34);
L35:
	*((int *)M0) = 1;
	SP = FP;
	LOAD ( FP, STACK, SP, 4);
	SP = SP + 4;
	RETURN;
L34:
	LOAD( GPR0 , STACK , FP + (4) + ZERO , 4 );
	 GPR1  = 2;
	 GPR2  =  GPR0 - GPR1 ;
	STORE( GPR2 , STACK , FP + (-8) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-8) + ZERO , 4 );
	STORE ( M0, STACK, SP - 4 ,4 );
	SP = SP - 4;
	CALL( fibo);
	STORE( M0 , STACK , FP + (-12) + ZERO , 4 );
	SP = SP + 4 ;
	LOAD( GPR0 , STACK , FP + (4) + ZERO , 4 );
	 GPR1  = 1;
	 GPR2  =  GPR0 - GPR1 ;
	STORE( GPR2 , STACK , FP + (-16) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-16) + ZERO , 4 );
	STORE ( M0, STACK, SP - 4 ,4 );
	SP = SP - 4;
	CALL( fibo);
	STORE( M0 , STACK , FP + (-20) + ZERO , 4 );
	SP = SP + 4 ;
	LOAD( GPR0 , STACK , FP + (-12) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-20) + ZERO , 4 );
	 GPR2  =  GPR0 + GPR1 ;
	STORE( GPR2 , STACK , FP + (-24) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-24) + ZERO , 4 );
	SP = FP;
	LOAD ( FP, STACK, SP, 4);
	SP = SP + 4;
	RETURN;
L33:
	SP = FP;
	LOAD ( FP, STACK, SP, 4);
	SP = SP + 4;
	RETURN;
} // end of( fibo )

void myFunc() {

	STORE ( FP, STACK, SP - 4, 4);
	SP = SP - 4;
	FP = SP;
	SP = SP - 164;
	GOTO (L38);
L38:
	*((int *)M0) = 5;
	STORE( M0 , STACK , FP + (-12) + ZERO , 4 );
L37:
	 GPR0  = 8;
	 GPR1  = 1;
	 GPR2  =  GPR0 * GPR1 ;
	STORE( GPR2 , STACK , FP + (-16) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-16) + ZERO , 4 );
	STORE( M0 , STACK , FP + (-12) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (-12) + ZERO , 4 );
	 GPR1  = 0;
	 GPR2  =  GPR0 + GPR1 ;
	STORE( GPR2 , STACK , FP + (-28) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-28) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (16) + GPR1 , 4 );
	STORE( GPR0 , STACK , FP + (-32) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-32) + ZERO , 4 );
	STORE( M0 , STACK , FP + (-4) + ZERO , 4 );
L39:
	 GPR0  = 8;
	 GPR1  = 1;
	 GPR2  =  GPR0 * GPR1 ;
	STORE( GPR2 , STACK , FP + (-40) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-40) + ZERO , 4 );
	STORE( M0 , STACK , FP + (-36) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (-36) + ZERO , 4 );
	 GPR1  = 4;
	 GPR2  =  GPR0 + GPR1 ;
	STORE( GPR2 , STACK , FP + (-52) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-52) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (16) + GPR1 , 4 );
	STORE( GPR0 , STACK , FP + (-56) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-56) + ZERO , 4 );
	STORE( M0 , STACK , FP + (-8) + ZERO , 4 );
L40:
	LOAD( GPR0 , STACK , FP + (-4) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-8) + ZERO , 4 );
	 GPR2  =  GPR0 + GPR1 ;
	STORE( GPR2 , STACK , FP + (-60) + ZERO , 4 );
	 GPR0  = 8;
	 GPR1  = 0;
	 GPR2  =  GPR0 * GPR1 ;
	STORE( GPR2 , STACK , FP + (-68) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-68) + ZERO , 4 );
	STORE( M0 , STACK , FP + (-64) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (-64) + ZERO , 4 );
	 GPR1  = 0;
	 GPR2  =  GPR0 + GPR1 ;
	STORE( GPR2 , STACK , FP + (-80) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-80) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (16) + GPR1 , 4 );
	STORE( GPR0 , STACK , FP + (-84) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (-60) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-84) + ZERO , 4 );
	 GPR2  =  GPR0 + GPR1 ;
	STORE( GPR2 , STACK , FP + (-88) + ZERO , 4 );
	 GPR0  = 8;
	 GPR1  = 0;
	 GPR2  =  GPR0 * GPR1 ;
	STORE( GPR2 , STACK , FP + (-96) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-96) + ZERO , 4 );
	STORE( M0 , STACK , FP + (-92) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (-92) + ZERO , 4 );
	 GPR1  = 4;
	 GPR2  =  GPR0 + GPR1 ;
	STORE( GPR2 , STACK , FP + (-108) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-108) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (16) + GPR1 , 4 );
	STORE( GPR0 , STACK , FP + (-112) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (-88) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-112) + ZERO , 4 );
	 GPR2  =  GPR0 + GPR1 ;
	STORE( GPR2 , STACK , FP + (-116) + ZERO , 4 );
	 GPR0  = 4;
	 GPR1  = 0;
	 GPR2  =  GPR0 * GPR1 ;
	STORE( GPR2 , STACK , FP + (-124) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-124) + ZERO , 4 );
	STORE( M0 , STACK , FP + (-120) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-120) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (4) + GPR1 , 4 );
	STORE( GPR0 , STACK , FP + (-128) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (-116) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-128) + ZERO , 4 );
	 GPR2  =  GPR0 + GPR1 ;
	STORE( GPR2 , STACK , FP + (-132) + ZERO , 4 );
	 GPR0  = 4;
	 GPR1  = 1;
	 GPR2  =  GPR0 * GPR1 ;
	STORE( GPR2 , STACK , FP + (-140) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-140) + ZERO , 4 );
	STORE( M0 , STACK , FP + (-136) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-136) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (4) + GPR1 , 4 );
	STORE( GPR0 , STACK , FP + (-144) + ZERO , 4 );
	 GPR0  = 4;
	 GPR1  = 2;
	 GPR2  =  GPR0 * GPR1 ;
	STORE( GPR2 , STACK , FP + (-152) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-152) + ZERO , 4 );
	STORE( M0 , STACK , FP + (-148) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-148) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (4) + GPR1 , 4 );
	STORE( GPR0 , STACK , FP + (-156) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (-144) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-156) + ZERO , 4 );
	 GPR2  =  GPR0 * GPR1 ;
	STORE( GPR2 , STACK , FP + (-160) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (-132) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-160) + ZERO , 4 );
	 GPR2  =  GPR0 + GPR1 ;
	STORE( GPR2 , STACK , FP + (-164) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-164) + ZERO , 4 );
	SP = FP;
	LOAD ( FP, STACK, SP, 4);
	SP = SP + 4;
	RETURN;
L36:
	SP = FP;
	LOAD ( FP, STACK, SP, 4);
	SP = SP + 4;
	RETURN;
} // end of( myFunc )

void _main() {

	STORE ( FP, STACK, SP - 4, 4);
	SP = SP - 4;
	FP = SP;
	SP = SP - 220;
	*((int *)M0) = 0;
	STORE( M0 , STACK , FP + (-96) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-96) + ZERO , 4 );
	 GPR0  = 5;
	STORE( GPR0 , STACK , FP + (-52) + GPR1 , 4 );
L42:
	 GPR0  = 4;
	 GPR1  = 0;
	 GPR2  =  GPR0 * GPR1 ;
	STORE( GPR2 , STACK , FP + (-108) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-108) + ZERO , 4 );
	STORE( M0 , STACK , FP + (-104) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-104) + ZERO , 4 );
	 GPR0  = 1;
	STORE( GPR0 , STACK , FP + (-24) + GPR1 , 4 );
L43:
	 GPR0  = 4;
	 GPR1  = 1;
	 GPR2  =  GPR0 * GPR1 ;
	STORE( GPR2 , STACK , FP + (-120) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-120) + ZERO , 4 );
	STORE( M0 , STACK , FP + (-116) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-116) + ZERO , 4 );
	 GPR0  = 2;
	STORE( GPR0 , STACK , FP + (-24) + GPR1 , 4 );
L44:
	 GPR0  = 2;
	 GPR1  = 1;
	 GPR2  =  GPR0 > GPR1 ;
	STORE( GPR2 , STACK , FP + (-128) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (-128) + ZERO , 4 );
	IF ( GPR0 , L49 );
	GOTO (L48);
L49:
	 GPR0  = 3;
	 GPR1  = 2;
	 GPR2  =  GPR0 > GPR1 ;
	STORE( GPR2 , STACK , FP + (-132) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (-132) + ZERO , 4 );
	IF ( GPR0 , L46 );
	GOTO (L48);
L46:
	 GPR0  = 4;
	 GPR1  = 2;
	 GPR2  =  GPR0 * GPR1 ;
	STORE( GPR2 , STACK , FP + (-144) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-144) + ZERO , 4 );
	STORE( M0 , STACK , FP + (-140) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-140) + ZERO , 4 );
	 GPR0  = 4;
	STORE( GPR0 , STACK , FP + (-24) + GPR1 , 4 );
L47:
	GOTO (L45);
L48:
	 GPR0  = 4;
	 GPR1  = 2;
	 GPR2  =  GPR0 * GPR1 ;
	STORE( GPR2 , STACK , FP + (-144) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-144) + ZERO , 4 );
	STORE( M0 , STACK , FP + (-140) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-140) + ZERO , 4 );
	 GPR0  = 6;
	STORE( GPR0 , STACK , FP + (-24) + GPR1 , 4 );
L45:
	 GPR0  = 8;
	 GPR1  = 1;
	 GPR2  =  GPR0 * GPR1 ;
	STORE( GPR2 , STACK , FP + (-144) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-144) + ZERO , 4 );
	STORE( M0 , STACK , FP + (-140) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (-140) + ZERO , 4 );
	 GPR1  = 0;
	 GPR2  =  GPR0 + GPR1 ;
	STORE( GPR2 , STACK , FP + (-156) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-156) + ZERO , 4 );
	 GPR0  = 100;
	STORE( GPR0 , STACK , FP + (-48) + GPR1 , 4 );
L50:
	GOTO (L54);
L52:
	 GPR0  = 8;
	 GPR1  = 1;
	 GPR2  =  GPR0 * GPR1 ;
	STORE( GPR2 , STACK , FP + (-168) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-168) + ZERO , 4 );
	STORE( M0 , STACK , FP + (-164) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (-164) + ZERO , 4 );
	 GPR1  = 4;
	 GPR2  =  GPR0 + GPR1 ;
	STORE( GPR2 , STACK , FP + (-180) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-180) + ZERO , 4 );
	 GPR0  = 11;
	STORE( GPR0 , STACK , FP + (-48) + GPR1 , 4 );
L53:
	GOTO (L51);
L54:
	 GPR0  = 8;
	 GPR1  = 1;
	 GPR2  =  GPR0 * GPR1 ;
	STORE( GPR2 , STACK , FP + (-168) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-168) + ZERO , 4 );
	STORE( M0 , STACK , FP + (-164) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (-164) + ZERO , 4 );
	 GPR1  = 4;
	 GPR2  =  GPR0 + GPR1 ;
	STORE( GPR2 , STACK , FP + (-180) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-180) + ZERO , 4 );
	 GPR0  = 21;
	STORE( GPR0 , STACK , FP + (-48) + GPR1 , 4 );
L51:
	 GPR0  = 8;
	 GPR1  = 0;
	 GPR2  =  GPR0 * GPR1 ;
	STORE( GPR2 , STACK , FP + (-168) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-168) + ZERO , 4 );
	STORE( M0 , STACK , FP + (-164) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (-164) + ZERO , 4 );
	 GPR1  = 0;
	 GPR2  =  GPR0 + GPR1 ;
	STORE( GPR2 , STACK , FP + (-180) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-180) + ZERO , 4 );
	 GPR0  = 1;
	STORE( GPR0 , STACK , FP + (-48) + GPR1 , 4 );
L55:
	 GPR0  = 8;
	 GPR1  = 0;
	 GPR2  =  GPR0 * GPR1 ;
	STORE( GPR2 , STACK , FP + (-192) + ZERO , 4 );
	LOAD( M0 , STACK , FP + (-192) + ZERO , 4 );
	STORE( M0 , STACK , FP + (-188) + ZERO , 4 );
	LOAD( GPR0 , STACK , FP + (-188) + ZERO , 4 );
	 GPR1  = 4;
	 GPR2  =  GPR0 + GPR1 ;
	STORE( GPR2 , STACK , FP + (-204) + ZERO , 4 );
	LOAD( GPR1 , STACK , FP + (-204) + ZERO , 4 );
	 GPR0  = 2;
	STORE( GPR0 , STACK , FP + (-48) + GPR1 , 4 );
L56:
	 GPR0  = 2;
	 GPR1  = 2;
	IF ( GPR0 == GPR1 , L58 );
	 GPR0  = 2;
	 GPR1  = 3;
	IF ( GPR0 == GPR1 , L59 );
	GOTO (L60);
L58:
	*((int *)M0) = 5;
	STORE ( M0, STACK, SP - 4 ,4 );
	SP = SP - 4;
	CALL( printInt);
	SP = SP + 4 ;
L61:
	CALL( printNewLine);
	SP = SP + 0 ;
L62:
	GOTO (L57);
L59:
	LOAD( M0 , STACK , FP + (-48) + ZERO , 16 );
	STORE ( M0, STACK, SP - 16 ,16 );
	SP = SP - 16;
	LOAD( M0 , STACK , FP + (-24) + ZERO , 12 );
	STORE ( M0, STACK, SP - 12 ,12 );
	SP = SP - 12;
	CALL( myFunc);
	STORE( M0 , STACK , FP + (-212) + ZERO , 4 );
	SP = SP + 28 ;
	LOAD( M0 , STACK , FP + (-212) + ZERO , 4 );
	STORE ( M0, STACK, SP - 4 ,4 );
	SP = SP - 4;
	CALL( printInt);
	SP = SP + 4 ;
L63:
	CALL( printNewLine);
	SP = SP + 0 ;
L64:
	GOTO (L57);
L60:
	*((int *)M0) = 6;
	STORE ( M0, STACK, SP - 4 ,4 );
	SP = SP - 4;
	CALL( printInt);
	SP = SP + 4 ;
L57:
	LOAD( M0 , STACK , FP + (-48) + ZERO , 16 );
	STORE ( M0, STACK, SP - 16 ,16 );
	SP = SP - 16;
	LOAD( M0 , STACK , FP + (-24) + ZERO , 12 );
	STORE ( M0, STACK, SP - 12 ,12 );
	SP = SP - 12;
	CALL( myFunc);
	STORE( M0 , STACK , FP + (-212) + ZERO , 4 );
	SP = SP + 28 ;
	LOAD( M0 , STACK , FP + (-212) + ZERO , 4 );
	STORE ( M0, STACK, SP - 4 ,4 );
	SP = SP - 4;
	CALL( printInt);
	SP = SP + 4 ;
L65:
	CALL( printNewLine);
	SP = SP + 0 ;
L66:
	*((int *)M0) = 1;
	SP = FP;
	LOAD ( FP, STACK, SP, 4);
	SP = SP + 4;
	RETURN;
L41:
	SP = FP;
	LOAD ( FP, STACK, SP, 4);
	SP = SP + 4;
	RETURN;
} // end of( main )


}