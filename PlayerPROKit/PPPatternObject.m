//
//  PPPatternObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/17/13.
//
//

#import "PPPatternObject.h"
#import "PPPatternObject_PPKPrivate.h"
#import "PPPatternObject_PcmdHandling.h"
#import "PPMusicObject_PPKPrivate.h"

#define kPPPatternName @"PlayerPROKit Pattern Name"
#define kPPPatternCommands @"PlayerPROKit Pattern Commands"
#define kPPPatternIndex @"PlayerPROKit Pattern Index"

static inline void SwapPcmd(Pcmd *toswap)
{
	if (!toswap) {
		return;
	}
	PPBE32(&toswap->structSize);
	PPBE16(&toswap->length);
	PPBE16(&toswap->posStart);
	PPBE16(&toswap->tracks);
	PPBE16(&toswap->trackStart);
}

@implementation PPPatternObject
{
	@package
	PatHeader patternHeader;
}

@synthesize commands;
@synthesize index;
@synthesize patternHeader;

- (void)writeBackToStruct
{
	
}

- (PatHeader)patternHeader
{
	[self writeBackToStruct];
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

- (id)initWithMusic:(PPMusicObjectWrapper *)mus
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
			PPMadCommandObject *tmpObj = [[PPMadCommandObject alloc] initWithCmd:NULL];
			[self.commands addObject:tmpObj];
		}
		self.patternName = @"";

	}
	return self;
}

- (id)initWithMusic:(PPMusicObjectWrapper *)mus patternAtIndex:(short)ptnIdx
{
	if (self = [self initWithMusic:mus]) {
		patternHeader = _musicWrapper._currentMusic->partition[ptnIdx]->header;
		index = ptnIdx;
		NSInteger size = _musicWrapper._currentMusic->header->numChn * patternHeader.size;
		self.commands = [[NSMutableArray alloc] initWithCapacity:size];
		for (int i = 0; i < size; i++) {
			PPMadCommandObject *tmpObj = [[PPMadCommandObject alloc] initWithCmd:&_musicWrapper._currentMusic->partition[ptnIdx]->Cmds[i]];
			[self.commands addObject:tmpObj];
		}
		self.patternName = [[NSString alloc] initWithCString:patternHeader.name encoding:NSMacOSRomanStringEncoding];
	}
	return self;
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
- (id)initWithCoder:(NSCoder *)aDecoder
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
	[aCoder encodeObject:_patternName forKey:kPPPatternName];
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

+ (OSErr)testPcmdFileAtURL:(NSURL*)theURL
{
	OSErr err = noErr;
	Pcmd thePcmd;
	NSData *pcmdData = [[NSData alloc] initWithContentsOfURL:theURL];
	if (!pcmdData) {
		return MADReadingErr;
	}
	[pcmdData getBytes:&thePcmd length:sizeof(thePcmd)];
	SwapPcmd(&thePcmd);
	if (thePcmd.structSize != [pcmdData length]) {
		err = MADIncompatibleFile;
	}
	return err;
}

- (OSErr)importPcmdFromURL:(NSURL*)theURL
{
	OSErr theErr = noErr;
	NSNumber *curNum;
	theErr = [[self class] testPcmdFileAtURL:theURL];
	if (theErr) {
		return theErr;
	}
	const Pcmd *thePcmd;
	NSMutableData *pcmdData = [[NSMutableData alloc] initWithContentsOfURL:theURL];
	if (!pcmdData) {
		return MADReadingErr;
	}
	unsigned long pcmdLen;
	[theURL getResourceValue:&curNum forKey:NSURLFileSizeKey error:NULL];
	
	if (!curNum) {
		pcmdLen = [pcmdData length];
	} else {
		pcmdLen = [curNum unsignedLongValue];
	}
	
	SwapPcmd([pcmdData mutableBytes]);
	
	thePcmd = [pcmdData bytes];
	//TODO: put cmd data into the pattern
	
	return noErr;
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

static NSString* octaveNameFromNote(short octNote)
{
	const char NNames[][3] = {"C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B "};
	
	if (octNote > 95) {
		return @"---";
	}
	
	return [[NSString alloc] initWithFormat:@"%s%i", NNames[octNote % 12], octNote / 12];
}


BOOL CreateNoteString(Cmd *theCommand, NSMutableString *mainStr, BOOL AllStr)
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
		Note = true;
		[mainStr appendString:octaveNameFromNote(theCommand->note)];
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

+ (NSString *)stringFromPcmdData:(Pcmd*)myPcmd
{
	int	i, x;
	NSMutableString *myText = [[NSMutableString alloc] init];
	NSMutableString *myStr;
	size_t	mSize;
	
	mSize = 5 + myPcmd->tracks * myPcmd->length * 16L;
	
	for (i = 0; i < myPcmd->length; i++) {
		for (x = 0; x < myPcmd->tracks; x++) {
			myStr = [[NSMutableString alloc] init];
			Cmd *myCmd = GetCmd(i, x, myPcmd);
			
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
	
	if ([myText length] >= mSize)
		PPDebugStr(__LINE__, __FILE__, "ZZZ");
	
	return [[NSString alloc] initWithString:myText];
}

static inline Cmd *GetMADCommandFromPatternObj(short PosX, short TrackIdX, PPPatternObject *tempMusicPat)
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

- (Pcmd*)pcmdWithLeft:(int)left right:(int)right top:(int)top bottom:(int)bottom
{
	[self writeBackToStruct];
	int count = (bottom - top + 1) * (right - left + 1), X, Y;
	Cmd *cmd, *cmd2;

	size_t theSize = sizeof(Pcmd) + count * sizeof(Cmd);
	Pcmd *thePcmd = calloc(theSize, 1);
	thePcmd->structSize = (int)theSize;
	thePcmd->tracks		= right - left + 1;
	thePcmd->length		= bottom - top + 1;
	thePcmd->trackStart = left;
	thePcmd->posStart	= top;

	for (X = left; X <= right; X++) {
		for (Y = top; Y <= bottom; Y++) {
			cmd = GetMADCommandFromPatternObj(Y, X, self);
			cmd2 = GetCmd(Y - top, X - left, thePcmd);
			
			*cmd2 = *cmd;
			free(cmd);
		}
	}
	
	return thePcmd;
}

- (Pcmd*)pcmdWithTrackRange:(NSRange)trackRange positionRange:(NSRange)posRange
{
	[self writeBackToStruct];
	NSInteger count = (trackRange.length) * (posRange.length), X, Y;
	Cmd *cmd, *cmd2;
	
	size_t theSize = sizeof(Pcmd) + count * sizeof(Cmd);
	Pcmd *thePcmd = calloc(theSize, 1);
	thePcmd->structSize = (int)theSize;
	thePcmd->tracks		= trackRange.length;
	thePcmd->length		= posRange.length;
	thePcmd->trackStart = trackRange.location;
	thePcmd->posStart	= posRange.location;
	
	for (X = trackRange.location; X <= NSMaxRange(trackRange); X++) {
		for (Y = posRange.location; Y <= NSMaxRange(posRange); Y++) {
			cmd = GetMADCommandFromPatternObj(Y, X, self);
			cmd2 = GetCmd(Y - NSMaxRange(posRange), X - NSMaxRange(trackRange), thePcmd);
			
			*cmd2 = *cmd;
			free(cmd);
		}
	}
	
	return thePcmd;
}

- (OSErr)exportPcmdToURL:(NSURL*)theURL
{
	return noErr;
}

@end
