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
{
	@private
	Cmd theCommand;
}

@synthesize theCommand;

- (MADByte)instrument
{
	return theCommand.ins;
}

- (void)setInstrument:(MADByte)instrument
{
	theCommand.ins = instrument;
}

- (MADByte)note
{
	return theCommand.note;
}

- (void)setNote:(MADByte)note
{
	theCommand.note = note;
}

- (MADByte)command
{
	return theCommand.cmd;
}

- (void)setCommand:(MADByte)command
{
	theCommand.cmd = command;
}

- (MADByte)argument
{
	return theCommand.arg;
}

- (void)setArgument:(MADByte)argument
{
	theCommand.arg = argument;
}

- (MADByte)volume
{
	return theCommand.vol;
}

- (void)setVolume:(MADByte)volume
{
	theCommand.vol = volume;
}

- (instancetype)init
{
	return [self initWithCmd:NULL];
}

- (instancetype)initWithCmd:(Cmd *)theCmd
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

- (instancetype)initWithCoder:(NSCoder *)aDecoder
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
