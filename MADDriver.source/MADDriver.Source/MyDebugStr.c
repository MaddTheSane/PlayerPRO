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

static void (__callback *MyDebugFunc)(short, Ptr, Ptr) = NULL;

void PPRegisterDebugFunc(void (__callback *debugFunc)(short, Ptr, Ptr))
{
	MyDebugFunc = debugFunc;
}

#ifdef _MAC_H
#include <CoreFoundation/CFBase.h>

extern void NSLog(CFStringRef format, ...);

void PPDebugStr( short line, Ptr file, Ptr text)
{
	if (MyDebugFunc != NULL) {
		(*MyDebugFunc)(line, file, text);
	} else {
		NSLog(CFSTR("%s:%u error text: %s!"), file, line, text);
		
		abort();
	}
}
#else
#include <stdio.h>

void PPDebugStr( short line, Ptr file, Ptr text)
{
	if (MyDebugFunc != NULL) {
		(*MyDebugFunc)(line, file, text);
	} else {
		fprintf(stderr, "%s:%u error text: %s!", file, line, text);
		
		abort();
	}
}
#endif
