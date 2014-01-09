//
//  PPMadCommandObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/17/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

@interface PPMadCommandObject : NSObject <NSCopying, NSCoding>

- (id)init;
- (id)initWithCmd:(Cmd *)theCmd;

@property (readonly) Cmd theCommand;
@property Byte instrument;
@property Byte note;
@property Byte command;
@property Byte argument;
@property Byte volume;
- (void)resetCommand;
@end
