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

@implementation PPPatternObject
@synthesize commands;
@synthesize index;
@synthesize patternHeader;

- (int)patternSize
{
	return patternHeader.size;
}

- (void)setPatternSize:(int)patternSize
{
	[self willChangeValueForKey:@"patternSize"];
	//TODO: more work here!
	patternHeader.size = patternSize;
	[self didChangeValueForKey:@"patternSize"];
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

@end
