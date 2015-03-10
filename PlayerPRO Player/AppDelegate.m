//
//  AppDelegate.m
//  PlayerPRO Player
//
//  Created by C.W. Betts on 9/6/13.
//
//

#import "AppDelegate.h"
#import "PPPreferences.h"
#import "UserDefaultKeys.h"
#import "PPInstrumentWindowController.h"
#include "PPByteswap.h"
#import <AVFoundation/AVFoundation.h>
#import "PlayerPRO_Player-Swift.h"

#define kUnresolvableFile @"Unresolvable files"
#define kUnresolvableFileDescription @"There were %lu file(s) that were unable to be resolved."

static void CocoaDebugStr(short line, const char *file, const char *text)
{
	NSLog(@"%s:%u error text:%s!", file, line, text);
	NSInteger alert = NSRunAlertPanel(NSLocalizedString(@"MyDebugStr_Error", @"Error"),
									  NSLocalizedString(@"MyDebugStr_MainText", @"The Main text to display"),
									  NSLocalizedString(@"MyDebugStr_Quit", @"Quit"),
									  NSLocalizedString(@"MyDebugStr_Continue", @"Continue"),
									  NSLocalizedString(@"MyDebugStr_Debug", @"Debug"), text);
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

@interface PlayerAppDelegate ()
@property (strong) NSString *musicInfo;
@property (strong, readonly) NSDictionary	*trackerDict;
@property (strong, readonly) NSArray		*trackerUTIs;
@property (strong) MusicList				*musicList;
@property (strong) CurrentlyPlayingIndex	*currentlyPlayingIndex;
@property (strong) CurrentlyPlayingIndex	*previouslyPlayingIndex;
@property (strong) Preferences				*preferences;
@property (strong) NSMutableArray			*plugInInfos;
@property (readwrite, strong) NSString		*musicName;
@property (getter = isQuitting) BOOL		quitting;

- (void)selectCurrentlyPlayingMusic;
- (void)selectMusicAtIndex:(NSInteger)anIdx;
- (void)musicListContentsDidMove;
- (BOOL)musicListWillChange;
- (void)musicListDidChange;
- (BOOL)loadMusicURL:(NSURL*)musicToLoad error:(out NSError *__autoreleasing*)theErr autoPlay:(BOOL)autoplay;
@end

@implementation PlayerAppDelegate
@synthesize madLib;
@synthesize toolsPanel;
@synthesize instrumentController;
@synthesize musicList;
@synthesize preferences;
@synthesize quitting = isQuitting;
@synthesize trackerUTIs = _trackerUTIs;
@synthesize trackerDict = _trackerDict;

- (NSDictionary *)trackerDict
{
	if (!_trackerDict) {
		NSMutableDictionary *trackerDict =
		[@{
		   NSLocalizedStringWithDefaultValue(@"PPMADKFile", @"InfoPlist",
											 [NSBundle mainBundle],
											 @"MADK Tracker", @"MADK Tracker") : @[MADNativeUTI],
		   NSLocalizedString(@"Generic MAD tracker", @"Generic MAD tracker"): @[MADGenericUTI]} mutableCopy];
		for (PPLibraryObject *obj in madLib) {
			trackerDict[obj.menuName] = obj.UTITypes;
		}
		_trackerDict = [[NSDictionary alloc] initWithDictionary:trackerDict];
	}
	
	return _trackerDict;
}

- (NSArray *)trackerUTIs
{
	if (!_trackerUTIs) {
		NSArray *arrayOfUTIs = [self.trackerDict allValues];
		NSMutableArray *toAddUTIArray = [NSMutableArray new];
		for (NSArray *anArray in arrayOfUTIs) {
			[toAddUTIArray addObjectsFromArray:anArray];
		}
		_trackerUTIs = [[NSArray alloc] initWithArray:toAddUTIArray];
	}
	
	return _trackerUTIs;
}

- (BOOL)loadMusicFromCurrentlyPlayingIndexWithError:(out NSError *__autoreleasing*)theErr
{
	self.currentlyPlayingIndex.playbackURL = [musicList URLAtIndex:self.currentlyPlayingIndex.index];
	BOOL isGood = [self loadMusicURL:self.currentlyPlayingIndex.playbackURL error:theErr];
	[self.currentlyPlayingIndex movePlayingIndexToOtherIndex:self.previouslyPlayingIndex];
	return isGood;
}

- (void)addMusicToMusicList:(NSURL* )theURL loadIfPreferencesAllow:(BOOL)load
{
	[self willChangeValueForKey:kMusicListKVO];
	BOOL okayMusic = [musicList addMusicURL:theURL force:NO];
	[self didChangeValueForKey:kMusicListKVO];
	if (!okayMusic) {
		NSInteger similarMusicIndex = [musicList indexOfObjectSimilarToURL:theURL];
		if (similarMusicIndex == NSNotFound) {
			return;
		}
		if (load && [[NSUserDefaults standardUserDefaults] boolForKey:PPLoadMusicAtMusicLoad]) {
			self.currentlyPlayingIndex.index = similarMusicIndex;
			[self selectCurrentlyPlayingMusic];
			NSError *err;
			if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err]) {
				[[NSAlert alertWithError:err] runModal];
			}
		} else {
			[self selectMusicAtIndex:similarMusicIndex];
		}
	} else if (load && [[NSUserDefaults standardUserDefaults] boolForKey:PPLoadMusicAtMusicLoad]) {
		self.currentlyPlayingIndex.index = [musicList countOfMusicList] - 1;
		//currentlyPlayingIndex.playbackURL = [musicList URLAtIndex:currentlyPlayingIndex.index];
		[self selectCurrentlyPlayingMusic];
		NSError *err;
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
	BOOL madWasReading = NO;
	long fullTime = 0, curTime = 0;
	NSError *returnerr;
	if (madDriver) {
		madWasReading = ![madDriver isPaused];
		[madDriver stop];
		//[madDriver stopDriver];
		
		[madDriver getMusicStatusWithCurrentTime:&curTime totalTime:&fullTime];
		
		//madDriver = nil;
	}
	MADDriverSettings init;
	MADGetBestDriver(&init);
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	//TODO: Sanity Checking
	init.surround = [defaults boolForKey:PPSurroundToggle];
	init.outPutRate = (unsigned int)[defaults integerForKey:PPSoundOutRate];
	init.outPutBits = [defaults integerForKey:PPSoundOutBits];
	if ([defaults boolForKey:PPOversamplingToggle]) {
		init.oversampling = (int)[defaults integerForKey:PPOversamplingAmount];
	} else {
		init.oversampling = 1;
	}
	init.Reverb = [defaults boolForKey:PPReverbToggle];
	init.ReverbSize = (int)[defaults integerForKey:PPReverbAmount];
	init.ReverbStrength = (int)[defaults integerForKey:PPReverbStrength];
	if ([defaults boolForKey:PPStereoDelayToggle]) {
		init.MicroDelaySize = (int)[defaults integerForKey:PPStereoDelayAmount];
	} else {
		init.MicroDelaySize = 0;
	}
	
	init.driverMode = [defaults integerForKey:PPSoundDriver];
	init.repeatMusic = FALSE;
	
	//OSErr returnerr = MADCreateDriver(&init, madLib, &madDriver);
	if (!madDriver)
		madDriver = [[PPDriver alloc] initWithLibrary:madLib settings:&init error:&returnerr];
	else
		returnerr = PPCreateErrorFromMADErrorType([madDriver changeDriverSettingsToSettings:&init]);
	[[NSNotificationCenter defaultCenter] postNotificationName:PPDriverDidChange object:self];
	if (returnerr != nil) {
		[[NSAlert alertWithError:returnerr] runModal];
		return;
	}
	//MADStartDriver(madDriver);
	if (self.music) {
		//MADAttachDriverToMusic(madDriver, music, NULL);
		if (madWasReading) {
			//MADSetMusicStatus(madDriver, 0, fullTime, curTime);
			[madDriver play];
		}
	}
}

+ (void)initialize
{
	[[NSUserDefaults standardUserDefaults]
	 registerDefaults:
	 @{PPRememberMusicList: @YES,
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
	   PPMOscilloscopeDrawLines: @YES}];
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
	if (self.currentlyPlayingIndex.index >= 0) {
		//currentlyPlayingIndex.playbackURL = [musicList URLAtIndex:currentlyPlayingIndex.index];
		[self selectMusicAtIndex:self.currentlyPlayingIndex.index];
	}
}

- (void)songIsDonePlaying
{
	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	switch ([userDefaults integerForKey:PPAfterPlayingMusic]) {
		case PPStopPlaying:
		default:
			[madDriver stop];
			[madDriver cleanDriver];
			if ([userDefaults boolForKey:PPGotoStartupAfterPlaying]) {
				madDriver.musicPosition = 0;
			}
			self.paused = YES;
			break;
			
		case PPLoopMusic:
			madDriver.musicPosition = 0;
			[madDriver play];
			break;
			
		case PPLoadNext:
			if ([musicList countOfMusicList] > ++self.currentlyPlayingIndex.index) {
				[self selectCurrentlyPlayingMusic];
				NSError *err;
				if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err]) {
					[[NSAlert alertWithError:err] runModal];
				}
			} else {
				if ([userDefaults boolForKey:PPLoopMusicWhenDone]) {
					self.currentlyPlayingIndex.index = 0;
					[self selectCurrentlyPlayingMusic];
					NSError *err;
					if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err])
					{
						[[NSAlert alertWithError:err] runModal];
					}
				} else {
					[madDriver stop];
					[madDriver cleanDriver];
					if ([userDefaults boolForKey:PPGotoStartupAfterPlaying]) {
						madDriver.musicPosition = 0;
					}
				}
			}
			break;
			
		case PPLoadRandom:
			self.currentlyPlayingIndex.index = random() % [musicList countOfMusicList];
			[self selectCurrentlyPlayingMusic];
			NSError *err;
			if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err]) {
				[[NSAlert alertWithError:err] runModal];
			}
			break;
	}
}

- (void)updateMusicStats:(NSTimer*)theTimer
{
	if (self.music) {
		long fT, cT;
		[madDriver getMusicStatusWithCurrentTime:&cT totalTime:&fT];
		if ([madDriver isDonePlayingMusic] && !self.paused && ! madDriver.isExporting) {
			[self songIsDonePlaying];
			[madDriver getMusicStatusWithCurrentTime:&cT totalTime:&fT];
		}
		[songPos setDoubleValue:cT];
		[songCurTime setIntegerValue:cT];
	}
}

- (void)saveMusicToURL:(NSURL *)tosave
{
	[self.music saveMusicToURL:tosave];
}

- (NSMutableData*)rawSoundData:(MADDriverSettings*)theSet
{
	NSError *err;
	PPDriver *theRec = [[PPDriver alloc] initWithLibrary:madLib settings:theSet error:&err];
	
	if (theRec == nil) {
		dispatch_async(dispatch_get_main_queue(), ^{
			NSError *NSerr = PPCreateErrorFromMADErrorType(MADUnknownErr);
			[[NSAlert alertWithError:NSerr] runModal];
		});
		
		return nil;
	}
	
	[theRec cleanDriver];
	[theRec setCurrentMusic:self.music];
	
	NSUInteger full = [theRec audioDataLength];
	if (theSet->outPutBits == 16) {
		full *= 2;
	} else if (theSet->outPutBits == 20 || theSet->outPutBits == 24 ) {
		full *= 3;
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
	
	[theRec play];
	
	NSMutableData *mutData = [[NSMutableData alloc] initWithCapacity:full * 60 * madDriver.totalMusicPlaybackTime / 2];
	
	NSData *tmpData;
	while ((tmpData = [theRec directSave])) {
		[mutData appendData:tmpData];
	}
	
	return mutData;
}

- (NSData*)rawBESoundData:(MADDriverSettings*)theSet
{
#ifdef __LITTLE_ENDIAN__
	NSMutableData *rsd = [self rawSoundData:theSet];
	if (theSet->outPutBits == 16) {
		size_t sndSize = [rsd length];
		short *bePtr = [rsd mutableBytes];
		dispatch_apply(sndSize / 2, dispatch_get_global_queue(0, 0), ^(size_t i) {
			MADBE16(&bePtr[i]);
		});
	}
	return [rsd copy];
#else
	return [[self rawSoundData:theSet] copy];
#endif
}

- (NSData*)rawLESoundData:(MADDriverSettings*)theSet
{
#ifdef __BIG_ENDIAN__
	NSMutableData *rsd = [self rawSoundData:theSet];
	if (theSet->outPutBits == 16) {
		size_t sndSize = [rsd length];
		short *lePtr = [rsd mutableBytes];
		dispatch_apply(sndSize / 2, dispatch_get_global_queue(0, 0), ^(size_t i) {
			MADLE16(&lePtr[i]);
		});
	}
	return [rsd copy];
#else
	return [[self rawSoundData:theSet] copy];
#endif
}

- (void)setMetadataToAudioFileID:(AudioFileID)theID
{
	//TODO:metadata
}

- (MADErr)saveMusicAsWaveToURL:(NSURL*)theURL usingSettings:(MADDriverSettings*)theSett
{
	NSData *saveData = [self rawLESoundData:theSett];
	if (!saveData) {
		return MADNeedMemory;
	}
	__block NSError *expErr = nil;
	dispatch_block_t errBlock = ^{
		if (isQuitting) {
			[NSApp replyToApplicationShouldTerminate:YES];
		} else {
			NSRunAlertPanel(@"Export failed", @"Export of the music file failed:\n%@", nil, nil, nil, [expErr localizedDescription]);
		}
	};
	
#define checkError(res) { \
if (res != noErr){ \
if (audioFile != NULL)\
AudioFileClose(audioFile);\
expErr = [NSError errorWithDomain:NSOSStatusErrorDomain code:res userInfo:nil];\
dispatch_async(dispatch_get_main_queue(), errBlock);\
return MADWritingErr; \
} \
}
	AudioStreamBasicDescription asbd = {0};
	asbd.mSampleRate = theSett->outPutRate;
	asbd.mFormatID = kAudioFormatLinearPCM;
	asbd.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
	asbd.mBitsPerChannel = theSett->outPutBits;
	switch (theSett->outPutMode) {
		case MonoOutPut:
			asbd.mChannelsPerFrame = 1;
			break;
			
		default:
		case StereoOutPut:
		case DeluxeStereoOutPut:
			asbd.mChannelsPerFrame = 2;
			break;
			
		case PolyPhonic:
			asbd.mChannelsPerFrame = 4;
			break;
	}
	asbd.mFramesPerPacket = 1;
	asbd.mBytesPerFrame = asbd.mBitsPerChannel * asbd.mChannelsPerFrame / 8;
	asbd.mBytesPerPacket = asbd.mBytesPerFrame * asbd.mFramesPerPacket;
	
	AudioFileID audioFile = NULL;
	OSStatus res;
	res = AudioFileCreateWithURL((__bridge CFURLRef)theURL, kAudioFileWAVEType, &asbd, kAudioFileFlags_EraseFile, &audioFile);
	checkError(res);
	
	UInt32 numBytes = (UInt32)[saveData length];
	
	res = AudioFileWriteBytes(audioFile, false, 0, &numBytes, [saveData bytes]);
	checkError(res);
	
	[self setMetadataToAudioFileID:audioFile];
	
	res = AudioFileClose(audioFile);
	audioFile = NULL;
	checkError(res);
#undef checkError
	return MADNoErr;
}

- (OSErr)saveMusicAsAIFFToURL:(NSURL*)theURL usingSettings:(MADDriverSettings*)theSett
{
	NSData *saveData = [self rawBESoundData:theSett];
	if (!saveData) {
		return MADNeedMemory;
	}
	__block NSError *expErr = nil;
	dispatch_block_t errBlock = ^{
		if (isQuitting) {
			[NSApp replyToApplicationShouldTerminate:YES];
		} else {
			NSRunAlertPanel(@"Export failed", @"Export of the music file failed:\n%@", nil, nil, nil, [expErr localizedDescription]);
		}
	};
	
#define checkError(res) { \
if (res != noErr){ \
if (audioFile != NULL)\
AudioFileClose(audioFile);\
expErr = [NSError errorWithDomain:NSOSStatusErrorDomain code:res userInfo:nil];\
dispatch_async(dispatch_get_main_queue(), errBlock);\
return MADWritingErr; \
} \
}
	
	AudioStreamBasicDescription asbd = {0};
	asbd.mSampleRate = theSett->outPutRate;
	asbd.mFormatID = kAudioFormatLinearPCM;
	asbd.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked | kAudioFormatFlagIsBigEndian;
	asbd.mBitsPerChannel = theSett->outPutBits;
	switch (theSett->outPutMode) {
		case MonoOutPut:
			asbd.mChannelsPerFrame = 1;
			break;
			
		default:
		case StereoOutPut:
		case DeluxeStereoOutPut:
			asbd.mChannelsPerFrame = 2;
			break;
			
		case PolyPhonic:
			asbd.mChannelsPerFrame = 4;
			break;
	}
	asbd.mFramesPerPacket = 1;
	asbd.mBytesPerFrame = asbd.mBitsPerChannel * asbd.mChannelsPerFrame / 8;
	asbd.mBytesPerPacket = asbd.mBytesPerFrame * asbd.mFramesPerPacket;
	
	AudioFileID audioFile = NULL;
	OSStatus res;
	res = AudioFileCreateWithURL((__bridge CFURLRef)theURL, kAudioFileAIFFType, &asbd, kAudioFileFlags_EraseFile, &audioFile);
	checkError(res);
	
	UInt32 numBytes = (UInt32)[saveData length];
	
	res = AudioFileWriteBytes(audioFile, false, 0, &numBytes, [saveData bytes]);
	checkError(res);
	
	[self setMetadataToAudioFileID:audioFile];
	
	res = AudioFileClose(audioFile);
	audioFile = NULL;
	checkError(res);
#undef checkError
	return MADNoErr;
}


- (void)beginExportSettingsWithHandler:(void(^)(NSModalResponse result))theHandle
{
	MADGetBestDriver(&exportSettings);
	exportSettings.driverMode = NoHardwareDriver;
	exportSettings.repeatMusic = FALSE;
	[exportController settingsFromDriverSettings:exportSettings];
	[self.window beginSheet:self.exportWindow completionHandler:theHandle];
}

- (IBAction)exportMusicAs:(id)sender
{
	NSInteger tag = [sender tag];
	[madDriver beginExport];
	NSSavePanel *savePanel = [NSSavePanel savePanel];
	[savePanel setCanCreateDirectories:YES];
	[savePanel setCanSelectHiddenExtension:YES];
	if (![self.musicName isEqualToString:@""]) {
		[savePanel setNameFieldStringValue:self.musicName];
	}
	[savePanel setPrompt:@"Export"];
	
	switch (tag) {
		case -1: //AIFF
		{
			[savePanel setAllowedFileTypes:@[AVFileTypeAIFF]];
			[savePanel setTitle:@"Export as AIFF audio"];
			[savePanel beginSheetModalForWindow:self.window completionHandler:^(NSInteger result) {
				if (result != NSFileHandlingPanelOKButton) {
					[madDriver endExport];
				}
				
				[self beginExportSettingsWithHandler:^(NSInteger result) {
					if (result != NSAlertDefaultReturn) {
						[madDriver endExport];
						return;
					}
					
					dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
						@autoreleasepool {
							OSErr thErr = [self saveMusicAsAIFFToURL:[savePanel URL] usingSettings:&exportSettings];
							[madDriver endExport];
							if (thErr != MADNoErr)
								return;
						}
						dispatch_async(dispatch_get_main_queue(), ^{
							if (isQuitting) {
								[NSApp replyToApplicationShouldTerminate:YES];
							} else {
								NSInteger retVal = NSRunInformationalAlertPanel(@"Export complete", @"The export of the file \"%@\" is complete.", @"OK", @"Show File", nil, [[savePanel URL] lastPathComponent]);
								if (retVal == NSAlertAlternateReturn) {
									[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:@[[savePanel URL]]];
								}
							}
						});
					});
				}];
			}];
		}
			break;
			
		case -2: //MP4
		{
			[savePanel setAllowedFileTypes:@[AVFileTypeAppleM4A]];
			[savePanel setTitle:@"Export as MPEG-4 Audio"];
			[savePanel beginSheetModalForWindow:self.window completionHandler:^(NSInteger result) {
				if (result != NSFileHandlingPanelOKButton) {
					[madDriver endExport];
					return;
				}
				
				[self beginExportSettingsWithHandler:^(NSInteger result) {
					if (result != NSAlertDefaultReturn) {
						[madDriver endExport];
						return;
					}
					
					dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
						OSErr theErr = MADNoErr;
						
						NSURL *oldURL = [[musicList objectInMusicListAtIndex:self.previouslyPlayingIndex.index] musicURL];
						NSArray *metadataInfo;
						__block NSError *expErr = nil;
						dispatch_block_t errBlock = ^{
							if (isQuitting) {
								[NSApp replyToApplicationShouldTerminate:YES];
							}else {
								NSRunAlertPanel(@"Export failed", @"Export/coversion of the music file failed:\n%@", nil, nil, nil, [expErr localizedDescription]);
							}
						};
						NSString *oldMusicName = self.musicName;
						NSString *oldMusicInfo = self.musicInfo;
						NSURL *tmpURL = [[[NSFileManager defaultManager] URLForDirectory:NSItemReplacementDirectory inDomain:NSUserDomainMask appropriateForURL:oldURL create:YES error:nil] URLByAppendingPathComponent:[NSString stringWithFormat:@"%@.aiff", (oldMusicName && ![oldMusicName isEqualToString:@""]) ? oldMusicName : @"untitled"] isDirectory:NO];
						
						if ((theErr = [self saveMusicAsAIFFToURL:tmpURL usingSettings:&exportSettings]) != MADNoErr) {
							expErr = PPCreateErrorFromMADErrorType(theErr);
							[[NSFileManager defaultManager] removeItemAtURL:tmpURL error:NULL];
							dispatch_async(dispatch_get_main_queue(), errBlock);
						}
						
						AVAsset *exportMov = [AVAsset assetWithURL:tmpURL];
						{
							AVMutableMetadataItem *titleName = [AVMutableMetadataItem new];
							[titleName setKeySpace:AVMetadataKeySpaceCommon];
							[titleName setKey:AVMetadataCommonKeyTitle];
							titleName.value = oldMusicName;
							
							AVMutableMetadataItem *dataInfo = [AVMutableMetadataItem new];
							dataInfo.keySpace = AVMetadataKeySpaceCommon;
							dataInfo.key = AVMetadataCommonKeySoftware;
							dataInfo.value = @"PlayerPRO Player";
							
							AVMutableMetadataItem *musicInfoQTUser = [AVMutableMetadataItem new];
							[musicInfoQTUser setKeySpace:AVMetadataKeySpaceQuickTimeUserData];
							[musicInfoQTUser setKey:AVMetadataQuickTimeUserDataKeyInformation];
							[musicInfoQTUser setValue:oldMusicInfo];
							
							AVMutableMetadataItem *musicNameQTUser = [AVMutableMetadataItem new];
							musicNameQTUser.keySpace = AVMetadataKeySpaceQuickTimeUserData;
							musicNameQTUser.key = AVMetadataQuickTimeUserDataKeyFullName;
							musicNameQTUser.value = oldMusicName;

							AVMutableMetadataItem *musicInfoiTunes = [AVMutableMetadataItem new];
							[musicInfoiTunes setKeySpace:AVMetadataKeySpaceiTunes];
							[musicInfoiTunes setKey:AVMetadataiTunesMetadataKeyUserComment];
							[musicInfoiTunes setValue:oldMusicInfo];
							
							AVMutableMetadataItem *musicInfoQTMeta = [AVMutableMetadataItem new];
							[musicInfoQTMeta setKeySpace:AVMetadataKeySpaceQuickTimeMetadata];
							[musicInfoQTMeta setKey:AVMetadataQuickTimeMetadataKeyInformation];
							[musicInfoQTMeta setValue:oldMusicInfo];
							
							metadataInfo = @[titleName, dataInfo, musicInfoQTUser, musicInfoiTunes, musicInfoQTMeta, musicNameQTUser];
						}
						
						oldMusicInfo = nil;
						if (!exportMov) {
							expErr = [NSError errorWithDomain:NSCocoaErrorDomain code:NSFileWriteUnknownError userInfo:nil];
							NSLog(@"Init Failed for %@, error: %@", oldMusicName, [expErr localizedDescription]);
							[[NSFileManager defaultManager] removeItemAtURL:tmpURL error:NULL];
							dispatch_async(dispatch_get_main_queue(), errBlock);
							return;
						}
						
						AVAssetExportSession *session = [[AVAssetExportSession alloc] initWithAsset:exportMov presetName:AVAssetExportPresetAppleM4A];
						if (!session) {
							expErr = [NSError errorWithDomain:NSCocoaErrorDomain code:NSFileWriteUnknownError userInfo:nil];
							NSLog(@"Export session creation for %@ failed, error: %@", oldMusicName, [expErr localizedDescription]);
							[[NSFileManager defaultManager] removeItemAtURL:tmpURL error:NULL];
							dispatch_async(dispatch_get_main_queue(), errBlock);
							return;
						}
						if ([[savePanel URL] checkResourceIsReachableAndReturnError:NULL]) {
							[[NSFileManager defaultManager] removeItemAtURL:[savePanel URL] error:NULL];
						}
						session.outputURL = [savePanel URL];
						session.outputFileType = AVFileTypeAppleM4A;
						session.metadata = metadataInfo;
						metadataInfo = nil;
						dispatch_semaphore_t sessionWaitSemaphore = dispatch_semaphore_create(0);
						[session exportAsynchronouslyWithCompletionHandler:^{
							dispatch_semaphore_signal(sessionWaitSemaphore);
						}];
						dispatch_semaphore_wait(sessionWaitSemaphore, DISPATCH_TIME_FOREVER);
						
						BOOL didFinish = [session status] == AVAssetExportSessionStatusCompleted;
						[[NSFileManager defaultManager] removeItemAtURL:tmpURL error:NULL];
						[madDriver endExport];
						
						if (didFinish) {
							dispatch_async(dispatch_get_main_queue(), ^{
								if (isQuitting) {
									[NSApp replyToApplicationShouldTerminate:YES];
								} else {
									NSInteger retVal = NSRunInformationalAlertPanel(@"Export complete", @"The export of the file \"%@\" is complete.", @"OK", @"Show File", nil, [[savePanel URL] lastPathComponent]);
									if (retVal == NSAlertAlternateReturn) {
										[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:@[[savePanel URL]]];
									}
								}
							});
						} else
							NSLog(@"%@", [session error]);
					});
				}];
			}];
		}
			break;
			
		case -3: // wave
		{
			[savePanel setAllowedFileTypes:@[AVFileTypeWAVE]];
			[savePanel setTitle:@"Export as Wave Audio"];
			[savePanel beginSheetModalForWindow:self.window completionHandler:^(NSInteger result) {
				if (result != NSFileHandlingPanelOKButton) {
					[madDriver endExport];
					return;
				}
				[self beginExportSettingsWithHandler:^(NSInteger result) {
					if (result != NSAlertDefaultReturn) {
						[madDriver endExport];
						return;
					}
					
					dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
						@autoreleasepool {
							NSData *saveData = [self rawLESoundData:&exportSettings];
							[madDriver endExport];
							
							if (!saveData)
								return;
							
							__block NSError *expErr = nil;
							dispatch_block_t errBlock = ^{
								if (isQuitting) {
									[NSApp replyToApplicationShouldTerminate:YES];
								} else {
									NSRunAlertPanel(@"Export failed", @"Export of the music file failed:\n%@", nil, nil, nil, [expErr localizedDescription]);
								}
							};
							
#define checkError(res) { \
if (res != MADNoErr){ \
expErr = [NSError errorWithDomain:NSOSStatusErrorDomain code:res userInfo:nil];\
dispatch_async(dispatch_get_main_queue(), errBlock);\
return; \
} \
}
							AudioStreamBasicDescription asbd = {0};
							asbd.mSampleRate = exportSettings.outPutRate;
							asbd.mFormatID = kAudioFormatLinearPCM;
							asbd.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
							asbd.mBitsPerChannel = exportSettings.outPutBits;
							switch (exportSettings.outPutMode) {
								case MonoOutPut:
									asbd.mChannelsPerFrame = 1;
									break;
									
								default:
								case StereoOutPut:
								case DeluxeStereoOutPut:
									asbd.mChannelsPerFrame = 2;
									break;
									
								case PolyPhonic:
									asbd.mChannelsPerFrame = 4;
									break;
							}
							asbd.mFramesPerPacket = 1;
							asbd.mBytesPerFrame = asbd.mBitsPerChannel * asbd.mChannelsPerFrame / 8;
							asbd.mBytesPerPacket = asbd.mBytesPerFrame * asbd.mFramesPerPacket;
							
							NSURL *url = [savePanel URL];
							
							AudioFileID audioFile;
							OSStatus res;
							res = AudioFileCreateWithURL((__bridge CFURLRef)(url), kAudioFileWAVEType, &asbd, kAudioFileFlags_EraseFile, &audioFile);
							checkError(res);
							
							UInt32 numBytes = (UInt32)[saveData length];
							
							res = AudioFileWriteBytes(audioFile, false, 0, &numBytes, [saveData bytes]);
							checkError(res);
							
							res = AudioFileClose(audioFile);
							checkError(res);
#undef checkError
							
						}
						dispatch_async(dispatch_get_main_queue(), ^{
							if (isQuitting) {
								[NSApp replyToApplicationShouldTerminate:YES];
							} else {
								NSInteger retVal = NSRunInformationalAlertPanel(@"Export complete", @"The export of the file \"%@\" is complete.", @"OK", @"Show File", nil, [[savePanel URL] lastPathComponent]);
								if (retVal == NSAlertAlternateReturn) {
									[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:@[[savePanel URL]]];
								}
							}
						});
					});
				}];
			}];
			break;
		}
			
		default:
		{
			if (tag > madLib.pluginCount || tag < 0) {
				NSBeep();
				[madDriver endExport];
				if (isQuitting) {
					[NSApp replyToApplicationShouldTerminate:YES];
				}
				
				return;
			}
			[savePanel setAllowedFileTypes:madLib[tag].UTITypes];
			[savePanel setTitle:[NSString stringWithFormat:@"Export as %@", madLib[tag].menuName]];
			
			[savePanel beginSheetModalForWindow:self.window completionHandler:^(NSInteger result) {
				if (result != NSFileHandlingPanelOKButton) {
					[madDriver endExport];
					return;
				}
				
				NSURL *fileURL = [savePanel URL];
				OSErr err = [self.music exportMusicToURL:fileURL format:madLib[tag].type library:madLib];
				if (err != MADNoErr) {
					if (isQuitting) {
						[NSApp replyToApplicationShouldTerminate:YES];
					} else {
						NSError *aerr = PPCreateErrorFromMADErrorType(err);
						[[NSAlert alertWithError:aerr] runModal];
					}
				} else {
					[self addMusicToMusicList:fileURL loadIfPreferencesAllow:NO];
					if (isQuitting) {
						[NSApp replyToApplicationShouldTerminate:YES];
					} else {
						NSInteger retVal = NSRunInformationalAlertPanel(@"Export complete", @"The export of the file \"%@\" is complete.", @"OK", @"Show File", nil, [[savePanel URL] lastPathComponent]);
						if (retVal == NSAlertAlternateReturn) {
							[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:@[fileURL]];
						}
					}
				}
			}];
			break;
		}
	}
}

- (IBAction)okayExportSettings:(id)sender
{
	[self.window endSheet:self.exportWindow returnCode:NSAlertDefaultReturn];
	[self.exportWindow close];
}

- (IBAction)cancelExportSettings:(id)sender
{
	[self.window endSheet:self.exportWindow returnCode:NSAlertAlternateReturn];
	[self.exportWindow close];
}

- (IBAction)saveMusicAs:(id)sender
{
	[madDriver beginExport];
	
	NSSavePanel * savePanel = [NSSavePanel savePanel];
	[savePanel setAllowedFileTypes:@[MADNativeUTI]];
	[savePanel setCanCreateDirectories:YES];
	[savePanel setCanSelectHiddenExtension:YES];
	if (![self.musicName isEqualToString:@""]) {
		[savePanel setNameFieldStringValue:self.musicName];
	}
	
	[savePanel beginWithCompletionHandler:^(NSInteger result) {
		if (result != NSFileHandlingPanelOKButton) {
			NSURL *saveURL = [savePanel URL];
			[self saveMusicToURL:saveURL];
			[self addMusicToMusicList:saveURL loadIfPreferencesAllow:NO];
		}
		[madDriver endExport];
	}];
}

- (IBAction)saveMusic:(id)sender
{
	[madDriver beginExport];
	
	if (self.previouslyPlayingIndex.index == -1) {
		// saveMusicAs: will end the exporting when it is done.
		[self saveMusicAs:sender];
	} else {
		NSURL *fileURL = self.previouslyPlayingIndex.playbackURL;
		NSString *filename = [fileURL path];
		NSWorkspace *sharedWorkspace = [NSWorkspace sharedWorkspace];
		NSString *utiFile = [sharedWorkspace typeOfFile:filename error:nil];
		if (/*[sharedWorkspace type:utiFile conformsToType:MADNativeUTI]*/ [utiFile isEqualToString:MADNativeUTI]) {
			[self saveMusicToURL:fileURL];
			[madDriver endExport];
		} else {
			// saveMusicAs: will end the exporting when it is done.
			[self saveMusicAs:sender];
		}
	}
}

- (BOOL)loadMusicURL:(NSURL*)musicToLoad error:(out NSError *__autoreleasing*)theErr
{
	return [self loadMusicURL:musicToLoad error:theErr autoPlay:YES];
}

- (BOOL)loadMusicURL:(NSURL*)musicToLoad error:(out NSError *__autoreleasing*)theErr autoPlay:(BOOL)autoplay
{
	NSString *fileType;
	MADErr theOSErr = MADNoErr;
	NSError *error;
	if (self.music) {
		[madDriver stop];
		[madDriver stopDriver];
	}
	NSString *fileUTI = [[NSWorkspace sharedWorkspace] typeOfFile:[musicToLoad path] error:NULL];
	
	if (fileUTI) {
		fileType = [madLib typeFromUTI:fileUTI];
		if (fileType) {
			theOSErr = [madLib testFileAtURL:musicToLoad stringType:fileType];
		} else {
			theOSErr = -1;
		}
	} else {
		theOSErr = -1;
	}
	if (theOSErr != MADNoErr) {
		theOSErr = [madLib identifyFileAtURL:musicToLoad stringType:&fileType];
	}
	
	if (theOSErr != MADNoErr) {
		if (theErr) {
			*theErr = PPCreateErrorFromMADErrorType(theOSErr);
		}
		self.paused = YES;
		[self clearMusic];
		return NO;
	}
	
	self.music = [[PPMusicObject alloc] initWithURL:musicToLoad stringType:fileType library:madLib error:&error];
	if (!self.music) {
		theOSErr = MADReadingErr;
	}
	
	if (theOSErr != MADNoErr) {
		if (theErr) {
			*theErr = PPCreateErrorFromMADErrorType(theOSErr);
		}
		self.paused = YES;
		[self clearMusic];
		return NO;
	}
	
	[self.music attachToDriver:madDriver];
	if (autoplay) {
		[madDriver play];
	}
	self.paused = NO;
	{
		long fT, cT;
		[madDriver getMusicStatusWithCurrentTime:&cT totalTime:&fT];
		[songPos setMaxValue:fT];
		[songPos setMinValue:0.0];
		[self setTitleForSongLabelBasedOnMusic];
		[songTotalTime setIntegerValue:fT];
	}
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPMusicDidChange object:self];
	
	if (theErr)
		*theErr = nil;
	
	return YES;
}

- (BOOL)loadMusicURL:(NSURL*)musicToLoad
{
	return [self loadMusicURL:musicToLoad error:NULL autoPlay:YES];
}

- (IBAction)showPreferences:(id)sender
{
	if (!preferences) {
		preferences = [Preferences newPreferenceController];
		[[preferences window] center];
	}
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
	NSError *err;
	self.currentlyPlayingIndex.index = [tableView selectedRow];
	if ([self loadMusicFromCurrentlyPlayingIndexWithError:&err] == NO)
		[[NSAlert alertWithError:err] runModal];
}

- (IBAction)showPlugInInfo:(id)sender
{
	PlugInInfo *inf = (self.plugInInfos)[[sender tag]];
	if (!inf) {
		return;
	}
	
	PlugInInfoController *infoCont = [PlugInInfoController windowControllerFromInfo:inf];
	NSWindow *infoWindow = [infoCont window];
	[infoWindow center];
	[NSApp runModalForWindow:infoWindow];
}

- (void)updatePlugInInfoMenu
{
	for (PPLibraryObject *obj in madLib) {
		PlugInInfo *tmpInfo = [[PlugInInfo alloc] initWithPlugName:obj.menuName author:obj.authorString plugType:NSLocalizedString(@"TrackerPlugName", @"Tracker plug-in name") plugURL:[obj.bundle bundleURL]];
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
	
	for (NSInteger i = 0; i < [self.plugInInfos count]; i++) {
		PlugInInfo *pi = (self.plugInInfos)[i];
		NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:pi.plugName action:@selector(showPlugInInfo:) keyEquivalent:@""];
		[mi setTag:i];
		[mi setTarget:self];
		[aboutPlugInMenu addItem:mi];
	}
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	self.quitting = NO;
	srandom(time(NULL) & 0xffffffff);
	MADRegisterDebugFunc(CocoaDebugStr);
	madLib = [[PPLibrary alloc] init:YES];
	//the NIB won't store the value anymore, so do this hackery to make sure there's some value in it.
	[songTotalTime setIntegerValue:0];
	[songCurTime setIntegerValue:0];
	
	[tableView registerForDraggedTypes:@[PPMLDCUTI, (NSString*)kUTTypeFileURL]];
	[self addObserver:self forKeyPath:@"paused" options:NSKeyValueObservingOptionNew context:NULL];
	self.paused = YES;
	[self willChangeValueForKey:kMusicListKVO];
	musicList = [[MusicList alloc] init];
	if ([[NSUserDefaults standardUserDefaults] boolForKey:PPRememberMusicList]) {
		[musicList loadApplicationMusicList];
	}
	NSInteger selMus = musicList.selectedMusic;
	[self didChangeValueForKey:kMusicListKVO];
	
	[tableView setDoubleAction:@selector(doubleClickMusicList)];
	
	NSNotificationCenter *defaultCenter = [NSNotificationCenter defaultCenter];
	[defaultCenter addObserver:self selector:@selector(soundPreferencesDidChange:) name:PPSoundPreferencesDidChange object:nil];
	
	[self MADDriverWithPreferences];
	
	for (NSInteger i = 0; i < [madLib pluginCount]; i++) {
		PPLibraryObject *obj = madLib[i];
		if (obj.canExport) {
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:[NSString stringWithFormat:@"%@...", obj.menuName] action:@selector(exportMusicAs:) keyEquivalent:@""];
			[mi setTag:i];
			[mi setTarget:self];
			[musicExportMenu addItem:mi];
		}
	}
	
	self.plugInInfos = [[NSMutableArray alloc] init];
	[self updatePlugInInfoMenu];
	
	self.previouslyPlayingIndex = [[CurrentlyPlayingIndex alloc] init];
	self.previouslyPlayingIndex.index = -1;
	self.currentlyPlayingIndex = [[CurrentlyPlayingIndex alloc] init];
	[self.previouslyPlayingIndex movePlayingIndexToOtherIndex:self.currentlyPlayingIndex];
	
	exportController = [SoundSettingsViewController newSoundSettingWindow];
	exportController.delegate = self;
	[exportSettingsBox setContentView:[exportController view]];
	
	instrumentController = [[PPInstrumentWindowController alloc] init];
	
	timeChecker = [[NSTimer alloc] initWithFireDate:[NSDate dateWithTimeIntervalSinceNow:0] interval:1/8.0 target:self selector:@selector(updateMusicStats:) userInfo:nil repeats:YES];
	[[NSRunLoop mainRunLoop] addTimer:timeChecker forMode:NSRunLoopCommonModes];
	NSUInteger lostCount = musicList.lostMusicCount;
	if (lostCount) {
		NSRunAlertPanel(kUnresolvableFile, kUnresolvableFileDescription, nil, nil, nil, (unsigned long)lostCount);
	}
	if (selMus != -1) {
		NSError *err;
		
		[self selectMusicAtIndex:selMus];
		if (![self loadMusicURL:[musicList URLAtIndex:selMus] error:&err autoPlay:NO]) {
			[[NSAlert alertWithError:err] runModal];
		}
	} else {
		self.music = [PPMusicObject new];
		[self.music attachToDriver:madDriver];
		[self setTitleForSongLabelBasedOnMusic];
	}
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
	if ([madDriver isExporting]) {
		NSInteger selection = NSRunAlertPanel(@"Exporting", @"PlayerPRO is currently exporting a tracker file.\nQuitting will stop this. Do you really wish to quit?", @"Wait", @"Quit", @"Cancel"); //TODO: localize
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
				if ([madDriver isExporting]) {
					self.quitting = YES;
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
	
	[madDriver stopDriver];
	if ([[NSUserDefaults standardUserDefaults] boolForKey:PPRememberMusicList]) {
		[musicList saveApplicationMusicList];
	}
	
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)musicListContentsDidMove
{
	NSInteger i;
	if (self.currentlyPlayingIndex.index != -1) {
		for (i = 0; i < [musicList countOfMusicList]; i++) {
			if ([self.currentlyPlayingIndex.playbackURL isEqual:[musicList URLAtIndex:i]]) {
				self.currentlyPlayingIndex.index = i;
				break;
			}
		}
	}
	if (self.previouslyPlayingIndex.index != -1) {
		for (i = 0; i < [musicList countOfMusicList]; i++) {
			if ([self.previouslyPlayingIndex.playbackURL isEqual:[musicList URLAtIndex:i]]) {
				self.previouslyPlayingIndex.index = i;
				break;
			}
		}
	}
}

- (IBAction)sortMusicList:(id)sender
{
	[self willChangeValueForKey:kMusicListKVO];
	[musicList sortMusicListByName];
	[self didChangeValueForKey:kMusicListKVO];
	[self musicListContentsDidMove];
}

- (IBAction)playSelectedMusic:(id)sender
{
	NSError *error;
	self.currentlyPlayingIndex.index = [tableView selectedRow];
	if ([self loadMusicFromCurrentlyPlayingIndexWithError:&error] == NO) {
		[[NSAlert alertWithError:error] runModal];
	}
}

- (void)addMusicsToMusicList:(NSArray*)theMusList
{
	for (NSInteger i = 0; i < [theMusList count] - 1; i++) {
		[self addMusicToMusicList:theMusList[i] loadIfPreferencesAllow:NO];
	}
	[self addMusicToMusicList:[theMusList lastObject] loadIfPreferencesAllow:YES];
}

- (IBAction)addMusic:(id)sender
{
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	
	OpenPanelViewController *av = [[OpenPanelViewController alloc] initWithOpenPanel:panel trackerDictionary:self.trackerDict playlistDictionary:nil instrumentDictionary:nil additionalDictionary:nil];
	[av setupDefaults];
	av.allowsMultipleSelectionOfTrackers = YES;
	[av beginOpenPanelWithParentWindow:self.window completionHandler:^(NSInteger result) {
		if (result == NSFileHandlingPanelOKButton) {
			[self addMusicsToMusicList:[panel URLs]];
		}
	}];
}

- (IBAction)toggleInfo:(id)sender
{
	[infoDrawer toggle:sender];
}

- (void)setTitleForSongLabelBasedOnMusic
{
	self.musicName = self.music.title;
	self.musicInfo = self.music.information;
}

- (void)clearMusic
{
	if (self.music) {
		[madDriver stop];
		[madDriver cleanDriver];
	}
	
	self.paused = YES;
	self.currentlyPlayingIndex.index = -1;
	self.currentlyPlayingIndex.playbackURL = nil;
	[self.currentlyPlayingIndex movePlayingIndexToOtherIndex:self.previouslyPlayingIndex];
	self.music = [PPMusicObject new];
	[self setTitleForSongLabelBasedOnMusic];
	[[NSNotificationCenter defaultCenter] postNotificationName:PPMusicDidChange object:self];
	[self.music attachToDriver:madDriver];
}

- (IBAction)removeSelectedMusic:(id)sender
{
	NSIndexSet *selMusic = [tableView selectedRowIndexes];
	[self willChangeValueForKey:kMusicListKVO];
	[musicList removeObjectsInMusicListAtIndexes:selMusic];
	[self didChangeValueForKey:kMusicListKVO];
	[self musicListContentsDidMove];
}

- (IBAction)newMusic:(id)sender
{
	[self clearMusic];
}

- (IBAction)clearMusicList:(id)sender
{
	if ([musicList countOfMusicList]) {
		NSInteger returnVal = NSRunAlertPanel(NSLocalizedString(@"Clear list", @"Clear Music List"), NSLocalizedString(@"The music list contains %ld items. Do you really want to remove them?", @"Clear Music List?"), NSLocalizedString(@"No", @"No"), NSLocalizedString(@"Yes", @"Yes"), nil, (long)[musicList countOfMusicList]);
		
		if (returnVal == NSAlertAlternateReturn) {
			[self willChangeValueForKey:kMusicListKVO];
			[musicList clearMusicList];
			[self didChangeValueForKey:kMusicListKVO];
			if (self.currentlyPlayingIndex.index != -1) {
				[self clearMusic];
			}
		}
	} else {
		NSBeep();
	}
}

typedef NS_ENUM(NSInteger, PPMusicToolbarTypes) {
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
			if ([infoDrawer state] == NSDrawerOpeningState || [infoDrawer state] == NSDrawerOpenState)
				return YES;
		case PPToolbarPlayMusic:
			if ([[tableView selectedRowIndexes] count] == 1)
				return YES;
			else
				return NO;
			break;
			
		case PPToolbarRemoveMusic:
			if ([[tableView selectedRowIndexes] count] > 0)
				return YES;
			else
				return NO;
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
	if ([theUTI isEqualToString:MADGenericUTI]) {
		NSInteger retVal = NSRunInformationalAlertPanel(NSLocalizedString(@"Invalid Extension", @"Invalid extension"), NSLocalizedString(@"The file %@ is identified as as a generic MAD tracker, and not a specific one. Renaming it will fix this. Do you want to rename the file extension?", @"Invalid extension description"), NSLocalizedString(@"Rename", @"rename file"), NSLocalizedString(@"Open", @"Open a file"), NSLocalizedString(@"Cancel", @"Cancel"), [theURL lastPathComponent]);
		switch (retVal) {
			case NSAlertDefaultReturn:
			{
				NSDictionary *rec;
				{
					NSString *ostype;
					if ([madLib identifyFileAtURL:theURL stringType:&ostype] != MADNoErr || [madLib getInformationFromFileAtURL:theURL stringType:ostype info:&rec]) {
						NSRunCriticalAlertPanel(NSLocalizedString(@"Unknown File", @"unknown file"), NSLocalizedString(@"The file type could not be identified.", @"Unidentified file"), nil, nil, nil);
						return NO;
					}
				}
				NSString *ostype = rec[kPPSignature];
				
				NSURL *tmpURL = [[theURL URLByDeletingPathExtension] URLByAppendingPathExtension:[ostype lowercaseString]];
				NSError *err;
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
			[musicList loadMusicListFromURL:theURL];
			selMusFromList = musicList.selectedMusic;
			[self didChangeValueForKey:kMusicListKVO];
			[self musicListDidChange];
			return YES;
		}
	} else if ([sharedWorkspace type:theUTI conformsToType:PPOldMusicListUTI]) {
		if ([self musicListWillChange]) {
			[self willChangeValueForKey:kMusicListKVO];
			[musicList beginLoadingOfOldMusicListAtURL:theURL completionHandle:^(NSError *theErr) {
				selMusFromList = musicList.selectedMusic;
				[self didChangeValueForKey:kMusicListKVO];
				[self musicListDidChange];
				if (theErr) {
					[[NSAlert alertWithError:theErr] runModal];
				};
			}];
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
				if ([sharedWorkspace type:theUTI conformsToType:aUTI]) {
					[self addMusicToMusicList:theURL];
					return YES;
				}
			}
		}
	}
	return NO;
}

- (IBAction)openFile:(id)sender
{
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	NSDictionary *playlistDict = @{@"PlayerPRO Music List" : @[PPMusicListUTI],
								   @"PlayerPRO Old Music List" : @[PPOldMusicListUTI]};
	
	OpenPanelViewController *av = [[OpenPanelViewController alloc] initWithOpenPanel:panel trackerDictionary:self.trackerDict playlistDictionary:playlistDict instrumentDictionary:nil additionalDictionary:nil];
	[av setupDefaults];
	av.allowsMultipleSelectionOfTrackers = YES;
	[av beginOpenPanelWithParentWindow:[self window] completionHandler:^(NSInteger result) {
		if (result != NSFileHandlingPanelOKButton) {
			return;
		}
		NSArray *panelURLS = [panel URLs];
		for (NSURL *theURL in panelURLS) {
			NSString *filename = [theURL path];
			NSError *err;
			NSString *utiFile = [[NSWorkspace sharedWorkspace] typeOfFile:filename error:&err];
			if (err) {
				NSRunAlertPanel(@"Error opening file", @"Unable to open %@: %@",
								nil, nil, nil, [filename lastPathComponent], [err localizedFailureReason]);
				return;
			}
			[self handleFile:theURL ofType:utiFile]; //TODO: more efficient way of doing this!
		}
	}];
}

- (IBAction)saveMusicList:(id)sender
{
	NSSavePanel *savePanel = [NSSavePanel savePanel];
	[savePanel setAllowedFileTypes:@[PPMusicListUTI]];
	[savePanel setCanCreateDirectories:YES];
	[savePanel setCanSelectHiddenExtension:YES];
	[savePanel beginSheetModalForWindow:[self window] completionHandler:^(NSInteger result) {
		if (result == NSFileHandlingPanelOKButton) {
			[musicList saveMusicListToURL:[savePanel URL]];
		}
	}];
}

- (IBAction)fastForwardButtonPressed:(id)sender
{
	
}

- (IBAction)loopButtonPressed:(id)sender
{
	
}

- (IBAction)nextButtonPressed:(id)sender
{
	NSError *err;
	
	if (self.currentlyPlayingIndex.index + 1 < [musicList countOfMusicList]) {
		self.currentlyPlayingIndex.index++;
		[self selectCurrentlyPlayingMusic];
		if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err]) {
			[[NSAlert alertWithError:err] runModal];
		}
	} else if ([[NSUserDefaults standardUserDefaults] boolForKey:PPLoopMusicWhenDone]) {
		self.currentlyPlayingIndex.index = 0;
		[self selectCurrentlyPlayingMusic];
		if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err]) {
			[[NSAlert alertWithError:err] runModal];
		}
	} else
		NSBeep();
}

- (IBAction)playButtonPressed:(id)sender
{
	if (self.music) {
		[madDriver play];
		self.paused = NO;
	}
}

- (IBAction)prevButtonPressed:(id)sender
{
	if (self.currentlyPlayingIndex.index > 0) {
		self.currentlyPlayingIndex.index--;
		[self selectCurrentlyPlayingMusic];
		NSError *err;
		if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err]) {
			[[NSAlert alertWithError:err] runModal];
		}
	} else
		NSBeep();
}

- (IBAction)rewindButtonPressed:(id)sender
{
	
}

- (IBAction)sliderChanged:(id)sender
{
	if (self.music) {
		[madDriver setMusicStatusWithCurrentTime:[songPos doubleValue] maximumTime:[songPos maxValue] minimumTime:0];
	}
}

- (IBAction)stopButtonPressed:(id)sender
{
	if (self.music) {
		[madDriver stop];
		[madDriver cleanDriver];
		[madDriver setMusicStatusWithCurrentTime:0 maximumTime:100 minimumTime:0];
		self.paused = YES;
	}
}

- (IBAction)pauseButtonPressed:(id)sender
{
	if (self.music) {
		if (self.paused) {
			[madDriver play];
		} else {
			[madDriver pause];
		}
		self.paused = !_paused;
	}
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
	NSIndexSet *selected = [tableView selectedRowIndexes];
	NSWorkspace *ws = [NSWorkspace sharedWorkspace];
	NSURL *musicURL;
	NSDictionary *theInfo;
	MusicListObject *obj;
	NSString *info;
	NSString *fileUTI;
	
	if ([selected count] > 0) {
		musicList.selectedMusic = [selected firstIndex];
	}
	
	if ([selected count] != 1)
		goto badTracker;
	
	obj = [musicList objectInMusicListAtIndex:[selected lastIndex]];
	musicURL = obj.musicURL;
	fileUTI = [ws typeOfFile:[musicURL path] error:NULL];
	
	if (!fileUTI) {
	badValues:

		if ([madLib identifyFileAtURL:musicURL stringType:&info] != MADNoErr)
			goto badTracker;
		if ([madLib testFileAtURL:musicURL stringType:info] != MADNoErr)
			goto badTracker;
	} else {
		info = [madLib typeFromUTI:fileUTI];
		if ([madLib getInformationFromFileAtURL:musicURL stringType:info info:&theInfo] != MADNoErr) {
			goto badValues;
		}
	}
	
	fileName.stringValue = obj.fileName;
	internalName.stringValue = theInfo[kPPInternalFileName];
	fileSize.integerValue = obj.fileSize;
	musicInstrument.objectValue = theInfo[kPPTotalInstruments];
	musicPatterns.objectValue = theInfo[kPPTotalPatterns];
	musicPlugType.stringValue = theInfo[kPPFormatDescription];
	musicSignature.stringValue = theInfo[kPPSignature];
	
	fileLocation.stringValue = musicURL.absoluteURL.path;
	return;
	
badTracker:
	[fileName setStringValue:PPDoubleDash];
	[internalName setStringValue:PPDoubleDash];
	[fileSize setStringValue:PPDoubleDash];
	[musicInstrument setStringValue:PPDoubleDash];
	[musicPatterns setStringValue:PPDoubleDash];
	[musicPlugType setStringValue:PPDoubleDash];
	[musicSignature setStringValue:PPDoubleDash];
	[fileLocation setStringValue:PPDoubleDash];
}

- (void)musicListDidChange
{
	if (self.currentlyPlayingIndex.index != -1) {
		[madDriver stop];
		[madDriver cleanDriver];
	}
	if ([[NSUserDefaults standardUserDefaults] boolForKey:PPLoadMusicAtListLoad] && [musicList countOfMusicList] > 0) {
		NSError *err;
		self.currentlyPlayingIndex.index = selMusFromList != -1 ? selMusFromList : 0;
		[self selectCurrentlyPlayingMusic];
		if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err]) {
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
	return YES;
}

- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename
{
	NSError *err;
	NSString *utiFile = [[NSWorkspace sharedWorkspace] typeOfFile:filename error:&err];
	if (err) {
		NSRunAlertPanel(@"Error opening file", @"Unable to open %@: %@", nil, nil, nil, [filename lastPathComponent], [err localizedFailureReason]);
		return NO;
	}
	return [self handleFile:[NSURL fileURLWithPath:filename] ofType:utiFile];
}

#pragma mark Table View delegate methods

- (NSString *)tableView:(NSTableView *)tableView toolTipForCell:(NSCell *)cell rect:(NSRectPointer)rect tableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row mouseLocation:(NSPoint)mouseLocation
{
	if (row >= 0 || row <= [musicList countOfMusicList]) {
		return nil;
	}
	return [[musicList URLAtIndex:row] path];
}

#pragma mark SoundSettingsViewControllerDelegate methods

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

- (void)soundOutReverbStrengthDidChange:(int)rev
{
	exportSettings.ReverbStrength = rev;
}

- (void)soundOutReverbSizeDidChange:(int)rev
{
	exportSettings.ReverbSize = rev;
}

- (void)soundOutOversamplingAmountDidChange:(int)ovs
{
	exportSettings.oversampling = ovs;
}

- (void)soundOutStereoDelayAmountDidChange:(int)std
{
	exportSettings.MicroDelaySize = std;
}

#pragma mark NSTableViewDataSource methods

- (NSDragOperation)tableView:(NSTableView *)tableView1 validateDrop:(id <NSDraggingInfo>)info proposedRow:(NSInteger)row proposedDropOperation:(NSTableViewDropOperation)operation
{
	NSDragOperation result = NSDragOperationNone;
	
	if ([info draggingSource] == tableView1) {
		result = NSDragOperationMove;
		//TODO: check for number of indexes that are greater than the drop row.
		[tableView1 setDropRow:row dropOperation:NSTableViewDropAbove];
	} else {
		NSPasteboard* pb = info.draggingPasteboard;
		
		//list the file type UTIs we want to accept
		NSArray* acceptedTypes = self.trackerUTIs;
		NSArray* urls = [pb readObjectsForClasses:@[[NSURL class]]
										  options:@{NSPasteboardURLReadingFileURLsOnlyKey : @YES,
													NSPasteboardURLReadingContentsConformToTypesKey : acceptedTypes}];
		
		if ([urls count] > 0) {
			result = NSDragOperationCopy;
			[tableView1 setDropRow:row dropOperation:NSTableViewDropAbove];
		}
	}
	
	return result;
}

- (BOOL)tableView:(NSTableView *)tableView1 acceptDrop:(id <NSDraggingInfo>)info row:(NSInteger)row dropOperation:(NSTableViewDropOperation)operation
{
	if (row < 0) {
		row = 0;
	}
	NSPasteboard *dragPB = [info draggingPasteboard];
	NSArray *tmpArray = [dragPB readObjectsForClasses:@[[MusicListDragClass class]] options:nil];
	if (tmpArray && [tmpArray count] != 0) {
		NSUInteger minRow = 0;
		MusicListDragClass *dragClass = tmpArray[0];
		NSIndexSet *dragIndexSet = dragClass.theIndexSet;
		
		NSUInteger currentIndex = [dragIndexSet firstIndex];
		while (currentIndex != NSNotFound) {
			if (currentIndex <= row) {
				minRow++;
			}
			currentIndex = [dragIndexSet indexGreaterThanIndex:currentIndex];
		}
		[self willChangeValueForKey:kMusicListKVO];
		
		NSArray *selArray = [musicList arrayOfObjectsInMusicListAtIndexes:dragIndexSet];
		[musicList removeObjectsInMusicListAtIndexes:dragIndexSet];
		[musicList insertObjects:selArray inMusicListAtIndex:row - minRow];
		
		[self didChangeValueForKey:kMusicListKVO];
		[self musicListContentsDidMove];
		return YES;
	} else if ((tmpArray = [dragPB readObjectsForClasses:
							@[[NSURL class]] options:@{NSPasteboardURLReadingFileURLsOnlyKey : @YES,
													   NSPasteboardURLReadingContentsConformToTypesKey : self.trackerUTIs}])) {
		if ([tmpArray count] < 1)
			return NO;
		
		[self willChangeValueForKey:kMusicListKVO];
		NSMutableArray *mutArray = [NSMutableArray new];
		for (NSURL *curURL in tmpArray) {
			[mutArray addObject:[[MusicListObject alloc] initWithURL:curURL date:[NSDate date]]];
		}
		
		[musicList insertObjects:mutArray inMusicListAtIndex:row];
		
		[self didChangeValueForKey:kMusicListKVO];
		[self musicListContentsDidMove];
		return YES;
	}
	
	return NO;
}

- (BOOL)tableView:(NSTableView *)aTableView writeRowsWithIndexes:(NSIndexSet *)rowIndices toPasteboard:(NSPasteboard*)pboard
{
	BOOL status = NO;
	MusicListDragClass *dragClass = [[MusicListDragClass alloc] initWithIndexSet:rowIndices];
	NSMutableArray *urlArrays = [[NSMutableArray alloc] initWithCapacity:[rowIndices count]];
	NSArray *ppmobjects = [musicList arrayOfObjectsInMusicListAtIndexes:rowIndices];
	for (MusicListObject *obj in ppmobjects) {
		[urlArrays addObject:obj.musicURL];
	}
	[pboard clearContents]; // clear pasteboard to take ownership
	status = [pboard writeObjects:[[urlArrays copy] arrayByAddingObject:dragClass]]; // write the URLs
	return status;
}

- (void)tableView:(NSTableView *)atableView sortDescriptorsDidChange:(NSArray *)oldDescriptors
{
	[self willChangeValueForKey:kMusicListKVO];
	[musicList sortMusicListUsingDescriptors: [atableView sortDescriptors]];
	[self didChangeValueForKey:kMusicListKVO];
	
	[self musicListContentsDidMove];
}

@end
