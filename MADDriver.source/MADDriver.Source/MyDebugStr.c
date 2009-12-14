/*
 *  MyDebugStr.c
 *  PPMacho
 *
 *  Created by C.W. Betts on 11/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <CoreFoundation/CoreFoundation.h>
#include <Carbon/Carbon.h>

extern void NSLog(CFStringRef format, ...);

void MyDebugStr( short line, Ptr file, Ptr text)
{
	NSLog(CFSTR("ERROR!! %s:%u error text:%s!"), file, line, text);
	
	Debugger();
}
