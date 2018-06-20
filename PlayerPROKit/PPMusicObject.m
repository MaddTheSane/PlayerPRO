//
//  PPMusicObject.m
//  PPMacho
//
//  Created by C.W. Betts on 12/1/12.
//
//

#import <PlayerPROKit/PlayerPROKit-Swift.h>
#import "PPMusicObject.h"
#import "PPLibrary_PPKPrivate.h"
#import "PPDriver.h"
#import "PPDriver_Private.h"
#import "PPMusicObject_PPKPrivate.h"
#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/RDriverInt.h>
#include "PPByteswap.h"
#import "PPInstrumentObject.h"
#import "PPInstrumentObject_PPKPrivate.h"
#import "PPPatternObject.h"
#import "PPErrors.h"

static NSString *utf8OrMacRoman(const char *text) NS_RETURNS_RETAINED;
static NSString *utf8OrMacRoman(const char *text)
{
	if (!text) {
		return nil;
	}
	if (memcmp(text, "\xEF\xBB\xBF", 3) == 0) {
		NSString *uniStr = [[NSString alloc] initWithUTF8String:text];
		if (uniStr) {
			return uniStr;
		}
	}
	return [[NSString alloc] initWithCString:text encoding:NSMacOSRomanStringEncoding];
}

static MADMusic *DeepCopyMusic(MADMusic* oldMus)
{
	if (!oldMus) {
		return NULL;
	}
	MADMusic *toreturn = calloc(sizeof(MADMusic), 1);
	if (toreturn == NULL) {
		return NULL;
	}
	memcpy(toreturn, oldMus, sizeof(MADMusic));
	
	short		i, x;
	size_t		inOutCount, OffSetToSample = 0;
	PatHeader	tempPatHeader;
	
	/**** HEADER ****/
	inOutCount = sizeof(MADSpec);
	
	toreturn->header = (MADSpec*)calloc(inOutCount, 1);
	
	memcpy(toreturn->header, oldMus->header, inOutCount);
	OffSetToSample += inOutCount;
	
	/**** PARTITION ****/
	for (int iTmp = 0; iTmp < MAXPATTERN - toreturn->header->numPat; iTmp++) {
		size_t i = iTmp + toreturn->header->numPat;
		toreturn->partition[i] = NULL;
	}
	
	for (i = 0; i < toreturn->header->numPat; i++) {
		/** Lecture du header de la partition **/
		inOutCount = sizeof(PatHeader);
		
		memcpy(&tempPatHeader, &oldMus->partition[i]->header, inOutCount);
		
		inOutCount = sizeof(PatHeader) + toreturn->header->numChn * tempPatHeader.size * sizeof(Cmd);
		
		toreturn->partition[i] = (PatData*)malloc(inOutCount);
		if (toreturn->partition[i] == NULL) {
			for (x = 0; x < i; x++) {
				if (toreturn->partition[x] != NULL)
					free(toreturn->partition[x]);
			}
			free(toreturn->header);
			free(toreturn);
			
			return NULL;
		}
		
		memcpy(toreturn->partition[i], oldMus->partition[i], inOutCount);
		OffSetToSample += inOutCount;
	}
	
	/**** INSTRUMENTS ****/
	toreturn->fid = (InstrData*)calloc(sizeof(InstrData), MAXINSTRU);
	if (!toreturn->fid) {
		for (x = 0; x < toreturn->header->numPat; x++) {
			if (toreturn->partition[x] != NULL)
				free(toreturn->partition[x]);
		}
		free(toreturn->header);
		free(toreturn);
		
		return NULL;
	}
	
	inOutCount = sizeof(InstrData) * toreturn->header->numInstru;
	memcpy(toreturn->fid, oldMus->fid, inOutCount);
	OffSetToSample += inOutCount;
	
	for (i = toreturn->header->numInstru - 1; i >= 0 ; i--) {
		InstrData *curIns = &toreturn->fid[i];
		
		if (i != curIns->no) {
			toreturn->fid[curIns->no] = *curIns;
			MADResetInstrument(curIns);
		}
	}
	toreturn->header->numInstru = MAXINSTRU;
	
	/**** SAMPLES ****/
	toreturn->sample = (sData**) calloc(sizeof(sData*), MAXINSTRU * MAXSAMPLE);
	if (!toreturn->sample) {
		for (x = 0; x < MAXINSTRU ; x++)
			MADKillInstrument(toreturn, x);
		
		for (x = 0; x < toreturn->header->numPat; x++) {
			if (toreturn->partition[x] != NULL)
				free(toreturn->partition[x]);
		}
		free(toreturn->header);
		free(toreturn);
		
		return NULL;
	}
	
	for (i = 0; i < MAXINSTRU ; i++) {
		for (x = 0; x < toreturn->fid[i].numSamples ; x++) {
			sData	*curData;
			sData	*oldData = oldMus->sample[i * MAXSAMPLE + x];
			
			// ** Read Sample header **
			
			curData = toreturn->sample[i * MAXSAMPLE + x] = (sData*)malloc(sizeof(sData));
			if (curData == NULL) {
				for (x = 0; x < MAXINSTRU ; x++)
					MADKillInstrument(toreturn, x);
				
				for (x = 0; x < toreturn->header->numPat; x++) {
					if (toreturn->partition[x] != NULL)
						free(toreturn->partition[x]);
				}
				free(toreturn->header);
				free(toreturn);
				
				return NULL;
			}
			
			inOutCount = sizeof(sData);
			
			memcpy(curData, oldMus, inOutCount);
			OffSetToSample += inOutCount;
			
			// ** Read Sample DATA
			
			curData->data = (char*)malloc(curData->size);
			if (curData->data == NULL) {
				for (x = 0; x < MAXINSTRU ; x++)
					MADKillInstrument(toreturn, x);
				
				for (x = 0; x < toreturn->header->numPat; x++) {
					if (toreturn->partition[x] != NULL)
						free(toreturn->partition[x]);
				}
				free(toreturn->header);
				free(toreturn);
				
				return NULL;
			}
			
			inOutCount = curData->size;
			memcpy(curData->data, oldData->data, inOutCount);
			OffSetToSample += inOutCount;
		}
	}
	for (int ixi = 0; ixi < MAXINSTRU; ixi++) {
		toreturn->fid[ixi].firstSample = ixi * MAXSAMPLE;
	}
	
	// EFFECTS *** *** *** *** *** *** *** *** *** *** *** ***
	
	{
		short	alpha;
		
		toreturn->sets = (FXSets*)calloc(sizeof(FXSets), MAXTRACK);
		
		alpha = 0;
		
		for (i = 0; i < 10 ; i++) {	// Global Effects
			if (toreturn->header->globalEffect[i]) {
				inOutCount = sizeof(FXSets);
				toreturn->sets[alpha] = oldMus->sets[alpha];
				OffSetToSample += inOutCount;
				alpha++;
			}
		}
		
		for (i = 0; i < toreturn->header->numChn ; i++) {	// Channel Effects
			for (x = 0; x < 4; x++) {
				if (toreturn->header->chanEffect[i][x]) {
					inOutCount = sizeof(FXSets);
					toreturn->sets[alpha] = oldMus->sets[alpha];
					OffSetToSample += inOutCount;
					alpha++;
				}
			}
		}
	}
	
	toreturn->header->MAD = 'MADK';
	
	return toreturn;
}

@interface PPMusicObject ()
{
	NSMutableArray<PPInstrumentObject*>	*_instruments;
}
@property (readwrite, strong, nonatomic) NSMutableArray *patterns;
@property (readwrite, strong, nonatomic) NSMutableArray *buses;
@end

@implementation PPMusicObject
@synthesize information = madInfo;
@synthesize attachedDriver;
@synthesize _currentMusic = currentMusic;
@synthesize title = internalFileName;

- (BOOL)usesLinearPitchTable
{
	return currentMusic->header->XMLinear;
}

- (void)setUsesLinearPitchTable:(BOOL)usesLinearPitchTable
{
	currentMusic->header->XMLinear = usesLinearPitchTable;
}

- (BOOL)limitPitchToMODTable
{
	return currentMusic->header->MODMode;
}

- (void)setLimitPitchToMODTable:(BOOL)limitPitchToMODTable
{
	currentMusic->header->MODMode = limitPitchToMODTable;
}

- (BOOL)showsCopyright
{
	return currentMusic->header->showCopyright;
}

- (void)setShowsCopyright:(BOOL)showsCopyright
{
	currentMusic->header->showCopyright = showsCopyright;
}

- (int)newPitch
{
	return currentMusic->header->EPitch;
}

- (void)setNewPitch:(int)newPitch
{
	currentMusic->header->EPitch = newPitch;
}

- (int)newSpeed
{
	return currentMusic->header->ESpeed;
}

- (void)setNewSpeed:(int)newSpeed
{
	currentMusic->header->ESpeed = newSpeed;
}

- (MADByte)generalPitch
{
	return currentMusic->header->generalPitch;
}

- (void)setGeneralPitch:(MADByte)generalPitch
{
	currentMusic->header->generalPitch = generalPitch;
}

- (MADByte)generalSpeed
{
	return currentMusic->header->generalSpeed;
}

- (void)setGeneralSpeed:(MADByte)generalSpeed
{
	currentMusic->header->generalSpeed = generalSpeed;
}

- (MADByte)generalVolume
{
	return currentMusic->header->generalVol;
}

- (void)setGeneralVolume:(MADByte)generalVolume
{
	currentMusic->header->generalVol = generalVolume;
}

- (MADErr)exportInstrumentListToURL:(NSURL*)outURL
{
	NSFileHandle *outData = [NSFileHandle fileHandleForWritingToURL:outURL error:nil];
	if (!outData) {
		return MADNeedMemory;
	}
	
	int i, x;
	{
		InstrData *tempInstrData = calloc(sizeof(InstrData), MAXINSTRU);
		if (!tempInstrData) {
			return MADNeedMemory;
		}
		memcpy(tempInstrData, [self internalMadMusicStruct]->fid, sizeof(InstrData) * MAXINSTRU);
		
		for (int x = 0; x < MAXINSTRU; x++) {
			ByteSwapInstrData(&tempInstrData[x]);
		}
		[outData writeData:[[NSData alloc] initWithBytesNoCopy:tempInstrData length:sizeof(InstrData)* MAXINSTRU]];
	}
	
	for (i = 0; i < MAXINSTRU ; i++) {
		for (x = 0; x < [self internalMadMusicStruct]->fid[i].numSamples ; x++) {
			sData tempData, *curData = [self internalMadMusicStruct]->sample[i * MAXSAMPLE +  x];
			sData32 writeData;
			memcpy(&tempData, curData, sizeof(sData));
			ByteSwapsData(&tempData);
			memcpy(&writeData, &tempData, sizeof(sData32));
			writeData.data = 0;
			[outData writeData:[[NSData alloc] initWithBytes:&writeData length:sizeof(sData32)]];
#ifdef __LITTLE_ENDIAN__
			{
				Ptr dataData = malloc(curData->size);
				if (!dataData)
					return MADNeedMemory;
				
				memcpy(dataData, curData->data, curData->size);
				if (curData->amp == 16) {
					short *shortPtr = (short*) dataData;
					
					for (int y = 0; y < curData->size / 2; y++) {
						MADBE16(&shortPtr[y]);
					}
				}
				[outData writeData:[[NSData alloc] initWithBytesNoCopy:dataData length:curData->size]];
			}
#else
			[outdata writeData:[[NSData alloc] initWithBytes:curData->data length: curData->size]];
#endif
		}
	}
	
	return MADNoErr;
}

- (NSArray *)instruments
{
	if (!_instruments) {
		NSMutableArray *array = [[NSMutableArray alloc] initWithCapacity:MAXINSTRU];
		for (NSInteger i = 0; i < MAXINSTRU; i++) {
			PPInstrumentObject *immIns = [[PPInstrumentObject alloc] initWithMusicStruct:currentMusic atIndex:i];
			[array addObject:immIns];
		}
		_instruments = array;
	}
	
	return _instruments;
}

- (void)setInstruments:(NSArray<PPInstrumentObject*> *)instruments
{
	//TODO: write back to struct
	_instruments = [instruments mutableCopy];
}

+ (MADErr)info:(MADInfoRec*)theInfo fromTrackerAtURL:(NSURL*)thURL usingLibrary:(PPLibrary*)theLib
{
	char filetype[5];
	MADErr theErr = MADNoErr;
	if (!theInfo || !thURL || !theLib) {
		return MADParametersErr;
	}
	
	if ((theErr = MADMusicIdentifyCFURL(theLib.theLibrary, filetype, (__bridge CFURLRef)(thURL))) != MADNoErr)
		return theErr;
	
	return MADMusicInfoCFURL(theLib.theLibrary, filetype, (__bridge CFURLRef)(thURL), theInfo);
}

- (NSString *)title
{
	if (!internalFileName) {
		internalFileName = [[NSString alloc] initWithCString:currentMusic->header->name encoding:NSMacOSRomanStringEncoding];
	}
	if (!internalFileName) {
		internalFileName = @"";
	}
	return internalFileName;
}

- (void)setTitle:(NSString*)newInfo
{
	internalFileName = nil;
	NSData *outMacRoman = [newInfo dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
	if (!outMacRoman || outMacRoman.length == 0) {
		memset(currentMusic->header->name, 0, sizeof(currentMusic->header->name));
	} else {
		char fileNameInt[32] = {0};
		[outMacRoman getBytes:fileNameInt length:MIN(outMacRoman.length, sizeof(fileNameInt) - 1)];
		strlcpy(currentMusic->header->name, fileNameInt, sizeof(currentMusic->header->name));
	}
}

- (void)setInformation:(NSString*)newInfo
{
	madInfo = nil;
	// Prefer Mac OS Roman for backwards compatibility.
	NSData *outMacRoman = [newInfo dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:NO];
	if (newInfo.length != 0 && (!outMacRoman || outMacRoman.length == 0)) {
		NSMutableData *utf8BOM = [NSMutableData dataWithBytes:"\xEF\xBB\xBF" length:3];
		NSData *outUTF8 = [[newInfo precomposedStringWithCanonicalMapping] dataUsingEncoding:NSUTF8StringEncoding allowLossyConversion:YES];
		[utf8BOM appendData:outUTF8];
		outMacRoman = utf8BOM;
	}
	char fileNameInt[239] = {0};
	[outMacRoman getBytes:fileNameInt length:MIN(outMacRoman.length, sizeof(fileNameInt) - 1)];
	strlcpy(currentMusic->header->infos, fileNameInt, sizeof(currentMusic->header->infos));
}

- (NSString*)information
{
	if (!madInfo) {
		madInfo = utf8OrMacRoman(currentMusic->header->infos);
	}
	if (!madInfo) {
		madInfo = @"";
	}
	return madInfo;
}

- (instancetype)initWithMusicStruct:(MADMusic*)theStruct
{
	return [self initWithMusicStruct:theStruct copy:YES];
}

- (instancetype)initWithMusicStruct:(MADMusic*)theStruct copy:(BOOL)copyData
{
	if (self = [super init]) {
		if (copyData) {
			currentMusic = DeepCopyMusic(theStruct);
		} else {
			currentMusic = theStruct;
		}
	}
	return self;
}

- (instancetype)initWithURL:(NSURL *)url error:(out NSError* __autoreleasing*)error
{
	if (self = [super init]) {
		MADErr iErr = MADLoadMusicCFURLFile(NULL, &currentMusic, "MADK", (__bridge CFURLRef)url);
		if (iErr != MADNoErr) {
			if (error) {
				*error = PPCreateErrorFromMADErrorType(iErr);
			}
			return nil;
		}
		
	}
	
	return self;
}

- (instancetype)initWithPath:(NSString *)url error:(out NSError* __autoreleasing*)error
{
	return [self initWithURL:[NSURL fileURLWithPath:url] error:error];
}

- (instancetype)initWithURL:(NSURL *)url library:(PPLibrary *)theLib error:(out NSError* __autoreleasing*)error
{
	char type[5];
	MADErr iErr = MADMusicIdentifyCFURL(theLib.theLibrary, type, (__bridge CFURLRef)(url));
	if (iErr != MADNoErr) {
		if (error) {
			*error = PPCreateErrorFromMADErrorType(iErr);
		}
		return nil;
	}
	
	return [self initWithURL:url type:type library:theLib error:error];
}

- (instancetype)initWithPath:(NSString *)path type:(in const char*)type library:(PPLibrary *)theLib error:(out NSError* __autoreleasing*)error
{
	return [self initWithURL:[NSURL fileURLWithPath:path] type:type library:theLib error:error];
}

- (instancetype)initWithURL:(NSURL *)url stringType:(NSString*)type library:(PPLibrary *)theLib error:(out NSError* __autoreleasing*)error
{
	return [self initWithURL:url type:[type cStringUsingEncoding:NSMacOSRomanStringEncoding] library:theLib error:error];
}

- (instancetype)initWithPath:(NSString *)url stringType:(NSString*)type library:(PPLibrary *)theLib error:(out NSError* __autoreleasing*)error
{
	return [self initWithPath:url type:[type cStringUsingEncoding:NSMacOSRomanStringEncoding] library:theLib error:error];
}

- (instancetype)initWithURL:(NSURL *)url type:(in const char*)type library:(PPLibrary *)theLib error:(out NSError* __autoreleasing*)error
{
	if (self = [super init]) {
		MADErr iErr = MADLoadMusicCFURLFile(theLib.theLibrary, &currentMusic, (char*)type, (__bridge CFURLRef)(url));
		if (iErr != MADNoErr) {
			if (error) {
				*error = PPCreateErrorFromMADErrorType(iErr);
			}
			return nil;
		}
	}
	return self;
}

- (instancetype)initWithPath:(NSString *)url library:(PPLibrary *)theLib error:(out NSError* __autoreleasing*)error
{
	return [self initWithURL:[NSURL fileURLWithPath:url] library:theLib error:error];
}

- (instancetype)init
{
	return [self initWithMusicStruct:CreateFreeMADK() copy:NO];
}

- (instancetype)initWithURL:(NSURL *)url stringType:(NSString*)type driver:(PPDriver *)theDriv error:(out NSError* __autoreleasing*)error
{
	return [self initWithURL:url type:[type cStringUsingEncoding:NSMacOSRomanStringEncoding] driver:theDriv error:error];
}

- (instancetype)initWithPath:(NSString *)url stringType:(NSString*)type driver:(PPDriver *)theDriv error:(out NSError* __autoreleasing*)error
{
	return [self initWithPath:url type:[type cStringUsingEncoding:NSMacOSRomanStringEncoding] driver:theDriv error:error];
}

- (instancetype)initWithURL:(NSURL *)url type:(in const char*)type driver:(PPDriver *)theLib error:(out NSError* __autoreleasing*)error
{
	if (self = [self initWithURL:url type:type library:theLib.theLibrary error:error]) {
		[self attachToDriver:theLib];
	}
	return self;
}

- (instancetype)initWithURL:(NSURL *)url driver:(PPDriver *)theLib error:(out NSError* __autoreleasing*)error
{
	if (self = [self initWithURL:url library:theLib.theLibrary error:error]) {
		[self attachToDriver:theLib];
	}
	return self;
}

- (instancetype)initWithPath:(NSString *)url type:(in const char*)type driver:(PPDriver *)theLib error:(out NSError* __autoreleasing*)error
{
	return [self initWithURL:[NSURL fileURLWithPath:url] type:type driver:theLib error:error];
}

- (instancetype)initWithPath:(NSString *)url driver:(PPDriver *)theLib error:(out NSError* __autoreleasing*)error
{
	return [self initWithURL:[NSURL fileURLWithPath:url] driver:theLib error:error];
}

- (void)attachToDriver:(PPDriver *)theDriv
{
	self.attachedDriver = theDriv;
	theDriv.currentMusic = self;
}

- (void)dealloc
{
	if (self.attachedDriver && self.attachedDriver.currentMusic == self) {
		self.attachedDriver.currentMusic = nil;
	}
	if (currentMusic) {
		MADDisposeMusic(&currentMusic, NULL);
		currentMusic = NULL;
	}
}

- (BOOL)saveMusicToURL:(NSURL *)tosave error:(out NSError**)error;
{
	return [self saveMusicToURL:tosave compress:NO error:error];
}

- (BOOL)saveMusicToURL:(NSURL *)tosave compress:(BOOL)mad1Comp error:(out NSError**)error;
{
	MADErr retErr;
	if ((retErr = MADMusicSaveCFURL(currentMusic, (__bridge CFURLRef)tosave, mad1Comp)) == MADNoErr) {
		currentMusic->hasChanged = false;
	}
	
	if (retErr != MADNoErr && error) {
		*error = PPCreateErrorFromMADErrorType(retErr);
	}
	
	return retErr == MADNoErr;
}

- (BOOL)exportMusicToURL:(NSURL *)tosave format:(NSString*)form library:(PPLibrary*)otherLib error:(out NSError**)error;
{
	MADErr theErr = MADNoErr;
	if ([form isEqualToString:@"MADK"]) {
		return [self saveMusicToURL:tosave error:error];
	} else {
		theErr = MADMusicExportCFURL(otherLib.theLibrary, self._currentMusic, (char*)[form cStringUsingEncoding:NSMacOSRomanStringEncoding], (__bridge CFURLRef)tosave);
	}
	
	if (error) {
		*error = PPCreateErrorFromMADErrorType(theErr);
	}
	
	return theErr == MADNoErr;
}

- (MADMusic *)copyMadMusicStruct
{
	return DeepCopyMusic(currentMusic);
}

- (MADMusic *)internalMadMusicStruct
{
	return currentMusic;
}

- (id)copyWithZone:(NSZone *)zone
{
	PPMusicObject *copyWrap = [[PPMusicObject alloc] initWithMusicStruct:currentMusic];
	
	copyWrap.information = self.information;
	copyWrap.title = self.title;
	//copyWrap.instruments = [[NSMutableArray alloc] initWithArray:_instruments copyItems:YES];
	//copyWrap.patterns = [[NSMutableArray alloc] initWithArray:_patterns copyItems:YES];
	return copyWrap;
}

#if 0
- (void)setUpObjCStructures
{
	int i;
	self.internalFileName = [NSString stringWithCString:currentMusic->header->name encoding:NSMacOSRomanStringEncoding];
	self.madInfo = [NSString stringWithCString:currentMusic->header->infos encoding:NSMacOSRomanStringEncoding];
	self.madType = currentMusic->header->MAD;
	self.instruments = [[NSMutableArray alloc] initWithCapacity:MAXINSTRU];
	for (i = 0; i < MAXINSTRU; i++) {
		PPInstrumentObject *insObj = [[PPInstrumentObject alloc] initWithMusic:self instrumentIndex:i];
		[self.instruments addObject:insObj];
	}
	self.patterns = [[NSMutableArray alloc] initWithCapacity:currentMusic->header->numPat];
	for (i = 0; i < currentMusic->header->numPat; i++) {
		PPPatternObject *obj = [[PPPatternObject alloc] initWithMusic:self patternAtIndex:i];
		[self.patterns addObject:obj];
	}
}
#endif

- (NSArray *)sDatas {
	NSMutableArray *ourArray = [[NSMutableArray alloc] initWithCapacity:MAXINSTRU * MAXSAMPLE];
	for (PPInstrumentObject *instrument in self.instruments) {
		for (PPSampleObject *sample in instrument.samples) {
			[ourArray addObject:sample];
		}
	}
	
	return [ourArray copy];
}

- (BOOL)addInstrument:(PPInstrumentObject*)theIns
{
	if ([[self instruments] count] >= MAXINSTRU) {
		return NO;
	}
	[self addInstrumentObject:theIns];
	return YES;
}

- (BOOL)importInstrumentListFromURL:(NSURL *)insURL error:(out NSError *__autoreleasing*)theErr
{
	NSData *fileData = [NSData dataWithContentsOfURL:insURL];
	if (!fileData) {
		if (theErr) {
			*theErr = PPCreateErrorFromMADErrorType(MADReadingErr);
		}
		return NO;
	}
	int		x, i;
	long	inOutCount, filePos = 0;
	InstrData *tempInstrData;
	{
		tempInstrData = calloc(sizeof(InstrData), MAXINSTRU);
		if (tempInstrData == NULL) {
			if (theErr) {
				*theErr = PPCreateErrorFromMADErrorType(MADIncompatibleFile);
			}
			return NO;
		}
		
		// **** HEADER ***
		inOutCount = sizeof(InstrData) * MAXINSTRU;
		if ([fileData length] <= inOutCount) {
			if (theErr) {
				*theErr = PPCreateErrorFromMADErrorType(MADIncompatibleFile);
			}
			free(tempInstrData);
			return NO;
		}
		[fileData getBytes:tempInstrData range:NSMakeRange(filePos, inOutCount)];
		
		for (int x = 0; x < MAXINSTRU; x++) {
			ByteSwapInstrData(&tempInstrData[x]);
		}
		filePos += inOutCount;
	}
	
	//Clean up old instruments
	
	sData **tmpsData = calloc(sizeof(sData*), MAXINSTRU * MAXSAMPLE);
	
	// **** INSTRUMENTS ***
	for (i = 0; i < MAXINSTRU ; i++) {
		for (x = 0; x < tempInstrData[i].numSamples ; x++) {
			sData	*curData;
			
			// ** Read Sample header **
			
			curData = tmpsData[i * MAXSAMPLE + x] = (sData*)malloc(sizeof(sData));
			if (curData == NULL) {
				if (theErr) {
					*theErr = PPCreateErrorFromMADErrorType(MADNeedMemory);
				}
				free(tempInstrData);
				for (int x = 0; x < MAXSAMPLE * MAXINSTRU; x++) {
					if (tmpsData[x]) {
						if (tmpsData[x]->data) {
							free(tmpsData[x]->data);
						}
						free(tmpsData[x]);
					}
				}
				
				free(tmpsData);
				return NO;
			}
			
			inOutCount = sizeof(sData32);
			[fileData getBytes:curData range:NSMakeRange(filePos, inOutCount)];
			ByteSwapsData(curData);
			filePos += inOutCount;
			
			// ** Read Sample DATA
			
			curData->data = malloc(curData->size);
			if (curData->data == NULL) {
				if (theErr) {
					*theErr = PPCreateErrorFromMADErrorType(MADNeedMemory);
				}
				
				free(tempInstrData);
				for (int x = 0; x < MAXSAMPLE * MAXINSTRU; x++) {
					if (tmpsData[x]) {
						if (tmpsData[x]->data) {
							free(tmpsData[x]->data);
						}
						free(tmpsData[x]);
					}
				}
				
				free(tmpsData);
				return NO;
			}
			
			inOutCount = curData->size;
			[fileData getBytes:curData->data range:NSMakeRange(filePos, inOutCount)];
			filePos += inOutCount;
			if (curData->amp == 16) {
				short *shortPtr = (short*)curData->data;
				
				for (int y = 0; y < inOutCount / 2; y++) {
					MADBE16(&shortPtr[y]);
				}
			}
		}
	}
	// *********************
	
	for (x = 0; x < MAXINSTRU ; x++) MADKillInstrument(currentMusic, x);
	memcpy(currentMusic->fid, tempInstrData, inOutCount);
	free(tempInstrData);
	
	for (int x = 0; x < MAXSAMPLE * MAXINSTRU; x++) {
		if (currentMusic->sample[x]) {
			if (currentMusic->sample[x]->data) {
				free(currentMusic->sample[x]->data);
			}
			free(currentMusic->sample[x]);
		}
	}
	free(currentMusic->sample);
	currentMusic->sample = tmpsData;
	for (i = 0; i < MAXINSTRU; i++) {
		PPInstrumentObject *insObj = [[PPInstrumentObject alloc] initWithMusic:self instrumentIndex:i];
		[self replaceObjectInInstrumentsAtIndex:i withObject:insObj];
	}
	
	return YES;
}
#pragma mark KVO/KVC helpers

+ (NSSet*)keyPathsForValuesAffectingInternalMadMusicStruct
{
	return [NSSet setWithObjects:@"title", @"information", @"usesLinearPitcchTable", @"limitPitchToMODTable", @"showsCopyright", @"newPitch", @"newSpeed", @"generalPitch", @"generalSpeed", @"generalVolume", @"instruments", @"patterns", @"buses", nil];
}

+ (NSSet*)keyPathsForValuesAffectingSDatas
{
	return [NSSet setWithObject:@"instruments"];
}

- (void)addInstrumentObject:(PPInstrumentObject *)object
{
	NSInteger indexOfAddedInstrument = -1;
	for (NSInteger i = 0; i > MAXINSTRU; i++) {
		if ([_instruments[i] isBlankInstrument]) {
			indexOfAddedInstrument = i;
			break;
		}
	}
	if (indexOfAddedInstrument == -1) {
		return;
	}
	NSIndexSet *ourIdxSet = [NSIndexSet indexSetWithIndex:indexOfAddedInstrument];
	[self willChange:NSKeyValueChangeReplacement valuesAtIndexes:ourIdxSet forKey:@"instruments"];
	_instruments[indexOfAddedInstrument] = [object copy];
	_instruments[indexOfAddedInstrument].number = indexOfAddedInstrument * MAXSAMPLE;
	[self didChange:NSKeyValueChangeReplacement valuesAtIndexes:ourIdxSet forKey:@"instruments"];
}

- (void)replaceObjectInInstrumentsAtIndex:(NSInteger)index withObject:(PPInstrumentObject *)object
{
	NSParameterAssert(index < MAXINSTRU);
	_instruments[index] = object;
}

- (NSInteger)countOfInstruments
{
	return [_instruments count];
}

- (PPInstrumentObject*)instrumentsObjectAtIndex:(NSInteger)idx
{
	NSParameterAssert(idx < MAXINSTRU);
	return _instruments[idx];
}

- (PPInstrumentObject*)instrumentObjectAtIndex:(NSInteger)idx
{
	return [self instrumentsObjectAtIndex:idx];
}

- (void)clearInstrumentsAtIndexes:(NSIndexSet *)indexes
{
	[self willChange:NSKeyValueChangeReplacement valuesAtIndexes:indexes forKey:@"instruments"];
	NSUInteger currentIndex = [indexes firstIndex];
	while (currentIndex != NSNotFound) {
		[(PPInstrumentObject*)_instruments[currentIndex] resetInstrument];
		currentIndex = [indexes indexGreaterThanIndex:currentIndex];
	}

	[self didChange:NSKeyValueChangeReplacement valuesAtIndexes:indexes forKey:@"instruments"];
}

- (void)clearInstrumentObjectAtIndex:(NSInteger)index
{
	[self clearInstrumentsAtIndexes:[NSIndexSet indexSetWithIndex:index]];
}

- (NSInteger)countOfPatterns
{
	return [_patterns count];
}

- (NSInteger)lengthOfPartitions
{
	return currentMusic->header->numPointers;
}

- (NSInteger)totalTracks
{
	return currentMusic->header->numChn;
}

@end
