#pragma once

#include <stdlib.h>
#include <string.h>

namespace subcc
{
void _main();

void printInt();
void printStr();
void printNewLine();

/* stack 8KB */
#define STACK_SIZE 8096
extern char STACK[];
extern int SP;
extern int FP;

#define GOTO(label) \
	goto label

#define IF(expr, label) \
	if ((expr))         \
	goto label

#define IF_FALSE(expr, label) \
	if (!(expr))              \
	goto label

#define PARAM(var, size) \
	SP = SP - (size);    \
	memcpy((void *)(STACK + SP), (void *)&(var), (size))

#define CALL(funcname) \
	(funcname)()

#define LOAD(dst, base, offset, size) \
	memcpy((char *)&(dst), (((char *)&(base)) + (offset)), size);

#define STORE(src, base, offset, size) \
	memcpy((((char *)&(base)) + (offset)), (char *)&(src), size)

#define RETURN \
	return

/*
	* If you want to see what a data is loading, you can see the name of symbol
	* by using following macros. The gcc preprocessor will substitute as
	* follows if you use the following macros.
	*
	* in source file,
	* LOAD( A, B, O, 4 );
	*
	* after preprocessing,
	* memcpy( (char*) &(A), (((char*) &(B)) + (O)), 4 );
	* printf( "Loading B[O] to A\n" );
	*
	#define	LOAD( dst, base, offset, size )	\
	memcpy( (char*) &(dst), (((char*) &(base)) + (offset)), size ); \
	printf( "Loading "#base"["#offset"] to "#dst"\n" )

	#define	STORE( src, base, offset, size )	\
	printf( "Storing "#base"["#offset"] to "#dst"\n" )
	memcpy( (((char*) &(base)) + (offset)), (char*) &(src), size )
	*/
} // namespace subcc
