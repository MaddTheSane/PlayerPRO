//
//  CocoaFuncs.m
//  PPMacho
//
//  Created by C.W. Betts on 10/24/13.
//
//

#import <Foundation/Foundation.h>
#include "PPPrivate.h"

void SetOSType(CFURLRef theURL, OSType theType)
{
	@autoreleasepool {
		NSURL *theNSURL = (__bridge NSURL*)theURL;
		
		[theNSURL setResourceValue:@(theType) forKey:NSFileHFSTypeCode error:NULL];
	}
}
