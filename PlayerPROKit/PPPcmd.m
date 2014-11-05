//
//  PPPcmd.m
//  PPMacho
//
//  Created by C.W. Betts on 11/5/14.
//
//

#import "PPPcmd.h"

#define kPPPcmdTracksKey @"PPPcmd Tracks"
#define kPPPcmdLengthKey @"PPPcmd Length"
#define kPPPcmdStartTrackKey @"PPPcmd Start Track"
#define kPPPcmdStartPositionKey @"PPPcmd Start Position"
#define kPPPcmdCommandsKey @"PPPcmd Commands"

@interface PPPcmd ()
- (instancetype)initWithCoder:(NSCoder *)aDecoder NS_DESIGNATED_INITIALIZER;
@end

@interface NSValue (Cmd)
+ (instancetype)valuewithCmd:(Cmd)value;
@property (readonly) Cmd cmdValue;
@end

@implementation NSValue (Cmd)

+ (instancetype)valuewithCmd:(Cmd)value
{
	return [self valueWithBytes:&value objCType:@encode(Cmd)];
}

- (Cmd)cmdValue
{
	Cmd value;
	[self getValue:&value];
	return value;
}

@end

@implementation PPPcmd
{
	NSMutableArray *_commands;
}
@synthesize tracks;
@synthesize length;
@synthesize startTrack;
@synthesize startPosition;

- (instancetype)init
{
	if (self = [super init]) {
		_commands = [[NSMutableArray alloc] init];
	}
	return self;
}
/*
 PPEXPORT IntPcmd MADPcmdToInt(Pcmd *inVal, bool copyValues);
 PPEXPORT Pcmd *MADIntPcmdToPcmd(IntPcmd inVal, bool freeIntPcmd);
 PPEXPORT MADErr MADCopyPcmdToPcmd(Pcmd* toCopy, Pcmd** outCopy);
*/

- (instancetype)initWithPcmd:(Pcmd*)thePcmd
{
	IntPcmd tmpIntPcmd = MADPcmdToInt(thePcmd, YES);
	if (self = [self init]) {
	}
	return self;
}

- (instancetype)initWithIntPcmd:(IntPcmd)thePcmd
{
	return self = [self initWithIntPcmd:thePcmd freeWhenDone:NO];
}

- (instancetype)initWithIntPcmd:(IntPcmd)thePcmd freeWhenDone:(BOOL)aFree
{
	if (self = [self init]) {
		
	}
	return self;
}


- (NSArray*)commands
{
	return [NSArray arrayWithArray:_commands];
}

- (void)setTheCommands:(NSArray *)commands
{
	_commands = [commands mutableCopy];
}

- (int)getCommandIndexForRow:(short)row track:(short)track
{
	if (row < 0) {
		row = 0;
	} else if (row >= length) {
		row = length - 1;
	}

	if (track < 0) {
		track = 0;
	} else if (track >= tracks) {
		track = tracks - 1;
	}

	return length * track + row;
}

- (Cmd)getCommand:(short)row track:(short)track
{
	int ourIdx = [self getCommandIndexForRow:row track:track];
	NSValue *wrappedObj = _commands[ourIdx];
	
	Cmd toRet = wrappedObj.cmdValue;
	return toRet;
}

- (void)replaceCmd:(short)row track:(short)track command:(Cmd)command
{
	int ourIdx = [self getCommandIndexForRow:row track:track];

	_commands[ourIdx] = [NSValue valuewithCmd:command];
}

- (void)modifyCmdAtRow:(short)row track:(short)track block:(void (^)(Cmd*))block
{
	int ourIdx = [self getCommandIndexForRow:row track:track];
	NSValue *wrappedObj = _commands[ourIdx];
	Cmd toRet = wrappedObj.cmdValue;
	block(&toRet);
	_commands[ourIdx] = [NSValue valuewithCmd:toRet];
}

#pragma mark NSFastEnumeration

- (NSUInteger)countByEnumeratingWithState:(NSFastEnumerationState *)state objects:(__unsafe_unretained id [])buffer count:(NSUInteger)len
{
	return [_commands countByEnumeratingWithState:state objects:buffer count:len];
}

#pragma mark NSSecureCoding

+ (BOOL)supportsSecureCoding
{
	return YES;
}

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
	if (self = [super init]) {
		_commands = [[aDecoder decodeObjectForKey:kPPPcmdCommandsKey] mutableCopy];
		tracks = [aDecoder decodeIntForKey:kPPPcmdTracksKey];
		length = [aDecoder decodeIntForKey:kPPPcmdLengthKey];
		startTrack = [aDecoder decodeIntForKey:kPPPcmdStartTrackKey];
		startPosition = [aDecoder decodeIntForKey:kPPPcmdStartPositionKey];
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
	[aCoder encodeObject:_commands forKey:kPPPcmdCommandsKey];
	[aCoder encodeInt:tracks forKey:kPPPcmdTracksKey];
	[aCoder encodeInt:length forKey:kPPPcmdLengthKey];
	[aCoder encodeInt:startTrack forKey:kPPPcmdStartTrackKey];
	[aCoder encodeInt:startPosition forKey:kPPPcmdStartPositionKey];
}

@end
