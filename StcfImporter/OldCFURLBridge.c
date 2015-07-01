//
//  OldCFURLBridge.c
//  PPMacho
//
//  Created by C.W. Betts on 7/1/15.
//
//

#include <stdio.h>
#include "OldCFURLBridge.h"

//CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (__bridge CFStringRef)CFaStr, kCFURLHFSPathStyle, true)
CFURLRef CreateURLWithHFSPathStyle(CFAllocatorRef allocator, CFStringRef hfsPath, bool isDirectory)
{
	return CFURLCreateWithFileSystemPath(allocator, hfsPath, kCFURLHFSPathStyle, isDirectory);
}
