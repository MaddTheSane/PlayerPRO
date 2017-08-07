//
//  PPPlugInCommon.h
//  PPMacho
//
//  Created by C.W. Betts on 12/26/12.
//
//

#ifndef PPMacho_PPPlugInCommon_h
#define PPMacho_PPPlugInCommon_h

#import <Foundation/NSArray.h>

extern NSArray<NSURL*> * __nonnull DefaultPlugInLocations(void);
extern BOOL isOrderNotImplemented(NSError *__nonnull err);

#endif
