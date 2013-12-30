//
//  PPDocument.m
//  PPMacho
//
//  Created by C.W. Betts on 9/7/13.
//
//

#import "PPDocument.h"
#import <PlayerPROKit/PlayerPROKit.h>
#import <AVFoundation/AVFoundation.h>
#import "PPApp_AppDelegate.h"
#import "UserDefaultKeys.h"
#import "PPExportObject.h"
#import "PPErrors.h"
#import "BoxViewController.h"

@interface PPDocument ()
@property (strong) PPDriver *theDriver;
@property (strong) PPMusicObjectWrapper *theMusic;
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
	[self willChangeValueForKey:@"musicName"];
	self.theMusic.internalFileName = musicName;
	[self didChangeValueForKey:@"musicName"];
}

- (NSString*)musicInfo
{
	return self.theMusic.madInfo;
}

- (void)setMusicInfo:(NSString *)musicInfo
{
	[self willChangeValueForKey:@"musicInfo"];
	self.theMusic.madInfo = musicInfo;
	[self didChangeValueForKey:@"musicInfo"];
}

- (NSString*)authorString
{
	return self.theMusic.madAuthor;
}

- (void)setAuthorString:(NSString *)authorString
{
	[self willChangeValueForKey:@"authorString"];
	self.theMusic.madAuthor = authorString;
	[self didChangeValueForKey:@"authorString"];
}

- (void)soundPreferencesDidChange:(NSNotification *)notification
{
	[self MADDriverWithPreferences];
}

- (id)init
{
	self = [super init];
	if (self) {
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
		
		self.theDriver = [[PPDriver alloc] initWithLibrary:globalMadLib settings:&init];
		
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
		*outError = nil;
	}
	return self.theMusic.musicWrapper;
}

- (BOOL)readFromFileWrapper:(NSFileWrapper *)fileWrapper ofType:(NSString *)typeName error:(NSError **)outError
{
	self.theMusic = [[PPMusicObjectWrapper alloc] initWithFileWrapper:fileWrapper];
	if (self.theMusic) {
		if (outError) {
			*outError = nil;
		}
		
		return YES;
	} else {
		if (outError) {
			*outError = CreateErrorFromMADErrorType(MADReadingErr);
		}
		
		return NO;
	}
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
		[[NSAlert alertWithError:CreateErrorFromMADErrorType(returnerr)] beginSheetModalForWindow:[self windowForSheet] completionHandler:^(NSModalResponse returnCode) {
			;//Currently, do nothing
		}];
		//return;
	}
}

- (void)importMusicObject:(PPMusicObject*)theObj
{
	[self importMusicObjectWrapper:[[PPMusicObjectWrapper alloc] initFromMusicObject:theObj]];
}

- (void)importMusicObjectWrapper:(PPMusicObjectWrapper*)theWrap
{
	if (!_theMusic) {
		self.theMusic = theWrap;
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

static inline extended80 convertSampleRateToExtended80(unsigned int theNum)
{
	//Yes, the pragma pack is needed
	//otherwise the data will be improperly mapped.
#pragma pack(push, 2)
	union {
		extended80 shortman;
		struct Float80i {
			SInt16  exp;
			UInt32  man[2];
		} intman;
	} toreturn;
#pragma pack(pop)
	
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

- (NSMutableData *)rawSoundData:(MADDriverSettings*)theSet
{
	OSErr err = noErr;
	PPDriver *theRec = [[PPDriver alloc] initWithLibrary:globalMadLib settings:theSet error:&err];
	
	if (theRec == nil) {
		dispatch_async(dispatch_get_main_queue(), ^{
			NSError *NSerr = CreateErrorFromMADErrorType(err);
			[[NSAlert alertWithError:NSerr] beginSheetModalForWindow:[self windowForSheet] completionHandler:^(NSModalResponse returnCode) {
				;//Do nothing for now
			}];
		});
		
		return nil;
	}
	
	[theRec cleanDriver];
	[_theMusic attachToDriver:theRec];
	
	char *soundPtr = NULL;
	NSUInteger full = [theRec audioLength];
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
	
	while([theRec directSaveToPointer:soundPtr settings:theSet])
		[mutData appendBytes:soundPtr length:full];
	
	free(soundPtr);
	return mutData;
}

#import "getAIFF.i"

typedef struct {
	NSInteger	tag;
	CFURLRef	fileURL;
} ExportContextInfo;

- (void)exportSheetDidEnd:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo;
{
	ExportContextInfo *ei = contextInfo;
	NSURL *outURL = CFBridgingRelease(ei->fileURL);
	
	if (returnCode == NSAlertDefaultReturn) {
		switch (ei->tag)
		{
			case -1:
				//AIFF
			{
				PPExportObject *expObj = [[PPExportObject alloc] initWithDestination:outURL exportBlock:^OSErr(NSURL *theURL, NSString * __autoreleasing *errStr) {
					if (errStr)
						*errStr = nil;

					OSErr theErr = [self saveMusicAsAIFFToURL:theURL usingSettings:&exportSettings];
					[_theDriver endExport];
					return theErr;
				}];
				[[NSApp delegate] addExportObject:expObj];
			}
				break;
				
			case -2:
				//M4A
			{
				PPExportObject *expObj = [[PPExportObject alloc] initWithDestination:outURL exportBlock:^OSErr(NSURL *theURL, NSString * __autoreleasing *errStr) {
					OSErr theErr = noErr;
					if (errStr)
						*errStr = nil;
					
					NSArray *metadataInfo;
					NSString *oldMusicName = self.musicName;
					NSString *oldMusicInfo = self.musicInfo;
					NSURL *oldURL = [self fileURL];
					NSURL *tmpURL = [[[NSFileManager defaultManager] URLForDirectory:NSItemReplacementDirectory inDomain:NSUserDomainMask appropriateForURL:oldURL create:YES error:NULL] URLByAppendingPathComponent:[NSString stringWithFormat:@"%@.aiff", (oldMusicName && ![oldMusicName isEqualToString:@""]) ? oldMusicName : @"untitled"] isDirectory:NO];
					theErr = [self saveMusicAsAIFFToURL:tmpURL usingSettings:&exportSettings];
					[_theDriver endExport];
					if (theErr) {
						if (errStr)
							*errStr = [[NSString alloc] initWithFormat:@"Unable to save temporary file to %@, error %d.", [tmpURL path], theErr];

						return theErr;
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
						[titleName setValue:@"PlayerPRO 6"];
						
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
						if (errStr) {
							*errStr = [[NSString alloc] initWithFormat:@"Init Failed for %@.", oldMusicName];
						}
						[[NSFileManager defaultManager] removeItemAtURL:tmpURL error:NULL];
						return MADWritingErr;
					}
					
					AVAssetExportSession *session = [[AVAssetExportSession alloc] initWithAsset:exportMov presetName:AVAssetExportPresetAppleM4A];
					if (!session) {
						if (errStr)
							*errStr = [[NSString alloc] initWithFormat:@"Export session creation for %@ failed.", oldMusicName];
						
						[[NSFileManager defaultManager] removeItemAtURL:tmpURL error:NULL];
						return MADWritingErr;
					}
					[[NSFileManager defaultManager] removeItemAtURL:theURL error:NULL];
					session.outputURL = theURL;
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
						return noErr;
					} else {
						if (errStr)
							*errStr = [[NSString alloc] initWithFormat:@"export of \"%@\" failed.", oldMusicName];
						
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
	NSInteger tag = [sender tag];
	[_theDriver beginExport];
	
	NSString *musicName = self.musicName;
	
	switch (tag) {
		case -1:
			//AIFF
		{
			NSSavePanel *savePanel = [NSSavePanel savePanel];
			[savePanel setAllowedFileTypes:@[AVFileTypeAIFF]];
			[savePanel setCanCreateDirectories:YES];
			[savePanel setCanSelectHiddenExtension:YES];
			if (![musicName isEqualToString:@""]) {
				[savePanel setNameFieldStringValue:musicName];
			}
			[savePanel setPrompt:@"Export"];
			[savePanel setTitle:@"Export as AIFF audio"];
			[savePanel beginSheetModalForWindow:[self windowForSheet] completionHandler:^(NSInteger result) {
				if (result == NSFileHandlingPanelOKButton) {
					[self showExportSettingsWithExportType:-1 URL:[savePanel URL]];
				} else
					[_theDriver endExport];
			}];
			
		}
			break;
			
		case -2:
			//MP4
		{
			NSSavePanel *savePanel = [NSSavePanel savePanel];
			[savePanel setAllowedFileTypes:@[AVFileTypeAppleM4A]];
			[savePanel setCanCreateDirectories:YES];
			[savePanel setCanSelectHiddenExtension:YES];
			if (![musicName isEqualToString:@""]) {
				[savePanel setNameFieldStringValue:musicName];
			}
			[savePanel setPrompt:@"Export"];
			[savePanel setTitle:@"Export as MPEG-4 Audio"];
			[savePanel beginSheetModalForWindow:[self windowForSheet] completionHandler:^(NSInteger result) {
				if (result == NSFileHandlingPanelOKButton) {
					[self showExportSettingsWithExportType:-2 URL:[savePanel URL]];
				} else
					[_theDriver endExport];
			}];
		}
			break;
			
		case -3:
			//MADK
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
			[savePanel beginSheetModalForWindow:[self windowForSheet] completionHandler:^(NSInteger result) {
				if (result == NSFileHandlingPanelOKButton) {
					PPExportObject *expObj = [[PPExportObject alloc] initWithDestination:[savePanel URL] exportBlock:^OSErr(NSURL *theURL, NSString *__autoreleasing *errStr) {
						OSErr theErr = [_theMusic exportMusicToURL:theURL];
						[_theDriver endExport];
						return theErr;

					}];
					[[NSApp delegate] addExportObject:expObj];
				} else {
					[_theDriver endExport];
				}
			}];
		}
			break;
			
		default:
		{
			if (tag > [globalMadLib pluginCount] || tag < 0) {
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
			PPLibraryObject *tmpObj = [globalMadLib pluginAtIndex:tag];
			[savePanel setAllowedFileTypes:tmpObj.UTItypes];
			[savePanel setCanCreateDirectories:YES];
			[savePanel setCanSelectHiddenExtension:YES];
			if (![musicName isEqualToString:@""])
				[savePanel setNameFieldStringValue:musicName];
			
			[savePanel setPrompt:@"Export"];
			[savePanel setTitle:[NSString stringWithFormat:@"Export as %@", tmpObj.menuName]];
			[savePanel beginSheetModalForWindow:[self windowForSheet] completionHandler:^(NSInteger result) {
				if (result == NSFileHandlingPanelOKButton) {
					PPExportObject *expObj = [[PPExportObject alloc] initWithDestination:[savePanel URL] exportBlock:^OSErr(NSURL *theURL, NSString *__autoreleasing *errStr) {
						OSErr theErr = [_theMusic exportMusicToURL:theURL format:tmpObj.plugType library:globalMadLib];
						[_theDriver endExport];
						return theErr;
					}];
					[[NSApp delegate] addExportObject:expObj];
				} else
					[_theDriver endExport];
			}];
		}
			break;
	}
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

- (IBAction)showWavePreview:(id)sender
{
	
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
