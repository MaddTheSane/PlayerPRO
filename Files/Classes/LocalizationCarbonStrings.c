/*
 *  LocalizationCarbonStrings.c
 *  PPMacho
 *
 *  Created by C.W. Betts on 7/17/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "LocalizationCarbonStrings.h"
#include <CoreFoundation/CoreFoundation.h>

void GetCFIndexString(Str255 outStr, short listID, short indexID)
{
	CFStringRef strLocation = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%hd,%hd"), listID, indexID - 1);
	CFStringRef localizedStr = CFBundleCopyLocalizedString(CFBundleGetMainBundle(), strLocation, NULL, NULL);
	if (CFEqual(strLocation, localizedStr)) {
		GetIndString(outStr, listID, indexID);
	} else {
		CFStringGetPascalString(localizedStr, outStr, sizeof(Str255), kCFStringEncodingMacRoman);
	}
	
	CFRelease(strLocation);
	CFRelease(localizedStr);
}
