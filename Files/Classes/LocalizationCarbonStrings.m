/*
 *  LocalizationCarbonStrings.c
 *  PPMacho
 *
 *  Created by C.W. Betts on 7/17/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "LocalizationCarbonStrings.h"
#import <Foundation/Foundation.h>

void GetCFIndexString(Str255 outStr, short listID, short indexID)
{
#if 0
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSString *strLocation = [[NSString alloc] initWithFormat:@"%s,%s", listID, indexID];
	NSString *localizedStr = [[NSBundle mainBundle] localizedStringForKey:strLocation value:@"" table:nil];
	CFStringGetPascalString((CFStringRef)localizedStr, outStr, sizeof(Str255), kCFStringEncodingMacRoman);
	
	[strLocation release];
	[pool drain];
#else
	CFStringRef strLocation = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%hd,%hd"), listID, indexID);
	CFStringRef localizedStr = CFBundleCopyLocalizedString(CFBundleGetMainBundle(), strLocation, NULL, NULL);
	CFStringGetPascalString(localizedStr, outStr, sizeof(Str255), kCFStringEncodingMacRoman);
	
	
	CFRelease(strLocation);
	CFRelease(localizedStr);
#endif
}
