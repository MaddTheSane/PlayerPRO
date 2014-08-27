//
//  PPMusicObject.m
//  PPMacho
//
//  Created by C.W. Betts on 12/1/12.
//
//

#import "PPMusicObject.h"
#import "PPLibrary.h"
#import "PPLibrary_PPKPrivate.h"
#import "PPDriver.h"
#import "PPDriver_Private.h"
#import "PPMusicObject_PPKPrivate.h"
#include <PlayerPROCore/RDriverInt.h>
#include "PPByteswap.h"
#import "PPInstrumentObject.h"
#import "PPInstrumentObject_PPKPrivate.h"
#import "PPPatternObject.h"
#import "PPErrors.h"

static MADMusic *DeepCopyMusic(MADMusic* oldMus)
{
	if (!oldMus) {
		return NULL;
	}
	MADMusic *toreturn = calloc(sizeof(MADMusic), 1);
	memcpy(toreturn, oldMus, sizeof(MADMusic));
	
	return toreturn;
}

@interface PPMusicObject ()
{
	@package
	MADMusic *currentMusic;
	NSString *internalFileName;
	NSString *madInfo;
	NSDictionary* madClasses;
}
@property (readwrite, strong) NSURL *filePath;
@property (readwrite, strong, nonatomic) NSMutableArray *instruments;
@property (readwrite, strong, nonatomic) NSMutableArray *patterns;
@property (readwrite, strong, nonatomic) NSMutableArray *buses;
@end

@implementation PPMusicObject
@synthesize madInformation = madInfo;
@synthesize attachedDriver;
@synthesize _currentMusic = currentMusic;
@synthesize internalFileName;

- (MADErr)exportInstrumentListToURL:(NSURL*)outURL
{
	NSMutableData *outData = [[NSMutableData alloc] init];
	if (!outData) {
		return MADNeedMemory;
	}
	
	int i, x;
	{
		__block InstrData *tempInstrData = calloc(sizeof(InstrData), MAXINSTRU);
		if (!tempInstrData) {
			return MADNeedMemory;
		}
		memcpy(tempInstrData, [self internalMadMusicStruct]->fid, sizeof(InstrData) * MAXINSTRU);
		
		dispatch_apply(MAXINSTRU, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT , 0), ^(size_t x) {
			ByteSwapInstrData(&tempInstrData[x]);
		});
		[outData appendBytes:tempInstrData length:sizeof(InstrData) * MAXINSTRU];
		free(tempInstrData);
	}
	
	for (i = 0; i < MAXINSTRU ; i++) {
		for (x = 0; x < [self internalMadMusicStruct]->fid[i].numSamples ; x++) {
			sData tempData, *curData = [self internalMadMusicStruct]->sample[i * MAXSAMPLE +  x];
			sData32 writeData;
			memcpy(&tempData, curData, sizeof(sData));
			ByteSwapsData(&tempData);
			memcpy(&writeData, &tempData, sizeof(sData32));
			writeData.data = 0;
			[outData appendBytes:&writeData length:sizeof(sData32)];
#ifdef __LITTLE_ENDIAN__
			{
				Ptr dataData = malloc(curData->size);
				if (!dataData)
					return MADNeedMemory;
				
				memcpy(dataData, curData->data, curData->size);
				if (curData->amp == 16) {
					__block short *shortPtr = (short*) dataData;
					
					dispatch_apply(curData->size / 2, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT , 0), ^(size_t y) {
						MADBE16(&shortPtr[y]);
					});
				}
				[outData appendBytes:dataData length:curData->size];
				free(dataData);
			}
#else
			[outData appendBytes:curData->data length:curData->size]
#endif
		}
	}
	BOOL successful = [outData writeToURL:outURL atomically:YES];
	return successful ? MADNoErr : MADWritingErr;
}

- (NSMutableArray *)instruments
{
	if (!_instruments) {
		NSMutableArray *array = [NSMutableArray new];
		for (NSInteger i = 0; i < MAXINSTRU; i++) {
			PPInstrumentObject *immIns = [[PPInstrumentObject alloc] initWithMusicStruct:currentMusic atIndex:i];
			[array addObject:immIns];
		}
		self.instruments = array;
	}
	
	return _instruments;
}

- (NSDictionary*)musicClasses
{
	if (!madClasses) {
		NSMutableDictionary *madMutClasses = [[NSMutableDictionary alloc] init];
		madMutClasses[@"PlayerPRO Instruments"] = [self instruments];
		
		madClasses = [[NSDictionary alloc] initWithDictionary:madMutClasses];
	}
	return madClasses;
}

+ (MADErr)info:(MADInfoRec*)theInfo fromTrackerAtURL:(NSURL*)thURL usingLibrary:(PPLibrary*)theLib
{
	char filetype[5];
	MADErr theErr = MADNoErr;
	if (!theInfo || !thURL || !theLib) {
		return MADParametersErr;
	}
	
	if ((theErr = MADMusicIdentifyCFURL(theLib._madLib, filetype, (__bridge CFURLRef)(thURL))) != MADNoErr)
		return theErr;
	
	return MADMusicInfoCFURL(theLib._madLib, filetype, (__bridge CFURLRef)(thURL), theInfo);
}

- (NSString *)internalFileName
{
	if (!internalFileName) {
		internalFileName = [[NSString alloc] initWithCString:currentMusic->header->name encoding:NSMacOSRomanStringEncoding];
	}
	return internalFileName;
}

- (void)setInternalFileName:(NSString*)newInfo
{
	internalFileName = [newInfo copy];
	NSData *outMacRoman = [internalFileName dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
	if (!outMacRoman || outMacRoman.length == 0) {
		memset(currentMusic->header->infos, 0, sizeof(currentMusic->header->infos));
	} else {
		char fileNameInt[32] = {0};
		[outMacRoman getBytes:fileNameInt length:MIN(outMacRoman.length, sizeof(fileNameInt) - 1)];
		strlcpy(currentMusic->header->name, fileNameInt, sizeof(currentMusic->header->name));
	}
}

- (void)setMadInformation:(NSString*)newInfo
{
	madInfo = [newInfo copy];
	NSData *outMacRoman = [madInfo dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
	if (!outMacRoman || outMacRoman.length == 0) {
		memset(currentMusic->header->infos, 0, sizeof(currentMusic->header->infos));
	} else {
		char fileNameInt[239] = {0};
		[outMacRoman getBytes:fileNameInt length:MIN(outMacRoman.length, sizeof(fileNameInt) - 1)];
		strlcpy(currentMusic->header->infos, fileNameInt, sizeof(currentMusic->header->infos));
	}
}

- (NSString*)madInformation
{
	if (!madInfo) {
		madInfo = [[NSString alloc] initWithCString:currentMusic->header->infos encoding:NSMacOSRomanStringEncoding];
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

- (instancetype)initWithURL:(NSURL *)url
{
	if (self = [super init]) {
		if (MADLoadMusicCFURLFile(NULL, &currentMusic, "MADK", (__bridge CFURLRef)url) != MADNoErr)
			return nil;
		
		self.filePath = url;
	}
	
	return self;
}

- (instancetype)initWithPath:(NSString *)url
{
	return self = [self initWithURL:[NSURL fileURLWithPath:url]];
}

- (instancetype)initWithURL:(NSURL *)url library:(PPLibrary *)theLib
{
	if (self = [super init]) {
		char type[5];
		CFURLRef tmpURL = (__bridge CFURLRef)(url);
		if (MADMusicIdentifyCFURL(theLib._madLib, type, tmpURL) != MADNoErr) {
			return nil;
		}
		if (MADLoadMusicCFURLFile(theLib._madLib, &currentMusic, type, tmpURL) != MADNoErr) {
			return nil;
		}
		self.filePath = url;
	}
	return self;
}

- (instancetype)initWithPath:(NSString *)url library:(PPLibrary *)theLib
{
	return [self initWithURL:[NSURL fileURLWithPath:url] library:theLib];
}

- (instancetype)init
{
	return self = [self initWithMusicStruct:CreateFreeMADK() copy:NO];
}

- (instancetype)initWithURL:(NSURL *)url driver:(PPDriver *)theLib
{
	if (self = [self initWithURL:url library:theLib.theLibrary]) {
		[self attachToDriver:theLib];
	}
	return nil;
}

- (instancetype)initWithPath:(NSString *)url driver:(PPDriver *)theLib
{
	return [self initWithURL:[NSURL fileURLWithPath:url] driver:theLib];
}

- (void)attachToDriver:(PPDriver *)theDriv
{
	self.attachedDriver = theDriv;
	[theDriv setCurrentMusic:self];
}

- (void)dealloc
{
	if (currentMusic) {
		MADDisposeMusic(&currentMusic, NULL);
		currentMusic = NULL;
	}
}

- (MADErr)saveMusicToURL:(NSURL *)tosave
{
	return [self saveMusicToURL:tosave compress:NO];
}

- (MADErr)saveMusicToURL:(NSURL *)tosave compress:(BOOL)mad1Comp
{
	MADErr retErr;
	if ((retErr = MADMusicSaveCFURL(currentMusic, (__bridge CFURLRef)tosave, mad1Comp)) == MADNoErr) {
		currentMusic->hasChanged = false;
	}
	return retErr;
}

- (MADErr)exportMusicToURL:(NSURL *)tosave format:(NSString*)form library:(PPLibrary*)otherLib
{
	if ([form isEqualToString:@"MADK"]) {
		return [self saveMusicToURL:tosave];
	}
	return MADMusicExportCFURL(otherLib._madLib, self._currentMusic, (char*)[form cStringUsingEncoding:NSMacOSRomanStringEncoding], (__bridge CFURLRef)tosave);
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
	PPMusicObject *copyWrap = [[PPMusicObject alloc] init];
	if (self.filePath) {
		copyWrap.filePath = self.filePath;
	}
	
	copyWrap.madInformation = self.madInformation;
	copyWrap.internalFileName = internalFileName;
	copyWrap.instruments = [[NSMutableArray alloc] initWithArray:_instruments copyItems:YES];
	copyWrap.patterns = [[NSMutableArray alloc] initWithArray:_patterns copyItems:YES];
	return copyWrap;
}

#if 0
- (void)setUpObjCStructures
{
	int i;
	self.internalFileName = [NSString stringWithCString:currentMusic->header->name encoding:NSMacOSRomanStringEncoding];
	self.madInfo = [NSString stringWithCString:currentMusic->header->infos encoding:NSMacOSRomanStringEncoding];
	self.madAuthor = @"";
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

- (BOOL)addInstrument:(PPInstrumentObject*)theIns
{
	if ([[self instruments] count] >= MAXINSTRU) {
		return NO;
	}
	NSIndexSet *addIDXSet =[NSIndexSet indexSetWithIndex:[self.instruments count]];
	[self willChange:NSKeyValueChangeInsertion valuesAtIndexes:addIDXSet forKey:@"instruments"];
	theIns.number = [self.instruments count];
	[self.instruments addObject:theIns];
	[self didChange:NSKeyValueChangeInsertion valuesAtIndexes:addIDXSet forKey:@"instruments"];
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
	__block InstrData *tempInstrData;
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
		
		dispatch_apply(MAXINSTRU, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT , 0), ^(size_t x) {
			ByteSwapInstrData(&tempInstrData[x]);
		});
		filePos += inOutCount;
	}
	
	//Clean up old instruments
	
	sData **tmpsData = calloc(sizeof(sData*), MAXINSTRU * MAXSAMPLE);
	
	// **** INSTRUMENTS ***
	for (i = 0; i < MAXINSTRU ; i++) {
		for (x = 0; x < tempInstrData[i].numSamples ; x++) {
			sData	*curData;
			
			// ** Read Sample header **
			
			curData = tmpsData[i * MAXSAMPLE +  x] = (sData*)malloc(sizeof(sData));
			if (curData == NULL) {
				if (theErr) {
					*theErr = PPCreateErrorFromMADErrorType(MADNeedMemory);
				}
				free(tempInstrData);
				dispatch_apply(MAXSAMPLE * MAXINSTRU, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(size_t x) {
					if (tmpsData[x]) {
						if (tmpsData[x]->data) {
							free(tmpsData[x]->data);
						}
						free(tmpsData[x]);
					}
				});
				
				free(tmpsData);
				return NO;
			}
			
			inOutCount = sizeof(sData32);
			[fileData getBytes:curData range:NSMakeRange(filePos, inOutCount)];
			ByteSwapsData(curData);
			filePos += inOutCount;
			
			// ** Read Sample DATA
			
			curData->data = malloc(curData->size);
			if (curData->data == NULL)
			{
				if (theErr) {
					*theErr = PPCreateErrorFromMADErrorType(MADNeedMemory);
				}
				
				free(tempInstrData);
				dispatch_apply(MAXSAMPLE * MAXINSTRU, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(size_t x) {
					if (tmpsData[x]) {
						if (tmpsData[x]->data) {
							free(tmpsData[x]->data);
						}
						free(tmpsData[x]);
					}
				});
				
				free(tmpsData);
				return NO;
			}
			
			inOutCount = curData->size;
			[fileData getBytes:curData->data range:NSMakeRange(filePos, inOutCount)];
			filePos += inOutCount;
			if (curData->amp == 16) {
				__block short *shortPtr = (short*)curData->data;
				
				dispatch_apply(inOutCount / 2, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT , 0), ^(size_t y) {
					MADBE16(&shortPtr[y]);
				});
			}
		}
	}
	// *********************
	
	if (theErr) {
		*theErr = nil;
	}
	
	for (x = 0; x < MAXINSTRU ; x++) MADKillInstrument(currentMusic, x);
	memcpy(currentMusic->fid, tempInstrData, inOutCount);
	free(tempInstrData);
	
	dispatch_apply(MAXSAMPLE * MAXINSTRU, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(size_t x) {
		if (currentMusic->sample[x]) {
			if (currentMusic->sample[x]->data) {
				free(currentMusic->sample[x]->data);
			}
			free(currentMusic->sample[x]);
		}
	});
	free(currentMusic->sample);
	currentMusic->sample = tmpsData;
	for (i = 0; i < MAXINSTRU; i++) {
		PPInstrumentObject *insObj = [[PPInstrumentObject alloc] initWithMusic:self instrumentIndex:i];
		(self.instruments)[i] = insObj;
	}
	
	return YES;
}

@end
