//
//  PPFXBusObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/8/14.
//
//

#ifndef __PLAYERPROKIT_PPFXBUSOBJECT_H__
#define __PLAYERPROKIT_PPFXBUSOBJECT_H__

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROKit/PPObjectProtocol.h>

#ifndef NS_DESIGNATED_INITIALIZER
#define NS_DESIGNATED_INITIALIZER
#endif

@interface PPFXBusObject : NSObject <PPObject>
@property (readonly) FXBus theBus;
@property BOOL bypass;
@property short copyID;
@property (getter = isActive) BOOL active;

- (nonnull instancetype)initWithFXBus:(nullable FXBus *)set NS_DESIGNATED_INITIALIZER;

@end

#endif
