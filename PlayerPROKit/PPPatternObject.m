//
//  PPPatternObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/17/13.
//
//

#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/MADPlug.h>
#import <PlayerPROKit/PlayerPROKit-Swift.h>
#import "PPPatternObject.h"
#import "PPPatternObject_PPKPrivate.h"
#import "PPPatternObject_PcmdHandling.h"
#import "PPMusicObject_PPKPrivate.h"
#import "PPSampleObject.h"
#if TARGET_OS_OSX
#import "PPPasteboardHandling.h"
#endif

#define kPPPatternName @"PlayerPROKit Pattern Name"
#define kPPPatternCommands @"PlayerPROKit Pattern Commands"
#define kPPPatternIndex @"PlayerPROKit Pattern Index"

static inline void SwapPcmd(Pcmd *toswap)
{
	if (!toswap) {
		return;
	}
	MADBE32(&toswap->structSize);
	MADBE16(&toswap->length);
	MADBE16(&toswap->posStart);
	MADBE16(&toswap->tracks);
	MADBE16(&toswap->trackStart);
}

static inline Pcmd* CopyPcmd(const Pcmd* tocopy)
{
	size_t structlen = tocopy->structSize;
	Pcmd *toret = calloc(structlen, 1);
	memcpy(toret, tocopy, structlen);
	return toret;
}

@interface PPPatternObject ()
- (instancetype)initWithCoder:(NSCoder *)aDecoder NS_DESIGNATED_INITIALIZER;
@end

@implementation PPPatternObject
{
	@package
	PatHeader patternHeader;
	PatData *patternData;
}

- (PPMadCommandObject *)objectAtIndexedSubscript:(NSInteger)index1;
{
	return commands[index1];
}

- (NSInteger)lengthOfCommands
{
	return commands.count;
}

#if TARGET_OS_OSX
NSString * const kPPKPatternPasteboardUTI = @"net.sourceforge.playerpro.pattern";

static NSArray *UTIArray;
static dispatch_once_t initUTIOnceToken;
static const dispatch_block_t initUTIArray = ^{
	UTIArray = @[kPPKPatternPasteboardUTI];
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
	if ([type isEqualToString:kPPKPatternPasteboardUTI]) {
		return [NSKeyedArchiver archivedDataWithRootObject:self];
	} else {
		return nil;
	}
}

+ (NSPasteboardReadingOptions)readingOptionsForType:(NSString *)type pasteboard:(NSPasteboard *)pasteboard
{
	if ([type isEqualToString:kPPKPatternPasteboardUTI])
		return NSPasteboardReadingAsKeyedArchive;
	else
		return NSPasteboardReadingAsData;
}
#endif


@synthesize commands;
@synthesize index;
@synthesize patternHeader;
@synthesize patternName = _patternName;

- (NSString*)patternName
{
	if (!_patternName) {
		_patternName = [[NSString alloc] initWithCString:patternHeader.name encoding:NSMacOSRomanStringEncoding];
		if (!_patternName) {
			memset(patternHeader.name, 0, sizeof(patternHeader.name));
			_patternName = @"";
		}
	}
	return _patternName;
}

- (void)setPatternName:(NSString *)patternName
{
	char theName[32] = {0};
	NSData *tmpCStr = [patternName dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
	NSInteger cStrLen = [tmpCStr length];
	if (cStrLen > sizeof(theName) - 1) {
		cStrLen = sizeof(theName) - 1;
	}
	[tmpCStr getBytes:theName length:cStrLen];
	
	strlcpy(patternHeader.name, theName, sizeof(patternHeader.name));
	
	_patternName = [[NSString alloc] initWithCString:patternHeader.name encoding:NSMacOSRomanStringEncoding];
}

- (PatHeader)patternHeader
{
	return patternHeader;
}

- (int)patternSize
{
	return patternHeader.size;
}

- (void)setPatternSize:(int)patternSize
{
	//TODO: more work here!
	patternHeader.size = patternSize;
}

- (instancetype)initWithMusic:(PPMusicObject *)mus
{
	if (self = [super init]) {
		if (!mus) {
			return nil;
		}
		//TODO: add watchers for changes in number of channels
		index = -1;
		_musicWrapper = mus;
		NSInteger size = _musicWrapper._currentMusic->header->numChn * 64;
		self.commands = [[NSMutableArray alloc] initWithCapacity:size];
		for (int i = 0; i < size; i++) {
			PPMadCommandObject *tmpObj = [[PPMadCommandObject alloc] init];
			[self.commands addObject:tmpObj];
		}
		self.patternName = @"";

	}
	return self;
}

- (instancetype)initWithMusic:(PPMusicObject *)mus patternAtIndex:(short)ptnIdx
{
	if (self = [self initWithMusic:mus]) {
		patternHeader = _musicWrapper._currentMusic->partition[ptnIdx]->header;
		index = ptnIdx;
		NSInteger size = _musicWrapper._currentMusic->header->numChn * patternHeader.size;
		self.commands = [[NSMutableArray alloc] initWithCapacity:size];
		for (int i = 0; i < size; i++) {
			PPMadCommandObject *tmpObj = [[PPMadCommandObject alloc] initWithCmdPtr:&_musicWrapper._currentMusic->partition[ptnIdx]->Cmds[i]];
			[self.commands addObject:tmpObj];
		}
		self.patternName = [[NSString alloc] initWithCString:patternHeader.name encoding:NSMacOSRomanStringEncoding];
	}
	return self;
}

- (PPMadCommandObject*)getCommandFromPosition:(short)PosX channel:(short)TrackIdX
{
	if (PosX < 0)
		PosX = 0;
	else if (PosX >= self.patternSize)
		PosX = self.patternSize - 1;

	return self.commands[(self.patternSize * TrackIdX) + PosX];
}

- (void)replaceCommandAtPosition:(short)PosX channel:(short)TrackIdX cmd:(Cmd)aCmd
{
	if (PosX < 0)
		PosX = 0;
	else if (PosX >= self.patternSize)
		PosX = self.patternSize - 1;
	
	self.commands[(self.patternSize * TrackIdX) + PosX] = [[PPMadCommandObject alloc] initWithCmd:aCmd];
}

- (void)replaceCommandAtPosition:(short)PosX channel:(short)TrackIdX command:(PPMadCommandObject*)aCmd
{
	if (PosX < 0)
		PosX = 0;
	else if (PosX >= self.patternSize)
		PosX = self.patternSize - 1;
	
	self.commands[(self.patternSize * TrackIdX) + PosX] = [aCmd copy];
}

- (void)modifyCommandAtPosition:(short)PosX channel:(short)TrackIdX commandBlock:(void (NS_NOESCAPE^)(Cmd*))block
{
	PPMadCommandObject *tmpMCmd = [self getCommandFromPosition:PosX channel:TrackIdX];
	Cmd tmpCmd = tmpMCmd.theCommand;
	block(&tmpCmd);
	[self replaceCommandAtPosition:PosX channel:TrackIdX cmd:tmpCmd];
}

- (void)modifyCommandAtPosition:(short)PosX channel:(short)TrackIdX madCommandBlock:(void (NS_NOESCAPE^)(PPMadCommandObject*))block
{
	PPMadCommandObject *tmpMCmd = [self getCommandFromPosition:PosX channel:TrackIdX];
	block(tmpMCmd);
}

#if 0
- (void)dealloc
{
	
}
#endif

#pragma mark NSFastEnumeration protocol
- (NSUInteger)countByEnumeratingWithState:(NSFastEnumerationState *)state objects:(id __unsafe_unretained [])buffer count:(NSUInteger)len
{
	return [self.commands countByEnumeratingWithState:state objects:buffer count:len];
}

#pragma mark NSCoding protocol
+ (BOOL)supportsSecureCoding
{
	return YES;
}

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
	if (self = [super init]) {
		self.patternName = [aDecoder decodeObjectForKey:kPPPatternName];
		self.commands = [(NSArray*)[aDecoder decodeObjectForKey:kPPPatternCommands] mutableCopy];
		index = [aDecoder decodeIntegerForKey:kPPPatternIndex];
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
	[aCoder encodeObject:self.patternName forKey:kPPPatternName];
	[aCoder encodeObject:commands forKey:kPPPatternCommands];
	[aCoder encodeInteger:index forKey:kPPPatternIndex];
}

#pragma mark NSCopying protocol

- (id)copyWithZone:(NSZone *)zone
{
	PPPatternObject *new = [[[self class] alloc] initWithMusic:_musicWrapper];
	new.commands = [[NSMutableArray alloc] initWithArray:commands copyItems:YES];
	new.index = -1;
	new.patternName = self.patternName;
	
	return new;
}

#pragma mark - Pcmd handling
+ (BOOL)testPcmdFileAtURL:(NSURL*)theURL error:(NSError *__autoreleasing _Nullable * _Nullable)error
{
	BOOL err = YES;
	Pcmd thePcmd;
	NSData *pcmdData = [[NSData alloc] initWithContentsOfURL:theURL options:0 error:error];
	if (!pcmdData) {
		return NO;
	}
	[pcmdData getBytes:&thePcmd length:sizeof(thePcmd)];
	SwapPcmd(&thePcmd);
	if (thePcmd.structSize != [pcmdData length]) {
		if (error) {
			*error = [NSError errorWithDomain:PPMADErrorDomain code:MADIncompatibleFile userInfo:@{NSURLErrorKey: theURL}];
		}
		err = NO;
	}
	return err;
}

- (BOOL)importPcmdFromURL:(NSURL*)theURL error:(NSError * _Nullable __autoreleasing * _Nullable)error
{
	BOOL isGood = YES;
	NSNumber *curNum;
	isGood = [[self class] testPcmdFileAtURL:theURL error:error];
	if (!isGood) {
		return NO;
	}
	const Pcmd *thePcmd;
	NSMutableData *pcmdData = [[NSMutableData alloc] initWithContentsOfURL:theURL options:(NSDataReadingOptions)0 error:error];
	if (!pcmdData) {
		return NO;
	}
	[theURL getResourceValue:&curNum forKey:NSURLFileSizeKey error:NULL];
	
	SwapPcmd([pcmdData mutableBytes]);
	
	thePcmd = [pcmdData bytes];
	
	return [self importPcmdFromPointer:thePcmd error:error];
}

static inline NSString *GetEffectString(short theEffect)
{
	short effectChar = theEffect;
	char theChar;
	switch (theEffect) {
		case 16:
		case 17:
		case 18:
			effectChar++;
			//fall through
			
		case 10 ... 15:
			theChar = 'A' + effectChar - 10;
			return [[NSString alloc] initWithFormat:@"%c", theChar];
			break;
			
		case 0 ... 9:
			return [@(theEffect) stringValue];
			break;
			
		default:
			break;
	}
	
	return @" ";
}

static BOOL CreateNoteString(Cmd *theCommand, NSMutableString *mainStr, BOOL AllStr)
{
 	BOOL Note = NO;
	
	if (theCommand->ins != 0) {
		Note = YES;
		[mainStr appendString:[NSString stringWithFormat:@"%03d", theCommand->ins]];
	} else {
		[mainStr appendString:@"   "];
	}
	
	[mainStr appendString:@" "];
	
	if (theCommand->note == 0xFE) {
		Note = YES;
		[mainStr appendString:@"OFF"];
	} else if (theCommand->note != 0xFF) {
		Note = YES;
		[mainStr appendString: [PPSampleObject octaveNameFromNote:theCommand->note]];
	} else {
		[mainStr appendString:@"   "];
	}
	
	[mainStr appendString:@" "];
	
	if (theCommand->cmd != 0) {
		Note = YES;
		[mainStr appendString:GetEffectString(theCommand->cmd)];
	} else {
		if (theCommand->arg != 0)
			[mainStr appendString:GetEffectString(theCommand->cmd)];
		else
			[mainStr appendString:@" "];
	}
	
	[mainStr appendString:@" "];
	
	if (theCommand->arg != 0) {
		Note = YES;
		[mainStr appendString:[NSString stringWithFormat:@"%02X", theCommand->arg]];
	} else {
		[mainStr appendString:@"  "];
	}
	
	[mainStr appendString:@" "];
	
	if (theCommand->vol != 0xFF) {
		Note = YES;
		[mainStr appendString:[NSString stringWithFormat:@"%02X", theCommand->vol]];
	} else {
		[mainStr appendString:@"  "];
	}
	
	return Note;
}

+ (NSString *)stringFromPcmdData:(in const Pcmd*)thePcmd
{
	int	i, x;
	NSMutableString *myText = [[NSMutableString alloc] init];
	Pcmd *myPcmd = CopyPcmd(thePcmd);
	size_t	mSize = 5 + myPcmd->tracks * myPcmd->length * 16;
	
	for (i = 0; i < myPcmd->length; i++) {
		for (x = 0; x < myPcmd->tracks; x++) {
			NSMutableString *myStr = [[NSMutableString alloc] init];
			Cmd *myCmd = MADGetCmd(i, x, myPcmd);
			
			if (CreateNoteString(myCmd, myStr, YES)) {
				[myText appendString:myStr];
			} else
				[myText appendString:@"              "];
			
			if (x < myPcmd->tracks - 1)
				[myText appendString:@"\t"];
			else
				[myText appendString:@"\r"];
		}
	}
	free(myPcmd);
	
	if ([myText length] >= mSize)
		MADDebugStr(__LINE__, __FILE__, "ZZZ");
	
	return [[NSString alloc] initWithString:myText];
}

static Cmd *GetMADCommandFromPatternObj(short PosX, short TrackIdX, PPPatternObject *tempMusicPat)
{
	Cmd *theCmd, tmpCmd;
	if (tempMusicPat == NULL)
		return NULL;
	
	if (PosX < 0)
		PosX = 0;
	else if (PosX >= tempMusicPat->patternHeader.size)
		PosX = tempMusicPat->patternHeader.size -1;
	
	tmpCmd = [[tempMusicPat commands][(tempMusicPat->patternHeader.size * TrackIdX) + PosX] theCommand];
	theCmd = malloc(sizeof(Cmd));
	*theCmd = tmpCmd;
	
	return theCmd;
}

+ (NSData*)dataFromPcmd:(in const Pcmd*)thePcmd
{
	size_t structSize = thePcmd->structSize;
	Pcmd *newPcmd = calloc(structSize, 1);
	memcpy(newPcmd, thePcmd, structSize);
	SwapPcmd(newPcmd);
	return [[NSData alloc] initWithBytesNoCopy:newPcmd length:structSize freeWhenDone:YES];
}

- (Pcmd*)newPcmdWithTrackRange:(NSRange)trackRange positionRange:(NSRange)posRange
{
	NSInteger count = (trackRange.length) * (posRange.length);
	
	size_t theSize = sizeof(Pcmd) + count * sizeof(Cmd);
	Pcmd *thePcmd = calloc(theSize, 1);
	if (!thePcmd) {
		return NULL;
	}
	thePcmd->structSize = (int)theSize;
	thePcmd->tracks		= trackRange.length;
	thePcmd->length		= posRange.length;
	thePcmd->trackStart = trackRange.location;
	thePcmd->posStart	= posRange.location;
	
	for (NSInteger X = trackRange.location; X <= NSMaxRange(trackRange); X++) {
		for (NSInteger Y = posRange.location; Y <= NSMaxRange(posRange); Y++) {
			Cmd *cmd, *cmd2;
			cmd = GetMADCommandFromPatternObj(Y, X, self);
			cmd2 = MADGetCmd(Y - NSMaxRange(posRange), X - NSMaxRange(trackRange), thePcmd);
			
			*cmd2 = *cmd;
			free(cmd);
		}
	}
	
	return thePcmd;
}

- (BOOL)exportPcmdToURL:(NSURL*)theURL withTrackRange:(NSRange)trackRange positionRange:(NSRange)posRange error:(NSError**)error;
{
	NSData *datToWrite;
	if (trackRange.length == 0 || posRange.length == 0) {
		if (error) {
			*error = PPCreateErrorFromMADErrorType(MADParametersErr);
		}
		return NO;
	}
	Pcmd *thePcmd = [self newPcmdWithTrackRange:trackRange positionRange:posRange];
	if (!thePcmd) {
		if (error) {
			*error = PPCreateErrorFromMADErrorType(MADNeedMemory);
		}
		return NO;
	}
	NSInteger theLength = thePcmd->structSize;
	SwapPcmd(thePcmd);
	datToWrite = [[NSData alloc] initWithBytesNoCopy:thePcmd length:theLength freeWhenDone:YES];
	
	if (![datToWrite writeToURL:theURL options:NSDataWritingAtomic error:error]) {
		//The Foundation method will fill out the error data for us.
		return NO;
	}
	
	return YES;
}

- (BOOL)importPcmdFromPointer:(in const Pcmd*)thePcmd error:(NSError**)error
{
	//TODO: put cmd data into the pattern
	
	return YES;
}

- (BOOL)importIntPcmdFromPointer:(IntPcmd*)theIntPcmd freeCommandsWhenDone:(BOOL)freeCmds error:(NSError**)error;
{
	Pcmd *tmpPcmd = MADIntPcmdToPcmd(theIntPcmd, freeCmds);
	BOOL iErr = [self importPcmdFromPointer:tmpPcmd error:error];
	free(tmpPcmd);
	return iErr;
}

#ifndef __MACERRORS__
#define unimpErr (-4)
#endif

- (BOOL)importPcmdFromString:(NSString*)pcmdStr error:(NSError**)error
{
	//TODO: implement
	
	if (error) {
		*error = [NSError errorWithDomain:NSOSStatusErrorDomain code:unimpErr userInfo:nil];
	}
	return NO;
}

@end
