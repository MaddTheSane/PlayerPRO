/*
 *  MyDebugStr.c
 *  PPMacho
 *
 *  Created by C.W. Betts on 11/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <CoreFoundation/CFBase.h>
#include <CoreServices/CoreServices.h>

extern void NSLog(CFStringRef format, ...);

extern void MyDebugStr( short line, Ptr file, Ptr text)
{
	NSLog(CFSTR("%s:%u error text:%s!"), file, line, text);
	
	Debugger();
}
