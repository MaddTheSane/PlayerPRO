/*
 *  MADDebugStr.c
 *  PPMacho
 *
 *  Created by C.W. Betts on 11/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "MAD.h"
#include <stdio.h>

static void(*privateDebugStr)(short, const char*, const char*) = NULL;

extern void MADRegisterDebugFunc(void(*adebug)(short, const char*, const char*))
{
	privateDebugStr = adebug;
}

extern void MADDebugStr(short line, const char* file, const char* text)
{
	if (privateDebugStr) {
		(*privateDebugStr)(line, file, text);
	} else {
		fprintf(stderr, "%s:%u error text:%s!", file, line, text);
		fflush(stderr);
		
		abort();
	}
}
