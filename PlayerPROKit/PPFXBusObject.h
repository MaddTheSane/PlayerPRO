//
//  PPFXBusObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/8/14.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

@interface PPFXBusObject : NSObject <NSCopying, NSCoding>
@property (readonly) FXBus theBus;
@property BOOL bypass;
@property short copyId;
@property (getter = isActive) BOOL active;

- (id)initWithFXBus:(FXBus *)set;

@end
