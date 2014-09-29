//
//  PPDigitalPlugInObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/1/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#import "PPPlugInObject.h"

@class PPDocument;

@interface PPDigitalPlugInObject : PPPlugInObject
- (instancetype)init UNAVAILABLE_ATTRIBUTE;
- (instancetype)initWithBundle:(NSBundle*)toInit NS_DESIGNATED_INITIALIZER;

- (void)beginCallWithPcmd:(inout Pcmd*)myPcmd driver:(PPDriver*)driver parentDocument:(PPDocument*)theDoc handler:(PPPlugErrorBlock)handle;
@end
