//
//  PPMadCommandObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/17/13.
//
//

#import "PPMadCommandObject.h"
#include <PlayerPROCore/RDriverInt.h>

#define kPPMadCommandInstrument @"PlayerPROKit Cmd Instrument"
#define kPPMadCommandNote @"PlayerPROKit Cmd Note"
#define kPPMadCommandCommand @"PlayerPROKit Cmd Command"
#define kPPMadCommandArgument @"PlayerPROKit Cmd Argument"
#define kPPMadCommandVolume @"PlayerPROKit Cmd Volume"

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
			[self resetCommand];
		} else {
			theCommand = *theCmd;
		}
	}
	return self;
}

- (id)copyWithZone:(NSZone *)zone
{
	return [[[self class] alloc] initWithCmd:&theCommand];
}

- (void)resetCommand;
{
	[self willChangeValueForKey:@"instrument"];
	[self willChangeValueForKey:@"note"];
	[self willChangeValueForKey:@"command"];
	[self willChangeValueForKey:@"argument"];
	[self willChangeValueForKey:@"volume"];

	MADKillCmd(&theCommand);
	
	[self didChangeValueForKey:@"instrument"];
	[self didChangeValueForKey:@"note"];
	[self didChangeValueForKey:@"command"];
	[self didChangeValueForKey:@"argument"];
	[self didChangeValueForKey:@"volume"];
}

- (NSString*)description
{
	return [NSString stringWithFormat:@"ins: %u note: %u cmd: %u arg: %u vol: %u", theCommand.ins, theCommand.note, theCommand.cmd, theCommand.arg, theCommand.vol];
}

- (void)writeBackToStruct
{
	//We don't need to do anything here
}

#pragma mark NSCoding protocol

- (id)initWithCoder:(NSCoder *)aDecoder
{
	if (self = [super init]) {
		theCommand.ins = [(NSNumber*)[aDecoder decodeObjectForKey:kPPMadCommandInstrument] unsignedCharValue];
		theCommand.arg = [(NSNumber*)[aDecoder decodeObjectForKey:kPPMadCommandArgument] unsignedCharValue];
		theCommand.cmd = [(NSNumber*)[aDecoder decodeObjectForKey:kPPMadCommandCommand] unsignedCharValue];
		theCommand.note = [(NSNumber*)[aDecoder decodeObjectForKey:kPPMadCommandNote] unsignedCharValue];
		theCommand.vol = [(NSNumber*)[aDecoder decodeObjectForKey:kPPMadCommandVolume] unsignedCharValue];
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
	[aCoder encodeObject:@(theCommand.ins) forKey:kPPMadCommandInstrument];
	[aCoder encodeObject:@(theCommand.cmd) forKey:kPPMadCommandCommand];
	[aCoder encodeObject:@(theCommand.arg) forKey:kPPMadCommandArgument];
	[aCoder encodeObject:@(theCommand.note) forKey:kPPMadCommandNote];
	[aCoder encodeObject:@(theCommand.vol) forKey:kPPMadCommandVolume];
}

@end
