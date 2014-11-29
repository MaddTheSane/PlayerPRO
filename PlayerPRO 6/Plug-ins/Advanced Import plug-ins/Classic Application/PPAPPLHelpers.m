//
//  PPAPPLHelpers.m
//  PPMacho
//
//  Created by C.W. Betts on 11/29/14.
//
//

#import "PPAPPLHelpers.h"
#include <Carbon/Carbon.h>

void PPGetResInfo(Handle resHandle, ResID *resIDShort, ResType *resTypeCode, long *size, NSString **name)
{
	Str255 nameStr = {0};
	GetResInfo(resHandle, resIDShort, resTypeCode, nameStr);
	*size = GetResourceSizeOnDisk(resHandle);
	NSString *ourName = CFBridgingRelease(CFStringCreateWithPascalString(kCFAllocatorDefault, nameStr, kCFStringEncodingMacRoman));
	if (!ourName) {
		ourName = @"";
	}
	
	*name = ourName;
}

NSData *PPAPPLDataFromResource(ResID resourceID, ResType resourceType)
{
	Handle myRes = Get1Resource(resourceType, resourceID);
	DetachResource(myRes);
	
	HLock(myRes);
	NSData *ourData = [[NSData alloc] initWithBytes:*myRes length:GetHandleSize(myRes)];
	HUnlock(myRes);
	DisposeHandle(myRes);
	
	return ourData;
}

Handle PPGet1IndResource(ResType type, ResourceIndex index)
{
	return Get1IndResource(type, index);
}
