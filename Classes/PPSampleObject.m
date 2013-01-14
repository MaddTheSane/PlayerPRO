//
//  PPInstrumentObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import "PPSampleObject.h"
#import "ARCBridge.h"

@implementation PPSampleObject

@synthesize stereo;
@synthesize name;
@synthesize loopType;
@synthesize c2spd;
@synthesize amplitude = amp;
@synthesize volume = vol;
@synthesize data;
@synthesize loopBegin = loopBeg;
@synthesize loopSize;
@synthesize relativeNote = relNote;
//@synthesize dataSize;

- (SInt32)dataSize
{
	return [data length];
}

- (id)initWithsData:(sData *)theData
{
	if (self = [super init]) {
		if (!theData) {
			AUTORELEASEOBJNORETURN(self);
			return nil;
		}
		data = [[NSData alloc] initWithBytes:theData->data length:theData->size];
		loopBeg = theData->loopBeg;
		loopSize = theData->loopSize;
		vol = theData->vol;
		c2spd = theData->c2spd;
		loopType = theData->loopType;
		amp = theData->amp;
		relNote = theData->relNote;
		name = [[NSString alloc] initWithCString:theData->name encoding:NSMacOSRomanStringEncoding];
		stereo = theData->stereo;
	}
	return self;
}

- (sData *)createSData
{
	sData *toReturn = malloc(sizeof(sData));
	toReturn->loopBeg = loopBeg;
	toReturn->loopSize = loopSize;
	toReturn->vol = vol;
	toReturn->loopType = loopType;
	toReturn->c2spd = c2spd;
	char theName[32] = {0};
	strlcpy(theName, [name cStringUsingEncoding:NSMacOSRomanStringEncoding], sizeof(theName));
	memcpy(toReturn->name, theName, sizeof(toReturn->name));
	toReturn->amp = amp;
	toReturn->stereo = stereo;
	toReturn->relNote = relNote;
	toReturn->size = [data length];
	NSInteger dataSize2 = [data length];
	toReturn->data = malloc(dataSize2);
	memcpy(toReturn->data, [data bytes], dataSize2);
	
	return toReturn;
}

- (NSString*)description
{
	return [NSString stringWithFormat:@"%@: size: %ld stereo: %@ Loop type: %d size: %ld volume: %d amp: %d", name, (long)self.dataSize, stereo ? @"Yes": @"No", loopType, (long)loopSize, vol, amp];
}

#if !__has_feature(objc_arc)
- (void)dealloc
{
	[data release];
	[name release];
	
	[super dealloc];
}
#endif

@end
