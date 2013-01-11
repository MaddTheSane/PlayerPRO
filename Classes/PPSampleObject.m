//
//  PPInstrumentObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import "PPSampleObject.h"

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
@synthesize dataSize = size;

- (id)initWithsData:(sData *)theData
{
	if (self = [super init]) {
		data = [[NSData alloc] initWithBytes:theData->data length:theData->size];
		size = theData->size;
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

- (NSString*)description
{
	return [NSString stringWithFormat:@"%@: size: %ld stereo: %@ Loop type: %d size: %ld volume: %d amp: %d", name, (long)size, stereo ? @"Yes": @"No", loopType, (long)loopSize, vol, amp];
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
