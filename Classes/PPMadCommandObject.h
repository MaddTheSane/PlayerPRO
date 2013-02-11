//
//  PPMadCommandObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/17/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

@interface PPMadCommandObject : NSObject <NSCopying>
{
	Cmd theCommand;
}

@property (readonly) Cmd theCommand;
@property (readwrite) Byte instrument;
@property (readwrite) Byte note;
@property (readwrite) Byte command;
@property (readwrite) Byte argument;
@property (readwrite) Byte volume;

- (id)initWithCmd:(Cmd *)theCmd;

@end
