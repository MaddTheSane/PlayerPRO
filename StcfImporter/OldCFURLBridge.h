//
//  OldCFURLBridge.h
//  PPMacho
//
//  Created by C.W. Betts on 7/1/15.
//
//

#ifndef PPMacho_OldCFURLBridge_h
#define PPMacho_OldCFURLBridge_h

#include <CoreFoundation/CoreFoundation.h>
CFURLRef CreateURLWithHFSPathStyle(CFAllocatorRef allocator, CFStringRef hfsPath, bool isDirectory) CF_RETURNS_RETAINED;


#endif
