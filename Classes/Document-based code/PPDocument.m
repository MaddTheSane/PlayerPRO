//
//  PPDocument.m
//  PPMacho
//
//  Created by C.W. Betts on 9/7/13.
//
//

#import "PPDocument.h"
#import <PlayerPROKit/PlayerPROKit.h>
#import <QTKit/QTKit.h>
#import <QTKit/QTExportSession.h>
#import <QTKit/QTExportOptions.h>
#import "PPApp_AppDelegate.h"
#import "UserDefaultKeys.h"
#import "PPExportObject.h"
#import "PPErrors.h"

@interface PPDocument ()
@property (strong) PPDriver *theDriver;
@property (strong) PPMusicObjectWrapper *theMusic;
@property (weak) PPLibrary *globalLib;
@property MADDriverSettings exportSettings;
@end

@implementation PPDocument
@synthesize exportSettings;

- (NSString*)musicName
{
	return self.theMusic.internalFileName;
}

- (void)setMusicName:(NSString *)musicName
{
	self.theMusic.internalFileName = musicName;
}

- (NSString*)musicInfo
{
	return self.theMusic.madInfo;
}

- (void)setMusicInfo:(NSString *)musicInfo
{
	self.theMusic.madInfo = musicInfo;
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
		}

		self.theDriver = [[PPDriver alloc] initWithLibrary:self.globalLib settings:&init];
		
		self.exportController = [[PPSoundSettingsViewController alloc] init];
		self.exportController.delegate = self;
		
		NSNotificationCenter *defaultCenter = [NSNotificationCenter defaultCenter];
		[defaultCenter addObserver:self selector:@selector(soundPreferencesDidChange:) name:PPSoundPreferencesDidChange object:nil];

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

- (void)MADDriverWithPreferences
{
	OSErr returnerr = noErr;
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
	
	returnerr = [_theDriver changeDriverSettingsToSettings:init];
	//[[NSNotificationCenter defaultCenter] postNotificationName:PPDriverDidChange object:self];
	
	if (returnerr != noErr) {
		[[NSAlert alertWithError:CreateErrorFromMADErrorType(returnerr)] runModal];
		//return;
	}
}

#if 0
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

- (IBAction)saveInstrumentList:(id)sender
{
	[_theDriver beginExport];
	NSSavePanel *savePanel = [NSSavePanel savePanel];
	[savePanel setAllowedFileTypes:@[PPInstrumentListUTI]];
	[savePanel setCanCreateDirectories:YES];
	[savePanel setCanSelectHiddenExtension:YES];
	if (![musicName isEqualToString:@""]) {
		[savePanel setNameFieldStringValue:[NSString stringWithFormat:@"%@'s instruments", musicName]];
	} else {
		[savePanel setNameFieldStringValue:@"Tracker Instruments"];
	}
	
	if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
		OSErr fileErr = [instrumentController exportInstrumentListToURL:[savePanel URL]];
		if (fileErr) {
			NSError *theErr = CreateErrorFromMADErrorType(fileErr);
			[[NSAlert alertWithError:theErr] runModal];
		}
	}
	
	[_theDriver endExport];
}

#endif

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
		container.numSampleFrames = (UInt32)(dataLen / todiv);
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
	PPDriver *theRec = [[PPDriver alloc] initWithLibrary:_globalLib settings:theSet];
	
	if (theRec == nil) {
#if 0
		dispatch_async(dispatch_get_main_queue(), ^{
			NSError *NSerr = CreateErrorFromMADErrorType(err);
			[[NSAlert alertWithError:NSerr] runModal];
		});
#endif
		
		return nil;
	}
	[theRec cleanDriver];
	[_theMusic attachToDriver:theRec];
	
	Ptr soundPtr = NULL;
	NSUInteger full = [theRec audioLength];
	if (theSet->outPutBits == 8) {
		//full = audLen;
	}else if (theSet->outPutBits == 16) {
		full *= 2;
	} else if (theSet->outPutBits == 20 || theSet->outPutBits == 24 ) {
		full *= 3;
	} else {
		//This is just to make the Static analyzer happy
		//full = MADAudioLength(theRec);
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
	
	while([theRec directSaveToPointer:soundPtr settings:theSet])
	{
		[mutData appendBytes:soundPtr length:full];
	}
	
	free(soundPtr);
	return [self newAIFFDataFromSettings:theSet data:mutData];
}

#ifndef PPEXPORT_CREATE_TMP_AIFF
#define PPEXPORT_CREATE_TMP_AIFF 1
#endif

typedef struct {
	NSInteger	tag;
	CFURLRef	fileURL;
} ExportContextInfo;

- (void)exportSheetDidEnd:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo;
{
	ExportContextInfo *ei = contextInfo;
	NSURL *outURL = CFBridgingRelease(ei->fileURL);
	
	if (returnCode == NSAlertDefaultReturn) {
		
		switch (ei->tag) {
			case -1:
			{
				PPExportObject *expObj = [[PPExportObject alloc] initWithDestination:outURL exportBlock:^OSErr(NSURL *theURL, NSString * __autoreleasing *errStr) {
					if (errStr) {
						*errStr = nil;
					}
					NSData *saveData = [self getSoundData:&exportSettings];
					if (!saveData) {
						return MADNeedMemory;
					}
					[_theDriver endExport];
					return [saveData writeToURL:theURL atomically:YES] ? noErr : MADWritingErr;
				}];
				[[NSApp delegate] addExportObject:expObj];
			}
				break;
				
			case -2:
			{
				PPExportObject *expObj = [[PPExportObject alloc] initWithDestination:outURL exportBlock:^OSErr(NSURL *theURL, NSString * __autoreleasing *errStr) {
					if (errStr) {
						*errStr = nil;
					}
					NSData *saveData = [self getSoundData:&exportSettings];
					if (!saveData) {
						return MADNeedMemory;
					}
					NSString *oldMusicName = self.musicName;
					NSString *oldMusicInfo = self.musicInfo;
					NSURL *oldURL = [self fileURL];
					[_theDriver endExport];
					NSError *expErr = nil;
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
						if (errStr) {
							*errStr = [[NSString alloc] initWithFormat:@"Init Failed for %@, error: %@", oldMusicName, [expErr localizedDescription]];
						}
#if PPEXPORT_CREATE_TMP_AIFF
						[[NSFileManager defaultManager] removeItemAtURL:tmpURL error:NULL];
#endif
						return MADWritingErr;
					}
					
					QTExportSession *session = [[QTExportSession alloc] initWithMovie:exportMov exportOptions:[QTExportOptions exportOptionsWithIdentifier:QTExportOptionsAppleM4A] outputURL:theURL error:&expErr];
					if (!session) {
						if (errStr) {
							*errStr = [[NSString alloc] initWithFormat:@"Export session creation for %@ failed, error: %@", oldMusicName, [expErr localizedDescription]];
						}
#if PPEXPORT_CREATE_TMP_AIFF
						[[NSFileManager defaultManager] removeItemAtURL:tmpURL error:NULL];
#endif
						
						return MADWritingErr;
					}
					[session run];
					BOOL didFinish = [session waitUntilFinished:&expErr];
#if PPEXPORT_CREATE_TMP_AIFF
					[[NSFileManager defaultManager] removeItemAtURL:tmpURL error:NULL];
#endif
					
					if (didFinish) {
						return noErr;
					} else {
						if (errStr) {
							*errStr = [[NSString alloc] initWithFormat:@"export of \"%@\" failed, error: %@", oldMusicName, [expErr localizedDescription]];
						}
						return MADWritingErr;
					}
				}];
				[[NSApp delegate] addExportObject:expObj];
			}
				break;
				
			default:
				break;
		}
	}
	[_theDriver endExport];
	
	free(contextInfo);
}
	
- (void)showExportSettingsWithExportType:(NSInteger)expType URL:(NSURL*)theURL
{
	MADGetBestDriver(&exportSettings);
	exportSettings.driverMode = NoHardwareDriver;
	exportSettings.repeatMusic = FALSE;
	[_exportController settingsFromDriverSettings:&exportSettings];
	ExportContextInfo *ei = calloc(sizeof(ExportContextInfo), 1);
	ei->tag = expType;
	ei->fileURL = CFBridgingRetain(theURL);
	
	[NSApp beginSheet:_exportWindow modalForWindow:[self windowForSheet] modalDelegate:self didEndSelector:@selector(exportSheetDidEnd:returnCode:contextInfo:) contextInfo:ei];
}

- (IBAction)exportMusicAs:(id)sender
{
	//TODO: show the save dialog on the document window, not the whole app!
	NSInteger tag = [sender tag];
	[_theDriver beginExport];
	
	NSString *musicName = self.musicName;
	
	switch (tag) {
		case -1:
			//AIFF
		{
			NSSavePanel *savePanel = [NSSavePanel savePanel];
			[savePanel setAllowedFileTypes:@[@"public.aiff-audio"]];
			[savePanel setCanCreateDirectories:YES];
			[savePanel setCanSelectHiddenExtension:YES];
			if (![musicName isEqualToString:@""]) {
				[savePanel setNameFieldStringValue:musicName];
			}
			[savePanel setPrompt:@"Export"];
			[savePanel setTitle:@"Export as AIFF audio"];
			if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
				[self showExportSettingsWithExportType:-1 URL:[savePanel URL]];
			} else {
				[_theDriver endExport];
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
			if (![musicName isEqualToString:@""]) {
				[savePanel setNameFieldStringValue:musicName];
			}
			[savePanel setPrompt:@"Export"];
			[savePanel setTitle:@"Export as MPEG-4 Audio"];
			if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
				[self showExportSettingsWithExportType:-2 URL:[savePanel URL]];
			} else {
				[_theDriver endExport];
			}
		}
			break;
			
		case -3:
		{
			NSSavePanel *savePanel = [NSSavePanel savePanel];
			[savePanel setAllowedFileTypes:@[MADNativeUTI]];
			[savePanel setCanCreateDirectories:YES];
			[savePanel setCanSelectHiddenExtension:YES];
			if (![musicName isEqualToString:@""]) {
				[savePanel setNameFieldStringValue:musicName];
			}
			[savePanel setPrompt:@"Export"];
			[savePanel setTitle:@"Export as MADK"];
			if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
				PPExportObject *expObj = [[PPExportObject alloc] initWithDestination:[savePanel URL] exportBlock:^OSErr(NSURL *theURL, NSString *__autoreleasing *errStr) {
					return [_theMusic exportMusicToURL:theURL];
				}];
				[[NSApp delegate] addExportObject:expObj];
			}
		}
			break;
			
		default:
		{
			if (tag > [_globalLib pluginCount] || tag < 0) {
				NSBeep();
				[_theDriver endExport];
#if 0
				if (isQuitting) {
					[NSApp replyToApplicationShouldTerminate:YES];
				}
#endif
				
				return;
			}
			NSSavePanel *savePanel = [NSSavePanel savePanel];
			PPLibraryObject *tmpObj = [_globalLib pluginAtIndex:tag];
			[savePanel setAllowedFileTypes:tmpObj.UTItypes];
			[savePanel setCanCreateDirectories:YES];
			[savePanel setCanSelectHiddenExtension:YES];
			if (![musicName isEqualToString:@""]) {
				[savePanel setNameFieldStringValue:musicName];
			}
			[savePanel setPrompt:@"Export"];
			[savePanel setTitle:[NSString stringWithFormat:@"Export as %@", tmpObj.menuName]];
			if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
				PPExportObject *expObj = [[PPExportObject alloc] initWithDestination:[savePanel URL] exportBlock:^OSErr(NSURL *theURL, NSString *__autoreleasing *errStr) {
					OSErr theErr = [_theMusic exportMusicToURL:theURL format:tmpObj.plugType library:_globalLib];
					[_theDriver endExport];
					return theErr;
				}];
				[[NSApp delegate] addExportObject:expObj];
			} else {
				[_theDriver endExport];
			}
		}
			break;
	}
}

- (IBAction)okayExportSettings:(id)sender {
	[NSApp endSheet:self.exportWindow returnCode:NSAlertDefaultReturn];
	[self.exportWindow close];
}

- (IBAction)cancelExportSettings:(id)sender {
	[NSApp endSheet:self.exportWindow returnCode:NSAlertAlternateReturn];
	[self.exportWindow close];
}

- (void)awakeFromNib
{
	[self.exportSettingsBox setContentView:self.exportController.view];
}

- (void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
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
