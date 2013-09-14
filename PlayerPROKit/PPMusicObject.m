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
@property (readwrite, strong, nonatomic) NSString *internalFileName;
@end

@implementation PPMusicObject
@synthesize attachedDriver;
@synthesize _currentMusic = currentMusic;

- (NSString *)internalFileName
{
	
}

- (id)initWithURL:(NSURL *)url library:(PPLibrary *)theLib
{
	if (self = [super init]) {
		self.currentLibrary = theLib;
		char type[5];
		CFURLRef tmpURL = CFBridgingRetain(url);
		if (MADMusicIdentifyCFURL(theLib._madLib, type, tmpURL) != noErr) {
			CFRelease(tmpURL);
			return nil;
		}
		if (MADLoadMusicCFURLFile(theLib._madLib, &currentMusic, type, tmpURL) != noErr) {
			CFRelease(tmpURL);
			return nil;
		}
		
		CFRelease(tmpURL);
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

- (id)initWithURL:(NSURL *)url driver:(PPDriver *)theLib setAsCurrentMusic:(BOOL)toSet
{
	if (self = [self initWithURL:url library:theLib.theLibrary]) {
		self.attachedDriver = theLib;
		if (toSet) {
			[attachedDriver setCurrentMusic:self];
		}
	}
	return nil;
}

- (id)initWithPath:(NSString *)url driver:(PPDriver *)theLib setAsCurrentMusic:(BOOL)toSet
{
	return [self initWithURL:[NSURL fileURLWithPath:url] driver:theLib setAsCurrentMusic:toSet];
}

- (id)initWithURL:(NSURL *)url driver:(PPDriver *)theLib
{
	return [self initWithURL:url driver:theLib setAsCurrentMusic:NO];
}

- (id)initWithPath:(NSString *)url driver:(PPDriver *)theLib
{
	return [self initWithPath:url driver:theLib setAsCurrentMusic:NO];
}

- (void)attachToDriver:(PPDriver *)theDriv
{
	[self attachToDriver:theDriv setAsCurrentMusic:NO];
}

- (void)attachToDriver:(PPDriver *)theDriv setAsCurrentMusic:(BOOL)toSet
{
	self.attachedDriver = theDriv;
	if (toSet) {
		[attachedDriver setCurrentMusic:self];
	}
}

- (void)dealloc
{
	if (currentMusic) {
		MADDisposeMusic(&currentMusic, attachedDriver ? attachedDriver.rec : NULL);
	}
}

- (BOOL)saveMusicToURL:(NSURL *)tosave
{
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
	return YES;
}


@end

@implementation PPMusicObjectWrapper

- (id)init
{
	if (self = [super init]) {
		
	}
	return self;
}

- (id)initFromMusicObject:(PPMusicObject*)oldFromat
{
	
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
	
}

- (BOOL)saveMusicToURL:(NSURL *)tosave
{
	[self syncMusicDataTypes];
	
	return [super saveMusicToURL:tosave];
}


@end
