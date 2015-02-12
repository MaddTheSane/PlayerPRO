//
//  PPInstrumentObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import "PPSampleObject.h"
#import "PPSampleObject_PPKPrivate.h"
#if !TARGET_OS_IPHONE
#import "PPPasteboardHandling.h"
#endif

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
{
	@private
	sData theSample;
	sData *sampleWriteTo;
}
@synthesize theSample;
@synthesize name = _name;
@synthesize data = _data;

- (NSString*)name
{
	if (!_name) {
		_name = [[NSString alloc] initWithCString:sampleWriteTo->name encoding:NSMacOSRomanStringEncoding];
	}
	return _name;
}

- (void)setName:(NSString *)name
{
	char theName[32] = {0};
	NSData *tmpCStr = [name dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
	NSInteger cStrLen = [tmpCStr length];
	if (cStrLen > sizeof(theName) - 1) {
		cStrLen = sizeof(theName) - 1;
	}
	[tmpCStr getBytes:theName length:cStrLen];
	
	strlcpy(sampleWriteTo->name, theName, sizeof(sampleWriteTo->name));
	
	_name = [[NSString alloc] initWithCString:sampleWriteTo->name encoding:NSMacOSRomanStringEncoding];
}

- (NSData*)data
{
	if (!_data) {
		_data = [[NSData alloc] initWithBytesNoCopy:sampleWriteTo->data length:sampleWriteTo->size freeWhenDone:NO];
	}
	if (!_data) {
		_data = [[NSData alloc] init];
	}
	return _data;
}

- (void)setData:(NSData *)data
{
	_data = nil;
	NSInteger dataSize2 = [data length];
	sampleWriteTo->size = (int)dataSize2;
	if (sampleWriteTo->data) {
		free(sampleWriteTo->data);
		sampleWriteTo->data = NULL;
	}
	if (dataSize2 == 0) {
		sampleWriteTo->data = NULL;
		_data = [[NSData alloc] init];
		return;
	}
	sampleWriteTo->data = malloc(dataSize2);
	[data getBytes:sampleWriteTo->data length:dataSize2];
	_data = [[NSData alloc] initWithBytesNoCopy:sampleWriteTo->data length:sampleWriteTo->size freeWhenDone:NO];
}

#if !TARGET_OS_IPHONE
NSString * const kPPKSamplePasteboardUTI = @"net.sourceforge.playerpro.sData";

static NSArray *UTIArray;
static dispatch_once_t initUTIOnceToken;
static const dispatch_block_t initUTIArray = ^{
	UTIArray = @[kPPKSamplePasteboardUTI];
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
	if ([type isEqualToString:kPPKSamplePasteboardUTI])
		return [NSKeyedArchiver archivedDataWithRootObject:self];
	else
		return nil;
}

+ (NSPasteboardReadingOptions)readingOptionsForType:(NSString *)type pasteboard:(NSPasteboard *)pasteboard
{
	if ([type isEqualToString:kPPKSamplePasteboardUTI])
		return NSPasteboardReadingAsKeyedArchive;
	else
		return NSPasteboardReadingAsData;
}
#endif

- (void)setToSDataPointer:(inout sData *)theData
{
	if (theData->data) {
		free(theData->data);
		theData->data = NULL;
	}
	NSData *currentData = self.data;
	self.data = nil;
	memmove(theData, sampleWriteTo, sizeof(sData));
	sampleWriteTo = theData;
	self.data = currentData;
}

- (instancetype)initWithSDataPointer:(inout sData *)theData
{
	if (self = [self initWithSData:theData]) {
		self.data = nil;
		sampleWriteTo = theData;
	}
	return self;
}

- (sData)theSample
{
	return *sampleWriteTo;
}

- (void)setAmplitude:(Byte)amplitude
{
	sampleWriteTo->amp = amplitude;
}

- (Byte)amplitude
{
	return sampleWriteTo->amp;
}

- (void)setLoopSize:(int)loopSize
{
	sampleWriteTo->loopSize = loopSize;
}

- (int)loopSize
{
	return sampleWriteTo->loopSize;
}

- (void)setLoopBegin:(int)loopBegin
{
	sampleWriteTo->loopBeg = loopBegin;
}

- (int)loopBegin
{
	return sampleWriteTo->loopBeg;
}

- (void)setC2spd:(unsigned short)c2spd
{
	sampleWriteTo->c2spd = c2spd;
}

- (unsigned short)c2spd
{
	return sampleWriteTo->c2spd;
}

- (void)setLoopType:(MADLoopType)loopType
{
	sampleWriteTo->loopType = loopType;
}

- (MADLoopType)loopType
{
	return sampleWriteTo->loopType;
}

- (void)setRelativeNote:(char)relativeNote
{
	sampleWriteTo->relNote = relativeNote;
}

- (char)relativeNote
{
	return sampleWriteTo->relNote;
}

- (Byte)volume
{
	return sampleWriteTo->vol;
}

- (void)setVolume:(Byte)avolume
{
	sampleWriteTo->vol = avolume;
}

- (BOOL)isStereo
{
	return sampleWriteTo->stereo;
}

- (void)setStereo:(BOOL)astereo
{
	sampleWriteTo->stereo = astereo;
}

- (int)dataSize
{
	return (int)[self.data length];
}

- (instancetype)init
{
	return [self initWithSData:NULL];
}

- (instancetype)initWithSData:(sData *)theData
{
	if (self = [super init]) {
		if (!theData) {
			strcpy(theSample.name, "");
			theSample.data = NULL;
			theSample.size = 0;
			theSample.loopType = 0;
			theSample.c2spd = NOFINETUNE;
			theSample.amp = 8;
			theSample.vol = 64;
			theSample.stereo = FALSE;
			theSample.loopBeg = 0;
			theSample.loopSize = 0;
			theSample.relNote = 0;
			sampleWriteTo = &theSample;
		} else {
			theSample = *theData;
			theSample.data = NULL;
			sampleWriteTo = &theSample;
			self.data = [[NSData alloc] initWithBytes:theData->data length:theData->size];
		}
	}
	return self;
}

- (void)dealloc
{
	if (sampleWriteTo == &theSample && theSample.data) {
		free(theSample.data);
		theSample.data = NULL;
	}
}

- (sData *)createSData
{
	sData *toReturn = malloc(sizeof(sData));
	memcpy(toReturn, sampleWriteTo, sizeof(sData));
	char theName[32] = {0};
	NSData *tmpCStr = [_name dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
	NSInteger cStrLen = [tmpCStr length];
	if (cStrLen > sizeof(theName) - 1) {
		cStrLen = sizeof(theName) - 1;
	}
	[tmpCStr getBytes:theName length:cStrLen];
	tmpCStr = nil;
	
	strlcpy(toReturn->name, theName, sizeof(toReturn->name));
	NSInteger dataSize2 = [_data length];
	toReturn->size = (int)dataSize2;
	toReturn->data = malloc(dataSize2);
	[_data getBytes:toReturn->data length:dataSize2];
	
	return toReturn;
}

- (BOOL)isBlankSample
{
	if (self.name && ![self.name isEqualToString:@""]) {
		return NO;
	}
	if (self.data && ([self.data length] != 0)) {
		return NO;
	}
	
	return YES;
}

- (NSRange)loop
{
	if (sampleWriteTo->loopSize == 0) {
		return NSMakeRange(NSNotFound, 0);
	}
	return NSMakeRange(sampleWriteTo->loopBeg, sampleWriteTo->loopSize);
}

- (void)setLoop:(NSRange)loop
{
	if (loop.location == NSNotFound) {
		sampleWriteTo->loopBeg = 0;
		sampleWriteTo->loopSize = 0;
	} else {
		sampleWriteTo->loopBeg = (int)loop.location;
		sampleWriteTo->loopSize = (int)loop.length;
	}
}

- (NSString*)description
{
	return [NSString stringWithFormat:@"%@: size: %ld stereo: %@ Loop type: %d start: %d size: %d volume: %d amp: %d", self.name, (long)[self.data length], self.stereo ? @"Yes": @"No", self.loopType, self.loopBegin, self.loopSize, self.volume, self.amplitude];
}

#pragma mark KVO/KVC helpers
+ (NSSet*)keyPathsForValuesAffectingTheSample
{
	return [NSSet setWithObjects:@"loopBegin", @"loopSize", @"volume", @"c2spd", @"loopType", @"amplitude", @"relativeNote", @"name", @"stereo", @"data", nil];
}

+ (NSSet*)keyPathsForValuesAffectingBlankSample
{
	return [NSSet setWithObjects:@"name", @"data", nil];
}

+ (NSSet*)keyPathsForValuesAffectingLoop
{
	return [NSSet setWithObjects:@"loopBegin", @"loopSize", nil];
}

+ (NSSet*)keyPathsForValuesAffectingLoopBegin
{
	return [NSSet setWithObjects:@"loop", nil];
}

+ (NSSet*)keyPathsForValuesAffectingLoopSize
{
	return [NSSet setWithObjects:@"loop", nil];
}

#pragma mark NSCopying implementation
- (id)copyWithZone:(NSZone *)zone
{
	PPSampleObject *obj;
	if ((obj = [[[self class] allocWithZone:zone] init])) {
		obj.name = self.name;
		obj.data = self.data;
		obj.amplitude = self.amplitude;
		obj.volume = self.volume;
		obj.stereo = self.stereo;
		obj.c2spd = self.c2spd;
		obj.loopType = self.loopType;
		obj.relativeNote = self.relativeNote;
		obj.loop = self.loop;
	}
	return obj;
}

#pragma mark NSCoding implementation
+ (BOOL)supportsSecureCoding
{
	return YES;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
	[aCoder encodeObject:self.name forKey:NAMEKEY];
	[aCoder encodeObject:self.data forKey:DATAKEY];
	[aCoder encodeInt:sampleWriteTo->loopBeg forKey:LOOPBEGINKEY];
	[aCoder encodeInt:sampleWriteTo->loopSize forKey:LOOPSIZEKEY];
	[aCoder encodeObject:@(sampleWriteTo->vol) forKey:VOLUMEKEY];
	[aCoder encodeObject:@(sampleWriteTo->c2spd) forKey:C2SPDKEY];
	[aCoder encodeObject:@(sampleWriteTo->loopType) forKey:LOOPTYPEKEY];
	[aCoder encodeObject:@(sampleWriteTo->amp) forKey:AMPLITUDEKEY];
	[aCoder encodeObject:@(sampleWriteTo->relNote) forKey:RELATIVENOTEKEY];
	[aCoder encodeBool:sampleWriteTo->stereo forKey:STEREOKEY];
	
	[aCoder encodeInteger:self.sampleIndex forKey:SAMPLEINDEXKEY];
	[aCoder encodeInteger:self.instrumentIndex forKey:INSTRUMENTINDEXKEY];
}

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
	if (self = [super init]) {
		sampleWriteTo = &theSample;
		self.name = [aDecoder decodeObjectForKey:NAMEKEY];
		self.data = [aDecoder decodeObjectForKey:DATAKEY];
		self.loopBegin = [aDecoder decodeIntForKey:LOOPBEGINKEY];
		self.loopSize = [aDecoder decodeIntForKey:LOOPSIZEKEY];
		self.volume = [[aDecoder decodeObjectForKey:VOLUMEKEY] unsignedCharValue];
		self.c2spd = [[aDecoder decodeObjectForKey:C2SPDKEY] unsignedShortValue];
		self.loopType = [[aDecoder decodeObjectForKey:LOOPTYPEKEY] unsignedCharValue];
		self.amplitude = [[aDecoder decodeObjectForKey:AMPLITUDEKEY] unsignedCharValue];
		self.relativeNote = [[aDecoder decodeObjectForKey:RELATIVENOTEKEY] charValue];
		self.stereo = [aDecoder decodeBoolForKey:STEREOKEY];
		
		self.sampleIndex = [aDecoder decodeIntegerForKey:SAMPLEINDEXKEY];
		self.instrumentIndex = [aDecoder decodeIntegerForKey:INSTRUMENTINDEXKEY];
	}
	return self;
}

@end
