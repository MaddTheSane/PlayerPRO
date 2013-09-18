//
//  PPInstrumentObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import "PPSampleObject.h"

#define LOOPBEGINKEY @"Loop Begin"
#define LOOPSIZEKEY @"Loop Size"
#define VOLUMEKEY @"Volume"
#define C2SPDKEY @"c2spd"
#define LOOPTYPEKEY @"Loop Type"
#define AMPLITUDEKEY @"Amplitude"
#define RELATIVENOTEKEY @"Relative Note"
#define NAMEKEY @"Name"
#define STEREOKEY @"Stereo"
#define DATAKEY @"Data"

#define SAMPLEINDEXKEY @"Sample Index"
#define INSTRUMENTINDEXKEY @"Instrument Index"

@implementation PPSampleObject
@synthesize name;
@synthesize data;

- (void)setAmplitude:(unsigned char)amplitude
{
	theSample.amp = amplitude;
}

- (unsigned char)amplitude
{
	return theSample.amp;
}

- (void)setLoopSize:(SInt32)loopSize
{
	theSample.loopSize = loopSize;
}

- (SInt32)loopSize
{
	return theSample.loopSize;
}

- (void)setLoopBegin:(SInt32)loopBegin
{
	theSample.loopBeg = loopBegin;
}

- (SInt32)loopBegin
{
	return theSample.loopBeg;
}

- (void)setC2spd:(unsigned short)c2spd
{
	theSample.c2spd = c2spd;
}

- (unsigned short)c2spd
{
	return theSample.c2spd;
}

- (void)setLoopType:(unsigned char)loopType
{
	theSample.loopType = loopType;
}

- (unsigned char)loopType
{
	return theSample.loopType;
}

- (void)setRelativeNote:(char)relativeNote
{
	theSample.relNote = relativeNote;
}

- (char)relativeNote
{
	return theSample.relNote;
}

- (unsigned char)volume
{
	return theSample.vol;
}

- (void)setVolume:(unsigned char)avolume
{
	theSample.vol = avolume;
}

- (BOOL)isStereo
{
	return theSample.stereo ? YES : NO;
}

- (void)setStereo:(BOOL)astereo
{
	theSample.stereo = astereo ? TRUE : FALSE;
}

- (SInt32)dataSize
{
	return (SInt32)[data length];
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
			theSample.loopType = 0;
			theSample.c2spd = NOFINETUNE;
			theSample.amp = 8;
			theSample.vol = 64;
			theSample.stereo = FALSE;
			theSample.loopBeg = 0;
			theSample.loopSize = 0;
			theSample.relNote = 0;
		} else {
			memcpy(&theSample, theData, sizeof(sData));
			data = [[NSData alloc] initWithBytes:theData->data length:theData->size];
			name = [[NSString alloc] initWithCString:theData->name encoding:NSMacOSRomanStringEncoding];
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
		obj.amplitude = theSample.amp;
		obj.volume = theSample.vol;
		obj.stereo = theSample.stereo ? YES : NO;
		obj.c2spd = theSample.c2spd;
		obj.loopType = theSample.loopType;
		obj.relativeNote = theSample.relNote;
		obj.loopBegin = theSample.loopBeg;
		obj.loopSize = theSample.loopSize;
	}
	return obj;
}

- (sData *)createSData
{
	sData *toReturn = malloc(sizeof(sData));
	memcpy(toReturn, &theSample, sizeof(sData));
	char theName[32] = {0};
	NSData *tmpCStr = [name dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
	NSInteger cStrLen = [tmpCStr length];
	if (cStrLen > sizeof(theName) - 1) {
		cStrLen = sizeof(theName) - 1;
	}
	[tmpCStr getBytes:theName length:cStrLen];
	tmpCStr = nil;
	
	strlcpy(toReturn->name, theName, sizeof(toReturn->name));
	NSInteger dataSize2 = [data length];
	toReturn->size = (SInt32)dataSize2;
	toReturn->data = malloc(dataSize2);
	[data getBytes:toReturn->data length:dataSize2];
	
	return toReturn;
}

- (NSString*)description
{
	return [NSString stringWithFormat:@"%@: size: %ld stereo: %@ Loop type: %d size: %ld volume: %d amp: %d", name, (long)self.dataSize, self.stereo ? @"Yes": @"No", self.loopType, (long)self.loopSize, self.volume, self.amplitude];
}

#pragma mark NSCoding implementation
- (void)encodeWithCoder:(NSCoder *)aCoder
{
	[aCoder encodeObject:name forKey:NAMEKEY];
	[aCoder encodeObject:data forKey:DATAKEY];
	[aCoder encodeObject:@(theSample.loopBeg) forKey:LOOPBEGINKEY];
	[aCoder encodeObject:@(theSample.loopSize) forKey:LOOPSIZEKEY];
	[aCoder encodeObject:@(theSample.vol) forKey:VOLUMEKEY];
	[aCoder encodeObject:@(theSample.c2spd) forKey:C2SPDKEY];
	[aCoder encodeObject:@(theSample.loopType) forKey:LOOPTYPEKEY];
	[aCoder encodeObject:@(theSample.amp) forKey:AMPLITUDEKEY];
	[aCoder encodeObject:@(theSample.relNote) forKey:RELATIVENOTEKEY];
	[aCoder encodeObject:@(self.stereo) forKey:STEREOKEY];
	
	[aCoder encodeObject:@(self.sampleIndex) forKey:SAMPLEINDEXKEY];
	[aCoder encodeObject:@(self.instrumentIndex) forKey:INSTRUMENTINDEXKEY];
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
	if (self = [super init]) {
		self.name = [aDecoder decodeObjectForKey:NAMEKEY];
		self.data = [aDecoder decodeObjectForKey:DATAKEY];
		theSample.loopBeg = [[aDecoder decodeObjectForKey:LOOPBEGINKEY] intValue];
		theSample.loopSize = [[aDecoder decodeObjectForKey:LOOPSIZEKEY] intValue];
		theSample.vol = [[aDecoder decodeObjectForKey:VOLUMEKEY] unsignedCharValue];
		theSample.c2spd = [[aDecoder decodeObjectForKey:C2SPDKEY] unsignedShortValue];
		theSample.loopType = [[aDecoder decodeObjectForKey:LOOPTYPEKEY] unsignedCharValue];
		theSample.amp = [[aDecoder decodeObjectForKey:AMPLITUDEKEY] unsignedCharValue];
		theSample.relNote = [[aDecoder decodeObjectForKey:RELATIVENOTEKEY] charValue];
		self.stereo = [[aDecoder decodeObjectForKey:STEREOKEY] boolValue];
		
		self.sampleIndex = [[aDecoder decodeObjectForKey:SAMPLEINDEXKEY] shortValue];
		self.instrumentIndex = [[aDecoder decodeObjectForKey:INSTRUMENTINDEXKEY] shortValue];
	}
	return self;
}

@end
