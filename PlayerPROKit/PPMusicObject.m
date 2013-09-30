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

@interface PPMusicObject ()
{
	@package
	MADMusic *currentMusic;
	NSString *internalFileName;
	NSString *madInfo;
}
@property (readwrite, strong, nonatomic) NSString *internalFileName;
@property (readwrite, strong, nonatomic) NSString *madInfo;
@property (readwrite, strong) NSURL *filePath;
@end

@implementation PPMusicObject
@synthesize madInfo;
@synthesize attachedDriver;
@synthesize _currentMusic = currentMusic;
@synthesize internalFileName;

+ (OSErr)info:(PPInfoRec*)theInfo fromTrackerAtURL:(NSURL*)thURL usingLibrary:(PPLibrary*)theLib
{
	char filetype[5];
	CFURLRef tmpCFURL;
	OSErr theErr = noErr;
	if (!theInfo || !thURL || !theLib) {
		return MADParametersErr;
	}
	
	tmpCFURL = CFBridgingRetain(thURL);
	
	if ((theErr = MADMusicIdentifyCFURL(theLib._madLib, filetype, tmpCFURL)) != noErr)
		goto end;
	
	theErr = MADMusicInfoCFURL(theLib._madLib, filetype, tmpCFURL, theInfo);
	
end:
	CFRelease(tmpCFURL);
	return theErr;
}

- (NSString *)internalFileName
{
	if (!internalFileName) {
		self.internalFileName = [[NSString alloc] initWithCString:currentMusic->header->name encoding:NSMacOSRomanStringEncoding];
	}
	return internalFileName;
}

- (NSString*)madInfo
{
	if (!madInfo) {
		self.madInfo = [[NSString alloc] initWithCString:currentMusic->header->infos encoding:NSMacOSRomanStringEncoding];
	}
	return madInfo;
}

- (id)initWithURL:(NSURL *)url
{
	return [self initWithPath:[url path]];
}

- (id)initWithPath:(NSString *)url
{
	if (self = [super init]) {
		if (MADLoadMADFileCString(&currentMusic, (char*)[url fileSystemRepresentation]) != noErr)
			return nil;
		self.filePath = [NSURL fileURLWithPath:url];
	}
	return self;
}

- (id)initWithURL:(NSURL *)url library:(PPLibrary *)theLib
{
	if ([[url pathExtension] caseInsensitiveCompare:@"madbundle"]) {
		return self = [[PPMusicObjectWrapper alloc] initWithURL:url];
	}
	if (self = [super init]) {
		char type[5];
		CFURLRef tmpURL = CFBridgingRetain(url);
		if (MADMusicIdentifyCFURL(theLib._madLib, type, tmpURL) != noErr) {
			CFRelease(tmpURL);
			return nil;
		}
		if (strcmp(type, "MADK") == 0) {
			CFRelease(tmpURL);
			return self = [[PPMusicObject alloc] initWithURL:url];
		}
		if (MADLoadMusicCFURLFile(theLib._madLib, &currentMusic, type, tmpURL) != noErr) {
			CFRelease(tmpURL);
			return nil;
		}
		CFRelease(tmpURL);
		self.filePath = url;
	}
	return self;
}

- (id)initWithPath:(NSString *)url library:(PPLibrary *)theLib
{
	return [self initWithURL:[NSURL fileURLWithPath:url] library:theLib];
}

- (id)init
{
	if (self = [super init]) {
		currentMusic = CreateFreeMADK();
	}
	return self;
}

- (id)initWithURL:(NSURL *)url driver:(PPDriver *)theLib
{
	if (self = [self initWithURL:url library:theLib.theLibrary]) {
		self.attachedDriver = theLib;
		[theLib setCurrentMusic:self];
	}
	return nil;
}

- (id)initWithPath:(NSString *)url driver:(PPDriver *)theLib
{
	return [self initWithPath:url library:theLib.theLibrary];
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
	}
}

- (OSErr)saveMusicToURL:(NSURL *)tosave
{
	//TODO: error-checking
	int i, x;
	size_t inOutCount;
	MADCleanCurrentMusic(currentMusic, attachedDriver ? attachedDriver.rec : NULL);
	MADMusic *music = currentMusic;
	NSMutableData *saveData = [[NSMutableData alloc] initWithCapacity:MADGetMusicSize(currentMusic)];
	for( i = 0, x = 0; i < MAXINSTRU; i++)
	{
		music->fid[ i].no = i;
		
		if( music->fid[ i].numSamples > 0 || music->fid[ i].name[ 0] != 0)	// Is there something in this instrument?
		{
			x++;
		}
	}
	
	music->header->numInstru = x;
	{
		MADSpec aHeader;
		aHeader = *music->header;
		ByteSwapMADSpec(&aHeader);
		[saveData appendBytes:&aHeader length:sizeof(MADSpec)];
	}
	{
		BOOL compressMAD = NO /*[[NSUserDefaults standardUserDefaults] boolForKey:PPMMadCompression]*/;
		if (compressMAD) {
			for( i = 0; i < music->header->numPat ; i++)
			{
				if (music->partition[i]) {
					PatData *tmpPat = CompressPartitionMAD1(music, music->partition[i]);
					inOutCount = tmpPat->header.patBytes + sizeof(PatHeader);
					tmpPat->header.compMode = 'MAD1';
					ByteSwapPatHeader(&tmpPat->header);
					[saveData appendBytes:tmpPat length:inOutCount];
					free(tmpPat);
				}
			}
		} else {
			for (i = 0; i < music->header->numPat; i++) {
				if (music->partition[i]) {
					inOutCount = sizeof( PatHeader);
					inOutCount += music->header->numChn * music->partition[ i]->header.size * sizeof( Cmd);
					PatData *tmpPat = calloc(inOutCount, 1);
					memcpy(tmpPat, music->partition[i], inOutCount);
					ByteSwapPatHeader(&tmpPat->header);
					[saveData appendBytes:tmpPat length:inOutCount];
					free(tmpPat);
				}
			}
		}
	}
	
	for( i = 0; i < MAXINSTRU; i++)
	{
		if( music->fid[ i].numSamples > 0 || music->fid[ i].name[ 0] != 0)	// Is there something in this instrument?
		{
			music->fid[ i].no = i;
			InstrData instData = music->fid[i];
			ByteSwapInstrData(&instData);
			[saveData appendBytes:&instData length:sizeof( InstrData)];
		}
	}
	
	for( i = 0; i < MAXINSTRU ; i++)
	{
		for( x = 0; x < music->fid[i].numSamples; x++)
		{
			sData	curData;
			sData32	copyData;
			curData = *music->sample[ music->fid[i].firstSample + x];
			
			inOutCount = sizeof( sData32);
			ByteSwapsData(&curData);
			memcpy(&copyData, &curData, inOutCount);
			copyData.data = 0;
			[saveData appendBytes:&copyData length:inOutCount];
			
			inOutCount = music->sample[ music->fid[i].firstSample + x]->size;
			Ptr dataCopy = malloc(inOutCount);
			memcpy(dataCopy, curData.data, inOutCount);
			if( curData.amp == 16)
			{
				__block short	*shortPtr = (short*) dataCopy;
				
				dispatch_apply(inOutCount / 2, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT , 0), ^(size_t y) {
					PPBE16(&shortPtr[y]);
				});
				
			}
			
			[saveData appendBytes:dataCopy length:inOutCount];
			free(dataCopy);
		}
	}
	
	// EFFECTS *** *** *** *** *** *** *** *** *** *** *** ***
	
	int alpha = 0;
	for( i = 0; i < 10 ; i++)	// Global Effects
	{
		if( music->header->globalEffect[ i])
		{
			inOutCount = sizeof( FXSets);
			__block FXSets aSet = music->sets[alpha];
			PPBE16(&aSet.id);
			PPBE16(&aSet.noArg);
			PPBE16(&aSet.track);
			PPBE32(&aSet.FXID);
			dispatch_apply(100, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(size_t y) {
				PPBE32(&aSet.values[y]);
			});
			
			[saveData appendBytes:&aSet length:inOutCount];
			alpha++;
		}
	}
	
	for( i = 0; i < music->header->numChn ; i++)	// Channel Effects
	{
		for( x = 0; x < 4; x++)
		{
			if( music->header->chanEffect[ i][ x])
			{
				inOutCount = sizeof( FXSets);
				__block FXSets aSet = music->sets[alpha];
				PPBE16(&aSet.id);
				PPBE16(&aSet.noArg);
				PPBE16(&aSet.track);
				PPBE32(&aSet.FXID);
				dispatch_apply(100, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(size_t y) {
					PPBE32(&aSet.values[y]);
				});
				
				[saveData appendBytes:&aSet length:inOutCount];
				alpha++;
			}
		}
	}
	
	[saveData writeToURL:tosave atomically:YES];
	
	music->header->numInstru = MAXINSTRU;
	
	music->hasChanged = FALSE;
	return noErr;
}

- (OSErr)exportMusicToURL:(NSURL *)tosave format:(NSString*)form library:(PPLibrary*)otherLib
{
	if ([form isEqualToString:@"MADK"]) {
		return [self saveMusicToURL:tosave];
	}
	return MADMusicExportCFURL(otherLib._madLib, self._currentMusic, (char*)[form cStringUsingEncoding:NSMacOSRomanStringEncoding], (__bridge CFURLRef)tosave);
}

@end

@interface PPMusicObjectWrapper ()
@property (readwrite) OSType madType;
@property (strong, readwrite) NSFileWrapper *musicWrapper;
- (void)syncMusicDataTypes;
@end

@implementation PPMusicObjectWrapper

+ (PPInfoRec)infoFromTrackerAtURL:(NSURL*)thURL
{
	PPMusicObjectWrapper *tmpVal = [[self alloc] initWithURL:thURL];
	PPInfoRec toReturn = {0};
	strcpy(toReturn.formatDescription, "MAD Bundle");
	NSData *nameData = [[tmpVal internalFileName] dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
	[nameData getBytes:toReturn.internalFileName length:MIN(nameData.length, (sizeof(toReturn.internalFileName) - 1))];
	toReturn.signature = tmpVal.madType;
	toReturn.totalInstruments = tmpVal.totalInstruments;
	toReturn.partitionLength = tmpVal.partitionLength;
	toReturn.totalPatterns = tmpVal.totalPatterns;
	toReturn.totalTracks = tmpVal.totalTracks;
	toReturn.fileSize = [[tmpVal.musicWrapper fileAttributes][NSFileSize] longValue];
	
	return toReturn;
}

+ (OSErr)info:(PPInfoRec*)theInfo fromTrackerAtURL:(NSURL*)thURL usingLibrary:(PPLibrary*)theLib
{
	if (!theInfo || !thURL || !theLib) {
		return MADParametersErr;
	}
	if ([[thURL pathExtension] caseInsensitiveCompare:@"madbundle"] == NSOrderedSame) {
		*theInfo = [self infoFromTrackerAtURL:thURL];
		return noErr;
	} else {
		return [super info:theInfo fromTrackerAtURL:thURL usingLibrary:theLib];
	}
}

@synthesize madType;

- (id)copyWithZone:(NSZone *)zone
{
	[self syncMusicDataTypes];
	return [[[self class] alloc] initFromMusicObject:self];
}

- (NSString*)internalFileName
{
	return internalFileName;
}

- (void)setInternalFileName:(NSString *)_internalFileName
{
	[self willChangeValueForKey:@"internalFileName"];
	internalFileName = [_internalFileName copy];
	[self didChangeValueForKey:@"internalFileName"];
}

- (NSString*)madInfo
{
	return madInfo;
}

- (void)setMadInfo:(NSString *)_madInfo
{
	[self willChangeValueForKey:@"madInfo"];
	madInfo = [_madInfo copy];
	[self didChangeValueForKey:@"madInfo"];
}

#define kMADMusicName @"Mad Name"
#define kMADMusicInfo @"Mad Info"
#define kMADMusicType @"Mad Type"

- (void)setUpObjCStructures
{
	self.internalFileName = [NSString stringWithCString:currentMusic->header->name encoding:NSMacOSRomanStringEncoding];
	self.madInfo = [NSString stringWithCString:currentMusic->header->infos encoding:NSMacOSRomanStringEncoding];
	self.madAuthor = @"";
	
	
}

- (id)init
{
	if (self = [super init]) {
		[self setUpObjCStructures];
	}
	return self;
}

static MADMusic *DeepCopyMusic(MADMusic* oldMus)
{
	MADMusic *toreturn = calloc(sizeof(MADMusic), 1);
	
	
	return toreturn;
}

- (id)initFromMusicObject:(PPMusicObject*)oldFormat
{
	if (self = [super init]) {
		MADDisposeMusic(&currentMusic, NULL);
		currentMusic = DeepCopyMusic(oldFormat._currentMusic);
		[self setUpObjCStructures];
	}
	return self;
}

- (void)syncMusicDataTypes
{
	
}

- (id)initWithPath:(NSString *)url
{
	return [self initWithURL:[NSURL fileURLWithPath:url]];
}

- (id)initWithURL:(NSURL *)url
{
	if (self = [self initWithFileWrapper:[[NSFileWrapper alloc] initWithURL:url options:NSFileWrapperReadingImmediate error:NULL]]) {
		self.filePath = url;
	}
	return self;
}

- (id)initWithFileWrapper:(NSFileWrapper*)wrapper
{
	if (self = [super init]) {
		self.musicWrapper = wrapper;
		NSDictionary *stuff = [wrapper fileWrappers];
		stuff = nil;
		return nil;
		
	}
	
	return self;
}

- (id)initWithURL:(NSURL *)url library:(PPLibrary *)theLib
{
	if ([[url pathExtension] caseInsensitiveCompare:@"madbundle"]) {
		return self = [self initWithURL:url];
	}
	PPMusicObject *tmpObj = [[PPMusicObject alloc] initWithURL:url library:theLib];
	return self = [self initFromMusicObject:tmpObj];
}

- (OSErr)exportMusicToURL:(NSURL *)tosave format:(NSString*)form library:(PPLibrary*)otherLib
{
	if ([form isEqualToString:@"MADK"]) {
		return [self exportMusicToURL:tosave];
	}
	[self syncMusicDataTypes];
	return [super exportMusicToURL:tosave format:form library:otherLib];
}

- (OSErr)exportMusicToURL:(NSURL *)tosave
{
	[self syncMusicDataTypes];
	
	return [super saveMusicToURL:tosave];
}

- (OSErr)saveMusicToURL:(NSURL *)tosave
{
	if (![self.musicWrapper writeToURL:tosave options:NSFileWrapperWritingWithNameUpdating originalContentsURL:self.filePath error:NULL]) {
		return MADWritingErr;
	} else {
		return noErr;
	}
}

- (MADMusic *)newMadMusicStruct
{
	[self syncMusicDataTypes];
	return DeepCopyMusic(self._currentMusic);
}

@end
