//
//  PPDigitalPlugInObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/1/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>
#import "PPPlugInObject.h"

@interface PPDigitalPlugInObject : PPPlugInObject
- (instancetype)init UNAVAILABLE_ATTRIBUTE;
- (instancetype)initWithBundle:(NSBundle*)toInit NS_DESIGNATED_INITIALIZER;
+ (instancetype)createWithBundle:(NSBundle*)toInit NS_RETURNS_RETAINED;

- (MADErr)callWithPcmd:(Pcmd*)myPcmd plugInfo:(PPInfoPlug*)pi;
@end
