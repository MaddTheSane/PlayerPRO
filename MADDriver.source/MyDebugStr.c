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

#ifdef __BLOCKS__
#include <Block.h>
static void (^MyDebugBlock)(short, const char*, const char*);

void PPRegisterDebugBlock(void (^newdebugBlock)(short, const char*, const char*))
{
	if (MyDebugBlock) {
		Block_release(MyDebugBlock);
	}
	if (newdebugBlock) {
		MyDebugBlock = Block_copy(newdebugBlock);
	}
}

void PPRegisterDebugFunc(void (__callback *debugFunc)(short, const char*, const char*))
{
	if (MyDebugBlock) {
		Block_release(MyDebugBlock);
	}
	if (debugFunc == NULL) {
		MyDebugBlock = NULL;
	} else {
		MyDebugBlock = Block_copy(^(short line, const char *file, const char *text){
			(*debugFunc)(line, file, text);
		});
	}
}

void PPDebugStr(short line, const char *file, const char *text)
{
	if (MyDebugBlock != NULL) {
		MyDebugBlock(line, file, text);
	} else {
		fprintf(stderr, "PlayerPROCore: %s:%u error text: %s!", file, line, text);
		
		abort();
	}
}

#else

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

#endif
