//
//  PPFXBusObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/8/14.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROKit/PPObjectProtocol.h>

@interface PPFXBusObject : NSObject <PPObject>
@property (readonly) FXBus theBus;
@property BOOL bypass;
@property short copyID;
@property (getter = isActive) BOOL active;

- (id)initWithFXBus:(FXBus *)set;

@end
