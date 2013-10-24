//
//  AppDelegate.m
//  PlayerPRO Player
//
//  Created by C.W. Betts on 9/6/13.
//
//

#import "AppDelegate.h"
#import "PPPreferences.h"
#import "PPMusicList.h"
#import "UserDefaultKeys.h"
#import "PPErrors.h"
#import "PPInstrumentWindowController.h"
#import "PPPlugInInfo.h"
#import "PPPlugInInfoController.h"
#import "OpenPanelViewController.h"
#include <PlayerPROCore/RDriverInt.h>
#include "PPByteswap.h"
#import <QTKit/QTKit.h>
#import <QTKit/QTExportSession.h>
#import <QTKit/QTExportOptions.h>

#define kUnresolvableFile @"Unresolvable files"
#define kUnresolvableFileDescription @"There were %lu file(s) that were unable to be resolved."

@interface PPCurrentlyPlayingIndex : NSObject
@property (readwrite) NSInteger index;
@property (readwrite, strong) NSURL *playbackURL;

- (void)movePlayingIndexToOtherIndex:(PPCurrentlyPlayingIndex *)othidx;

@end

@implementation PPCurrentlyPlayingIndex
@synthesize index;
@synthesize playbackURL;

- (void)movePlayingIndexToOtherIndex:(PPCurrentlyPlayingIndex *)othidx
{
	othidx.index = index;
	othidx.playbackURL = playbackURL;
}

- (NSString *)description
{
	return [NSString stringWithFormat:@"Index: %ld URL: %@ URL Path: %@", (long)index, playbackURL, [playbackURL path]];
}
@end

static void CocoaDebugStr( short line, const char *file, const char *text)
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

static NSInteger selMusFromList = -1;

@interface AppDelegate ()
- (void)selectCurrentlyPlayingMusic;
- (void)selectMusicAtIndex:(NSInteger)anIdx;
- (void)musicListContentsDidMove;
- (BOOL)musicListWillChange;
- (void)musicListDidChange;
- (void)moveMusicAtIndex:(NSUInteger)from toIndex:(NSUInteger)to;
@property (strong) NSString *musicInfo;
@property (strong, nonatomic, readonly) NSDictionary *trackerDict;
@property (strong) PPMusicList				*musicList;
@property (strong) PPCurrentlyPlayingIndex	*currentlyPlayingIndex;
@property (strong) PPCurrentlyPlayingIndex	*previouslyPlayingIndex;
@property (strong) PPPreferences			*preferences;
@property (strong) NSMutableArray			*plugInInfos;
@property BOOL isQuitting;
@end


@implementation AppDelegate
@synthesize toolsPanel;
@synthesize instrumentController;
@synthesize musicList;
@synthesize currentlyPlayingIndex, previouslyPlayingIndex;
@synthesize preferences;
@synthesize isQuitting;

@synthesize trackerDict = _trackerDict;
- (NSDictionary *)trackerDict
{
	if (!_trackerDict || [_trackerDict count] != madLib->TotalPlug - 1) {
		NSMutableDictionary *trackerDict = [NSMutableDictionary dictionaryWithDictionary:@{
											NSLocalizedStringWithDefaultValue(@"PPMADKFile", @"InfoPlist",
																				[NSBundle mainBundle],
																				@"MADK Tracker", @"MADK Tracker") : @[MADNativeUTI],
											NSLocalizedString(@"Generic MAD tracker", @"Generic MAD tracker"): @[MADGenericUTI],
											NSLocalizedString(@"MAD Package", @"MAD Package"):@[MADPackageUTI]}];
		for (int i = 0; i < madLib->TotalPlug; i++) {
			trackerDict[(__bridge NSString*)madLib->ThePlug[i].MenuName] = (__bridge NSArray*)madLib->ThePlug[i].UTItypes;
		}
		_trackerDict = [[NSDictionary alloc] initWithDictionary:trackerDict];
	}
	
	return _trackerDict;
}

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
				[[NSAlert alertWithError:err] runModal];
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
			[[NSAlert alertWithError:err] runModal];
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
	if (madDriver) {
		madWasReading = MADIsPlayingMusic(madDriver);
		MADStopMusic(madDriver);
		MADStopDriver(madDriver);
		if (madWasReading) {
			MADGetMusicStatus(madDriver, &fullTime, &curTime);
		}
		MADDisposeDriver(madDriver);
		madDriver = NULL;
	}
	MADDriverSettings init;
	MADGetBestDriver(&init);
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	//TODO: Sanity Checking
	init.surround = [defaults boolForKey:PPSurroundToggle];
	init.outPutRate = (unsigned int)[defaults integerForKey:PPSoundOutRate];
	init.outPutBits = [defaults integerForKey:PPSoundOutBits];
	if ([defaults boolForKey:PPOversamplingToggle]) {
		init.oversampling = (SInt32)[defaults integerForKey:PPOversamplingAmount];
	} else {
		init.oversampling = 1;
	}
	init.Reverb = [defaults boolForKey:PPReverbToggle];
	init.ReverbSize = (SInt32)[defaults integerForKey:PPReverbAmount];
	init.ReverbStrength = (SInt32)[defaults integerForKey:PPReverbStrength];
	if ([defaults boolForKey:PPStereoDelayToggle]) {
		init.MicroDelaySize = (SInt32)[defaults integerForKey:PPStereoDelayAmount];
	} else {
		init.MicroDelaySize = 0;
	}
	
	init.driverMode = [defaults integerForKey:PPSoundDriver];
	init.repeatMusic = FALSE;
	
	OSErr returnerr = MADCreateDriver(&init, madLib, &madDriver);
	[[NSNotificationCenter defaultCenter] postNotificationName:PPDriverDidChange object:self];
	if (returnerr != noErr) {
		NSError *err = CreateErrorFromMADErrorType(returnerr);
		[[NSAlert alertWithError:err] runModal];
		return;
	}
	MADStartDriver(madDriver);
	if (music) {
		MADAttachDriverToMusic(madDriver, music, NULL);
		if (madWasReading) {
			MADSetMusicStatus(madDriver, 0, fullTime, curTime);
			MADPlayMusic(madDriver);
		}
	}
}

+ (void)initialize
{
	static dispatch_once_t onceToken;
	dispatch_once(&onceToken, ^{
		PPMusicList *tempList = [[PPMusicList alloc] init];
		[[NSUserDefaults standardUserDefaults] registerDefaults:@{PPRememberMusicList: @YES,
																  PPLoadMusicAtListLoad: @NO,
																  PPAfterPlayingMusic: @(PPStopPlaying),
																  PPGotoStartupAfterPlaying: @YES,
																  PPSaveModList: @YES,
																  PPLoadMusicAtMusicLoad: @NO,
																  PPLoopMusicWhenDone: @NO,
																  
																  PPSoundOutBits: @16,
																  PPSoundOutRate: @44100,
																  PPSoundDriver: @(CoreAudioDriver),
																  PPStereoDelayToggle: @YES,
																  PPReverbToggle: @NO,
																  PPSurroundToggle: @NO,
																  PPOversamplingToggle: @NO,
																  PPStereoDelayAmount: @30,
																  PPReverbAmount: @25,
																  PPReverbStrength: @30,
																  PPOversamplingAmount: @1,
																  
																  
																  PPMAddExtension: @YES,
																  PPMMadCompression: @YES,
																  PPMNoLoadMixerFromFiles: @NO,
																  PPMOscilloscopeDrawLines: @YES,
																  
																  PPMMusicList: [NSKeyedArchiver archivedDataWithRootObject:tempList]}];
		
	});
	
}

- (IBAction)showMusicList:(id)sender
{
    [self.window makeKeyAndOrderFront:sender];
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
			MADStopMusic(madDriver);
			MADCleanDriver(madDriver);
			if ([userDefaults boolForKey:PPGotoStartupAfterPlaying]) {
				MADSetMusicStatus(madDriver, 0, 100, 0);
			}
			self.paused = YES;
			break;
			
		case PPLoopMusic:
			MADSetMusicStatus(madDriver, 0, 100, 0);
			MADPlayMusic(madDriver);
			break;
			
		case PPLoadNext:
		{
			if ([musicList countOfMusicList] > ++currentlyPlayingIndex.index) {
				[self selectCurrentlyPlayingMusic];
				NSError *err = nil;
				if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err])
				{
					[[NSAlert alertWithError:err] runModal];
				}
			} else {
				if ([userDefaults boolForKey:PPLoopMusicWhenDone]) {
					currentlyPlayingIndex.index = 0;
					[self selectCurrentlyPlayingMusic];
					NSError *err = nil;
					if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err])
					{
						[[NSAlert alertWithError:err] runModal];
					}
				} else {
					MADStopMusic(madDriver);
					MADCleanDriver(madDriver);
					if ([userDefaults boolForKey:PPGotoStartupAfterPlaying]) {
						MADSetMusicStatus(madDriver, 0, 100, 0);
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
				[[NSAlert alertWithError:err] runModal];
			}
		}
			break;
	}
}

- (void)updateMusicStats:(NSTimer*)theTimer
{
	if (music) {
		long fT, cT;
		MADGetMusicStatus(madDriver, &fT, &cT);
		if (MADIsDonePlaying(madDriver) && !self.paused && !MADIsExporting(madDriver)) {
			[self songIsDonePlaying];
			MADGetMusicStatus(madDriver, &fT, &cT);
		}
		[songPos setDoubleValue:cT];
		[songCurTime setIntegerValue:cT];
	}
}

- (void)saveMusicToURL:(NSURL *)tosave
{
	int i, x;
	size_t inOutCount;
	MADCleanCurrentMusic(music, madDriver);
	NSMutableData *saveData = [[NSMutableData alloc] initWithCapacity:MADGetMusicSize(music)];
	for( i = 0, x = 0; i < MAXINSTRU; i++)
	{
		music->fid[ i].no = i;
		
		if (music->fid[ i].numSamples > 0 || music->fid[ i].name[ 0] != 0)	// Is there something in this instrument?
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
		BOOL compressMAD = [[NSUserDefaults standardUserDefaults] boolForKey:PPMMadCompression];
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
		if (music->fid[ i].numSamples > 0 || music->fid[ i].name[ 0] != 0)	// Is there something in this instrument?
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
			char *dataCopy = malloc(inOutCount);
			memcpy(dataCopy, curData.data, inOutCount);
			if (curData.amp == 16)
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
		if (music->header->globalEffect[ i])
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
			if (music->header->chanEffect[ i][ x])
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
}

//Yes, the pragma pack is needed
//otherwise the data will be improperly mapped.
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

- (NSData *)newAIFFDataFromSettings:(MADDriverSettings*)sett data:(NSData*)dat
{
	//TODO: Write a little-endian AIFF exporter
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
		NSData *macRomanNameData = [self.musicName dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
		macRomanNameLength = [macRomanNameData length];
		BOOL isPadded = (macRomanNameLength & 1);
		
		nameChunk.ckSize = (SInt32)(macRomanNameLength + 1);
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
		NSData *macRomanInfoData = [self.musicInfo dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
		macRomanInfoLength = [macRomanInfoData length];
		BOOL isPadded = (macRomanInfoLength & 1);
		infoChunk.ckSize = (SInt32)(macRomanInfoLength + 1);
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
	
	NSMutableData *returnData = [[NSMutableData alloc] initWithCapacity:dataLen + sizeof(CommonChunk) + sizeof(SoundDataChunk) + sizeof(ContainerChunk) + [nameData length] + [infoData length]];
	header.ckID = FORMID;
	PPBE32(&header.ckID);
	header.ckSize = (SInt32)(dataLen + sizeof(CommonChunk) + sizeof(SoundDataChunk) + 4 + [nameData length] + [infoData length]);
	PPBE32(&header.ckSize);
	header.formType = AIFFID;
	PPBE32(&header.formType);
	[returnData appendBytes:&header length:sizeof(ContainerChunk)];
	
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
		container.numSampleFrames = (SInt32)(dataLen / todiv);
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
	dataChunk.ckSize = (SInt32)(dataLen + 8 + dataOffset);
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
	
	OSErr err = MADCreateDriver( theSet, madLib, &theRec);
	if (err != noErr) {
		dispatch_async(dispatch_get_main_queue(), ^{
			NSError *NSerr = CreateErrorFromMADErrorType(err);
			[[NSAlert alertWithError:NSerr] runModal];
		});
		
		return nil;
	}
	MADCleanDriver( theRec);
	
	MADAttachDriverToMusic( theRec, music, NULL);
	MADPlayMusic(theRec);
	
	char *soundPtr = NULL;
	long full = 0;
	switch (theset->outPutBits) {
		case 16:
			full = MADAudioLength(theRec) * 2;
			break;
			
		case 20:
		case 24:
			full = MADAudioLength(theRec) * 3;
			
		default:
		case 8:
			full = MADAudioLength(theRec);
			break;
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
	NSData *retData = [self newAIFFDataFromSettings:theSet data:mutData];
	mutData = nil;
	
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
	return [NSApp runModalForWindow:self.exportWindow];
}

#ifndef PPEXPORT_CREATE_TMP_AIFF
#define PPEXPORT_CREATE_TMP_AIFF 1
#endif

- (IBAction)exportMusicAs:(id)sender
{
	NSInteger tag = [sender tag];
	MADBeginExport(madDriver);
	
	switch (tag) {
		case -1:
			//AIFF
		{
			NSSavePanel *savePanel = [NSSavePanel savePanel];
			[savePanel setAllowedFileTypes:@[@"public.aiff-audio"]];
			[savePanel setCanCreateDirectories:YES];
			[savePanel setCanSelectHiddenExtension:YES];
			if (![self.musicName isEqualToString:@""]) {
				[savePanel setNameFieldStringValue:self.musicName];
			}
			[savePanel setPrompt:@"Export"];
			[savePanel setTitle:@"Export as AIFF audio"];
			if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
				if ([self showExportSettings] == NSAlertDefaultReturn) {
					dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
						NSData *saveData = [self getSoundData:&exportSettings];
						MADEndExport(madDriver);
						
						[saveData writeToURL:[savePanel URL] atomically:YES];
						saveData = nil;
						dispatch_async(dispatch_get_main_queue(), ^{
							if (isQuitting) {
								[NSApp replyToApplicationShouldTerminate:YES];
							} else {
								NSInteger retVal = NSRunInformationalAlertPanel(@"Export complete", @"The export of the file \"%@\" is complete.", @"Okay", @"Show File", nil, [[savePanel URL] lastPathComponent]);
								if (retVal == NSAlertAlternateReturn) {
									[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:@[[savePanel URL]]];
								}
							}
						});
					});
				} else {
					MADEndExport(madDriver);
				}
			} else {
				MADEndExport(madDriver);
			}
		}
			break;
			
		case -2:
			//MP4
		{
			NSSavePanel *savePanel = [NSSavePanel savePanel];
			[savePanel setAllowedFileTypes:@[@"com.apple.m4a-audio"]];
			[savePanel setCanCreateDirectories:YES];
			[savePanel setCanSelectHiddenExtension:YES];
			if (![self.musicName isEqualToString:@""]) {
				[savePanel setNameFieldStringValue:self.musicName];
			}
			[savePanel setPrompt:@"Export"];
			[savePanel setTitle:@"Export as MPEG-4 Audio"];
			if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
				if ([self showExportSettings] == NSAlertDefaultReturn) {
					dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
						NSData *saveData = [self getSoundData:&exportSettings];
						NSString *oldMusicName = self.musicName;
						NSString *oldMusicInfo = self.musicInfo;
						NSURL *oldURL = [[musicList objectInMusicListAtIndex:previouslyPlayingIndex.index] musicUrl];
						MADEndExport(madDriver);
						NSError *expErr = nil;
						dispatch_block_t errBlock = ^{
							if (isQuitting) {
								[NSApp replyToApplicationShouldTerminate:YES];
							}else {
								NSRunAlertPanel(@"Export failed", @"Export/coversion of the music file failed:\n%@", nil, nil, nil, [expErr localizedDescription]);
							}
						};
#if PPEXPORT_CREATE_TMP_AIFF
						NSURL *tmpURL = [[[NSFileManager defaultManager] URLForDirectory:NSItemReplacementDirectory inDomain:NSUserDomainMask appropriateForURL:oldURL create:YES error:nil] URLByAppendingPathComponent:[NSString stringWithFormat:@"%@.aiff", oldMusicName] isDirectory:NO];
						
						[saveData writeToURL:tmpURL atomically:NO];
						QTMovie *exportMov = [[QTMovie alloc] initWithURL:tmpURL error:&expErr];
						if (exportMov) {
							[exportMov setAttribute:oldMusicName forKey:QTMovieDisplayNameAttribute];
							[exportMov setAttribute:oldMusicInfo forKey:QTMovieCopyrightAttribute];
						}
#else
						//Attempts of using data directly have resulted in internal assertion failures in the export session initialization code
						QTDataReference *dataRef = [[QTDataReference alloc] initWithReferenceToData:saveData name:oldMusicName MIMEType:@"audio/aiff"];
						
						QTMovie *exportMov = [[QTMovie alloc] initWithAttributes:[NSDictionary dictionaryWithObjectsAndKeys:dataRef, QTMovieDataReferenceAttribute, @NO, QTMovieOpenAsyncOKAttribute, @YES, QTMovieDontInteractWithUserAttribute, @NO, QTMovieOpenForPlaybackAttribute, oldMusicName, QTMovieDisplayNameAttribute, oldMusicInfo, QTMovieCopyrightAttribute, nil] error:&expErr];
#endif
						oldMusicInfo = nil;
						saveData = nil;
						if (!exportMov) {
							NSLog(@"Init Failed for %@, error: %@", oldMusicName, [expErr localizedDescription]);
#if !PPEXPORT_CREATE_TMP_AIFF
#else
							[[NSFileManager defaultManager] removeItemAtURL:tmpURL error:NULL];
#endif
							dispatch_async(dispatch_get_main_queue(), errBlock);
							return;
						}
						
						QTExportSession *session = [[QTExportSession alloc] initWithMovie:exportMov exportOptions:[QTExportOptions exportOptionsWithIdentifier:QTExportOptionsAppleM4A] outputURL:[savePanel URL] error:&expErr];
						if (!session) {
							NSLog(@"Export session creation for %@ failed, error: %@", oldMusicName, [expErr localizedDescription]);
#if !PPEXPORT_CREATE_TMP_AIFF
#else
							[[NSFileManager defaultManager] removeItemAtURL:tmpURL error:NULL];
#endif
							dispatch_async(dispatch_get_main_queue(), errBlock);
							return;
						}
						[session run];
						BOOL didFinish = [session waitUntilFinished:&expErr];
						if (!didFinish)
						{
							NSLog(@"export of \"%@\" failed, error: %@", oldMusicName, [expErr localizedDescription]);
							dispatch_async(dispatch_get_main_queue(), errBlock);
						}
#if !PPEXPORT_CREATE_TMP_AIFF
#else
						[[NSFileManager defaultManager] removeItemAtURL:tmpURL error:NULL];
#endif
						if (didFinish) {
							dispatch_async(dispatch_get_main_queue(), ^{
								if (isQuitting) {
									[NSApp replyToApplicationShouldTerminate:YES];
								} else {
									NSInteger retVal = NSRunInformationalAlertPanel(@"Export complete", @"The export of the file \"%@\" is complete.", @"Okay", @"Show File", nil, [[savePanel URL] lastPathComponent]);
									if (retVal == NSAlertAlternateReturn) {
										[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:@[[savePanel URL]]];
									}
								}
							});
						}
						
					});
				} else {
					MADEndExport(madDriver);
				}
			} else {
				MADEndExport(madDriver);
			}
		}
			break;
			
		default:
		{
			if (tag > madLib->TotalPlug || tag < 0) {
				NSBeep();
				MADEndExport(madDriver);
				if (isQuitting) {
					[NSApp replyToApplicationShouldTerminate:YES];
				}
				
				return;
			}
			NSSavePanel *savePanel = [NSSavePanel savePanel];
			[savePanel setAllowedFileTypes:(__bridge NSArray*) madLib->ThePlug[tag].UTItypes];
			[savePanel setCanCreateDirectories:YES];
			[savePanel setCanSelectHiddenExtension:YES];
			if (![self.musicName isEqualToString:@""]) {
				[savePanel setNameFieldStringValue:self.musicName];
			}
			[savePanel setPrompt:@"Export"];
			[savePanel setTitle:[NSString stringWithFormat:@"Export as %@", (__bridge NSString*) madLib->ThePlug[tag].MenuName]];
			if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
				NSURL *fileURL = [savePanel URL];
				OSErr err = MADMusicExportCFURL(madLib, music, madLib->ThePlug[tag].type, (__bridge CFURLRef)fileURL);
				if (err != noErr) {
					if (isQuitting) {
						[NSApp replyToApplicationShouldTerminate:YES];
					} else {
						NSError *aerr = CreateErrorFromMADErrorType(err);
						[[NSAlert alertWithError:aerr] runModal];
					}
				} else {
					[self addMusicToMusicList:fileURL loadIfPreferencesAllow:NO];
					if (isQuitting) {
						[NSApp replyToApplicationShouldTerminate:YES];
					} else {
						NSInteger retVal = NSRunInformationalAlertPanel(@"Export complete", @"The export of the file \"%@\" is complete.", @"Okay", @"Show File", nil, [[savePanel URL] lastPathComponent]);
						if (retVal == NSAlertAlternateReturn) {
							[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:@[fileURL]];
						}
					}
				}
			}
		}
			MADEndExport(madDriver);
			break;
	}
}

- (IBAction)okayExportSettings:(id)sender {
	[NSApp stopModalWithCode:NSAlertDefaultReturn];
	[self.exportWindow close];
}

- (IBAction)cancelExportSettings:(id)sender {
	[NSApp stopModalWithCode:NSAlertAlternateReturn];
	[self.exportWindow close];
}

- (IBAction)saveMusicAs:(id)sender
{
	MADBeginExport(madDriver);
	
	NSSavePanel * savePanel = [NSSavePanel savePanel];
	[savePanel setAllowedFileTypes:@[MADNativeUTI]];
	[savePanel setCanCreateDirectories:YES];
	[savePanel setCanSelectHiddenExtension:YES];
	if (![self.musicName isEqualToString:@""]) {
		[savePanel setNameFieldStringValue:self.musicName];
	}
	if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
		NSURL *saveURL = [savePanel URL];
		[self saveMusicToURL:saveURL];
		[self addMusicToMusicList:saveURL loadIfPreferencesAllow:NO];
	}
	MADEndExport(madDriver);
}

- (IBAction)saveMusic:(id)sender
{
	MADBeginExport(madDriver);
	
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
			MADEndExport(madDriver);
		} else {
			// saveMusicAs: will end the exporting when it is done.
			[self saveMusicAs:sender];
		}
	}
}

- (BOOL)loadMusicURL:(NSURL*)musicToLoad error:(out NSError *__autoreleasing*)theErr
{
	if (music != NULL) {
		MADStopMusic(madDriver);
		MADCleanDriver(madDriver);
		MADDisposeMusic(&music, madDriver);
	}
	
	char fileType[5];
	OSErr theOSErr = MADMusicIdentifyCFURL(madLib, fileType, (__bridge CFURLRef) musicToLoad);
	
	if(theOSErr != noErr)
	{
		if (theErr) {
			*theErr = CreateErrorFromMADErrorType(theOSErr);
		}
		self.paused = YES;
		[self clearMusic];
		return NO;
	}
	theOSErr = MADLoadMusicCFURLFile(madLib, &music, fileType, (__bridge CFURLRef)musicToLoad);
	
	if (theOSErr != noErr) {
		if (theErr) {
			*theErr = CreateErrorFromMADErrorType(theOSErr);
		}
		self.paused = YES;
		[self clearMusic];
		return NO;
	}
	
	MADAttachDriverToMusic(madDriver, music, NULL);
	MADPlayMusic(madDriver);
	self.paused = NO;
	{
		long fT, cT;
		MADGetMusicStatus(madDriver, &fT, &cT);
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
		id boolVal = change[NSKeyValueChangeNewKey];
		
		//[pauseButton highlight:[boolVal boolValue]];
		switch ([boolVal boolValue]) {
			case NO:
			default:
				[pauseButton setState:NSOffState];
				[self.pauseDockMenuItem setState:NSOnState];
				break;
				
			case YES:
				[pauseButton setState:NSOnState];
				[self.pauseDockMenuItem setState:NSOffState];
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
		[[NSAlert alertWithError:err] runModal];
	}
}

- (IBAction)showPlugInInfo:(id)sender
{
	PPPlugInInfo *inf = (self.plugInInfos)[[sender tag]];
	if (!inf) {
		return;
	}
	
	PPPlugInInfoController *infoCont = [[PPPlugInInfoController alloc] initWithPlugInInfo:inf];
	[[infoCont window] center];
	[NSApp runModalForWindow:[infoCont window]];
	//[infoCont showWindow:sender];
}

- (void)updatePlugInInfoMenu
{
	NSInteger i;
	
	//[plugInInfos removeAllObjects];
	
	for (i = 0; i < madLib->TotalPlug ; i++) {
		PPPlugInInfo *tmpInfo = [[PPPlugInInfo alloc] initWithPlugName:(__bridge NSString*) madLib->ThePlug[i].MenuName author:(__bridge NSString*) madLib->ThePlug[i].AuthorString plugType:NSLocalizedString(@"TrackerPlugName", @"Tracker plug-in name") plugURL:CFBridgingRelease(CFBundleCopyBundleURL(madLib->ThePlug[i].file))];
		if (![self.plugInInfos containsObject:tmpInfo]) {
			[self.plugInInfos addObject:tmpInfo];
		}
	}
		
	[self.plugInInfos sortWithOptions:NSSortConcurrent usingComparator:^NSComparisonResult(id obj1, id obj2) {
		NSString *menuNam1 = [obj1 plugName];
		NSString *menuNam2 = [obj2 plugName];
		NSComparisonResult res = [menuNam1 localizedStandardCompare:menuNam2];
		return res;
	}];
	
	[aboutPlugInMenu removeAllItems];
	
	for (i = 0; i < [self.plugInInfos count]; i++) {
		PPPlugInInfo *pi = (self.plugInInfos)[i];
		NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:pi.plugName action:@selector(showPlugInInfo:) keyEquivalent:@""];
		[mi setTag:i];
		[mi setTarget:self];
		[aboutPlugInMenu addItem:mi];
	}
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	isQuitting = NO;
	srandom(time(NULL) & 0xffffffff);
	PPRegisterDebugFunc(CocoaDebugStr);
	MADInitLibrary(NULL, &madLib);
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
	NSInteger selMus = musicList.selectedMusic;
	[self didChangeValueForKey:kMusicListKVO];
	
	[tableView setDoubleAction:@selector(doubleClickMusicList)];
	
	NSNotificationCenter *defaultCenter = [NSNotificationCenter defaultCenter];
	[defaultCenter addObserver:self selector:@selector(soundPreferencesDidChange:) name:PPSoundPreferencesDidChange object:nil];
	
	[self MADDriverWithPreferences];
	music = CreateFreeMADK();
	MADAttachDriverToMusic(madDriver, music, NULL);
	[self setTitleForSongLabelBasedOnMusic];
	
	NSInteger i;
	for (i = 0; i < madLib->TotalPlug; i++) {
		if (madLib->ThePlug[i].mode == MADPlugImportExport || madLib->ThePlug[i].mode == MADPlugExport) {
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:[NSString stringWithFormat:@"%@...", (__bridge NSString*) madLib->ThePlug[i].MenuName] action:@selector(exportMusicAs:) keyEquivalent:@""];
			[mi setTag:i];
			[mi setTarget:self];
			[musicExportMenu addItem:mi];
		}
	}
	
	self.plugInInfos = [[NSMutableArray alloc] init];
	[self updatePlugInInfoMenu];
	
	previouslyPlayingIndex = [[PPCurrentlyPlayingIndex alloc] init];
	previouslyPlayingIndex.index = -1;
	currentlyPlayingIndex = [[PPCurrentlyPlayingIndex alloc] init];
	[previouslyPlayingIndex movePlayingIndexToOtherIndex:currentlyPlayingIndex];
	
	exportController = [[PPSoundSettingsViewController alloc] init];
	exportController.delegate = self;
	[exportSettingsBox setContentView:[exportController view]];
	
	instrumentController = [[PPInstrumentWindowController alloc] init];
	instrumentController.curMusic = &music;
	instrumentController.theDriver = &madDriver;
	
	timeChecker = [[NSTimer alloc] initWithFireDate:[NSDate dateWithTimeIntervalSinceNow:0] interval:1/8.0 target:self selector:@selector(updateMusicStats:) userInfo:nil repeats:YES];
	[[NSRunLoop mainRunLoop] addTimer:timeChecker forMode:NSRunLoopCommonModes];
	NSUInteger lostCount = musicList.lostMusicCount;
	if (lostCount) {
		NSRunAlertPanel(kUnresolvableFile, kUnresolvableFileDescription, nil, nil, nil, (unsigned long)lostCount);
	}
	if (selMus != -1) {
		[self selectMusicAtIndex:selMus];
	}
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
	if (MADIsExporting(madDriver)) {
		NSInteger selection = NSRunAlertPanel(@"Exporting", @"PlayerPRO is currently exporting a tracker file.\nQuitting will stop this. Do you really wish to quit?", @"Wait", @"Quit", @"Cancel");
		switch (selection) {
			default:
			case NSAlertOtherReturn:
				return NSTerminateCancel;
				break;
				
			case NSAlertAlternateReturn:
				return NSTerminateNow;
				break;
				
			case NSAlertDefaultReturn:
				//Double-check to make sure we're still exporting
				if (MADIsExporting(madDriver)) {
					isQuitting = YES;
					return NSTerminateLater;
				} else {
					return NSTerminateNow;
				}
				
				break;
		}
	}
	return NSTerminateNow;
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
	[timeChecker invalidate];
	
	[self removeObserver:self forKeyPath:@"paused"];
		
	if (music != NULL) {
		if (music->hasChanged) {
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
		MADStopMusic(madDriver);
		MADCleanDriver(madDriver);
		MADDisposeMusic(&music, madDriver);
	}
	MADStopDriver(madDriver);
	MADDisposeDriver(madDriver);
	MADDisposeLibrary(madLib);
	if ([[NSUserDefaults standardUserDefaults] boolForKey:PPRememberMusicList]) {
		[musicList saveMusicListToPreferences];
	} else {
		[[NSUserDefaults standardUserDefaults] removeObjectForKey:PPMMusicList];
	}
	
	[[NSNotificationCenter defaultCenter] removeObserver:self];
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
		[[NSAlert alertWithError:error] runModal];
	}
}

- (IBAction)addMusic:(id)sender
{
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	
	OpenPanelViewController *av = [[OpenPanelViewController alloc] initWithOpenPanel:panel trackerDictionary:self.trackerDict playlistDictionary:nil instrumentDictionary:nil additionalDictionary:nil];
	[av setupDefaults];
	if([panel runModal] == NSFileHandlingPanelOKButton)
	{
		[self addMusicToMusicList:[panel URL]];
	}
}

- (IBAction)toggleInfo:(id)sender
{
	[infoDrawer toggle:sender];
}

- (void)setTitleForSongLabelBasedOnMusic
{
	self.musicName = [[NSString alloc] initWithCString:music->header->name encoding:NSMacOSRomanStringEncoding];
	self.musicInfo = [[NSString alloc] initWithCString:music->header->infos encoding:NSMacOSRomanStringEncoding];
}

- (void)clearMusic
{
	if (music) {
		MADStopMusic(madDriver);
		MADCleanDriver(madDriver);
		MADDisposeMusic(&music, madDriver);
	}
	
	self.paused = YES;
	currentlyPlayingIndex.index = -1;
	currentlyPlayingIndex.playbackURL = nil;
	[currentlyPlayingIndex movePlayingIndexToOtherIndex:previouslyPlayingIndex];
	music = CreateFreeMADK();
	[self setTitleForSongLabelBasedOnMusic];
	[[NSNotificationCenter defaultCenter] postNotificationName:PPMusicDidChange object:self];
	MADAttachDriverToMusic(madDriver, music, NULL);
}

- (IBAction)removeSelectedMusic:(id)sender
{
	NSIndexSet *selMusic = [tableView selectedRowIndexes];
	if ([selMusic containsIndex:currentlyPlayingIndex.index]) {
		if (music->hasChanged) {
			NSInteger selection = NSRunAlertPanel(NSLocalizedString(@"Unsaved Changes", @"Unsaved Changes"), NSLocalizedString(@"The music file \"%@\" has unsaved changes. Do you want to save?", @"edited file"), NSLocalizedString(@"Save", @"Save"), NSLocalizedString(@"Don't Save", @"Don't Save"), NSLocalizedString(@"Cancel", @"Cancel"), [[musicList objectInMusicListAtIndex:currentlyPlayingIndex.index] fileName]);
			switch (selection) {
				case NSAlertDefaultReturn:
					[self saveMusic:sender];
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
	[musicList removeObjectsInMusicListAtIndexes:selMusic];
	[self didChangeValueForKey:kMusicListKVO];
	[self musicListContentsDidMove];
}

- (IBAction)newMusic:(id)sender
{
	if (music->hasChanged) {
		NSInteger selection = 0;
		if (currentlyPlayingIndex.index == -1) {
			selection = NSRunAlertPanel(NSLocalizedString(@"Unsaved Changes", @"Unsaved Changes"), NSLocalizedString(@"The new music file has unsaved changes. Do you want to save?", @"New unsaved file"), NSLocalizedString(@"Save", @"Save"), NSLocalizedString(@"Don't Save", @"Don't Save"), NSLocalizedString(@"Cancel", @"Cancel"));
		} else {
			selection = NSRunAlertPanel(NSLocalizedString(@"Unsaved Changes", @"Unsaved Changes"), NSLocalizedString(@"The music file \"%@\" has unsaved changes. Do you want to save?", @"Open unsaved file"), NSLocalizedString(@"Save", @"Save"), NSLocalizedString(@"Don't Save", @"Don't Save"), NSLocalizedString(@"Cancel", @"Cancel"), [[musicList objectInMusicListAtIndex:currentlyPlayingIndex.index] fileName]);
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
			if (music->hasChanged && currentlyPlayingIndex.index != -1) {
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
			return YES;
			break;
			
		case PPToolbarFileInfo:
			if ([infoDrawer state] == NSDrawerOpeningState || [infoDrawer state] == NSDrawerOpenState) {
				return YES;
			}
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
	if ([sharedWorkspace type:theUTI conformsToType:MADPackageUTI]) {
		// Do nothing right now
		return NO;
	} else if ([theUTI isEqualToString:MADGenericUTI]) {
		NSInteger retVal = NSRunInformationalAlertPanel(NSLocalizedString(@"Invalid Extension", @"Invalid extension"), NSLocalizedString(@"The file %@ is identified as as a generic MAD tracker, and not a specific one. Renaming it will fix this. Do you want to rename the file extension?", @"Invalid extension description"), NSLocalizedString(@"Rename", @"rename file"), NSLocalizedString(@"Open", @"Open a file"), NSLocalizedString(@"Cancel", @"Cancel"), [theURL lastPathComponent]);
		switch (retVal) {
			case NSAlertDefaultReturn:
			{
				PPInfoRec rec;
				char ostype[5] = {0};
				//theURL = [theURL fileReferenceURL];
				if (MADMusicIdentifyCFURL(madLib, ostype, (__bridge CFURLRef) theURL) != noErr || MADMusicInfoCFURL(madLib, ostype, (__bridge CFURLRef) theURL, &rec) != noErr) {
					NSRunCriticalAlertPanel(NSLocalizedString(@"Unknown File", @"unknown file"), NSLocalizedString(@"The file type could not be identified.", @"Unidentified file"), nil, nil, nil);
					return NO;
				}
				OSType2Ptr(rec.signature, ostype);
				
				NSURL *tmpURL = [[theURL URLByDeletingPathExtension] URLByAppendingPathExtension:[[NSString stringWithCString:ostype encoding:NSMacOSRomanStringEncoding] lowercaseString]];
				NSError *err = nil;
				if (![[NSFileManager defaultManager] moveItemAtURL:theURL toURL:tmpURL error:&err]) {
					NSLog(@"Could not move file, error: %@", err);
					NSRunInformationalAlertPanel(NSLocalizedString(@"Rename Error", @"Rename Error"), NSLocalizedString(@"The file could not be renamed to \"%@\".\n\nThe music file \"%@\" will still be loaded.", @"Could not rename file"), nil, nil, nil, [tmpURL lastPathComponent], [theURL lastPathComponent]);
				} else {
					theURL = tmpURL;
					//TODO: regenerate the UTI
				}
			}
				break;
				
			case NSAlertAlternateReturn:
				break;
				
			case NSAlertOtherReturn:
			default:
				return NO;
				break;
		}
	}
	if ([sharedWorkspace type:theUTI conformsToType:PPMusicListUTI]) {
		if ([self musicListWillChange]) {
			[self willChangeValueForKey:kMusicListKVO];
			[musicList loadMusicListAtURL:theURL];
			selMusFromList = musicList.selectedMusic;
			[self didChangeValueForKey:kMusicListKVO];
			[self musicListDidChange];
			return YES;
		}
	} else if ([sharedWorkspace type:theUTI conformsToType:PPOldMusicListUTI]) {
		if ([self musicListWillChange]) {
			[self willChangeValueForKey:kMusicListKVO];
			[musicList loadOldMusicListAtURL:theURL];
			selMusFromList = musicList.selectedMusic;
			[self didChangeValueForKey:kMusicListKVO];
			[self musicListDidChange];
			return YES;
		}
	} else {
		{
			//TODO: check for valid extension.
			NSDictionary *tracDic = self.trackerDict;
			NSMutableArray *trackerUTIs = [NSMutableArray array];
			for (NSString *key in tracDic) {
				[trackerUTIs addObjectsFromArray:tracDic[key]];
			}

			for (NSString *aUTI in trackerUTIs) {
				if([sharedWorkspace type:theUTI conformsToType:aUTI])
				{
					[self addMusicToMusicList:theURL];
					return YES;
				}
			}
		}
	}
	return NO;
}

- (IBAction)openFile:(id)sender {
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	NSDictionary *playlistDict = @{@"PlayerPRO Music List" : @[PPMusicListUTI], @"PlayerPRO Old Music List" : @[PPOldMusicListUTI]};
	
	NSMutableDictionary *samplesDict = nil;
	
	NSDictionary *otherDict = nil;
	
	OpenPanelViewController *av = [[OpenPanelViewController alloc] initWithOpenPanel:panel trackerDictionary:self.trackerDict playlistDictionary:playlistDict instrumentDictionary:samplesDict additionalDictionary:otherDict];
	[av setupDefaults];
	if([panel runModal] == NSFileHandlingPanelOKButton)
	{
		NSURL *panelURL = [panel URL];
		NSString *filename = [panelURL path];
		NSError *err = nil;
		NSString *utiFile = [[NSWorkspace sharedWorkspace] typeOfFile:filename error:&err];
		if (err) {
			NSRunAlertPanel(@"Error opening file", @"Unable to open %@: %@", nil, nil, nil, [filename lastPathComponent], [err localizedFailureReason]);
			return;
		}
		[self handleFile:panelURL ofType:utiFile];
	}
}

- (IBAction)saveMusicList:(id)sender
{
	NSSavePanel *savePanel = [NSSavePanel savePanel];
	[savePanel setAllowedFileTypes:@[PPMusicListUTI]];
	[savePanel setCanCreateDirectories:YES];
	[savePanel setCanSelectHiddenExtension:YES];
	if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
		[musicList saveMusicListToURL:[savePanel URL]];
	}
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
			[[NSAlert alertWithError:err] runModal];
		}
	} else if ([[NSUserDefaults standardUserDefaults] boolForKey:PPLoopMusicWhenDone]) {
		currentlyPlayingIndex.index = 0;
		[self selectCurrentlyPlayingMusic];
		NSError *err = nil;
		if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err]) {
			[[NSAlert alertWithError:err] runModal];
		}
	} else NSBeep();
}

- (IBAction)playButtonPressed:(id)sender
{
	if (music) {
		MADPlayMusic(madDriver);
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
			[[NSAlert alertWithError:err] runModal];
		}
	} else NSBeep();
}

- (IBAction)rewindButtonPressed:(id)sender
{
    
}

- (IBAction)sliderChanged:(id)sender
{
    if(music){
		MADSetMusicStatus(madDriver, 0, [songPos maxValue], [songPos doubleValue]);
	}
}

- (IBAction)stopButtonPressed:(id)sender
{
    if (music) {
		MADStopMusic(madDriver);
		MADCleanDriver(madDriver);
		MADSetMusicStatus(madDriver, 0, 100, 0);
		self.paused = YES;
	}
}

- (IBAction)pauseButtonPressed:(id)sender {
	if (music) {
		if (self.paused) {
			MADPlayMusic(madDriver);
		} else {
			MADStopMusic(madDriver);
			MADCleanDriver(madDriver);
		}
		self.paused = !self.paused;
	}
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
	NSIndexSet *selected = [tableView selectedRowIndexes];
	do {
		if ([selected count] > 0) {
			musicList.selectedMusic = [selected firstIndex];
		}
		
		if ([selected count] != 1)
			break;
		
		PPMusicListObject *obj = [musicList objectInMusicListAtIndex:[selected lastIndex]];
		
		NSURL *musicURL = obj.musicUrl;
		PPInfoRec theInfo;
		char info[5] = {0};
		if(MADMusicIdentifyCFURL(madLib, info, (__bridge CFURLRef) musicURL) != noErr) break;
		if(MADMusicInfoCFURL(madLib, info, (__bridge CFURLRef) musicURL, &theInfo) != noErr) break;
		[fileName setTitleWithMnemonic:obj.fileName];
		[internalName setTitleWithMnemonic:[NSString stringWithCString:theInfo.internalFileName encoding:NSMacOSRomanStringEncoding]];
		[fileSize setIntegerValue:theInfo.fileSize];
		[musicInstrument setTitleWithMnemonic:[NSString stringWithFormat:@"%d", theInfo.totalInstruments]];
		[musicPatterns setTitleWithMnemonic:[NSString stringWithFormat:@"%ld", (long)theInfo.totalPatterns]];
		[musicPlugType setTitleWithMnemonic:[NSString stringWithCString:theInfo.formatDescription encoding:NSMacOSRomanStringEncoding]];
		{
			char sig[5] = {0};
			OSType2Ptr(theInfo.signature, sig);
			NSString *NSSig = [NSString stringWithCString:sig encoding:NSMacOSRomanStringEncoding];
			if (!NSSig) {
				NSSig = [NSString stringWithFormat:@"0x%08X", (unsigned int)theInfo.signature];
			}
			[musicSignature setTitleWithMnemonic:NSSig];
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

- (void)moveMusicAtIndex:(NSUInteger)from toIndex:(NSUInteger)to
{
	PPMusicListObject *obj = [musicList objectInMusicListAtIndex:from];
	[self willChangeValueForKey:kMusicListKVO];
	[musicList removeObjectInMusicListAtIndex:from];
	if (to > from) {
		to--;
	}
	[musicList insertObject:obj inMusicListAtIndex:to];
	[self didChangeValueForKey:kMusicListKVO];
	[self musicListContentsDidMove];
}

- (void)musicListDidChange
{
	if (currentlyPlayingIndex.index != -1) {
		MADStopMusic(madDriver);
		MADCleanDriver(madDriver);
		MADDisposeMusic(&music, madDriver);
	}
	if ([[NSUserDefaults standardUserDefaults] boolForKey:PPLoadMusicAtListLoad] && [musicList countOfMusicList] > 0) {
		NSError *err = nil;
		currentlyPlayingIndex.index = selMusFromList != -1 ? selMusFromList : 0;
		[self selectCurrentlyPlayingMusic];
		if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err])
		{
			[[NSAlert alertWithError:err] runModal];
		}
	} else if (selMusFromList != -1)
		[self selectMusicAtIndex:selMusFromList];
	NSUInteger lostCount = musicList.lostMusicCount;
	if (lostCount) {
		NSRunAlertPanel(kUnresolvableFile, kUnresolvableFileDescription, nil, nil, nil, (unsigned long)lostCount);
	}
}

- (BOOL)musicListWillChange
{
	if (music) {
		if (music->hasChanged) {
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
