/*
 *  MyDebugStr.c
 *  PPMacho
 *
 *  Created by C.W. Betts on 11/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "MAD.h"
#include <stdio.h>

extern void MyDebugStr(short line, Ptr file, Ptr text)
{
	fprintf(stderr, "%s:%u error text:%s!", file, line, text);
	//TODO: call a debug function?
	abort();
}
