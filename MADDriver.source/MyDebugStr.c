/*
 *  MADDebugStr.c
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
#if __has_feature(objc_arc)
#import <Foundation/Foundation.h>
#endif
#include <Block.h>

static void (^MyDebugBlock)(short, const char*, const char*);

void MADRegisterDebugBlock(void (^newdebugBlock)(short, const char*, const char*))
{
#if __has_feature(objc_arc)
	MyDebugBlock = [newdebugBlock copy];
#else
	if (MyDebugBlock) {
		Block_release(MyDebugBlock);
		MyDebugBlock = NULL;
	}
	
	if (newdebugBlock) {
		MyDebugBlock = Block_copy(newdebugBlock);
	}
#endif
}

void MADRegisterDebugFunc(void (__callback *debugFunc)(short, const char*, const char*))
{
	if (debugFunc != NULL) {
		typeof(MyDebugBlock) funcBlock = ^(short line, const char *file, const char *text) {
			(*debugFunc)(line, file, text);
		};
		
		MADRegisterDebugBlock(funcBlock);
	} else {
		MADRegisterDebugBlock(NULL);
	}
}

void MADDebugStr(short line, const char *file, const char *text)
{
	if (MyDebugBlock != NULL) {
		MyDebugBlock(line, file, text);
	} else {
		fprintf(stderr, "PlayerPROCore: %s:%u error text: %s!\n", file, line, text);
		fflush(stderr);
		
		abort();
	}
}

#else

static void (__callback *MyDebugFunc)(short, const char*, const char*);

void MADRegisterDebugFunc(void (__callback *debugFunc)(short, const char*, const char*))
{
	MyDebugFunc = debugFunc;
}

void MADDebugStr(short line, const char *file, const char *text)
{
	if (MyDebugFunc != NULL) {
		(*MyDebugFunc)(line, file, text);
	} else {
		fprintf(stderr, "PlayerPROCore: %s:%u error text: %s!\n", file, line, text);
		fflush(stderr);
		
		abort();
	}
}

#endif
