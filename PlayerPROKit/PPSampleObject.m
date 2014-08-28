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
	@protected
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
	tmpCStr = nil;
	
	strlcpy(sampleWriteTo->name, theName, sizeof(sampleWriteTo->name));
	
	_name = [[NSString alloc] initWithCString:sampleWriteTo->name encoding:NSMacOSRomanStringEncoding];
}

- (NSData*)data
{
	if (!_data) {
		_data = [[NSData alloc] initWithBytesNoCopy:sampleWriteTo->data length:sampleWriteTo->size freeWhenDone:NO];
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
	sampleWriteTo->data = malloc(dataSize2);
	[data getBytes:theSample.data length:dataSize2];
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
	return [self initWithsData:NULL];
}

- (instancetype)initWithsData:(sData *)theData
{
	return self = [self initWithSData:theData];
}

- (instancetype)initWithSData:(sData *)theData
{
	if (self = [super init]) {
		if (!theData) {
			_name = @"";
			_data = [[NSData alloc] init];
			theSample.size = 0;
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
			//theSample.data = NULL;
			self.data = [[NSData alloc] initWithBytes:theData->data length:theData->size];
			//name = [[NSString alloc] initWithCString:theData->name encoding:NSMacOSRomanStringEncoding];
		}
		sampleWriteTo = &theSample;
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

- (id)copyWithZone:(NSZone *)zone
{
	PPSampleObject *obj;
	if ((obj = [[[self class] allocWithZone:zone] init])) {
		obj.name = _name;
		obj.data = _data;
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

- (NSString*)description
{
	return [NSString stringWithFormat:@"%@: size: %ld stereo: %@ Loop type: %d start: %d size: %d volume: %d amp: %d", self.name, (long)[self.data length], self.stereo ? @"Yes": @"No", self.loopType, self.loopBegin, self.loopSize, self.volume, self.amplitude];
}

#define kNoteCompareOptions (NSCaseInsensitiveSearch | NSWidthInsensitiveSearch | NSDiacriticInsensitiveSearch)

+ (short)noteFromString:(NSString *)myTT
{
	if (!myTT || [myTT isEqualToString:@""] || [myTT isEqualToString:@"---"] || [myTT length] < 2) {
		return 0xFF;
	}
	
	short		Oct;
	NSString	*val1 = [myTT substringWithRange:[myTT rangeOfComposedCharacterSequencesForRange:NSMakeRange(0, 1)]];
	NSString	*val2 = [myTT length] >= 3 ? [myTT substringWithRange:[myTT rangeOfComposedCharacterSequencesForRange:NSMakeRange(1, 1)]] : @" ";
	NSString	*val3 = [myTT substringWithRange:[myTT rangeOfComposedCharacterSequencesForRange:NSMakeRange([myTT length] >= 3 ? 2 : 1, 1)]];
	Oct = [val3 intValue];
	Oct *= 12;
	
	//	0	1	 2	 3	 4	 5	 6	 7 	 8	 9	 10	 11
	//	C-  C#   D-  D#  E-  F-  F#  G-  G#  A-  A#  B-
	if ([val1 compare:@"C" options:kNoteCompareOptions] == NSOrderedSame) {
		Oct += 0;
	} else if ([val1 compare:@"D" options:kNoteCompareOptions] == NSOrderedSame) {
		Oct += 2;
	} else if ([val1 compare:@"E" options:kNoteCompareOptions] == NSOrderedSame) {
		Oct += 4;
	} else if ([val1 compare:@"F" options:kNoteCompareOptions] == NSOrderedSame) {
		Oct += 5;
	} else if ([val1 compare:@"G" options:kNoteCompareOptions] == NSOrderedSame) {
		Oct += 7;
	} else if ([val1 compare:@"A" options:kNoteCompareOptions] == NSOrderedSame) {
		Oct += 9;
	} else if ([val1 compare:@"B" options:kNoteCompareOptions] == NSOrderedSame) {
		Oct += 11;
	} else
		Oct = 0xFF;
	
	if (Oct != 0xFF) {
		if ([val2 isEqualToString:@"#"] || [val2 isEqualToString:@"♯"]/*Unicode sharp sign, just in case*/)
			Oct++;
		if (Oct > 95)
			Oct = 0xFF;
		if (Oct < 0)
			Oct = 0xFF;
	}
	
	return Oct;
}

+ (int)numberFromHexString:(NSString *)str
{
	NSScanner *tmpScanner = [[NSScanner alloc] initWithString:str];
	unsigned int tmpVal = 0;
	if (![tmpScanner scanHexInt:&tmpVal])
		return -1;
	else if (tmpVal == UINT32_MAX || tmpVal > 0xFF) {
		return -1;
	}
	
	return tmpVal;
}

#if 0
+ (NSString*)octaveNameFromNote:(short)octNote usingLetters:(BOOL)isUseLetters
{
#if 0
	const char **NNames;
	const char NNames_Letters[12][3] = {"C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B "};
	const char NNames_nonEnglish[12][4] = {"Do", "Do#", "Ré", "Ré#", "Mi", "Fa", "Fa#", "Sol", "Sol#", "La", "La#", "Si"};

	if (isUseLetters) {
		NNames = (const char**)NNames_Letters;
	} else {
		NNames = (const char**)NNames_nonEnglish;
	}
#else
	const char NNames[12][3] = {"C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B "};
#endif
	
	if (octNote > 95) {
		return @"---";
	}
	
	return [[NSString alloc] initWithFormat:@"%@%i", [NSString stringWithUTF8String:NNames[octNote % 12]], octNote / 12];
}

+ (NSString*)octaveNameFromNote:(short)octNote
{
	return [self octaveNameFromNote:octNote usingLetters:YES];
}
#endif

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
