//
//  PPMadCommandObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/17/13.
//
//

#import "PPMadCommandObject.h"
#include <PlayerPROCore/RDriverInt.h>

@implementation PPMadCommandObject
@synthesize theCommand;

- (Byte)instrument
{
	return theCommand.ins;
}

- (void)setInstrument:(Byte)instrument
{
	theCommand.ins = instrument;
}

- (Byte)note
{
	return theCommand.note;
}

- (void)setNote:(Byte)note
{
	theCommand.note = note;
}

- (Byte)command
{
	return theCommand.cmd;
}

- (void)setCommand:(Byte)command
{
	theCommand.cmd = command;
}

- (Byte)argument
{
	return theCommand.arg;
}

- (void)setArgument:(Byte)argument
{
	theCommand.arg = argument;
}

- (Byte)volume
{
	return theCommand.vol;
}

- (void)setVolume:(Byte)volume
{
	theCommand.vol = volume;
}


- (id)init
{
	return [self initWithCmd:NULL];
}

- (id)initWithCmd:(Cmd *)theCmd
{
	if (self = [super init]) {
		if (!theCmd) {
			MADKillCmd(&theCommand);
		} else {
			theCommand = *theCmd;
		}
	}
	return self;
}

- (id)copyWithZone:(NSZone *)zone
{
	PPMadCommandObject *ret = [[PPMadCommandObject alloc] initWithCmd:&theCommand];

	return ret;
}

- (NSString*)description
{
	return [NSString stringWithFormat:@"ins: %u note: %u cmd: %u arg: %u vol: %u", theCommand.ins, theCommand.note, theCommand.cmd, theCommand.arg, theCommand.vol];
}

@end
