//
//  PPMadCommandObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/17/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROKit/PPObjectProtocol.h>

#ifndef NS_DESIGNATED_INITIALIZER
#define NS_DESIGNATED_INITIALIZER
#endif

@interface PPMadCommandObject : NSObject <PPObject>

- (instancetype)init;
- (instancetype)initWithCmdPtr:(Cmd *)theCmd NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithCmd:(Cmd)theCmd;
- (instancetype)initWithCoder:(NSCoder *)aDecoder NS_DESIGNATED_INITIALIZER;

@property (readonly) Cmd theCommand;
@property MADByte instrument;
@property MADByte note;
@property MADEffectID command;
@property MADByte argument;
@property MADByte volume;
- (void)resetCommand;
@end
