//
//  PPDocument.m
//  PPMacho
//
//  Created by C.W. Betts on 9/7/13.
//
//

#import "PPDocument.h"
#import <PlayerPROKit/PlayerPROKit.h>
#import "PPApp_AppDelegate.h"
#import "UserDefaultKeys.h"

@interface PPDocument ()
@property (strong) PPDriver *theDriver;
@property (strong) PPMusicObject *theMusic;
@property (weak) PPLibrary *globalLib;
@end

@implementation PPDocument

- (IBAction)exportMusic:(id)sender
{
	
}

- (id)init
{
    self = [super init];
    if (self) {
		self.globalLib = [[NSApp delegate] madLib];
		MADDriverSettings init;
		{
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
		}

		self.theDriver = [[PPDriver alloc] initWithLibrary:self.globalLib settings:&init];
		
		self.exportController = [[PPSoundSettingsViewController alloc] init];
		self.exportController.delegate = self;
    }
    return self;
}

- (NSString *)windowNibName
{
    // Override returning the nib file name of the document
    // If you need to use a subclass of NSWindowController or if your document supports multiple NSWindowControllers, you should remove this method and override -makeWindowControllers instead.
    return @"PPDocument";
}

- (void)windowControllerDidLoadNib:(NSWindowController *)aController
{
    [super windowControllerDidLoadNib:aController];
    // Add any code here that needs to be executed once the windowController has loaded the document's window.
}

- (NSFileWrapper *)fileWrapperOfType:(NSString *)typeName error:(NSError **)outError
{
	if (outError) {
        *outError = [NSError errorWithDomain:NSOSStatusErrorDomain code:unimpErr userInfo:NULL];
    }
    return nil;

}

- (BOOL)readFromFileWrapper:(NSFileWrapper *)fileWrapper ofType:(NSString *)typeName error:(NSError **)outError
{
	    if (outError) {
        *outError = [NSError errorWithDomain:NSOSStatusErrorDomain code:unimpErr userInfo:NULL];
    }
    return YES;

}

+ (BOOL)autosavesInPlace
{
    return YES;
}

#if 0

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
	
	OSErr returnerr = MADCreateDriver(&init, madLib, &madDriver);
	[[NSNotificationCenter defaultCenter] postNotificationName:PPDriverDidChange object:self];
	if (returnerr != noErr) {
		NSError *err = CreateErrorFromMADErrorType(returnerr);
		[[NSAlert alertWithError:err] runModal];
		RELEASEOBJ(err);
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
	
	NSMutableData *returnData = [[NSMutableData alloc] initWithCapacity:dataLen + sizeof(CommonChunk) + sizeof(SoundDataChunk) + sizeof(ContainerChunk) + [nameData length] + [infoData length]];
	header.ckID = FORMID;
	PPBE32(&header.ckID);
	header.ckSize = dataLen + sizeof(CommonChunk) + sizeof(SoundDataChunk) + 4 + [nameData length] + [infoData length];
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
		container.numSampleFrames = dataLen / todiv;
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
	
	OSErr err = MADCreateDriver( theSet, madLib, &theRec);
	if (err != noErr) {
		dispatch_async(dispatch_get_main_queue(), ^{
			NSError *NSerr = CreateErrorFromMADErrorType(err);
			[[NSAlert alertWithError:NSerr] runModal];
			RELEASEOBJ(NSerr);
		});
		
		return nil;
	}
	MADCleanDriver( theRec);
	
	MADAttachDriverToMusic( theRec, music, NULL);
	MADPlayMusic(theRec);
	
	Ptr soundPtr = NULL;
	long full = 0;
	if (theSet->outPutBits == 8) {
		full = MADAudioLength(theRec);
	}else if (theSet->outPutBits == 16) {
		full = MADAudioLength(theRec) * 2;
	} else if (theSet->outPutBits == 20 || theSet->outPutBits == 24 ) {
		full = MADAudioLength(theRec) * 3;
	} else {
		//This is just to make the Static analyzer happy
		full = MADAudioLength(theRec);
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
	RELEASEOBJ(mutData);
	mutData = nil;
	
	MADStopMusic(theRec);
	MADCleanDriver(theRec);
	MADDisposeDriver(theRec);
	free(soundPtr);
	
	return AUTORELEASEOBJ(retData);
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
	MADBeginExport(madDriver);
	
	switch (tag) {
		case -1:
			//AIFF
		{
			NSSavePanel *savePanel = RETAINOBJ([NSSavePanel savePanel]);
			[savePanel setAllowedFileTypes:@[@"public.aiff-audio"]];
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
						MADEndExport(madDriver);
						
						[saveData writeToURL:[savePanel URL] atomically:YES];
						RELEASEOBJ(saveData);
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
			RELEASEOBJ(savePanel);
		}
			break;
			
		case -2:
			//MP4
		{
			NSSavePanel *savePanel = RETAINOBJ([NSSavePanel savePanel]);
			[savePanel setAllowedFileTypes:@[@"com.apple.m4a-audio"]];
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
						RELEASEOBJ(oldMusicInfo);
						oldMusicInfo = nil;
						RELEASEOBJ(saveData);
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
			RELEASEOBJ(savePanel);
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
			NSSavePanel *savePanel = RETAINOBJ([NSSavePanel savePanel]);
			[savePanel setAllowedFileTypes:BRIDGE(NSArray*, madLib->ThePlug[tag].UTItypes)];
			[savePanel setCanCreateDirectories:YES];
			[savePanel setCanSelectHiddenExtension:YES];
			if (![musicName isEqualToString:@""]) {
				[savePanel setNameFieldStringValue:musicName];
			}
			[savePanel setPrompt:@"Export"];
			[savePanel setTitle:[NSString stringWithFormat:@"Export as %@", BRIDGE(NSString*, madLib->ThePlug[tag].MenuName)]];
			if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
				NSURL *fileURL = RETAINOBJ([savePanel URL]);
				OSErr err = MADMusicExportCFURL(madLib, music, madLib->ThePlug[tag].type, BRIDGE(CFURLRef, fileURL));
				if (err != noErr) {
					if (isQuitting) {
						[NSApp replyToApplicationShouldTerminate:YES];
					} else {
						NSError *aerr = CreateErrorFromMADErrorType(err);
						[[NSAlert alertWithError:aerr] runModal];
						RELEASEOBJ(aerr);
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
				RELEASEOBJ(fileURL);
			}
			RELEASEOBJ(savePanel);
		}
			MADEndExport(madDriver);
			break;
	}
}



#endif

- (IBAction)okayExportSettings:(id)sender {
	[NSApp stopModalWithCode:NSAlertDefaultReturn];
	[self.exportWindow close];
}

- (IBAction)cancelExportSettings:(id)sender {
	[NSApp stopModalWithCode:NSAlertAlternateReturn];
	[self.exportWindow close];
}

- (void)awakeFromNib
{
	[self.exportSettingsBox setContentView:self.exportController.view];
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
