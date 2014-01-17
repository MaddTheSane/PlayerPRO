//
//  PPInstrumentObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import "PPSampleObject.h"
#import "PPSampleObject_PPKPrivate.h"

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
@synthesize theSample;
@synthesize name;
@synthesize data;

- (void)writeBackToStruct
{
	char theName[32] = {0};
	NSData *tmpCStr = [name dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
	NSInteger cStrLen = [tmpCStr length];
	if (cStrLen > sizeof(theName) - 1) {
		cStrLen = sizeof(theName) - 1;
	}
	[tmpCStr getBytes:theName length:cStrLen];
	tmpCStr = nil;
	
	strlcpy(theSample.name, theName, sizeof(theSample.name));
	NSInteger dataSize2 = [data length];
	theSample.size = (SInt32)dataSize2;
	if (theSample.data) {
		free(theSample.data);
		theSample.data = NULL;
	}
	theSample.data = malloc(dataSize2);
	[data getBytes:theSample.data length:dataSize2];
}

- (sData)theSample
{
	[self writeBackToStruct];
	return theSample;
}

- (void)setAmplitude:(Byte)amplitude
{
	theSample.amp = amplitude;
}

- (Byte)amplitude
{
	return theSample.amp;
}

- (void)setLoopSize:(int)loopSize
{
	theSample.loopSize = loopSize;
}

- (int)loopSize
{
	return theSample.loopSize;
}

- (void)setLoopBegin:(int)loopBegin
{
	theSample.loopBeg = loopBegin;
}

- (int)loopBegin
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

- (void)setLoopType:(Byte)loopType
{
	theSample.loopType = loopType;
}

- (Byte)loopType
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

- (Byte)volume
{
	return theSample.vol;
}

- (void)setVolume:(Byte)avolume
{
	theSample.vol = avolume;
}

- (BOOL)isStereo
{
	return theSample.stereo;
}

- (void)setStereo:(BOOL)astereo
{
	theSample.stereo = astereo;
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
			theSample = *theData;
			theSample.data = NULL;
			data = [[NSData alloc] initWithBytes:theData->data length:theData->size];
			name = [[NSString alloc] initWithCString:theData->name encoding:NSMacOSRomanStringEncoding];
		}
	}
	return self;
}

- (void)dealloc
{
	if (theSample.data) {
		free(theSample.data);
		theSample.data = NULL;
	}
}

- (id)copyWithZone:(NSZone *)zone
{
	PPSampleObject *obj = nil;
	if ((obj = [[[self class] allocWithZone:zone] init])) {
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
	return [NSString stringWithFormat:@"%@: size: %ld stereo: %@ Loop type: %d start: %d size: %d volume: %d amp: %d", name, (long)[data length], self.stereo ? @"Yes": @"No", self.loopType, self.loopBegin, self.loopSize, self.volume, self.amplitude];
}

#pragma mark NSCoding implementation
- (void)encodeWithCoder:(NSCoder *)aCoder
{
	[aCoder encodeObject:name forKey:NAMEKEY];
	[aCoder encodeObject:data forKey:DATAKEY];
	[aCoder encodeInt:theSample.loopBeg forKey:LOOPBEGINKEY];
	[aCoder encodeInt:theSample.loopSize forKey:LOOPSIZEKEY];
	[aCoder encodeObject:@(theSample.vol) forKey:VOLUMEKEY];
	[aCoder encodeObject:@(theSample.c2spd) forKey:C2SPDKEY];
	[aCoder encodeObject:@(theSample.loopType) forKey:LOOPTYPEKEY];
	[aCoder encodeObject:@(theSample.amp) forKey:AMPLITUDEKEY];
	[aCoder encodeObject:@(theSample.relNote) forKey:RELATIVENOTEKEY];
	[aCoder encodeBool:theSample.stereo forKey:STEREOKEY];
	
	[aCoder encodeInteger:self.sampleIndex forKey:SAMPLEINDEXKEY];
	[aCoder encodeInteger:self.instrumentIndex forKey:INSTRUMENTINDEXKEY];
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
	if (self = [super init]) {
		name = [aDecoder decodeObjectForKey:NAMEKEY];
		data = [aDecoder decodeObjectForKey:DATAKEY];
		theSample.loopBeg = [aDecoder decodeIntForKey:LOOPBEGINKEY];
		theSample.loopSize = [aDecoder decodeIntForKey:LOOPSIZEKEY];
		theSample.vol = [[aDecoder decodeObjectForKey:VOLUMEKEY] unsignedCharValue];
		theSample.c2spd = [[aDecoder decodeObjectForKey:C2SPDKEY] unsignedShortValue];
		theSample.loopType = [[aDecoder decodeObjectForKey:LOOPTYPEKEY] unsignedCharValue];
		theSample.amp = [[aDecoder decodeObjectForKey:AMPLITUDEKEY] unsignedCharValue];
		theSample.relNote = [[aDecoder decodeObjectForKey:RELATIVENOTEKEY] charValue];
		self.stereo = [aDecoder decodeBoolForKey:STEREOKEY];
		
		self.sampleIndex = [aDecoder decodeIntegerForKey:SAMPLEINDEXKEY];
		self.instrumentIndex = [aDecoder decodeIntegerForKey:INSTRUMENTINDEXKEY];
	}
	return self;
}

@end

@implementation PPSampleObjectImmutable

- (id)copyWithZone:(NSZone *)zone
{
	return nil;
}

- (void)setData:(NSData *)data
{
	NSAssert(NO, @"Mutable command called on immutable object!");
}

- (void)setAmplitude:(Byte)amplitude
{
	NSAssert(NO, @"Mutable command called on immutable object!");
}

- (void)setLoopSize:(int)loopSize
{
	NSAssert(NO, @"Mutable command called on immutable object!");
}

- (void)setLoopBegin:(int)loopBegin
{
	NSAssert(NO, @"Mutable command called on immutable object!");
}

- (void)setC2spd:(unsigned short)c2spd
{
	NSAssert(NO, @"Mutable command called on immutable object!");
}

- (void)setLoopType:(Byte)loopType
{
	NSAssert(NO, @"Mutable command called on immutable object!");
}

- (void)setRelativeNote:(char)relativeNote
{
	NSAssert(NO, @"Mutable command called on immutable object!");
}

- (void)setVolume:(Byte)avolume
{
	NSAssert(NO, @"Mutable command called on immutable object!");
}

- (void)setStereo:(BOOL)astereo
{
	NSAssert(NO, @"Mutable command called on immutable object!");
}


@end
