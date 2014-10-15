/*
 ============================================================================
 Name        : log2.c
 Author      : Leorge Takeuchi
 Version     : 1.0.0
 Copyright   : free
 Description : int log2N(size_t n) in C, Ansi-style
 Options     : -O3 -std=c99  -lm  [-DDEBUG]
 ============================================================================
 */

#include <math.h>
#include "log2.h"

int log2N(TYPE n)
{
	if (n <= 0) return -1;		// log(0) is infinite.
	register int width = 8 * sizeof(TYPE);
	register int rtn = width - 1;
	register TYPE pattern = ~(TYPE)0;
	for (pattern <<= (width >> 1) ; width > 1 ; )
	{
		TYPE    found = pattern & n;
		pattern &= pattern << ((width >>= 1) >> 1);
		if (found) continue;
		rtn -= width;
		pattern >>= width;
	}
//#ifdef	DEBUG
//	static int call_count = 0;
//	if (call_count++ == 0) fprintf(stderr, "log2N(%ld) = %d\n", n, rtn);	// once
//#endif
	return  rtn;
}

int log2c(TYPE n)
{	// count
	register int rtn = 0;
	for(int i = n; i; i >>= 1) rtn++;
	return  rtn;
}

int log2b(TYPE n)
{	// built in
	return  (int)log2((double)n);
}
