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
	if (MyDebugBlock) {
#if __has_feature(objc_arc)
		MyDebugBlock = nil;
#else
		Block_release(MyDebugBlock);
#endif
	}
	
	if (newdebugBlock) {
#if __has_feature(objc_arc)
		MyDebugBlock = [newdebugBlock copy];
#else
		MyDebugBlock = Block_copy(newdebugBlock);
#endif
	} else {
		MyDebugBlock = NULL;
	}
}

void MADRegisterDebugFunc(void (__callback *debugFunc)(short, const char*, const char*))
{
	if (MyDebugBlock) {
#if __has_feature(objc_arc)
		MyDebugBlock = nil;
#else
		Block_release(MyDebugBlock);
#endif
	}
	
	if (debugFunc == NULL) {
		MyDebugBlock = NULL;
	} else {
		typeof(MyDebugBlock) funcBlock = ^(short line, const char *file, const char *text){
			(*debugFunc)(line, file, text);
		};
#if __has_feature(objc_arc)
		MyDebugBlock = [funcBlock copy];
#else
		MyDebugBlock = Block_copy(funcBlock);
#endif
	}
}

void MADDebugStr(short line, const char *file, const char *text)
{
	if (MyDebugBlock != NULL) {
		MyDebugBlock(line, file, text);
	} else {
		fprintf(stderr, "PlayerPROCore: %s:%u error text: %s!", file, line, text);
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
		fprintf(stderr, "PlayerPROCore: %s:%u error text: %s!", file, line, text);
		fflush(stderr);
		
		abort();
	}
}

#endif
