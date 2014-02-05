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
#import "PPMusicListDragClass.h"
#include <PlayerPROCore/RDriverInt.h>
#include "PPByteswap.h"
#import <AVFoundation/AVFoundation.h>

#define kUnresolvableFile @"Unresolvable files"
#define kUnresolvableFileDescription @"There were %lu file(s) that were unable to be resolved."

@interface PPCurrentlyPlayingIndex : NSObject
@property NSInteger index;
@property (strong) NSURL *playbackURL;
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

static void CocoaDebugStr(short line, const char *file, const char *text)
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
@property (strong) NSString *musicInfo;
@property (strong, readonly) NSDictionary	*trackerDict;
@property (strong, readonly) NSArray		*trackerUTIs;
@property (strong) PPMusicList				*musicList;
@property (strong) PPCurrentlyPlayingIndex	*currentlyPlayingIndex;
@property (strong) PPCurrentlyPlayingIndex	*previouslyPlayingIndex;
@property (strong) PPPreferences			*preferences;
@property (strong) NSMutableArray			*plugInInfos;
@property BOOL isQuitting;

- (void)selectCurrentlyPlayingMusic;
- (void)selectMusicAtIndex:(NSInteger)anIdx;
- (void)musicListContentsDidMove;
- (BOOL)musicListWillChange;
- (void)musicListDidChange;
- (void)moveMusicAtIndex:(NSUInteger)from toIndex:(NSUInteger)to;
@end

@implementation AppDelegate
@synthesize music;
@synthesize madLib;
@synthesize toolsPanel;
@synthesize instrumentController;
@synthesize musicList;
@synthesize currentlyPlayingIndex, previouslyPlayingIndex;
@synthesize preferences;
@synthesize isQuitting;
@synthesize trackerUTIs = _trackerUTIs;
@synthesize trackerDict = _trackerDict;
- (NSDictionary *)trackerDict
{
	if (!_trackerDict || [_trackerDict count] != [madLib pluginCount]) {
		NSMutableDictionary *trackerDict =
		[NSMutableDictionary dictionaryWithDictionary:@{
														NSLocalizedStringWithDefaultValue(@"PPMADKFile", @"InfoPlist",
																						  [NSBundle mainBundle],
																						  @"MADK Tracker", @"MADK Tracker") : @[MADNativeUTI],
														NSLocalizedString(@"Generic MAD tracker", @"Generic MAD tracker"): @[MADGenericUTI],
														NSLocalizedString(@"MAD Package", @"MAD Package"):@[MADPackageUTI]}];
		for (PPLibraryObject *obj in madLib) {
			trackerDict[obj.menuName] = obj.UTItypes;
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
			NSError *err;
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
	Boolean madWasReading = false;
	long fullTime = 0, curTime = 0;
	OSErr returnerr = noErr;
	if (madDriver) {
		madWasReading = [madDriver isPlayingMusic];
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
	
	//OSErr returnerr = MADCreateDriver(&init, madLib, &madDriver);
	if (!madDriver)
		madDriver = [[PPDriver alloc] initWithLibrary:madLib settings:&init error:&returnerr];
	else
		returnerr = [madDriver changeDriverSettingsToSettings:init];
	[[NSNotificationCenter defaultCenter] postNotificationName:PPDriverDidChange object:self];
	if (returnerr != noErr) {
		NSError *err = CreateErrorFromMADErrorType(returnerr);
		[[NSAlert alertWithError:err] runModal];
		return;
	}
	//MADStartDriver(madDriver);
	if (music) {
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
			if ([musicList countOfMusicList] > ++currentlyPlayingIndex.index) {
				[self selectCurrentlyPlayingMusic];
				NSError *err;
				if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err])
				{
					[[NSAlert alertWithError:err] runModal];
				}
			} else {
				if ([userDefaults boolForKey:PPLoopMusicWhenDone]) {
					currentlyPlayingIndex.index = 0;
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
			currentlyPlayingIndex.index = random() % [musicList countOfMusicList];
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
	if (music) {
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
	[music saveMusicToURL:tosave];
}

- (NSMutableData*)rawSoundData:(MADDriverSettings*)theSet
{
	OSErr err = noErr;
	PPDriver *theRec = [[PPDriver alloc] initWithLibrary:madLib settings:theSet error:&err];
	
	if (theRec == nil) {
		dispatch_async(dispatch_get_main_queue(), ^{
			NSError *NSerr = CreateErrorFromMADErrorType(MADUnknownErr);
			[[NSAlert alertWithError:NSerr] runModal];
		});
		
		return nil;
	}
	
	[theRec cleanDriver];
	[theRec setCurrentMusic:music];
	
	char *soundPtr = NULL;
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
	
	NSMutableData *mutData = [[NSMutableData alloc] init];
	soundPtr = calloc(full, 1);
	
	while ([theRec directSaveToPointer:soundPtr settings:theSet])
		[mutData appendBytes:soundPtr length:full];
	
	free(soundPtr);
	return mutData;
}

#import "getAIFF.mi"

- (NSInteger)showExportSettings
{
	MADGetBestDriver(&exportSettings);
	exportSettings.driverMode = NoHardwareDriver;
	exportSettings.repeatMusic = FALSE;
	[exportController settingsFromDriverSettings:&exportSettings];
	return [NSApp runModalForWindow:self.exportWindow];
}

- (IBAction)exportMusicAs:(id)sender
{
	NSInteger tag = [sender tag];
	[madDriver beginExport];
	
	switch (tag) {
		case -1:
			//AIFF
		{
			NSSavePanel *savePanel = [NSSavePanel savePanel];
			[savePanel setAllowedFileTypes:@[AVFileTypeAIFF]];
			[savePanel setCanCreateDirectories:YES];
			[savePanel setCanSelectHiddenExtension:YES];
			if (![self.musicName isEqualToString:@""])
				[savePanel setNameFieldStringValue:self.musicName];
			
			[savePanel setPrompt:@"Export"];
			[savePanel setTitle:@"Export as AIFF audio"];
			if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
				if ([self showExportSettings] == NSAlertDefaultReturn) {
					dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
						@autoreleasepool {
							OSErr thErr =[self saveMusicAsAIFFToURL:[savePanel URL] usingSettings:&exportSettings];
							[madDriver endExport];
							if (thErr != noErr)
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
				} else {
					[madDriver endExport];
				}
			} else {
				[madDriver endExport];
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
						OSErr theErr = noErr;
						
						NSURL *oldURL = [[musicList objectInMusicListAtIndex:previouslyPlayingIndex.index] musicUrl];
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
						
						if ((theErr = [self saveMusicAsAIFFToURL:tmpURL usingSettings:&exportSettings]) != noErr) {
							expErr = CreateErrorFromMADErrorType(theErr);
							[[NSFileManager defaultManager] removeItemAtURL:tmpURL error:NULL];
							dispatch_async(dispatch_get_main_queue(), errBlock);
						}
						
						AVAsset *exportMov = [AVAsset assetWithURL:tmpURL];
						{
							AVMutableMetadataItem *titleName = [AVMutableMetadataItem new];
							[titleName setKeySpace:AVMetadataKeySpaceCommon];
							[titleName setKey:AVMetadataCommonKeyTitle];
							[titleName setValue:oldMusicName];
							
							AVMutableMetadataItem *dataInfo = [AVMutableMetadataItem new];
							[titleName setKeySpace:AVMetadataKeySpaceCommon];
							[titleName setKey:AVMetadataCommonKeySoftware];
							[titleName setValue:@"PlayerPRO Player"];
							
							AVMutableMetadataItem *musicInfoQTUser = [AVMutableMetadataItem new];
							[musicInfoQTUser setKeySpace:AVMetadataKeySpaceQuickTimeUserData];
							[musicInfoQTUser setKey:AVMetadataQuickTimeUserDataKeyInformation];
							[musicInfoQTUser setValue:oldMusicInfo];
							
							AVMutableMetadataItem *musicInfoiTunes = [AVMutableMetadataItem new];
							[musicInfoiTunes setKeySpace:AVMetadataKeySpaceiTunes];
							[musicInfoiTunes setKey:AVMetadataiTunesMetadataKeyUserComment];
							[musicInfoiTunes setValue:oldMusicInfo];
							
							AVMutableMetadataItem *musicInfoQTMeta = [AVMutableMetadataItem new];
							[musicInfoQTMeta setKeySpace:AVMetadataKeySpaceQuickTimeMetadata];
							[musicInfoQTMeta setKey:AVMetadataQuickTimeMetadataKeyInformation];
							[musicInfoQTMeta setValue:oldMusicInfo];
							
							metadataInfo = @[titleName, dataInfo, musicInfoQTUser, musicInfoiTunes, musicInfoQTMeta];
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
						[[NSFileManager defaultManager] removeItemAtURL:[savePanel URL] error:NULL];
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
				} else
					[madDriver endExport];
			} else
				[madDriver endExport];
		}
			break;
			
			case -3:
			// wave
		{
			NSSavePanel *savePanel = [NSSavePanel savePanel];
			[savePanel setAllowedFileTypes:@[AVFileTypeWAVE]];
			[savePanel setCanCreateDirectories:YES];
			[savePanel setCanSelectHiddenExtension:YES];
			if (![self.musicName isEqualToString:@""])
				[savePanel setNameFieldStringValue:self.musicName];
			
			[savePanel setPrompt:@"Export"];
			[savePanel setTitle:@"Export as Wave Audio"];
			if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
				if ([self showExportSettings] == NSAlertDefaultReturn) {
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
if (res != noErr){ \
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
							
							CFURLRef url = CFBridgingRetain([savePanel URL]);
							
							AudioFileID audioFile;
							OSStatus res;
							res = AudioFileCreateWithURL(url, kAudioFileWAVEType, &asbd, kAudioFileFlags_EraseFile, &audioFile);
							CFRelease(url);
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
				} else
					[madDriver endExport];
			} else
				[madDriver endExport];
		}
			break;
			
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
			NSSavePanel *savePanel = [NSSavePanel savePanel];
			[savePanel setAllowedFileTypes:[madLib pluginAtIndex:tag].UTItypes];
			[savePanel setCanCreateDirectories:YES];
			[savePanel setCanSelectHiddenExtension:YES];
			if (![self.musicName isEqualToString:@""]) {
				[savePanel setNameFieldStringValue:self.musicName];
			}
			[savePanel setPrompt:@"Export"];
			[savePanel setTitle:[NSString stringWithFormat:@"Export as %@", [madLib pluginAtIndex:tag].menuName]];
			if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
				NSURL *fileURL = [savePanel URL];
				OSErr err = [music exportMusicToURL:fileURL format:[madLib pluginAtIndex:tag].plugType library:madLib];
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
						NSInteger retVal = NSRunInformationalAlertPanel(@"Export complete", @"The export of the file \"%@\" is complete.", @"OK", @"Show File", nil, [[savePanel URL] lastPathComponent]);
						if (retVal == NSAlertAlternateReturn) {
							[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:@[fileURL]];
						}
					}
				}
			}
		}
			[madDriver endExport];
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
	[madDriver beginExport];
	
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
	[madDriver endExport];
}

- (IBAction)saveMusic:(id)sender
{
	[madDriver beginExport];
	
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
			[madDriver endExport];
		} else {
			// saveMusicAs: will end the exporting when it is done.
			[self saveMusicAs:sender];
		}
	}
}

- (BOOL)loadMusicURL:(NSURL*)musicToLoad error:(out NSError *__autoreleasing*)theErr
{
	char fileType[5];
	OSErr theOSErr = noErr;
	if (music != NULL) {
		[madDriver stop];
		[madDriver stopDriver];
	}
	
	theOSErr = [madLib identifyFileAtURL:musicToLoad type:fileType];
	
	if (theOSErr != noErr) {
		if (theErr) {
			*theErr = CreateErrorFromMADErrorType(theOSErr);
		}
		self.paused = YES;
		[self clearMusic];
		return NO;
	}
	
	self.music = [[PPMusicObject alloc] initWithURL:musicToLoad library:madLib];
	if (!music) {
		theOSErr = MADReadingErr;
	}
	
	if (theOSErr != noErr) {
		if (theErr) {
			*theErr = CreateErrorFromMADErrorType(theOSErr);
		}
		self.paused = YES;
		[self clearMusic];
		return NO;
	}
	
	[music attachToDriver:madDriver];
	[madDriver play];
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
	return [self loadMusicURL:musicToLoad error:NULL];
}

- (IBAction)showPreferences:(id)sender
{
	if (!preferences) {
		preferences = [[PPPreferences alloc] init];
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
	currentlyPlayingIndex.index = [tableView selectedRow];
	if ([self loadMusicFromCurrentlyPlayingIndexWithError:&err] == NO)
		[[NSAlert alertWithError:err] runModal];
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
}

- (void)updatePlugInInfoMenu
{
	for (PPLibraryObject *obj in madLib) {
		PPPlugInInfo *tmpInfo = [[PPPlugInInfo alloc] initWithPlugName:obj.menuName author:obj.authorString plugType:NSLocalizedString(@"TrackerPlugName", @"Tracker plug-in name") plugURL:[obj.plugFile bundleURL]];
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
	madLib = [[PPLibrary alloc] init];
	//the NIB won't store the value anymore, so do this hackery to make sure there's some value in it.
	[songTotalTime setIntegerValue:0];
	[songCurTime setIntegerValue:0];
	
	[tableView registerForDraggedTypes:@[PPMLDCUTI, (NSString*)kUTTypeFileURL]];
	[self addObserver:self forKeyPath:@"paused" options:NSKeyValueObservingOptionNew context:NULL];
	self.paused = YES;
	[self willChangeValueForKey:kMusicListKVO];
	musicList = [[PPMusicList alloc] init];
	if ([[NSUserDefaults standardUserDefaults] boolForKey:PPRememberMusicList]) {
		[musicList loadApplicationMusicList];
	}
	NSInteger selMus = musicList.selectedMusic;
	[self didChangeValueForKey:kMusicListKVO];
	
	[tableView setDoubleAction:@selector(doubleClickMusicList)];
	
	NSNotificationCenter *defaultCenter = [NSNotificationCenter defaultCenter];
	[defaultCenter addObserver:self selector:@selector(soundPreferencesDidChange:) name:PPSoundPreferencesDidChange object:nil];
	
	[self MADDriverWithPreferences];
	self.music = [PPMusicObject new];
	[music attachToDriver:madDriver];
	[self setTitleForSongLabelBasedOnMusic];
	
	for (NSInteger i = 0; i < [madLib pluginCount]; i++) {
		PPLibraryObject *obj = [madLib pluginAtIndex:i];
		if (obj.canExport) {
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:[NSString stringWithFormat:@"%@...", obj.menuName] action:@selector(exportMusicAs:) keyEquivalent:@""];
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
		[madDriver stop];
		[madDriver cleanDriver];
		self.music = nil;
	}
	[madDriver stopDriver];
	if ([[NSUserDefaults standardUserDefaults] boolForKey:PPRememberMusicList]) {
		[musicList saveApplicationMusicList];
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
	[musicList sortMusicListByName];
	[self didChangeValueForKey:kMusicListKVO];
	[self musicListContentsDidMove];
}

- (IBAction)playSelectedMusic:(id)sender
{
	NSError *error;
	currentlyPlayingIndex.index = [tableView selectedRow];
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
	if ([panel runModal] == NSFileHandlingPanelOKButton) {
		[self addMusicsToMusicList:[panel URLs]];
	}
}

- (IBAction)toggleInfo:(id)sender
{
	[infoDrawer toggle:sender];
}

- (void)setTitleForSongLabelBasedOnMusic
{
	self.musicName = music.internalFileName;
	self.musicInfo = music.madInfo;
}

- (void)clearMusic
{
	if (music) {
		[madDriver stop];
		[madDriver cleanDriver];
	}
	
	self.paused = YES;
	currentlyPlayingIndex.index = -1;
	currentlyPlayingIndex.playbackURL = nil;
	[currentlyPlayingIndex movePlayingIndexToOtherIndex:previouslyPlayingIndex];
	self.music = [PPMusicObject new];
	[self setTitleForSongLabelBasedOnMusic];
	[[NSNotificationCenter defaultCenter] postNotificationName:PPMusicDidChange object:self];
	[music attachToDriver:madDriver];
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
#if 0
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
#endif
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
				if ([madLib identifyFileAtURL:theURL type:ostype] != noErr || [madLib getInformationFromFileAtURL:theURL type:ostype info:&rec]) {
					NSRunCriticalAlertPanel(NSLocalizedString(@"Unknown File", @"unknown file"), NSLocalizedString(@"The file type could not be identified.", @"Unidentified file"), nil, nil, nil);
					return NO;
				}
				OSType2Ptr(rec.signature, ostype);
				
				NSURL *tmpURL = [[theURL URLByDeletingPathExtension] URLByAppendingPathExtension:[[NSString stringWithCString:ostype encoding:NSMacOSRomanStringEncoding] lowercaseString]];
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
	if ([panel runModal] == NSFileHandlingPanelOKButton) {
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
	NSError *err;

	if (currentlyPlayingIndex.index + 1 < [musicList countOfMusicList]) {
		currentlyPlayingIndex.index++;
		[self selectCurrentlyPlayingMusic];
		if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err]) {
			[[NSAlert alertWithError:err] runModal];
		}
	} else if ([[NSUserDefaults standardUserDefaults] boolForKey:PPLoopMusicWhenDone]) {
		currentlyPlayingIndex.index = 0;
		[self selectCurrentlyPlayingMusic];
		if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err]) {
			[[NSAlert alertWithError:err] runModal];
		}
	} else
		NSBeep();
}

- (IBAction)playButtonPressed:(id)sender
{
	if (music) {
		[madDriver play];
		self.paused = NO;
	}
}

- (IBAction)prevButtonPressed:(id)sender
{
	if (currentlyPlayingIndex.index > 0) {
		currentlyPlayingIndex.index--;
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
	if (music) {
		[madDriver setMusicStatusWithCurrentTime:[songPos doubleValue] maximumTime:[songPos maxValue] minimumTime:0];
	}
}

- (IBAction)stopButtonPressed:(id)sender
{
	if (music) {
		[madDriver stop];
		[madDriver cleanDriver];
		[madDriver setMusicStatusWithCurrentTime:0 maximumTime:100 minimumTime:0];
		self.paused = YES;
	}
}

- (IBAction)pauseButtonPressed:(id)sender {
	if (music) {
		if (self.paused) {
			[madDriver play];
		} else {
			[madDriver stop];
			[madDriver cleanDriver];
		}
		self.paused = !self.paused;
	}
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
	NSIndexSet *selected = [tableView selectedRowIndexes];
	NSURL *musicURL;
	PPInfoRec theInfo;
	PPMusicListObject *obj;
	char info[5] = {0};
	char sig[5] = {0};
	NSString *NSSig;
	
	if ([selected count] > 0) {
		musicList.selectedMusic = [selected firstIndex];
	}
	
	if ([selected count] != 1)
		goto badTracker;
	
	obj = [musicList objectInMusicListAtIndex:[selected lastIndex]];
	
	musicURL = obj.musicUrl;
	if ([madLib identifyFileAtURL:musicURL type:info] != noErr)
		goto badTracker;
	if ([madLib getInformationFromFileAtURL:musicURL type:info info:&theInfo] != noErr)
		goto badTracker;
	[fileName setStringValue:obj.fileName];
	[internalName setStringValue:[NSString stringWithCString:theInfo.internalFileName encoding:NSMacOSRomanStringEncoding]];
	[fileSize setIntegerValue:theInfo.fileSize];
	[musicInstrument setIntegerValue:theInfo.totalInstruments];
	[musicPatterns setIntegerValue:theInfo.totalPatterns];
	[musicPlugType setStringValue:[NSString stringWithCString:theInfo.formatDescription encoding:NSMacOSRomanStringEncoding]];
	OSType2Ptr(theInfo.signature, sig);
	NSSig = [NSString stringWithCString:sig encoding:NSMacOSRomanStringEncoding];
	if (!NSSig) {
		NSSig = [NSString stringWithFormat:@"0x%08X", (unsigned int)theInfo.signature];
	}
	[musicSignature setStringValue:NSSig];
	
	[fileLocation setStringValue:[musicURL path]];
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
		[madDriver stop];
		[madDriver cleanDriver];
	}
	if ([[NSUserDefaults standardUserDefaults] boolForKey:PPLoadMusicAtListLoad] && [musicList countOfMusicList] > 0) {
		NSError *err;
		currentlyPlayingIndex.index = selMusFromList != -1 ? selMusFromList : 0;
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
	NSArray *tmpArray = [dragPB readObjectsForClasses:@[[PPMusicListDragClass class]] options:nil];
	if (tmpArray && [tmpArray count] != 0) {
		NSUInteger minRow = 0;
		PPMusicListDragClass *dragClass = tmpArray[0];
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
			[mutArray addObject:[[PPMusicListObject alloc] initWithURL:curURL]];
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
	PPMusicListDragClass *dragClass = [[PPMusicListDragClass alloc] initWithIndexSet:rowIndices];
	NSMutableArray *urlArrays = [NSMutableArray new];
	NSArray *ppmobjects = [musicList arrayOfObjectsInMusicListAtIndexes:rowIndices];
	for (PPMusicListObject *obj in ppmobjects) {
		[urlArrays addObject:obj.musicUrl];
	}
	[pboard clearContents]; // clear pasteboard to take ownership
	status = [pboard writeObjects:[[urlArrays copy] arrayByAddingObject:dragClass]]; // write the URLs
	return status;
}

@end
