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

- (id)init
{
	return [self initWithsData:NULL];
}

- (id)initWithsData:(sData *)theData
{
	if (self = [super init]) {
		if (!theData) {
			name = @"";
			data = [[NSData alloc] init];
			loopType = 0;
			c2spd = NOFINETUNE;
			amp = 8;
			vol = 64;
			stereo = NO;
			loopBeg = 0;
			loopSize = 0;
			relNote = 0;
		} else {
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
	}
	return self;
}

- (id)copyWithZone:(NSZone *)zone
{
	PPSampleObject *obj = nil;
	if ((obj = [[PPSampleObject allocWithZone:zone] init])) {
		obj.name = name;
		obj.data = data;
		obj.amplitude = amp;
		obj.volume = vol;
		obj.stereo = stereo;
		obj.c2spd = c2spd;
		obj.loopType = loopType;
		obj.relativeNote = relNote;
		obj.loopBegin = loopBeg;
		obj.loopSize = loopSize;
	}
	return obj;
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
	NSData *tmpCStr = [name dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
	NSInteger cStrLen = [tmpCStr length];
	if (cStrLen > sizeof(theName) - 1) {
		cStrLen = sizeof(theName) - 1;
	}
	[tmpCStr getBytes:theName length:cStrLen];
	tmpCStr = nil;
	
	memcpy(toReturn->name, theName, sizeof(toReturn->name));
	toReturn->amp = amp;
	toReturn->stereo = stereo;
	toReturn->relNote = relNote;
	NSInteger dataSize2 = [data length];
	toReturn->size = dataSize2;
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
