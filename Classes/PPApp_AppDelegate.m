//
//  PPApp_AppDelegate.m
//  PPMacho
//
//  Created by C.W. Betts on 7/12/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "PPApp_AppDelegate.h"
#import "PPPreferences.h"
#import "PPMusicList.h"
#import "UserDefaultKeys.h"
#import "NSColor+PPPreferences.h"
#import "PPErrors.h"
#import "OpenPanelViewController.h"
#import "ARCBridge.h"
#import "PPInstrumentImporter.h"
#import "PPInstrumentImporterObject.h"
#import "PPInstrumentWindowController.h"
#import "PPPlugInInfo.h"
#import "PPPlugInInfoController.h"
#import "PPDigitalPlugInHandler.h"
#import "PPDigitalPlugInObject.h"
#import "PPFilterPlugHandler.h"
#import "PPFilterPlugObject.h"
#include <PlayerPROCore/RDriverInt.h>
#import <QTKit/QTKit.h>
#import <QTKit/QTExportSession.h>
#import <QTKit/QTExportOptions.h>

@interface PPCurrentlyPlayingIndex : NSObject
{
	NSInteger index;
	NSURL *playbackURL;
}

@property (readwrite) NSInteger index;
@property (readwrite, retain) NSURL *playbackURL;

- (void)movePlayingIndexToOtherIndex:(PPCurrentlyPlayingIndex *)othidx;

@end

@implementation PPCurrentlyPlayingIndex

- (void)movePlayingIndexToOtherIndex:(PPCurrentlyPlayingIndex *)othidx
{
	othidx.index = index;
	othidx.playbackURL = playbackURL;
}

@synthesize index;
@synthesize playbackURL;

- (NSString *)description
{
	return [NSString stringWithFormat:@"Index: %ld URL: %@ URL Path: %@", (long)index, playbackURL, [playbackURL path]];
}

#if !__has_feature(objc_arc)
- (void)dealloc
{
	[playbackURL release];
	
	[super dealloc];
}
#endif

@end

void CocoaDebugStr( short line, Ptr file, Ptr text)
{
	NSLog(@"%s:%u error text:%s!", file, line, text);
	NSInteger alert = NSRunAlertPanel(NSLocalizedString(@"MyDebugStr_Error", @"Error"),
									  [NSString stringWithFormat:NSLocalizedString(@"MyDebugStr_MainText", @"The Main text to display"), text],
									  NSLocalizedString(@"MyDebugStr_Quit", @"Quit"), NSLocalizedString(@"MyDebugStr_Continue", @"Continue"),
									  NSLocalizedString(@"MyDebugStr_Debug", @"Debug"));
	switch (alert) {
		case NSAlertAlternateReturn:
			break;
		case NSAlertOtherReturn:
			//Debugger();
			NSCAssert(NO, @"Chose to go to debugger.");
			break;
		case NSAlertDefaultReturn:
			NSLog(@"Choosing to fail!");
		default:
			abort();
			break;
	}
}

@interface PPApp_AppDelegate ()
- (void)selectCurrentlyPlayingMusic;
- (void)selectMusicAtIndex:(NSInteger)anIdx;
- (BOOL)loadMusicURL:(NSURL*)musicToLoad error:(out NSError *__autoreleasing*)theErr;
- (void)musicListContentsDidMove;
- (BOOL)musicListWillChange;
- (void)musicListDidChange;
@end

@implementation PPApp_AppDelegate

@synthesize paused;
@synthesize window;
@synthesize exportWindow;

- (BOOL)loadMusicFromCurrentlyPlayingIndexWithError:(out NSError *__autoreleasing*)theErr
{
	currentlyPlayingIndex.playbackURL = [musicList URLAtIndex:currentlyPlayingIndex.index];
	BOOL isGood = [self loadMusicURL:currentlyPlayingIndex.playbackURL error:theErr];
	[currentlyPlayingIndex movePlayingIndexToOtherIndex:previouslyPlayingIndex];
	return isGood;
}

- (void)addMusicToMusicList:(NSURL* )theURL loadIfPreferencesAllow:(BOOL)load
{
	[self willChangeValueForKey:kMusicListKVO];
	BOOL okayMusic = [musicList addMusicURL:theURL];
	[self didChangeValueForKey:kMusicListKVO];
	if (!okayMusic) {
		NSInteger similarMusicIndex = [musicList indexOfObjectSimilarToURL:theURL];
		if (similarMusicIndex == NSNotFound) {
			return;
		}
		if (load && [[NSUserDefaults standardUserDefaults] boolForKey:PPLoadMusicAtMusicLoad]) {
			currentlyPlayingIndex.index = similarMusicIndex;
			[self selectCurrentlyPlayingMusic];
			NSError *err = nil;
			if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err]) {
				NSAlert *errAlert = [NSAlert alertWithError:err];
				[errAlert runModal];
			}
		} else {
			[self selectMusicAtIndex:similarMusicIndex];
		}
	} else if (load && [[NSUserDefaults standardUserDefaults] boolForKey:PPLoadMusicAtMusicLoad]) {
		currentlyPlayingIndex.index = [musicList countOfMusicList] - 1;
		//currentlyPlayingIndex.playbackURL = [musicList URLAtIndex:currentlyPlayingIndex.index];
		[self selectCurrentlyPlayingMusic];
		NSError *err = nil;
		if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err]) {
			NSAlert *errAlert = [NSAlert alertWithError:err];
			[errAlert runModal];
		}
	} else {
		[self selectMusicAtIndex:[musicList countOfMusicList] - 1];
	}
}

- (void)addMusicToMusicList:(NSURL* )theURL
{
	[self addMusicToMusicList:theURL loadIfPreferencesAllow:YES];
}

- (void)MADDriverWithPreferences
{
	Boolean madWasReading = false;
	long fullTime = 0, curTime = 0;
	if (MADDriver) {
		madWasReading = MADIsPlayingMusic(MADDriver);
		MADStopMusic(MADDriver);
		MADStopDriver(MADDriver);
		if (madWasReading) {
			MADGetMusicStatus(MADDriver, &fullTime, &curTime);
		}
		MADDisposeDriver(MADDriver);
		MADDriver = NULL;
	}
	MADDriverSettings init;
	MADGetBestDriver(&init);
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	//TODO: Sanity Checking
	init.surround = [defaults boolForKey:PPSurroundToggle];
	init.outPutRate = [defaults integerForKey:PPSoundOutRate];
	init.outPutBits = [defaults integerForKey:PPSoundOutBits];
	if ([defaults boolForKey:PPOversamplingToggle]) {
		init.oversampling = [defaults integerForKey:PPOversamplingAmount];
	} else {
		init.oversampling = 1;
	}
	init.Reverb = [defaults boolForKey:PPReverbToggle];
	init.ReverbSize = [defaults integerForKey:PPReverbAmount];
	init.ReverbStrength = [defaults integerForKey:PPReverbStrength];
	if ([defaults boolForKey:PPStereoDelayToggle]) {
		init.MicroDelaySize = [defaults integerForKey:PPStereoDelayAmount];
	} else {
		init.MicroDelaySize = 0;
	}
	
	init.driverMode = [defaults integerForKey:PPSoundDriver];
	init.repeatMusic = FALSE;
	
	OSErr returnerr = MADCreateDriver(&init, MADLib, &MADDriver);
	[[NSNotificationCenter defaultCenter] postNotificationName:PPDriverDidChange object:self];
	if (returnerr != noErr) {
		NSError *err = CreateErrorFromMADErrorType(returnerr);
		NSAlert *alert = [NSAlert alertWithError:err];
		[alert runModal];
		RELEASEOBJ(err);
		return;
	}
	MADStartDriver(MADDriver);
	if (Music) {
		MADAttachDriverToMusic(MADDriver, Music, NULL);
		if (madWasReading) {
			MADSetMusicStatus(MADDriver, 0, fullTime, curTime);
			MADPlayMusic(MADDriver);
		}
	}
}

+ (void)initialize
{
	PPMusicList *tempList = [[PPMusicList alloc] init];
	
	[[NSUserDefaults standardUserDefaults] registerDefaults:[NSDictionary dictionaryWithObjectsAndKeys:
															 @YES, PPRememberMusicList,
															 @NO, PPLoadMusicAtListLoad,
															 @(PPStopPlaying), PPAfterPlayingMusic,
															 @YES, PPGotoStartupAfterPlaying,
															 @YES, PPSaveModList,
															 @NO, PPLoadMusicAtMusicLoad,
															 @NO, PPLoopMusicWhenDone,
															 
															 @16, PPSoundOutBits,
															 @44100, PPSoundOutRate,
															 @(CoreAudioDriver), PPSoundDriver,
															 @NO, PPStereoDelayToggle,
															 @NO, PPReverbToggle,
															 @NO, PPSurroundToggle,
															 @NO, PPOversamplingToggle,
															 @30, PPStereoDelayAmount,
															 @25, PPReverbAmount,
															 @30, PPReverbStrength,
															 @1, PPOversamplingAmount,
															 
															 @YES, PPDEShowInstrument,
															 @YES, PPDEShowNote,
															 @YES, PPDEShowEffect,
															 @YES, PPDEShowArgument,
															 @YES, PPDEShowVolume,
															 @YES, PPDEShowMarkers,
															 @0, PPDEMarkerOffsetPref,
															 @3, PPDEMarkerLoopPref,
															 [[NSColor yellowColor] PPencodeColor], PPDEMarkerColorPref,
															 @YES, PPDEMouseClickControlPref,
															 @NO, PPDEMouseClickShiftPref,
															 @NO, PPDEMouseClickCommandPref,
															 @NO, PPDEMouseClickOptionPref,
															 @YES, PPDELineHeightNormal,
															 @YES, PPDEMusicTraceOn,
															 @YES, PPDEPatternWrappingPartition,
															 @YES, PPDEDragAsPcmd,
															 
															 [[NSColor redColor] PPencodeColor], PPCColor1,
															 [[NSColor greenColor] PPencodeColor], PPCColor2,
															 [[NSColor blueColor] PPencodeColor], PPCColor3,
															 [[NSColor cyanColor] PPencodeColor], PPCColor4,
															 [[NSColor yellowColor] PPencodeColor], PPCColor5,
															 [[NSColor magentaColor] PPencodeColor], PPCColor6,
															 [[NSColor orangeColor] PPencodeColor], PPCColor7,
															 [[NSColor purpleColor] PPencodeColor], PPCColor8,
															 
															 @YES, PPBEMarkersEnabled,
															 @0, PPBEMarkersOffset,
															 @3, PPBEMarkersLoop,
															 @YES, PPBEOctaveMarkers,
															 @NO, PPBENotesProjection,
															 
															 @YES, PPMAddExtension,
															 @YES, PPMMadCompression,
															 @NO, PPMNoLoadMixerFromFiles,
															 @YES, PPMOscilloscopeDrawLines,
															 
															 @NO, PPCEShowNotesLen,
															 @YES, PPCEShowMarkers,
															 @0, PPCEMarkerOffset,
															 @3, PPCEMarkerLoop,
															 @4, PPCETempoNum,
															 @4, PPCETempoUnit,
															 @130, PPCETrackHeight,
															 
															 [NSKeyedArchiver archivedDataWithRootObject:tempList], PPMMusicList,
															 nil]];
	RELEASEOBJ(tempList);
}

- (IBAction)showMusicList:(id)sender
{
    [window makeKeyAndOrderFront:sender];
}

- (void)selectMusicAtIndex:(NSInteger)anIdx
{
	if (anIdx < 0 || anIdx >= [musicList countOfMusicList]) {
		NSBeep();
		return;
	}
	NSIndexSet *idx = [[NSIndexSet alloc] initWithIndex:anIdx];
	[tableView selectRowIndexes:idx byExtendingSelection:NO];
	[tableView scrollRowToVisible:anIdx];
	RELEASEOBJ(idx);
}

- (void)selectCurrentlyPlayingMusic
{
	if (currentlyPlayingIndex.index >= 0) {
		//currentlyPlayingIndex.playbackURL = [musicList URLAtIndex:currentlyPlayingIndex.index];
		[self selectMusicAtIndex:currentlyPlayingIndex.index];
	}
}

- (void)songIsDonePlaying
{
	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	switch ([userDefaults integerForKey:PPAfterPlayingMusic]) {
		case PPStopPlaying:
		default:
			MADStopMusic(MADDriver);
			MADCleanDriver(MADDriver);
			if ([userDefaults boolForKey:PPGotoStartupAfterPlaying]) {
				MADSetMusicStatus(MADDriver, 0, 100, 0);
			}
			self.paused = YES;
			break;
			
		case PPLoopMusic:
			MADSetMusicStatus(MADDriver, 0, 100, 0);
			MADPlayMusic(MADDriver);
			break;
			
		case PPLoadNext:
		{
			if ([musicList countOfMusicList] > ++currentlyPlayingIndex.index) {
				[self selectCurrentlyPlayingMusic];
				NSError *err = nil;
				if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err])
				{
					NSAlert *alert = [NSAlert alertWithError:err];
					[alert runModal];
				}
			} else {
				if ([userDefaults boolForKey:PPLoopMusicWhenDone]) {
					currentlyPlayingIndex.index = 0;
					[self selectCurrentlyPlayingMusic];
					NSError *err = nil;
					if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err])
					{
						NSAlert *alert = [NSAlert alertWithError:err];
						[alert runModal];
					}
				} else {
					MADStopMusic(MADDriver);
					MADCleanDriver(MADDriver);
					if ([userDefaults boolForKey:PPGotoStartupAfterPlaying]) {
						MADSetMusicStatus(MADDriver, 0, 100, 0);
					}
				}
			}
		}
			break;
			
		case PPLoadRandom:
		{
			currentlyPlayingIndex.index = random() % [musicList countOfMusicList];
			[self selectCurrentlyPlayingMusic];
			NSError *err = nil;
			if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err])
			{
				NSAlert *alert = [NSAlert alertWithError:err];
				[alert runModal];
			}
		}
			break;
	}
}

- (void)updateMusicStats:(NSTimer*)theTimer
{
	if (Music) {
		long fT, cT;
		MADGetMusicStatus(MADDriver, &fT, &cT);
		if (MADIsDonePlaying(MADDriver) && !self.paused && !MADIsExporting(MADDriver)) {
			[self songIsDonePlaying];
			MADGetMusicStatus(MADDriver, &fT, &cT);
		}
		[songPos setDoubleValue:cT];
		[songCurTime setIntegerValue:cT];
	}
}

static inline void ByteSwapMADSpec(MADSpec *toSwap)
{
	PPBE32( &toSwap->MAD);
	PPBE16( &toSwap->speed);
	PPBE16( &toSwap->tempo);
	PPBE32( &toSwap->EPitch);
	PPBE32( &toSwap->ESpeed);
}

static inline void ByteSwapPatHeader(PatHeader *toSwap)
{
	PPBE32( &toSwap->size);
	PPBE32( &toSwap->compMode);
	PPBE32( &toSwap->patBytes);
	PPBE32( &toSwap->unused2);
}

static inline void ByteSwapInstrData(InstrData *toSwap)
{
	PPBE16( &toSwap->numSamples);
	PPBE16( &toSwap->firstSample);
	PPBE16( &toSwap->volFade);
	
	PPBE16( &toSwap->MIDI);
	PPBE16( &toSwap->MIDIType);
	__block InstrData *blockHappy = toSwap;
	dispatch_apply(12, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT , 0), ^(size_t x) {
		PPBE16( &blockHappy->volEnv[ x].pos);
		PPBE16( &blockHappy->volEnv[ x].val);

		PPBE16( &blockHappy->pannEnv[ x].pos);
		PPBE16( &blockHappy->pannEnv[ x].val);
		
		PPBE16( &blockHappy->pitchEnv[ x].pos);
		PPBE16( &blockHappy->pitchEnv[ x].val);
	});
}

static inline void ByteSwapsData(sData *toSwap)
{
	PPBE32( &toSwap->size);
	PPBE32( &toSwap->loopBeg);
	PPBE32( &toSwap->loopSize);
	PPBE16( &toSwap->c2spd);
}

- (void)saveMusicToURL:(NSURL *)tosave
{
	//[instrumentController writeInstrumentsBackToMusic];
	int i, x;
	size_t inOutCount;
	MADCleanCurrentMusic(Music, MADDriver);
	NSMutableData *saveData = [[NSMutableData alloc] initWithCapacity:MADGetMusicSize(Music)];
	for( i = 0, x = 0; i < MAXINSTRU; i++)
	{
		Music->fid[ i].no = i;
		
		if( Music->fid[ i].numSamples > 0 || Music->fid[ i].name[ 0] != 0)	// Is there something in this instrument?
		{
			x++;
		}
	}

	Music->header->numInstru = x;
	{
		MADSpec aHeader;
		aHeader = *Music->header;
		ByteSwapMADSpec(&aHeader);
		[saveData appendBytes:&aHeader length:sizeof(MADSpec)];
	}
	{
		BOOL compressMAD = [[NSUserDefaults standardUserDefaults] boolForKey:PPMMadCompression];
		if (compressMAD) {
			for( i = 0; i < Music->header->numPat ; i++)
			{
				if (Music->partition[i]) {
					PatData *tmpPat = CompressPartitionMAD1(Music, Music->partition[i]);
					inOutCount = tmpPat->header.patBytes + sizeof(PatHeader);
					tmpPat->header.compMode = 'MAD1';
					ByteSwapPatHeader(&tmpPat->header);
					[saveData appendBytes:tmpPat length:inOutCount];
					free(tmpPat);
				}
			}
		} else {
			for (i = 0; i < Music->header->numPat; i++) {
				if (Music->partition[i]) {
					inOutCount = sizeof( PatHeader);
					inOutCount += Music->header->numChn * Music->partition[ i]->header.size * sizeof( Cmd);
					PatData *tmpPat = calloc(inOutCount, 1);
					memcpy(tmpPat, Music->partition[i], inOutCount);
					ByteSwapPatHeader(&tmpPat->header);
					[saveData appendBytes:tmpPat length:inOutCount];
					free(tmpPat);
				}
			}
		}
	}
	
	for( i = 0; i < MAXINSTRU; i++)
	{
		if( Music->fid[ i].numSamples > 0 || Music->fid[ i].name[ 0] != 0)	// Is there something in this instrument?
		{
			Music->fid[ i].no = i;
			InstrData instData = Music->fid[i];
			ByteSwapInstrData(&instData);
			[saveData appendBytes:&instData length:sizeof( InstrData)];
		}
	}
	
	for( i = 0; i < MAXINSTRU ; i++)
	{
		for( x = 0; x < Music->fid[i].numSamples; x++)
		{
			sData	curData;
			sData32	copyData;
			curData = *Music->sample[ Music->fid[i].firstSample + x];
			
			inOutCount = sizeof( sData32);
			ByteSwapsData(&curData);
			memcpy(&copyData, &curData, inOutCount);
			copyData.data = 0;
			[saveData appendBytes:&copyData length:inOutCount];
			
			inOutCount = Music->sample[ Music->fid[i].firstSample + x]->size;
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
		if( Music->header->globalEffect[ i])
		{
			inOutCount = sizeof( FXSets);
			__block FXSets aSet = Music->sets[alpha];
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
	
	for( i = 0; i < Music->header->numChn ; i++)	// Channel Effects
	{
		for( x = 0; x < 4; x++)
		{
			if( Music->header->chanEffect[ i][ x])
			{
				inOutCount = sizeof( FXSets);
				__block FXSets aSet = Music->sets[alpha];
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
	RELEASEOBJ(saveData);
	
	Music->header->numInstru = MAXINSTRU;
	
	Music->hasChanged = FALSE;
}

//Yes, the pragma pack is needed
//otherwise the parts will be improperly mapped.
#pragma pack(push, 2)
struct Float80i {
	SInt16  exp;
	UInt32  man[2];
};
#pragma pack(pop)

static inline extended80 convertSampleRateToExtended80(unsigned int theNum)
{
	union {
		extended80 shortman;
		struct Float80i intman;
	} toreturn;

	unsigned int shift, exponent;

	
	for(shift = 0U; (theNum >> (31 - shift)) == 0U; ++shift)
		;
	theNum <<= shift;
	exponent = 63U - (shift + 32U); /* add 32 for unused second word */

	toreturn.intman.exp = (exponent+0x3FFF);
	PPBE16(&toreturn.intman.exp);
	toreturn.intman.man[0] = theNum;
	PPBE32(&toreturn.intman.man[0]);
	toreturn.intman.man[1] = 0;
	PPBE32(&toreturn.intman.man[1]);
	
	return toreturn.shortman;
}

- (NSMutableData *)newAIFFDataFromSettings:(MADDriverSettings*)sett data:(NSData*)dat
{
	NSInteger dataLen = [dat length];
	
	ContainerChunk header;
	
	CommonChunk container;

	SoundDataChunk dataChunk;
	
	NSData *nameData;
	NSData *infoData;

	{
#if 0
		TextChunk *nameChunk;
		NSInteger macRomanNameLength = 0;
		NSData *macRomanNameData = [musicName dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
		macRomanNameLength = [macRomanNameData length];
		BOOL isPadded = (macRomanNameLength & 1);
		NSInteger nameChunkLen = sizeof(TextChunk) + macRomanNameLength;

		if (!isPadded) {
			nameChunkLen--;
		}
		
		nameChunk = calloc(nameChunkLen, 1);
		char *firstChar;
		nameChunk->ckID = NameID;
		PPBE32(&nameChunk->ckID);
		nameChunk->ckSize = 1 + macRomanNameLength;
		PPBE32(&nameChunk->ckSize);
		nameChunk->text[0] = macRomanNameLength;
		firstChar = &nameChunk->text[1];
		memcpy(firstChar, [macRomanNameData bytes], macRomanNameLength);
		nameData = [NSData dataWithBytes:nameChunk length:nameChunkLen];
		free(nameChunk);
#else
		ChunkHeader nameChunk;
		NSInteger macRomanNameLength = 0;
		NSData *macRomanNameData = [musicName dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
		macRomanNameLength = [macRomanNameData length];
		BOOL isPadded = (macRomanNameLength & 1);
		
		nameChunk.ckSize = macRomanNameLength + 1;
		char pStrLen = macRomanNameLength;
		PPBE32(&nameChunk.ckSize);
		
		nameChunk.ckID = NameID;
		PPBE32(&nameChunk.ckID);
		
		NSMutableData *tmpNameDat = [NSMutableData dataWithBytes:&nameChunk length:sizeof(ChunkHeader)];
		[tmpNameDat appendBytes:&pStrLen length:1];
		[tmpNameDat appendData:macRomanNameData];
		
		if (!isPadded) {
			char padbyte = 0;
			[tmpNameDat appendBytes:&padbyte length:1];
		}
		nameData = tmpNameDat;
#endif
	}
	
	{
#if 0
		ApplicationSpecificChunk *infoChunk;
		NSInteger macRomanInfoLength = 0;
		NSData *macRomanInfoData = [musicInfo dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
		macRomanInfoLength = [macRomanInfoData length];
		BOOL isPadded = (macRomanInfoLength & 1);
		NSInteger infoChunkLen = sizeof(ApplicationSpecificChunk) + macRomanInfoLength;
		
		if (!isPadded) {
			infoChunkLen--;
		}
		
		infoChunk = calloc(infoChunkLen, 0);
		infoChunk->applicationSignature = CommentID;
		PPBE32(&infoChunk->applicationSignature);
		infoChunk->ckID = ApplicationSpecificID;
		PPBE32(&infoChunk->ckID);
		infoChunk->ckSize = macRomanInfoLength + 1;
		PPBE32(&infoChunk->ckSize);
		memcpy(infoChunk->data, [macRomanInfoData bytes], macRomanInfoLength);
		infoData = [NSData dataWithBytes:infoChunk length:infoChunkLen];
		free(infoChunk);
#else
		ChunkHeader infoChunk;
		NSInteger macRomanInfoLength = 0;
		NSData *macRomanInfoData = [musicInfo dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
		macRomanInfoLength = [macRomanInfoData length];
		BOOL isPadded = (macRomanInfoLength & 1);
		infoChunk.ckSize = macRomanInfoLength + 1;
		char pStrLen = macRomanInfoLength;
		PPBE32(&infoChunk.ckSize);
		
		infoChunk.ckID = CommentID;
		PPBE32(&infoChunk.ckID);
		NSMutableData *tmpInfoDat = [NSMutableData dataWithBytes:&infoChunk length:sizeof(ChunkHeader)];
		[tmpInfoDat appendBytes:&pStrLen length:1];
		[tmpInfoDat appendData:macRomanInfoData];
		
		if (!isPadded) {
			char padbyte = 0;
			[tmpInfoDat appendBytes:&padbyte length:1];
		}

		infoData = tmpInfoDat;
#endif
	}
	
	
	header.ckID = FORMID;
	PPBE32(&header.ckID);
	header.ckSize = dataLen + sizeof(CommonChunk) + sizeof(SoundDataChunk) + 4 + [nameData length] + [infoData length];
	PPBE32(&header.ckSize);
	header.formType = AIFFID;
	PPBE32(&header.formType);
	NSMutableData *returnData = [[NSMutableData alloc] initWithBytes:&header length:sizeof(ContainerChunk)];
	
	container.ckID = CommonID;
	PPBE32(&container.ckID);
	container.ckSize = /*sizeof(CommonChunk)*/ 18;
	PPBE32(&container.ckSize);
	short chanNums = 0;
	{
		int todiv = sett->outPutBits / 8;

		switch (sett->outPutMode) {
			case DeluxeStereoOutPut:
			case StereoOutPut:
			default:
				todiv *= 2;
				chanNums = 2;
				break;
				
			case PolyPhonic:
				todiv *= 4;
				chanNums = 4;
				break;
				
			case MonoOutPut:
				chanNums = 1;
				break;
		}
		container.numSampleFrames = (dataLen + (todiv - 1)) / todiv;
	}

	container.numChannels = chanNums;
	PPBE16(&container.numChannels);
	container.sampleSize = sett->outPutBits;
	PPBE16(&container.sampleSize);
	PPBE32(&container.numSampleFrames);
	
	container.sampleRate = convertSampleRateToExtended80(sett->outPutRate);
	[returnData appendBytes:&container length:sizeof(CommonChunk)];
	
	int dataOffset = 0;
	dataChunk.ckID = SoundDataID;
	PPBE32(&dataChunk.ckID);
	dataChunk.blockSize = 0;
	PPBE32(&dataChunk.blockSize);
	dataChunk.ckSize = dataLen + 8 + dataOffset;
	PPBE32(&dataChunk.ckSize);
	dataChunk.offset = dataOffset;
	PPBE32(&dataChunk.offset);
	

	[returnData appendBytes:&dataChunk length:sizeof(SoundDataChunk)];
	if (sett->outPutBits == 16) {
		short swapdata;
		int i;
		for (i = 0; i < dataLen; i += 2) {
			[dat getBytes:&swapdata range:NSMakeRange(i, 2)];
			PPBE16(&swapdata);
			[returnData appendBytes:&swapdata length:2];
		}
	} else {
		[returnData appendData:dat];
	}
	
	[returnData appendData:nameData];
	[returnData appendData:infoData];
	
	return returnData;
}

- (NSData *)getSoundData:(MADDriverSettings*)theSet
{
	MADDriverRec *theRec = NULL;
	
	OSErr err = MADCreateDriver( theSet, MADLib, &theRec);
	if (err != noErr) {
		dispatch_async(dispatch_get_main_queue(), ^{
			NSError *NSerr = CreateErrorFromMADErrorType(err);
			NSAlert *alert = [NSAlert alertWithError:NSerr];
			[alert runModal];
			RELEASEOBJ(NSerr);
		});
		
		return nil;
	}
	MADCleanDriver( theRec);

	MADAttachDriverToMusic( theRec, Music, NULL);
	MADPlayMusic(theRec);
	
	Ptr soundPtr = NULL;
	long full = 0;
	if (theSet->outPutBits == 8) {
		full = (theRec->ASCBUFFERReal - theRec->BytesToRemoveAtEnd);
	}else if (theSet->outPutBits == 16) {
		full = (theRec->ASCBUFFERReal - theRec->BytesToRemoveAtEnd) * 2;
	} else if (theSet->outPutBits == 20 || theSet->outPutBits == 24 ) {
		full = (theRec->ASCBUFFERReal - theRec->BytesToRemoveAtEnd) * 3;
	} else {
		//This is just to make the Static analyzer happy
		full = (theRec->ASCBUFFERReal - theRec->BytesToRemoveAtEnd);
	}
	
	switch (theSet->outPutMode) {
		case DeluxeStereoOutPut:
		case StereoOutPut:
			full *= 2;
			break;
			
		case PolyPhonic:
			full *= 4;
			break;
			
		default:
			break;
	}
	
	NSMutableData *mutData = [[NSMutableData alloc] init];
	soundPtr = calloc(full, 1);
	
	while(DirectSave(soundPtr, theSet, theRec))
	{
		[mutData appendBytes:soundPtr length:full];
	}
	NSMutableData *tmpData = [self newAIFFDataFromSettings:theSet data:mutData];
	RELEASEOBJ(mutData);
	mutData = nil;
	NSData *retData = [NSData dataWithData:tmpData];
	RELEASEOBJ(tmpData);
	tmpData = nil;
	
	MADStopMusic(theRec);
	MADCleanDriver(theRec);
	MADDisposeDriver(theRec);
	free(soundPtr);
	
	return retData;
}

- (NSInteger)showExportSettings
{
	MADGetBestDriver(&exportSettings);
	exportSettings.driverMode = NoHardwareDriver;
	exportSettings.repeatMusic = FALSE;
	[exportController settingsFromDriverSettings:&exportSettings];
	return [NSApp runModalForWindow:exportWindow];
}

#ifndef PPEXPORT_CREATE_TMP_AIFF
#define PPEXPORT_CREATE_TMP_AIFF 1
#endif

- (IBAction)exportMusicAs:(id)sender
{
	NSInteger tag = [sender tag];
	MADBeginExport(MADDriver);

	switch (tag) {
		case -1:
			//AIFF
		{
			NSSavePanel *savePanel = RETAINOBJ([NSSavePanel savePanel]);
			[savePanel setAllowedFileTypes:[NSArray arrayWithObject:@"public.aiff-audio"]];
			[savePanel setCanCreateDirectories:YES];
			[savePanel setCanSelectHiddenExtension:YES];
			if (![musicName isEqualToString:@""]) {
				[savePanel setNameFieldStringValue:musicName];
			}
			[savePanel setPrompt:@"Export"];
			[savePanel setTitle:@"Export as AIFF audio"];
			if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
				if ([self showExportSettings] == NSAlertDefaultReturn) {
					dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
						NSData *saveData = RETAINOBJ([self getSoundData:&exportSettings]);
						MADEndExport(MADDriver);

						[saveData writeToURL:[savePanel URL] atomically:YES];
						RELEASEOBJ(saveData);
						dispatch_async(dispatch_get_main_queue(), ^{
							NSInteger retVal = NSRunInformationalAlertPanel(@"Export complete", @"The export of the file \"%@\" is complete.", @"Okay", @"Show File", nil, [[savePanel URL] lastPathComponent]);
							if (retVal == NSAlertAlternateReturn) {
								[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:[NSArray arrayWithObject:[savePanel URL]]];
							}
						});
					});
				} else {
					MADEndExport(MADDriver);
				}
			} else {
				MADEndExport(MADDriver);
			}
			RELEASEOBJ(savePanel);
		}
			break;
			
		case -2:
			//MP4
		{
			NSSavePanel *savePanel = RETAINOBJ([NSSavePanel savePanel]);
			[savePanel setAllowedFileTypes:[NSArray arrayWithObject:@"com.apple.m4a-audio"]];
			[savePanel setCanCreateDirectories:YES];
			[savePanel setCanSelectHiddenExtension:YES];
			if (![musicName isEqualToString:@""]) {
				[savePanel setNameFieldStringValue:musicName];
			}
			[savePanel setPrompt:@"Export"];
			[savePanel setTitle:@"Export as MPEG-4 Audio"];
			if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
				if ([self showExportSettings] == NSAlertDefaultReturn) {
					dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
						NSData *saveData = RETAINOBJ([self getSoundData:&exportSettings]);
						NSString *oldMusicName = RETAINOBJ(musicName);
						NSString *oldMusicInfo = RETAINOBJ(musicInfo);
						MADEndExport(MADDriver);
						NSError *expErr = nil;
						dispatch_block_t errBlock = ^{
							NSRunAlertPanel(@"Export failed", @"Export/coversion of the music file failed.\n%@", nil, nil, nil, [expErr localizedDescription]);
						};
#if PPEXPORT_CREATE_TMP_AIFF
						NSURL *tmpURL = [[NSURL fileURLWithPath:NSTemporaryDirectory()] URLByAppendingPathComponent:[NSString stringWithFormat:@"%@.aiff", oldMusicName] isDirectory:NO];
						
						[saveData writeToURL:tmpURL atomically:NO];
						QTMovie *exportMov = [[QTMovie alloc] initWithURL:tmpURL error:&expErr];
						[exportMov setAttribute:oldMusicName forKey:QTMovieDisplayNameAttribute];
						[exportMov setAttribute:oldMusicInfo forKey:QTMovieCopyrightAttribute];
#else
						//Attempts of using data directly have resulted in internal assertion failures in the export session initialization code
						QTDataReference *dataRef = [[QTDataReference alloc] initWithReferenceToData:saveData name:oldMusicName MIMEType:@"audio/aiff"];
						
						QTMovie *exportMov = [[QTMovie alloc] initWithAttributes:[NSDictionary dictionaryWithObjectsAndKeys:dataRef, QTMovieDataReferenceAttribute, @NO, QTMovieOpenAsyncOKAttribute, @YES, QTMovieDontInteractWithUserAttribute, @NO, QTMovieOpenForPlaybackAttribute, oldMusicName, QTMovieDisplayNameAttribute, oldMusicInfo, QTMovieCopyrightAttribute, nil] error:&expErr];
#endif
						RELEASEOBJ(oldMusicInfo);
						oldMusicInfo = nil;
						if (!exportMov) {
							NSLog(@"Init Failed for %@, error: %@", oldMusicName, [expErr localizedDescription]);
#if !PPEXPORT_CREATE_TMP_AIFF
							RELEASEOBJ(dataRef);
#else
							[[NSFileManager defaultManager] removeItemAtURL:tmpURL error:NULL];
#endif
							RELEASEOBJ(saveData);
							RELEASEOBJ(oldMusicName);
							
							dispatch_async(dispatch_get_main_queue(), errBlock);
							return;
						}
						
						QTExportSession *session = [[QTExportSession alloc] initWithMovie:exportMov exportOptions:[QTExportOptions exportOptionsWithIdentifier:QTExportOptionsAppleM4A] outputURL:[savePanel URL] error:&expErr];
						if (!session) {
							NSLog(@"Export session creation for %@ failed, error: %@", oldMusicName, [expErr localizedDescription]);
#if !PPEXPORT_CREATE_TMP_AIFF
							RELEASEOBJ(dataRef);
#else
							[[NSFileManager defaultManager] removeItemAtURL:tmpURL error:NULL];

#endif
							RELEASEOBJ(saveData);
							RELEASEOBJ(exportMov);
							RELEASEOBJ(oldMusicName);

							dispatch_async(dispatch_get_main_queue(), errBlock);
							return;
						}
						[session run];
						
						if (![session waitUntilFinished:&expErr])
						{
							NSLog(@"export of \"%@\" failed, error: %@", oldMusicName, [expErr localizedDescription]);
							dispatch_async(dispatch_get_main_queue(), errBlock);
						}
#if !PPEXPORT_CREATE_TMP_AIFF
						RELEASEOBJ(dataRef);
#else
						[[NSFileManager defaultManager] removeItemAtURL:tmpURL error:NULL];
#endif
						RELEASEOBJ(oldMusicName);
						RELEASEOBJ(session);
						RELEASEOBJ(exportMov);
						
						dispatch_async(dispatch_get_main_queue(), ^{
							NSInteger retVal = NSRunInformationalAlertPanel(@"Export complete", @"The export of the file \"%@\" is complete.", @"Okay", @"Show File", nil, [[savePanel URL] lastPathComponent]);
							if (retVal == NSAlertAlternateReturn) {
								[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:[NSArray arrayWithObject:[savePanel URL]]];
							}
						});

						RELEASEOBJ(saveData);
					});
				} else {
					MADEndExport(MADDriver);
				}
			} else {
				MADEndExport(MADDriver);
			}
			RELEASEOBJ(savePanel);
		}
			break;
			
		default:
		{
			if (tag > MADLib->TotalPlug || tag < 0) {
				NSBeep();
				MADEndExport(MADDriver);
				return;
			}
			NSSavePanel *savePanel = RETAINOBJ([NSSavePanel savePanel]);
			[savePanel setAllowedFileTypes:BRIDGE(NSArray*, MADLib->ThePlug[tag].UTItypes)];
			[savePanel setCanCreateDirectories:YES];
			[savePanel setCanSelectHiddenExtension:YES];
			if (![musicName isEqualToString:@""]) {
				[savePanel setNameFieldStringValue:musicName];
			}
			[savePanel setPrompt:@"Export"];
			[savePanel setTitle:[NSString stringWithFormat:@"Export as %@", BRIDGE(NSString*, MADLib->ThePlug[tag].MenuName)]];
			if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
				NSURL *fileURL = RETAINOBJ([savePanel URL]);
				OSErr err = MADMusicExportCFURL(MADLib, Music, MADLib->ThePlug[tag].type, BRIDGE(CFURLRef, fileURL));
				if (err != noErr) {
					NSError *aerr = CreateErrorFromMADErrorType(err);
					NSAlert *alert = [NSAlert alertWithError:aerr];
					[alert runModal];
					RELEASEOBJ(aerr);
				} else {
					[self addMusicToMusicList:fileURL loadIfPreferencesAllow:NO];
					NSInteger retVal = NSRunInformationalAlertPanel(@"Export complete", @"The export of the file \"%@\" is complete.", @"Okay", @"Show File", nil, [[savePanel URL] lastPathComponent]);
					if (retVal == NSAlertAlternateReturn) {
						[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:[NSArray arrayWithObject:fileURL]];
					}
				}
				RELEASEOBJ(fileURL);
			}
			RELEASEOBJ(savePanel);
		}
			MADEndExport(MADDriver);
			break;
	}
}

- (IBAction)okayExportSettings:(id)sender {
	[NSApp stopModalWithCode:NSAlertDefaultReturn];
	[exportWindow close];
}

- (IBAction)cancelExportSettings:(id)sender {
	[NSApp stopModalWithCode:NSAlertAlternateReturn];
	[exportWindow close];
}

- (IBAction)saveMusicAs:(id)sender
{
	MADBeginExport(MADDriver);
	
	NSSavePanel * savePanel = RETAINOBJ([NSSavePanel savePanel]);
	[savePanel setAllowedFileTypes:[NSArray arrayWithObject:MADNativeUTI]];
	[savePanel setCanCreateDirectories:YES];
	[savePanel setCanSelectHiddenExtension:YES];
	if (![musicName isEqualToString:@""]) {
		[savePanel setNameFieldStringValue:musicName];
	}
	if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
		NSURL *saveURL = RETAINOBJ([savePanel URL]);
		[self saveMusicToURL:saveURL];
		[self addMusicToMusicList:saveURL loadIfPreferencesAllow:NO];
		RELEASEOBJ(saveURL);
	}
	RELEASEOBJ(savePanel);
	MADEndExport(MADDriver);
}

- (IBAction)saveMusic:(id)sender
{
	MADBeginExport(MADDriver);
	
	if (previouslyPlayingIndex.index == -1) {
		// saveMusicAs: will end the exporting when it is done.
		[self saveMusicAs:sender];
	} else {
		NSURL *fileURL = previouslyPlayingIndex.playbackURL;
		NSString *filename = [fileURL path];
		NSWorkspace *sharedWorkspace = [NSWorkspace sharedWorkspace];
		NSString *utiFile = [sharedWorkspace typeOfFile:filename error:nil];
		if (/*[sharedWorkspace type:utiFile conformsToType:MADNativeUTI]*/ [utiFile isEqualToString:MADNativeUTI]) {
			[self saveMusicToURL:fileURL];
			MADEndExport(MADDriver);
		} else {
			// saveMusicAs: will end the exporting when it is done.
			[self saveMusicAs:sender];
		}
	}
}

- (BOOL)loadMusicURL:(NSURL*)musicToLoad error:(out NSError *__autoreleasing*)theErr
{
	if (Music != NULL) {
		if (Music->hasChanged) {
			NSInteger selection = 0;
			if (previouslyPlayingIndex.index == -1) {
				selection = NSRunAlertPanel(NSLocalizedString(@"Unsaved Changes", @"Unsaved Changes"), NSLocalizedString(@"The new music file has unsaved changes. Do you want to save?", @"New unsaved file"), NSLocalizedString(@"Save", @"Save"), NSLocalizedString(@"Don't Save", @"Don't Save"), NSLocalizedString(@"Cancel", @"Cancel"));
			} else {
				selection = NSRunAlertPanel(NSLocalizedString(@"Unsaved Changes", @"Unsaved Changes"), NSLocalizedString(@"The music file \"%@\" has unsaved changes. Do you want to save?", @"file unsaved"), NSLocalizedString(@"Save", @"Save"), NSLocalizedString(@"Don't Save", @"Don't Save"), NSLocalizedString(@"Cancel", @"Cancel"), [[musicList objectInMusicListAtIndex:previouslyPlayingIndex.index] fileName]);
			}
			switch (selection) {
				case NSAlertDefaultReturn:
					[self saveMusic:nil];
					break;
					
				case NSAlertAlternateReturn:
				default:
					break;
					
				case NSAlertOtherReturn:
					[previouslyPlayingIndex movePlayingIndexToOtherIndex:currentlyPlayingIndex];
					if (theErr) {
						*theErr = AUTORELEASEOBJ([[NSError alloc] initWithDomain:NSCocoaErrorDomain code:NSUserCancelledError userInfo:nil]);
					}
					return NO;
					break;
			}
		}
		MADStopMusic(MADDriver);
		MADCleanDriver(MADDriver);
		MADDisposeMusic(&Music, MADDriver);
	}

	char fileType[5];
	OSErr theOSErr = MADMusicIdentifyCFURL(MADLib, fileType, BRIDGE(CFURLRef, musicToLoad));
		
	if(theOSErr != noErr)
	{
		if (theErr) {
			*theErr = AUTORELEASEOBJ(CreateErrorFromMADErrorType(theOSErr));
		}
		self.paused = YES;
		return NO;
	}
	theOSErr = MADLoadMusicCFURLFile(MADLib, &Music, fileType, BRIDGE(CFURLRef, musicToLoad));
	
	if (theOSErr != noErr) {
		if (theErr) {
			*theErr = AUTORELEASEOBJ(CreateErrorFromMADErrorType(theOSErr));
		}
		self.paused = YES;
		return NO;
	}
	
	MADAttachDriverToMusic(MADDriver, Music, NULL);
	MADPlayMusic(MADDriver);
	self.paused = NO;
	{
		long fT, cT;
		MADGetMusicStatus(MADDriver, &fT, &cT);
		[songPos setMaxValue:fT];
		[songPos setMinValue:0.0];
		[self setTitleForSongLabelBasedOnMusic];
		[songTotalTime setIntegerValue:fT];
	}
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPMusicDidChange object:self];

	if (theErr) {
		*theErr = nil;
	}
	return YES;
}

- (BOOL)loadMusicURL:(NSURL*)musicToLoad
{
	return [self loadMusicURL:musicToLoad error:NULL];
}

- (IBAction)showPreferences:(id)sender
{
    if (!preferences) {
		preferences = [[PPPreferences alloc] init];
	}
	[[preferences window] center];
	[preferences showWindow:sender];
}


- (IBAction)exportInstrumentAs:(id)sender
{
    [instrumentController exportInstrument:sender];
}

- (IBAction)showInstrumentsList:(id)sender
{
    [instrumentController showWindow:sender];
}

- (IBAction)showTools:(id)sender
{
    [toolsPanel makeKeyAndOrderFront:sender];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
	if ([keyPath isEqualToString:@"paused"]) {
		id boolVal = [change objectForKey:NSKeyValueChangeNewKey];
		
		//[pauseButton highlight:[boolVal boolValue]];
		switch ([boolVal boolValue]) {
			case NO:
			default:
				[pauseButton setState:NSOffState];
				[pauseDockMenuItem setState:NSOnState];
				break;
				
			case YES:
				[pauseButton setState:NSOnState];
				[pauseDockMenuItem setState:NSOffState];
				break;
		}
	}
	//[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
}

- (void)doubleClickMusicList
{
	NSError *err = nil;
	currentlyPlayingIndex.index = [tableView selectedRow];
	if ([self loadMusicFromCurrentlyPlayingIndexWithError:&err] == NO)
	{
		NSAlert *alert = [NSAlert alertWithError:err];
		[alert runModal];
	}
}

- (IBAction)showPlugInInfo:(id)sender
{
	PPPlugInInfo *inf = [plugInInfos objectAtIndex:[sender tag]];
	if (!inf) {
		return;
	}
	
	PPPlugInInfoController *infoCont = [[PPPlugInInfoController alloc] initWithPlugInInfo:inf];
	[[infoCont window] center];
	[[NSApplication sharedApplication] runModalForWindow:[infoCont window]];
	//[infoCont showWindow:sender];
	RELEASEOBJ(infoCont);
}

- (void)updatePlugInInfoMenu
{
	NSInteger i;

	[plugInInfos removeAllObjects];
	
	for (i = 0; i < MADLib->TotalPlug ; i++) {
		PPPlugInInfo *tmpInfo = [[PPPlugInInfo alloc] initWithPlugName:BRIDGE(NSString*,MADLib->ThePlug[i].MenuName) author:BRIDGE(NSString*,MADLib->ThePlug[i].AuthorString) plugType:NSLocalizedString(@"TrackerPlugName", @"Tracker plug-in name")];
		[plugInInfos addObject:tmpInfo];
		RELEASEOBJ(tmpInfo);
	}
	
	for (i = 0; i < [instrumentImporter plugInCount]; i++) {
		PPInstrumentImporterObject *obj = [instrumentImporter plugInAtIndex:i];
		PPPlugInInfo *tmpInfo = [[PPPlugInInfo alloc] initWithPlugName:obj.menuName author:obj.authorString plugType:NSLocalizedString(@"InstrumentPlugName", @"Instrument plug-in name")];
		[plugInInfos addObject:tmpInfo];
		RELEASEOBJ(tmpInfo);
	}
	
	for (i = 0; i < [digitalHandler plugInCount]; i++) {
		PPDigitalPlugInObject *obj = [digitalHandler plugInAtIndex:i];
		PPPlugInInfo *tmpInfo = [[PPPlugInInfo alloc] initWithPlugName:obj.menuName author:obj.authorString plugType:NSLocalizedString(@"DigitalPlugName", @"Digital plug-in name")];
		[plugInInfos addObject:tmpInfo];
		RELEASEOBJ(tmpInfo);
	}
	
	for (i = 0; i < [filterHandler plugInCount]; i++) {
		PPFilterPlugObject *obj = [filterHandler plugInAtIndex:i];
		PPPlugInInfo *tmpInfo = [[PPPlugInInfo alloc] initWithPlugName:obj.menuName author:obj.authorString plugType:NSLocalizedString(@"FilterPlugName", @"Filter plug-in name")];
		[plugInInfos addObject:tmpInfo];
		RELEASEOBJ(tmpInfo);
	}
	
	[plugInInfos sortUsingComparator:^NSComparisonResult(id obj1, id obj2) {
		NSString *menuNam1 = [obj1 plugName];
		NSString *menuNam2 = [obj2 plugName];
		NSComparisonResult res = [menuNam1 localizedStandardCompare:menuNam2];
		return res;
	}];
	
	[aboutPlugInMenu removeAllItems];
	
	for (i = 0; i < [plugInInfos count]; i++) {
		PPPlugInInfo *pi = [plugInInfos objectAtIndex:i];
		NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:pi.plugName action:@selector(showPlugInInfo:) keyEquivalent:@""];
		[mi setTag:i];
		[mi setTarget:self];
		[aboutPlugInMenu addItem:mi];
		RELEASEOBJ(mi);
	}
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	srandom(time(NULL) & 0xffffffff);
	PPRegisterDebugFunc(CocoaDebugStr);
	MADInitLibrary(NULL, &MADLib);
	//the NIB won't store the value anymore, so do this hackery to make sure there's some value in it.
	[songTotalTime setIntegerValue:0];
	[songCurTime setIntegerValue:0];

	[self addObserver:self forKeyPath:@"paused" options:NSKeyValueObservingOptionNew context:NULL];
	self.paused = YES;
	[self willChangeValueForKey:kMusicListKVO];
	musicList = [[PPMusicList alloc] init];
	if ([[NSUserDefaults standardUserDefaults] boolForKey:PPRememberMusicList]) {
		[musicList loadMusicListFromPreferences];
	}
	[self didChangeValueForKey:kMusicListKVO];

	[tableView setDoubleAction:@selector(doubleClickMusicList)];
	
	NSNotificationCenter *defaultCenter = [NSNotificationCenter defaultCenter];
	[defaultCenter addObserver:self selector:@selector(preferencesDidChange:) name:PPListPreferencesDidChange object:nil];
	[defaultCenter addObserver:self selector:@selector(soundPreferencesDidChange:) name:PPSoundPreferencesDidChange object:nil];
	
	[self MADDriverWithPreferences];
	Music = CreateFreeMADK();
	MADAttachDriverToMusic(MADDriver, Music, NULL);
	instrumentImporter = [[PPInstrumentImporter alloc] initWithMusic:&Music];
	
	digitalHandler = [[PPDigitalPlugInHandler alloc] initWithMusic:&Music];
	digitalHandler.driverRec = &MADDriver;
	
	filterHandler = [[PPFilterPlugHandler alloc] initWithMusic:&Music];
	filterHandler.driverRec = &MADDriver;
	
	instrumentController = [[PPInstrumentWindowController alloc] init];
	instrumentController.importer = instrumentImporter;
	instrumentController.curMusic = &Music;
	
	//Initialize the QTKit framework on the main thread. needed for 32-bit code.
	[QTMovie class];
	
	NSInteger i;
	for (i = 0; i < [instrumentImporter plugInCount]; i++) {
		PPInstrumentImporterObject *obj = [instrumentImporter plugInAtIndex:i];
		if (obj.mode == MADPlugImportExport || obj.mode == MADPlugExport) {
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:obj.menuName action:@selector(exportInstrument:) keyEquivalent:@""];
			[mi setTag:i];
			[mi setTarget:instrumentController];
			[instrumentExportMenu addItem:mi];
			RELEASEOBJ(mi);
		}
	}
	
	for (i = 0; i < MADLib->TotalPlug; i++) {
		if (MADLib->ThePlug[i].mode == MADPlugImportExport || MADLib->ThePlug[i].mode == MADPlugExport) {
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:[NSString stringWithFormat:@"%@...", BRIDGE(NSString*, MADLib->ThePlug[i].MenuName)] action:@selector(exportMusicAs:) keyEquivalent:@""];
			[mi setTag:i];
			[mi setTarget:self];
			[musicExportMenu addItem:mi];
			RELEASEOBJ(mi);
		}
	}
	
	plugInInfos = [[NSMutableArray alloc] init];
	[self updatePlugInInfoMenu];
	
	previouslyPlayingIndex = [[PPCurrentlyPlayingIndex alloc] init];
	previouslyPlayingIndex.index = -1;
	currentlyPlayingIndex = [[PPCurrentlyPlayingIndex alloc] init];
	[previouslyPlayingIndex movePlayingIndexToOtherIndex:currentlyPlayingIndex];
	
	exportController = [[PPSoundSettingsViewController alloc] init];
	exportController.delegate = self;
	[exportSettingsBox setContentView:[exportController view]];
	
	timeChecker = [[NSTimer alloc] initWithFireDate:[NSDate dateWithTimeIntervalSinceNow:0] interval:1/8.0 target:self selector:@selector(updateMusicStats:) userInfo:nil repeats:YES];
	[[NSRunLoop mainRunLoop] addTimer:timeChecker forMode:NSDefaultRunLoopMode];
	NSUInteger lostCount = musicList.lostMusicCount;
	if (lostCount) {
		NSRunAlertPanel(@"Unresolvable files", @"There were %lu file(s) that were unable to be resolved.", nil, nil, nil, (unsigned long)lostCount);
	}
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
	[timeChecker invalidate];

	[self removeObserver:self forKeyPath:@"paused"];
	
	RELEASEOBJ(instrumentImporter);
	instrumentImporter = nil;
	
	RELEASEOBJ(digitalHandler);
	digitalHandler = nil;
	
	RELEASEOBJ(filterHandler);
	filterHandler = nil;
	
	if (Music != NULL) {
		if (Music->hasChanged) {
			NSInteger selection = 0;
			if (currentlyPlayingIndex.index == -1) {
				selection = NSRunAlertPanel(NSLocalizedString(@"Unsaved Changes", @"Unsaved Changes"), NSLocalizedString(@"The new music file has unsaved changes. Do you want to save?", @"New unsaved file"), NSLocalizedString(@"Save", @"Save"), NSLocalizedString(@"Don't Save", @"Don't Save"), nil);
			} else {
				selection = NSRunAlertPanel(NSLocalizedString(@"Unsaved Changes", @"Unsaved Changes"), NSLocalizedString(@"The music file \"%@\" has unsaved changes. Do you want to save?", @"file unsaved"), NSLocalizedString(@"Save", @"Save"), NSLocalizedString(@"Don't Save", @"Don't Save"), nil, [[musicList objectInMusicListAtIndex:currentlyPlayingIndex.index] fileName]);
			}
			switch (selection) {
				case NSAlertDefaultReturn:
					[self saveMusic:nil];
					break;
					
				case NSAlertAlternateReturn:
				default:
					break;
			}
		}
		MADStopMusic(MADDriver);
		MADCleanDriver(MADDriver);
		MADDisposeMusic(&Music, MADDriver);
	}
	MADStopDriver(MADDriver);
	MADDisposeDriver(MADDriver);
	MADDisposeLibrary(MADLib);
	if ([[NSUserDefaults standardUserDefaults] boolForKey:PPRememberMusicList]) {
		[musicList saveMusicListToPreferences];
	} else {
		[[NSUserDefaults standardUserDefaults] removeObjectForKey:PPMMusicList];
	}
	
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)preferencesDidChange:(NSNotification *)notification
{
	
}

#if !__has_feature(objc_arc)
- (void)dealloc
{
	[preferences release];
	[musicList release];
	[timeChecker release];
	[plugInInfos release];
	[instrumentImporter release];
	[digitalHandler release];
	[previouslyPlayingIndex release];
	[currentlyPlayingIndex release];
	[musicName release];
	[exportController release];
	
	[super dealloc];
}
#endif

- (IBAction)deleteInstrument:(id)sender
{
    [instrumentController deleteInstrument:sender];
}

- (IBAction)showBoxEditor:(id)sender
{
    
}

- (IBAction)showClassicEditor:(id)sender
{
    
}

- (IBAction)showDigitalEditor:(id)sender
{
    
}

- (void)musicListContentsDidMove
{
	NSInteger i;
	if (currentlyPlayingIndex.index != -1) {
		for (i = 0; i < [musicList countOfMusicList]; i++) {
			if ([currentlyPlayingIndex.playbackURL isEqual:[musicList URLAtIndex:i]]) {
				currentlyPlayingIndex.index = i;
				break;
			}
		}
	}
	if (previouslyPlayingIndex.index != -1) {
		for (i = 0; i < [musicList countOfMusicList]; i++) {
			if ([previouslyPlayingIndex.playbackURL isEqual:[musicList URLAtIndex:i]]) {
				previouslyPlayingIndex.index = i;
				break;
			}
		}
	}
}

- (IBAction)sortMusicList:(id)sender
{
	[self willChangeValueForKey:kMusicListKVO];
	[musicList sortMusicList];
	[self didChangeValueForKey:kMusicListKVO];
	[self musicListContentsDidMove];
}

- (IBAction)playSelectedMusic:(id)sender
{
	NSError *error = nil;
	currentlyPlayingIndex.index = [tableView selectedRow];
	if ([self loadMusicFromCurrentlyPlayingIndexWithError:&error] == NO)
	{
		NSAlert *alert = [NSAlert alertWithError:error];
		[alert runModal];
	}
}

- (IBAction)addMusic:(id)sender
{
	NSOpenPanel *panel = RETAINOBJ([NSOpenPanel openPanel]);
	NSMutableArray *supportedUTIs = [NSMutableArray arrayWithObject:MADNativeUTI];
	int i = 0;
	for (i = 0; i < MADLib->TotalPlug; i++) {
		[supportedUTIs addObjectsFromArray:BRIDGE(NSArray*, MADLib->ThePlug[i].UTItypes)];
	}
	[panel setAllowsMultipleSelection:NO];
	[panel setAllowedFileTypes:supportedUTIs];
	if([panel runModal] == NSFileHandlingPanelOKButton)
	{
		[self addMusicToMusicList:[panel URL]];
	}
	
	RELEASEOBJ(panel);
}

- (void)setTitleForSongLabelBasedOnMusic
{
#if __has_feature(objc_arc)
	musicName = [[NSString alloc] initWithCString:Music->header->name encoding:NSMacOSRomanStringEncoding];
	musicInfo = [[NSString alloc] initWithCString:Music->header->infos encoding:NSMacOSRomanStringEncoding];
	if (!musicInfo) {
		//Just in case the copyright character is UTF-8
		musicInfo = [[NSString alloc] initWithUTF8String:Music->header->infos];
	}
#else
	NSString *tempNam = musicName;
	musicName = [[NSString alloc] initWithCString:Music->header->name encoding:NSMacOSRomanStringEncoding];
	[tempNam release];
	tempNam = musicInfo;
	musicInfo = [[NSString alloc] initWithCString:Music->header->infos encoding:NSMacOSRomanStringEncoding];
	if (!musicInfo) {
		//Just in case the copyright character is UTF-8
		musicInfo = [[NSString alloc] initWithUTF8String:Music->header->infos];
	}
	[tempNam release];
#endif
	
	[songLabel setTitleWithMnemonic:musicName];
}

- (void)clearMusic
{
	MADStopMusic(MADDriver);
	MADCleanDriver(MADDriver);
	MADDisposeMusic(&Music, MADDriver);
	self.paused = YES;
	currentlyPlayingIndex.index = -1;
	currentlyPlayingIndex.playbackURL = nil;
	[currentlyPlayingIndex movePlayingIndexToOtherIndex:previouslyPlayingIndex];
	Music = CreateFreeMADK();
	[self setTitleForSongLabelBasedOnMusic];
	MADAttachDriverToMusic(MADDriver, Music, NULL);
}

- (IBAction)removeSelectedMusic:(id)sender
{
#if 0
	NSIndexSet *selMusic = [tableView selectedRowIndexes];
	NSInteger i = 0;
	NSInteger selIndexes = [selMusic count];
	NSUInteger *indexArray = malloc(sizeof(NSUInteger) * selIndexes);
	NSRange theRange;
	theRange.location = 0;
	theRange.length = NSUIntegerMax;
	[selMusic getIndexes:indexArray maxCount:selIndexes inIndexRange:&theRange];
	[self willChangeValueForKey:kMusicListKVO];
	for (i = selIndexes - 1; i >= 0 ; i--) {
		[musicList removeObjectInMusicListAtIndex:indexArray[i]];
	}
	[self didChangeValueForKey:kMusicListKVO];
	free(indexArray);
#else
	NSInteger selMusic = [tableView selectedRow];
	if (currentlyPlayingIndex.index == selMusic) {
		if (Music->hasChanged) {
			NSInteger selection = NSRunAlertPanel(NSLocalizedString(@"Unsaved Changes",@"Unsaved Changes"), NSLocalizedString(@"The music file \"%@\" has unsaved changes. Do you want to save?", @"edited file"), NSLocalizedString(@"Save", @"Save"), NSLocalizedString(@"Don't Save", @"Don't Save"), NSLocalizedString(@"Cancel", @"Cancel"), [[musicList objectInMusicListAtIndex:currentlyPlayingIndex.index] fileName]);
			switch (selection) {
				case NSAlertDefaultReturn:
					[self saveMusic:nil];
				case NSAlertAlternateReturn:
				default:
					[self clearMusic];
					break;
					
				case NSAlertOtherReturn:
					return;
					break;
			}
		} else {
			[self clearMusic];
		}
	}
	[self willChangeValueForKey:kMusicListKVO];
	[musicList removeObjectInMusicListAtIndex:selMusic];
	[self didChangeValueForKey:kMusicListKVO];
	[self musicListContentsDidMove];
#endif
}

- (IBAction)newMusic:(id)sender
{
	if (Music->hasChanged) {
		NSInteger selection = 0;
		if (currentlyPlayingIndex.index == -1) {
			selection = NSRunAlertPanel(NSLocalizedString(@"Unsaved Changes", @"Unsaved Changes"), NSLocalizedString(@"The new music file has unsaved changes. Do you want to save?", @"New unsaved file"), NSLocalizedString(@"Save", @"Save"), NSLocalizedString(@"Don't Save", @"Don't Save"), NSLocalizedString(@"Cancel", @"Cancel"));
		} else {
			selection = NSRunAlertPanel(NSLocalizedString(@"Unsaved Changes",@"Unsaved Changes"), NSLocalizedString(@"The music file \"%@\" has unsaved changes. Do you want to save?", @"Open unsaved file"), NSLocalizedString(@"Save", @"Save"), NSLocalizedString(@"Don't Save", @"Don't Save"), NSLocalizedString(@"Cancel", @"Cancel"), [[musicList objectInMusicListAtIndex:currentlyPlayingIndex.index] fileName]);
		}
		switch (selection) {
			case NSAlertDefaultReturn:
				[self saveMusic:nil];
				
			case NSAlertAlternateReturn:
			default:
				break;
				
			case NSAlertOtherReturn:
				return;
				break;
		}
	}
	[self clearMusic];
}

- (IBAction)clearMusicList:(id)sender
{
	if ([musicList countOfMusicList]) {
		NSInteger returnVal = NSRunAlertPanel(NSLocalizedString(@"Clear list", @"Clear Music List"), NSLocalizedString(@"The music list contains %ld items. Do you really want to remove them?", @"Clear Music List?"), NSLocalizedString(@"No", @"No"), NSLocalizedString(@"Yes", @"Yes"), nil, (long)[musicList countOfMusicList]);
		
		if (returnVal == NSAlertAlternateReturn) {
			if (Music->hasChanged && currentlyPlayingIndex.index != -1) {
				NSInteger selection = NSRunAlertPanel(NSLocalizedString(@"Unsaved Changes", @"Unsaved Changes"), NSLocalizedString(@"The music file \"%@\" has unsaved changes. Do you want to save?", @"Save check with file name"), NSLocalizedString(@"Save", @"Save"), NSLocalizedString(@"Don't Save", @"Don't Save"), NSLocalizedString(@"Cancel", @"Cancel"), [[musicList objectInMusicListAtIndex:currentlyPlayingIndex.index] fileName]);
				switch (selection) {
					case NSAlertDefaultReturn:
						[self saveMusic:nil];
					case NSAlertAlternateReturn:
					default:
						break;
						
					case NSAlertOtherReturn:
						return;
						break;
				}
			}

			[self willChangeValueForKey:kMusicListKVO];
			[musicList clearMusicList];
			[self didChangeValueForKey:kMusicListKVO];
			if (currentlyPlayingIndex.index != -1) {
				[self clearMusic];
			}
		}
	} else {
		NSBeep();
	}
}

enum PPMusicToolbarTypes {
	PPToolbarSort = 1001,
	PPToolbarAddMusic,
	PPToolbarRemoveMusic,
	PPToolbarPlayMusic,
	PPToolbarFileInfo
};

- (BOOL)validateToolbarItem:(NSToolbarItem *)theItem
{
	switch ([theItem tag]) {
		case PPToolbarSort:
		case PPToolbarAddMusic:
		case PPToolbarFileInfo:
			return YES;
			break;
			
		case PPToolbarPlayMusic:
			if([[tableView selectedRowIndexes] count] == 1) {
				return YES;
			} else {
				return NO;
			}
			break;
			
		case PPToolbarRemoveMusic:
			if([[tableView selectedRowIndexes] count] > 0) {
				return YES;
			} else {
				return NO;
			}
			break;

		default:
			return NO;
			break;
	}
}

- (void)soundPreferencesDidChange:(NSNotification *)notification
{
	[self MADDriverWithPreferences];
}

- (BOOL)handleFile:(NSURL *)theURL ofType:(NSString *)theUTI
{
	NSWorkspace *sharedWorkspace = [NSWorkspace sharedWorkspace];
	static BOOL hasAPPLPlug = NO;
	if (!hasAPPLPlug && MADPlugAvailable(MADLib, "APPL")) {
		hasAPPLPlug = YES;
	}

	if ([sharedWorkspace type:theUTI conformsToType:PPGenericTrackerUTI] || (hasAPPLPlug &&  [sharedWorkspace type:theUTI conformsToType:BRIDGE(NSString*, kUTTypeApplicationFile)])) {
		[self addMusicToMusicList:theURL];
		return YES;
	}else if ([sharedWorkspace type:theUTI conformsToType:PPMusicListUTI]) {
		if ([self musicListWillChange]) {
			[self willChangeValueForKey:kMusicListKVO];
			[musicList loadMusicListAtURL:theURL];
			[self didChangeValueForKey:kMusicListKVO];
			[self musicListDidChange];
			return YES;
		}
	} else if ([sharedWorkspace type:theUTI conformsToType:PPOldMusicListUTI]) {
		if ([self musicListWillChange]) {
			[self willChangeValueForKey:kMusicListKVO];
			[musicList loadOldMusicListAtURL:theURL];
			[self didChangeValueForKey:kMusicListKVO];
			[self musicListDidChange];
			return YES;
		}
	} else if([sharedWorkspace type:theUTI conformsToType:PPGenericInstrumentUTI]) {
		if ([instrumentController isWindowLoaded]) {
			NSError *theErr = nil;
			if (![instrumentController importSampleFromURL:theURL error:&theErr])
			{
				NSAlert *theAlert = [NSAlert alertWithError:theErr];
				[theAlert runModal];
				return NO;
			}
			return YES;
		} else return NO;
	} else if ([sharedWorkspace type:theUTI conformsToType:PPPCMDUTI]) {
		if ([instrumentController isWindowLoaded]) {
			OSErr theOSErr = [instrumentController importPcmdFromURL:theURL];
			if (theOSErr != noErr) {
				NSError *theErr = CreateErrorFromMADErrorType(theOSErr);
				NSAlert *alert = [NSAlert alertWithError:theErr];
				[alert runModal];
				RELEASEOBJ(theErr);
				return NO;
			}
			return YES;
		}
	} else if ([instrumentController isWindowLoaded]) {
		NSInteger i;
		for (i = 0; i < [instrumentImporter plugInCount]; i++) {
			PPInstrumentImporterObject *obj = [instrumentImporter plugInAtIndex:i];
			for (NSString *uti in [obj UTITypes]) {
				if ([sharedWorkspace type:theUTI conformsToType:uti]) {
					NSError *theErr;
					if (![instrumentController importSampleFromURL:theURL error:&theErr]) {
						NSAlert *theAlert = [NSAlert alertWithError:theErr];
						[theAlert runModal];
						return NO;
					}
					return YES;
				}
			}
			
		}
	}
	return NO;
}

- (IBAction)openFile:(id)sender {
	NSOpenPanel *panel = RETAINOBJ([NSOpenPanel openPanel]);
	NSMutableArray *supportedUTIs = [NSMutableArray arrayWithObjects:MADNativeUTI, PPMusicListUTI, PPOldMusicListUTI, nil];
	int i = 0;
	NSMutableDictionary *trackerDict = [NSMutableDictionary dictionaryWithObject:[NSArray arrayWithObject:MADNativeUTI] forKey:@"MADK Tracker"];
	NSDictionary *playlistDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSArray arrayWithObject:PPMusicListUTI], @"PlayerPRO Music List", [NSArray arrayWithObject:PPOldMusicListUTI], @"PlayerPRO Old Music List", nil];
	for (i = 0; i < MADLib->TotalPlug; i++) {
		NSArray *tempArray = BRIDGE(NSArray*, MADLib->ThePlug[i].UTItypes);
		[supportedUTIs addObjectsFromArray:tempArray];
		NSString *menuName = BRIDGE(NSString*, MADLib->ThePlug[i].MenuName);
		[trackerDict setObject:tempArray forKey:menuName];
	}
		
	NSMutableDictionary *samplesDict = nil;
	if ([instrumentController isWindowLoaded]) {
		NSInteger plugCount = [instrumentImporter plugInCount];
		samplesDict = [[NSMutableDictionary alloc] initWithCapacity:plugCount];
		for (i = 0; i < plugCount; i++) {
			PPInstrumentImporterObject *obj = [instrumentImporter plugInAtIndex:i];
			NSArray *tmpArray = obj.UTITypes;
			[supportedUTIs addObjectsFromArray:tmpArray];
			[samplesDict setObject:tmpArray forKey:obj.menuName];
		}
	}

	NSDictionary *otherDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSArray arrayWithObject:PPPCMDUTI], @"PCMD", nil];
	
	for (NSString *key in otherDict) {
		NSArray *tempArray = [otherDict objectForKey:key];
		[supportedUTIs addObjectsFromArray:tempArray];
	}
	
	[panel setAllowsMultipleSelection:NO];
	[panel setAllowedFileTypes:supportedUTIs];
	OpenPanelViewController *av = [[OpenPanelViewController alloc] initWithOpenPanel:panel trackerDictionary:trackerDict playlistDictionary:playlistDict instrumentDictionary:samplesDict additionalDictionary:otherDict];
	[panel setAccessoryView:[av view]];
	RELEASEOBJ(samplesDict);
	if([panel runModal] == NSFileHandlingPanelOKButton)
	{
		NSURL *panelURL = [panel URL];
		NSString *filename = [panelURL path];
		NSError *err = nil;
		NSString *utiFile = [[NSWorkspace sharedWorkspace] typeOfFile:filename error:&err];
		if (err) {
			NSRunAlertPanel(@"Error opening file", @"Unable to open %@: %@", nil, nil, nil, [filename lastPathComponent], [err localizedFailureReason]);
			RELEASEOBJ(panel);
			RELEASEOBJ(av);
			return;
		}
		[self handleFile:panelURL ofType:utiFile];
	}
	RELEASEOBJ(panel);
	RELEASEOBJ(av);
}

- (IBAction)saveMusicList:(id)sender {
	NSSavePanel *savePanel = RETAINOBJ([NSSavePanel savePanel]);
	[savePanel setAllowedFileTypes:[NSArray arrayWithObject:PPMusicListUTI]];
	[savePanel setCanCreateDirectories:YES];
	[savePanel setCanSelectHiddenExtension:YES];
	if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
		[musicList saveMusicListToURL:[savePanel URL]];
	}
	RELEASEOBJ(savePanel);
}

- (IBAction)fastForwardButtonPressed:(id)sender
{
    
}

- (IBAction)loopButtonPressed:(id)sender
{
    
}

- (IBAction)nextButtonPressed:(id)sender
{
    if (currentlyPlayingIndex.index + 1 < [musicList countOfMusicList]) {
		currentlyPlayingIndex.index++;
		[self selectCurrentlyPlayingMusic];
		NSError *err = nil;
		if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err]) {
			NSAlert *ap = [NSAlert alertWithError:err];
			[ap runModal];
		}
	} else if ([[NSUserDefaults standardUserDefaults] boolForKey:PPLoopMusicWhenDone]) {
		currentlyPlayingIndex.index = 0;
		[self selectCurrentlyPlayingMusic];
		NSError *err = nil;
		if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err]) {
			NSAlert *ap = [NSAlert alertWithError:err];
			[ap runModal];
		}
	} else NSBeep();
}

- (IBAction)playButtonPressed:(id)sender
{
	if (Music) {
		MADPlayMusic(MADDriver);
		self.paused = NO;
	}
}

- (IBAction)prevButtonPressed:(id)sender
{
    if (currentlyPlayingIndex.index > 0) {
		currentlyPlayingIndex.index--;
		[self selectCurrentlyPlayingMusic];
		NSError *err = nil;
		if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err]) {
			NSAlert *ap = [NSAlert alertWithError:err];
			[ap runModal];
		}
	} else NSBeep();
}

- (IBAction)recordButtonPressed:(id)sender
{
    
}

- (IBAction)rewindButtonPressed:(id)sender
{
    
}

- (IBAction)sliderChanged:(id)sender
{
    if(Music){
		MADSetMusicStatus(MADDriver, 0, [songPos maxValue], [songPos doubleValue]);
	}
}

- (IBAction)stopButtonPressed:(id)sender
{
    if (Music) {
		MADStopMusic(MADDriver);
		MADCleanDriver(MADDriver);
		MADSetMusicStatus(MADDriver, 0, 100, 0);
		self.paused = YES;
	}
}

- (IBAction)pauseButtonPressed:(id)sender {
	if (Music) {
		if (paused) {
			MADPlayMusic(MADDriver);
		} else {
			MADStopMusic(MADDriver);
			MADCleanDriver(MADDriver);
		}
		self.paused = !self.paused;
	}
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
	NSInteger selected = [tableView selectedRow];
	do {
		if (selected < 0)
			break;
		
		PPMusicListObject *obj = [musicList objectInMusicListAtIndex:selected];
		
		NSURL *musicURL = obj.musicUrl;
		PPInfoRec theInfo;
		char info[5] = {0};
		if(MADMusicIdentifyCFURL(MADLib, info, BRIDGE(CFURLRef, musicURL)) != noErr) break;
		if(MADMusicInfoCFURL(MADLib, info, BRIDGE(CFURLRef, musicURL), &theInfo) != noErr) break;
		[fileName setTitleWithMnemonic:obj.fileName];
		[internalName setTitleWithMnemonic:[NSString stringWithCString:theInfo.internalFileName encoding:NSMacOSRomanStringEncoding]];
		[fileSize setTitleWithMnemonic:[NSString stringWithFormat:@"%.2f kiB", theInfo.fileSize / 1024.0]];
		[musicInstrument setTitleWithMnemonic:[NSString stringWithFormat:@"%d", theInfo.totalInstruments]];
		[musicPatterns setTitleWithMnemonic:[NSString stringWithFormat:@"%ld", (long)theInfo.totalPatterns]];
		[musicPlugType setTitleWithMnemonic:[NSString stringWithCString:theInfo.formatDescription encoding:NSMacOSRomanStringEncoding]];
		{
			char sig[5] = {0};
			OSType2Ptr(theInfo.signature, sig);
			[musicSignature setTitleWithMnemonic:[NSString stringWithCString:sig encoding:NSMacOSRomanStringEncoding]];
		}
		[fileLocation setTitleWithMnemonic:[musicURL path]];
		return;
	} while (0);
	
	[fileName setTitleWithMnemonic:PPDoubleDash];
	[internalName setTitleWithMnemonic:PPDoubleDash];
	[fileSize setTitleWithMnemonic:PPDoubleDash];
	[musicInstrument setTitleWithMnemonic:PPDoubleDash];
	[musicPatterns setTitleWithMnemonic:PPDoubleDash];
	[musicPlugType setTitleWithMnemonic:PPDoubleDash];
	[musicSignature setTitleWithMnemonic:PPDoubleDash];
	[fileLocation setTitleWithMnemonic:PPDoubleDash];
}

- (void)musicListDidChange
{
	if (currentlyPlayingIndex.index != -1) {
		MADStopMusic(MADDriver);
		MADCleanDriver(MADDriver);
		MADDisposeMusic(&Music, MADDriver);
	}
	if ([[NSUserDefaults standardUserDefaults] boolForKey:PPLoadMusicAtListLoad] && [musicList countOfMusicList] > 0) {
		NSError *err = nil;
		currentlyPlayingIndex.index = 0;
		[self selectCurrentlyPlayingMusic];
		if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err])
		{
			NSAlert *alert = [NSAlert alertWithError:err];
			[alert runModal];
		}
	}
	NSUInteger lostCount = musicList.lostMusicCount;
	if (lostCount) {
		NSRunAlertPanel(@"Unresolvable files", @"There were %lu file(s) that were unable to be resolved.", nil, nil, nil, (unsigned long)lostCount);
	}
}

- (BOOL)musicListWillChange
{
	if (Music) {
		if (Music->hasChanged) {
			if (currentlyPlayingIndex.index == -1) {
				return YES;
			} else {
				NSInteger selection = NSRunAlertPanel(NSLocalizedString(@"Unsaved Changes", @"Unsaved Changes"), NSLocalizedString(@"The music file \"%@\" has unsaved changes. Do you want to save?", @"file is unsaved"), NSLocalizedString(@"Save", @"Save"), NSLocalizedString(@"Don't Save", @"Don't save"), NSLocalizedString(@"Cancel", @"Cancel"), [[musicList objectInMusicListAtIndex:currentlyPlayingIndex.index] fileName]);
				switch (selection) {
					case NSAlertDefaultReturn:
						[self saveMusic:nil];
					case NSAlertAlternateReturn:
					default:
						return YES;
						break;
						
					case NSAlertOtherReturn:
						return NO;
						break;
				}
			}
		}
	}
	return YES;
}

- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename
{
	NSError *err = nil;
	NSString *utiFile = [[NSWorkspace sharedWorkspace] typeOfFile:filename error:&err];
	if (err) {
		NSRunAlertPanel(@"Error opening file", @"Unable to open %@: %@", nil, nil, nil, [filename lastPathComponent], [err localizedFailureReason]);
		return NO;
	}
	return [self handleFile:[NSURL fileURLWithPath:filename] ofType:utiFile];
}

#pragma mark PPSoundSettingsViewControllerDelegate methods

- (void)soundOutBitsDidChange:(short)bits
{
	exportSettings.outPutBits = bits;
}

- (void)soundOutRateDidChange:(unsigned int)rat
{
	exportSettings.outPutRate = rat;
}

- (void)soundOutReverbDidChangeActive:(BOOL)isAct
{
	exportSettings.Reverb = isAct;
}

- (void)soundOutOversamplingDidChangeActive:(BOOL)isAct
{
	if (!isAct) {
		exportSettings.oversampling = 1;
	}
}

- (void)soundOutStereoDelayDidChangeActive:(BOOL)isAct
{
	if (!isAct) {
		exportSettings.MicroDelaySize = 0;
	}
}

- (void)soundOutSurroundDidChangeActive:(BOOL)isAct
{
	exportSettings.surround = isAct;
}

- (void)soundOutReverbStrengthDidChange:(short)rev
{
	exportSettings.ReverbStrength = rev;
}

- (void)soundOutReverbSizeDidChange:(short)rev
{
	exportSettings.ReverbSize = rev;
}

- (void)soundOutOversamplingAmountDidChange:(short)ovs
{
	exportSettings.oversampling = ovs;
}

- (void)soundOutStereoDelayAmountDidChange:(short)std
{
	exportSettings.MicroDelaySize = std;
}

@end
