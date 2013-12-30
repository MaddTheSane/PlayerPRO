/*
 *  PPDebugStr.c
 *  PPMacho
 *
 *  Created by C.W. Betts on 11/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "MAD.h"
#include "RDriver.h"
#include <stdio.h>

static void (__callback *MyDebugFunc)(short, const char*, const char*);

void PPRegisterDebugFunc(void (__callback *debugFunc)(short, const char*, const char*))
{
	MyDebugFunc = debugFunc;
}

void PPDebugStr(short line, const char *file, const char *text)
{
	if (MyDebugFunc != NULL) {
		(*MyDebugFunc)(line, file, text);
	} else {
		fprintf(stderr, "PlayerPROCore: %s:%u error text: %s!", file, line, text);
		
		abort();
	}
}
