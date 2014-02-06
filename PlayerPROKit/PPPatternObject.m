//
//  PPPatternObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/17/13.
//
//

#import "PPPatternObject.h"
#import "PPPatternObject_PPKPrivate.h"
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

@end
