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

static void (__callback *MyDebugFunc)(short, const char*, const char*) = NULL;

void PPRegisterDebugFunc(void (__callback *debugFunc)(short, const char*, const char*))
{
	MyDebugFunc = debugFunc;
}

#ifdef _MAC_H
#include <CoreFoundation/CFBase.h>

extern void NSLog(CFStringRef format, ...);

void PPDebugStr( short line, const char *file, const char *text)
{
	if (MyDebugFunc != NULL) {
		(*MyDebugFunc)(line, file, text);
	} else {
		NSLog(CFSTR("PlayerPROCore: %s:%u error text: %s!"), file, line, text);
		
		abort();
	}
}
#else
#include <stdio.h>

void PPDebugStr( short line, const char *file, const char *text)
{
	if (MyDebugFunc != NULL) {
		(*MyDebugFunc)(line, file, text);
	} else {
		fprintf(stderr, "PlayerPROCore: %s:%u error text: %s!", file, line, text);
		
		abort();
	}
}
#endif
