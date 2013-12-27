//
//  CocoaFuncs.m
//  PPMacho
//
//  Created by C.W. Betts on 10/24/13.
//
//

#import <Foundation/Foundation.h>
#include "RDriver.h"
#include "RDriverInt.h"
#include "PPPrivate.h"

void SetOSType(CFURLRef theURL, OSType theType)
{
	@autoreleasepool {
		NSURL *theNSURL = (__bridge NSURL*)theURL;
		if (theType != 0) {
			[theNSURL setResourceValue:@(theType) forKey:NSFileHFSTypeCode error:NULL];
		}
	}
}
