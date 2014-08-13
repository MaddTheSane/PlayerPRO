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
@property (readwrite, copy, nonatomic) NSString *internalFileName;
@property (readwrite, copy, nonatomic) NSString *madInfo;
@property (readwrite, strong) NSURL *filePath;
@property (strong) NSArray *__instruments;
@end

@implementation PPMusicObject
@synthesize madInfo;
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
						PPBE16(&shortPtr[y]);
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

- (NSArray *)instruments
{
	if (!___instruments) {
		NSMutableArray *array = [NSMutableArray new];
		for (NSInteger i = 0; i < MAXINSTRU; i++) {
			PPInstrumentObject *immIns = [[PPInstrumentObject alloc] initWithMusicStruct:currentMusic atIndex:i];
			[array addObject:immIns];
		}
		self.__instruments = [[NSArray alloc] initWithArray:array];
	}
	
	return ___instruments;
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

+ (MADErr)info:(PPInfoRec*)theInfo fromTrackerAtURL:(NSURL*)thURL usingLibrary:(PPLibrary*)theLib
{
	char filetype[5];
	CFURLRef tmpCFURL;
	MADErr theErr = MADNoErr;
	if (!theInfo || !thURL || !theLib) {
		return MADParametersErr;
	}
	
	if ([[thURL pathExtension] caseInsensitiveCompare:@"madbundle"] == NSOrderedSame) {
		return [PPMusicObjectWrapper info:theInfo fromTrackerAtURL:thURL];
	}

	tmpCFURL = CFBridgingRetain(thURL);
	
	if ((theErr = MADMusicIdentifyCFURL(theLib._madLib, filetype, tmpCFURL)) != MADNoErr)
		goto end;
	
	theErr = MADMusicInfoCFURL(theLib._madLib, filetype, tmpCFURL, theInfo);
	
end:
	CFRelease(tmpCFURL);
	return theErr;
}

- (NSString *)internalFileName
{
	if (!internalFileName) {
		internalFileName = [[NSString alloc] initWithCString:currentMusic->header->name encoding:NSMacOSRomanStringEncoding];
	}
	return internalFileName;
}

- (NSString*)madInfo
{
	if (!madInfo) {
		madInfo = [[NSString alloc] initWithCString:currentMusic->header->infos encoding:NSMacOSRomanStringEncoding];
	}
	return madInfo;
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
	if ([[url pathExtension] caseInsensitiveCompare:@"madbundle"] == NSOrderedSame) {
		return self = [[PPMusicObjectWrapper alloc] initWithURL:url];
	}
	if (self = [super init]) {
		char type[5];
		CFURLRef tmpURL = CFBridgingRetain(url);
		if (MADMusicIdentifyCFURL(theLib._madLib, type, tmpURL) != MADNoErr) {
			CFRelease(tmpURL);
			return nil;
		}
		if (MADLoadMusicCFURLFile(theLib._madLib, &currentMusic, type, tmpURL) != MADNoErr) {
			CFRelease(tmpURL);
			return nil;
		}
		CFRelease(tmpURL);
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
	if (self = [super init]) {
		currentMusic = CreateFreeMADK();
	}
	return self;
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

- (instancetype)initWithoutCreatingMusicStruct
{
	return self = [super init];
}

@end

@interface PPMusicObjectWrapper ()
#if 0
{
	Byte		generalPan;					// General Panning
	Byte		MultiChanNo;				// Number of chan for multichannel
	Byte		MultiChan;					// MultiChannel per tracks?
	int			EPitch;						// New Pitch
	int			ESpeed;						// New Speed
	Byte		XMLinear;					// Linear picth table?
	Byte		MODMode;					// Limit pitch to MOD pitch table
	Byte		showCopyright;				// Show infos at startup? true or false
	Byte		generalPitch;				// General Pitch
	Byte		generalSpeed;				// General Speed
	Byte		generalVol;					// Software general volume
	Byte		numPat;						// Patterns number
	Byte		numChn;						// Channels number
	Byte 		numPointers;				// Partition length
	Byte		numInstru;					// Instruments number
	Byte		numSamples;					// Samples number
	Byte		oPointers[MAXPOINTER];		// Partition : Patterns ID List
	short		speed;						// Default speed
	short		tempo;						// Default tempo
	Byte		chanPan[MAXTRACK];			// Channel settings, from 0 to 256
	Byte		chanVol[MAXTRACK];			// Channel Volume, from 0 to 64
	
	int			globalEffect[10];			// Global Effects IDs
	BOOL		globalFXActive;				// Global FX Active?
	
	int			chanEffect[MAXTRACK][4];	// Channel Effect IDs
	FXBus		chanBus[MAXTRACK];
}
#endif
@property (readwrite) OSType madType;
@property (readwrite, strong) NSFileWrapper *musicWrapper;
@property (readwrite, strong) NSMutableArray *instruments;
@property (readwrite, strong) NSMutableArray *patterns;
@property (readwrite, strong) NSMutableArray *buses;
- (void)syncMusicDataTypes;
@end

@implementation PPMusicObjectWrapper
@synthesize madType;

+ (MADErr)info:(PPInfoRec *)theInfo fromTrackerAtURL:(NSURL *)thURL
{
	MADErr theErr = MADNoErr;
	if (!theInfo || !thURL)
		return MADParametersErr;
	
	if (![[thURL pathExtension] isEqualToString:@"madbundle"])
		return MADIncompatibleFile;
	
	PPMusicObjectWrapper *tmpVal = [[PPMusicObjectWrapper alloc] initWithURL:thURL];

	if (!tmpVal)
		return MADReadingErr;
	
	strcpy(theInfo->formatDescription, "MAD Bundle");
	@try {
		NSData *nameData = [tmpVal.internalFileName dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
		if (!nameData || nameData.length == 0) {
			memset(theInfo->internalFileName, 0, sizeof(theInfo->internalFileName));
		} else {
			char fileNameInt[60] = {0};
			[nameData getBytes:fileNameInt length:MIN(nameData.length, sizeof(fileNameInt) - 1)];
			strlcpy(theInfo->internalFileName, fileNameInt, sizeof(theInfo->internalFileName));
		}
		theInfo->signature = tmpVal.madType;
		theInfo->totalInstruments = tmpVal.totalInstruments;
		theInfo->partitionLength = tmpVal.partitionLength;
		theInfo->totalPatterns = tmpVal.totalPatterns;
		theInfo->totalTracks = tmpVal.totalTracks;
		theInfo->fileSize = [[tmpVal.musicWrapper fileAttributes][NSFileSize] longValue];
	}
	@catch (NSException *exception) {
		memset(theInfo, 0, sizeof(PPInfoRec));
		theErr = MADIncompatibleFile;
	}
	@finally {
		return theErr;
	}
}

- (id)copyWithZone:(NSZone *)zone
{
	[self syncMusicDataTypes];
	PPMusicObjectWrapper *copyWrap = [[[self class] alloc] init];
	if (self.filePath) {
		copyWrap.filePath = self.filePath;
	}
	
	copyWrap.madInfo = madInfo;
	copyWrap.internalFileName = internalFileName;
	copyWrap.madAuthor = _madAuthor;
	copyWrap.madType = madType;
	copyWrap.instruments = [[NSMutableArray alloc] initWithArray:_instruments copyItems:YES];
	copyWrap.patterns = [[NSMutableArray alloc] initWithArray:_patterns copyItems:YES];
	[copyWrap syncMusicDataTypes];
	return copyWrap;
}

#define kMADMusicName @"Mad Name"
#define kMADMusicInfo @"Mad Info"
#define kMADMusicType @"Mad Type"
#define kMADMusicAuthor @"Mad Author"

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

- (NSArray*)sDatas
{
	NSMutableArray *tmpMutArray = [[NSMutableArray alloc] initWithCapacity:MAXINSTRU * MAXSAMPLE];
	for (PPInstrumentObject *theObj in self.instruments) {
		[tmpMutArray addObjectsFromArray:theObj.samples];
	}
	
	return [[NSArray alloc] initWithArray:tmpMutArray];
}

- (instancetype)init
{
	if (self = [super init]) {
		[self setUpObjCStructures];
	}
	return self;
}

- (instancetype)initFromMusicObject:(PPMusicObject*)oldFormat
{
	if (self = [super initWithoutCreatingMusicStruct]) {
		currentMusic = DeepCopyMusic(oldFormat._currentMusic);
		[self setUpObjCStructures];
	}
	return self;
}

- (void)syncMusicDataTypes
{
	
}

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

- (instancetype)initWithPath:(NSString *)url
{
	return self = [self initWithURL:[NSURL fileURLWithPath:url]];
}

- (instancetype)initWithURL:(NSURL *)url
{
	if (self = [self initWithFileWrapper:[[NSFileWrapper alloc] initWithURL:url options:NSFileWrapperReadingImmediate error:NULL]]) {
		self.filePath = url;
	}
	return self;
}

- (instancetype)initWithFileWrapper:(NSFileWrapper*)wrapper
{
	if (self = [super init]) {
		self.musicWrapper = wrapper;
		NSDictionary *stuff = [wrapper fileWrappers];
		
		
		
		stuff = nil;
		return nil;
	}
	
	return self;
}

- (instancetype)initWithURL:(NSURL *)url library:(PPLibrary *)theLib
{
	if ([[url pathExtension] caseInsensitiveCompare:@"madbundle"]) {
		return self = [self initWithURL:url];
	}
	PPMusicObject *tmpObj = [[PPMusicObject alloc] initWithURL:url library:theLib];
	return self = [self initFromMusicObject:tmpObj];
}

- (MADErr)exportMusicToURL:(NSURL *)tosave format:(NSString*)form library:(PPLibrary*)otherLib
{
	if ([form isEqualToString:@"MADK"]) {
		return [self exportMusicToURL:tosave];
	}
	[self syncMusicDataTypes];
	return [super exportMusicToURL:tosave format:form library:otherLib];
}

- (MADErr)exportMusicToURL:(NSURL *)tosave
{
	[self syncMusicDataTypes];
	
	return [super saveMusicToURL:tosave];
}

- (MADErr)saveMusicToURL:(NSURL *)tosave compress:(BOOL)mad1Comp
{
	return [self saveMusicToURL:tosave];
}

- (MADErr)saveMusicToURL:(NSURL *)tosave
{
	if (![self.musicWrapper writeToURL:tosave options:NSFileWrapperWritingWithNameUpdating originalContentsURL:self.filePath error:NULL]) {
		return MADWritingErr;
	} else {
		self.filePath = tosave;
		return MADNoErr;
	}
}

- (MADErr)createCopyMusicToURL:(NSURL *)tosave
{
	if (![self.musicWrapper writeToURL:tosave options:NSFileWrapperWritingWithNameUpdating originalContentsURL:self.filePath error:NULL]) {
		return MADWritingErr;
	} else {
		return MADNoErr;
	}
}

- (NSDictionary*)musicClasses
{
	return nil;
}

- (MADErr)exportInstrumentListToURL:(NSURL*)outURL
{
	[self syncMusicDataTypes];
	return [super exportInstrumentListToURL:outURL];
}

- (MADMusic *)copyMadMusicStruct
{
	[self syncMusicDataTypes];
	return [super copyMadMusicStruct];
}

- (MADMusic *)internalMadMusicStruct
{
	[self syncMusicDataTypes];
	return [super internalMadMusicStruct];
}

- (BOOL)importInstrumentListFromURL:(NSURL *)insURL error:(out NSError *__autoreleasing*)theErr
{
	NSData *fileData = [NSData dataWithContentsOfURL:insURL];
	if (!fileData) {
		if (theErr) {
			*theErr = CreateErrorFromMADErrorType(MADReadingErr);
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
				*theErr = CreateErrorFromMADErrorType(MADIncompatibleFile);
			}
			return NO;
		}
		
		// **** HEADER ***
		inOutCount = sizeof(InstrData) * MAXINSTRU;
		if ([fileData length] <= inOutCount) {
			if (theErr) {
				*theErr = CreateErrorFromMADErrorType(MADIncompatibleFile);
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
					*theErr = CreateErrorFromMADErrorType(MADNeedMemory);
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
					*theErr = CreateErrorFromMADErrorType(MADNeedMemory);
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
					PPBE16(&shortPtr[y]);
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
