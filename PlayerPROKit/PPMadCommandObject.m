//
//  PPMadCommandObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/17/13.
//
//

#import "PPMadCommandObject.h"
#include <PlayerPROCore/RDriverInt.h>
#if !TARGET_OS_IPHONE
#import "PPPasteboardHandling.h"
#endif

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

#if !TARGET_OS_IPHONE
NSString * const kPPKMADCommandPasteboardUTI = @"net.sourceforge.playerpro.MADCommand";

static NSArray *UTIArray;
static dispatch_once_t initUTIOnceToken;
static const dispatch_block_t initUTIArray = ^{
	UTIArray = @[kPPKMADCommandPasteboardUTI];
};

+ (NSArray *)readableTypesForPasteboard:(NSPasteboard *)pasteboard
{
	dispatch_once(&initUTIOnceToken, initUTIArray);
	return UTIArray;
}

- (NSArray *)writableTypesForPasteboard:(NSPasteboard *)pasteboard
{
	dispatch_once(&initUTIOnceToken, initUTIArray);
	return UTIArray;
}
- (id)pasteboardPropertyListForType:(NSString *)type
{
	if ([type isEqualToString:kPPKMADCommandPasteboardUTI])
		return [NSKeyedArchiver archivedDataWithRootObject:self];
	else
		return nil;
}

+ (NSPasteboardReadingOptions)readingOptionsForType:(NSString *)type pasteboard:(NSPasteboard *)pasteboard
{
	if ([type isEqualToString:kPPKMADCommandPasteboardUTI])
		return NSPasteboardReadingAsKeyedArchive;
	else
		return NSPasteboardReadingAsData;
}
#endif

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

- (MADEffectID)command
{
	return theCommand.cmd;
}

- (void)setCommand:(MADEffectID)command
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
	return [self initWithCmdPtr:NULL];
}

- (instancetype)initWithCmd:(Cmd)theCmd
{
	Cmd aCmd = theCmd;
	return [self initWithCmdPtr:&aCmd];
}

- (instancetype)initWithCmdPtr:(Cmd *)theCmd
{
	if (self = [super init]) {
		if (!theCmd) {
			[self reset];
		} else {
			theCommand = *theCmd;
		}
	}
	return self;
}

- (id)copyWithZone:(NSZone *)zone
{
	return [[[self class] alloc] initWithCmdPtr:&theCommand];
}

- (void)reset
{
	[self willChangeValueForKey:@"theCommand"];

	MADKillCmd(&theCommand);
	
	[self didChangeValueForKey:@"theCommand"];
}

- (NSString*)description
{
	return [NSString stringWithFormat:@"ins: %u note: %u cmd: %u arg: %u vol: %u", theCommand.ins, theCommand.note, theCommand.cmd, theCommand.arg, theCommand.vol];
}

#pragma mark KVO/KVC helpers

+ (NSSet*)keyPathsForValuesAffectingTheCommand
{
	return [NSSet setWithObjects:@"instrument", @"note", @"command", @"argument", @"volume", nil];
}

#define keyVal(theVal) \
+ (NSSet*)keyPathsForValuesAffecting ## theVal \
{ \
 return [NSSet setWithObject:@"theCommand"]; \
}

keyVal(Instrument)
keyVal(Note)
keyVal(Command)
keyVal(Argument)
keyVal(Volume)

#undef keyVal

#pragma mark NSCoding protocol

+ (BOOL)supportsSecureCoding
{
	return YES;
}

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
