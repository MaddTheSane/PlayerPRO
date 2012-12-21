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

@implementation PPApp_AppDelegate

@synthesize paused;

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
	}
	MADDriverSettings init;
	MADGetBestDriver(&init);
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	//TODO: Sanity Checking
	init.surround = [defaults boolForKey:PPSurroundToggle];
	init.outPutRate = [defaults integerForKey:PPSoundOutRate];
	init.outPutBits = [defaults integerForKey:PPSoundOutBits];
	init.oversampling = [defaults integerForKey:PPOversamplingAmount];
	init.Reverb = [defaults boolForKey:PPReverbToggle];
	init.ReverbSize = [defaults integerForKey:PPReverbSize];
	init.ReverbStrength = [defaults integerForKey:PPReverbStrength];
	init.MicroDelaySize = [defaults integerForKey:PPStereoDelayAmount];
	init.repeatMusic = FALSE;
	
	OSErr returnerr = MADCreateDriver(&init, MADLib, &MADDriver);
	if (returnerr != noErr) {
		NSError *err = CreateErrorFromMADErrorType(returnerr);
		NSAlert *alert = [NSAlert alertWithError:err];
		[alert runModal];
		[err release];
		return;
	}
	MADStartDriver(MADDriver);
	if (Music) {
		MADAttachDriverToMusic(MADDriver, Music, NULL);
	}
	if (madWasReading) {
		MADSetMusicStatus(MADDriver, 0, fullTime, curTime);
		MADPlayMusic(MADDriver);
	}
}

+ (void)initialize
{
	PPMusicList *tempList = [[PPMusicList alloc] init];
	
	[[NSUserDefaults standardUserDefaults] registerDefaults:[NSDictionary dictionaryWithObjectsAndKeys:
															 [NSNumber numberWithBool:YES], PPRememberMusicList,
															 [NSNumber numberWithBool:NO], PPLoadMusicAtListLoad,
															 [NSNumber numberWithInt:PPStopPlaying], PPAfterPlayingMusic,
															 [NSNumber numberWithBool:YES], PPGotoStartupAfterPlaying,
															 [NSNumber numberWithBool:YES], PPSaveModList,
															 [NSNumber numberWithBool:NO], PPLoadMusicAtMusicLoad,
															 [NSNumber numberWithBool:NO], PPLoopMusicWhenDone,
															 
															 [NSNumber numberWithInt:16], PPSoundOutBits,
															 [NSNumber numberWithInt:44100], PPSoundOutRate,
															 [NSNumber numberWithInt:CoreAudioDriver], PPSoundDriver,
															 [NSNumber numberWithBool:NO], PPStereoDelayToggle,
															 [NSNumber numberWithBool:NO], PPReverbToggle,
															 [NSNumber numberWithBool:NO], PPSurroundToggle,
															 [NSNumber numberWithBool:NO], PPOversamplingToggle,
															 [NSNumber numberWithInt:30], PPStereoDelayAmount,
															 [NSNumber numberWithInt:25], PPReverbSize,
															 [NSNumber numberWithInt:30], PPReverbStrength,
															 [NSNumber numberWithInt:1], PPOversamplingAmount,
															 
															 [NSNumber numberWithBool:YES], PPDEShowInstrument,
															 [NSNumber numberWithBool:YES], PPDEShowNote,
															 [NSNumber numberWithBool:YES], PPDEShowEffect,
															 [NSNumber numberWithBool:YES], PPDEShowArgument,
															 [NSNumber numberWithBool:YES], PPDEShowVolume,
															 [NSNumber numberWithBool:YES], PPDEShowMarkers,
															 [NSNumber numberWithInt:0], PPDEMarkerOffsetPref,
															 [NSNumber numberWithInt:3], PPDEMarkerLoopPref,
															 [[NSColor yellowColor] PPencodeColor], PPDEMarkerColorPref,
															 [NSNumber numberWithBool:YES], PPDEMouseClickControlPref,
															 [NSNumber numberWithBool:NO], PPDEMouseClickShiftPref,
															 [NSNumber numberWithBool:NO], PPDEMouseClickCommandPref,
															 [NSNumber numberWithBool:NO], PPDEMouseClickOptionPref,
															 [NSNumber numberWithBool:YES], PPDELineHeightNormal,
															 [NSNumber numberWithBool:YES], PPDEMusicTraceOn,
															 [NSNumber numberWithBool:YES], PPDEPatternWrappingPartition,
															 [NSNumber numberWithBool:YES], PPDEDragAsPcmd,
															 
															 [NSNumber numberWithBool:YES], PPBEMarkersEnabled,
															 [NSNumber numberWithInt:0], PPBEMarkersOffset,
															 [NSNumber numberWithInt:3], PPBEMarkersLoop,
															 [NSNumber numberWithBool:YES], PPBEOctaveMarkers,
															 [NSNumber numberWithBool:NO], PPBENotesProjection,
															 
															 [NSNumber numberWithBool:YES], PPMAddExtension,
															 [NSNumber numberWithBool:YES], PPMMadCompression,
															 [NSNumber numberWithBool:NO], PPMNoLoadMixerFromFiles,
															 [NSNumber numberWithBool:YES], PPMOscilloscopeDrawLines,
															 
															 [NSKeyedArchiver archivedDataWithRootObject:tempList], PPMMusicList,
															 nil]];
	[tempList release];
}

- (IBAction)showMusicList:(id)sender
{
    [window makeKeyAndOrderFront:sender];
}

-(void)selectCurrentlyPlayingMusic
{
	NSIndexSet *idx = [[NSIndexSet alloc] initWithIndex:currentlyPlayingIndex];
	[tableView selectRowIndexes:idx byExtendingSelection:NO];
	[idx release];
}

- (void)songIsDonePlaying
{
	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	switch ([userDefaults integerForKey:PPAfterPlayingMusic]) {
		case PPStopPlaying:
		default:
			MADStopMusic(MADDriver);
			MADCleanDriver(MADDriver);
			MADSetMusicStatus(MADDriver, 0, 100, 0);
			break;
			
		case PPLoopMusic:
			MADSetMusicStatus(MADDriver, 0, 100, 0);
			break;
			
		case PPLoadNext:
		{
			NSInteger tableCount = [musicList countOfMusicList];
			if (tableCount > ++currentlyPlayingIndex) {
				[self selectCurrentlyPlayingMusic];
				NSError *err;
				if (![self loadMusicURL:[musicList URLAtIndex:currentlyPlayingIndex] error:&err])
				{
					NSAlert *alert = [NSAlert alertWithError:err];
					[alert runModal];
					[err release];
				}
			} else {
				if ([userDefaults boolForKey:PPLoopMusicWhenDone]) {
					currentlyPlayingIndex = 0;
					[self selectCurrentlyPlayingMusic];
					NSError *err;
					if (![self loadMusicURL:[musicList URLAtIndex:currentlyPlayingIndex] error:&err])
					{
						NSAlert *alert = [NSAlert alertWithError:err];
						[alert runModal];
						[err release];
					}
				} else {
					MADStopMusic(MADDriver);
					MADCleanDriver(MADDriver);
					MADSetMusicStatus(MADDriver, 0, 100, 0);
				}
			}
		}
			break;
			
		case PPLoadRandom:
		{
			currentlyPlayingIndex = random() % [musicList countOfMusicList];
			[self selectCurrentlyPlayingMusic];
			NSError *err;
			if (![self loadMusicURL:[musicList URLAtIndex:currentlyPlayingIndex] error:&err])
			{
				NSAlert *alert = [NSAlert alertWithError:err];
				[alert runModal];
				[err release];
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
		if (fT == cT) {
			[self songIsDonePlaying];
			MADGetMusicStatus(MADDriver, &fT, &cT);
		}
		[songPos setDoubleValue:cT];
		[songCurTime setIntegerValue:cT];
	}
}

- (BOOL)loadMusicURL:(NSURL*)musicToLoad error:(NSError **)theErr
{
	if (Music != NULL) {
		MADStopMusic(MADDriver);
		MADCleanDriver(MADDriver);
		MADDisposeMusic(&Music, MADDriver);
	}

	char fileType[5];
	OSErr theOSErr = MADMusicIdentifyCFURL(MADLib, fileType, (CFURLRef)musicToLoad);
		
	if(theOSErr != noErr)
	{
		if (theErr) {
			*theErr = CreateErrorFromMADErrorType(theOSErr);
		}
		return NO;
	}
	theOSErr = MADLoadMusicCFURLFile(MADLib, &Music, fileType, (CFURLRef)musicToLoad);
	
	if (theOSErr != noErr) {
		if (theErr) {
			*theErr = CreateErrorFromMADErrorType(theOSErr);
		}
		return NO;
	}
	
	MADAttachDriverToMusic(MADDriver, Music, NULL);
	MADPlayMusic(MADDriver);
	long fT, cT;
	self.paused = NO;
	MADGetMusicStatus(MADDriver, &fT, &cT);
	[songPos setMaxValue:fT];
	[songPos setMinValue:0.0];
	[songLabel setTitleWithMnemonic:[NSString stringWithCString:Music->header->name encoding:NSMacOSRomanStringEncoding]];
	[songTotalTime setIntegerValue:fT];

	return YES;
}

- (BOOL)loadMusicURL:(NSURL*)musicToLoad
{
	[self loadMusicURL:musicToLoad error:NULL];
}

- (IBAction)showPreferences:(id)sender
{
    if (!preferences) {
		preferences = [[PPPreferences alloc] init];
	}
	[preferences showWindow:sender];
}


- (IBAction)exportInstrumentAs:(id)sender {
    
}

- (IBAction)showInstrumentsList:(id)sender {
    
}

- (IBAction)showTools:(id)sender {
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
				break;
				
			case YES:
				[pauseButton setState:NSOnState];
				break;
		}
	}
	
	//[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
}

@synthesize window;

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
	[self willChangeValueForKey:@"musicList"];
	musicList = [[PPMusicList alloc] init];
	if ([[NSUserDefaults standardUserDefaults] boolForKey:PPRememberMusicList]) {
		[musicList loadMusicListFromPreferences];
	}
	[self didChangeValueForKey:@"musicList"];
	NSNotificationCenter *defaultCenter = [NSNotificationCenter defaultCenter];
	[defaultCenter addObserver:self selector:@selector(preferencesDidChange:) name:PPListPreferencesDidChange object:nil];
	[defaultCenter addObserver:self selector:@selector(soundPreferencesDidChange:) name:PPSoundPreferencesDidChange object:nil];
	[defaultCenter addObserver:self selector:@selector(digitalEditorPreferencesDidChange:) name:PPDigitalEditorPrefrencesDidChange object:nil];
	

	//[tableView setDataSource:musicList];
	[self MADDriverWithPreferences];
	
	timeChecker = [[NSTimer alloc] initWithFireDate:[NSDate dateWithTimeIntervalSinceNow:0] interval:1/4.0 target:self selector:@selector(updateMusicStats:) userInfo:nil repeats:YES];
	[[NSRunLoop mainRunLoop] addTimer:timeChecker forMode:NSDefaultRunLoopMode];
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
	[timeChecker invalidate];

	[self removeObserver:self forKeyPath:@"paused"];
	
	if (Music != NULL) {
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
}

- (void)preferencesDidChange:(NSNotification *)notification {
	
}

- (void)dealloc
{
	[preferences release];
	[musicList release];
	[timeChecker release];
	
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	[super dealloc];
}

- (IBAction)deleteInstrument:(id)sender
{
    
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

- (IBAction)sortMusicList:(id)sender {
	[self willChangeValueForKey:@"musicList"];
	[musicList sortMusicList];
	[self didChangeValueForKey:@"musicList"];
}

- (IBAction)playSelectedMusic:(id)sender
{
	NSError *error = nil;
	currentlyPlayingIndex = [tableView selectedRow];
	NSURL* musicURL = [musicList URLAtIndex:currentlyPlayingIndex];
	if ([self loadMusicURL:musicURL error:&error] == NO)
	{
		NSAlert *alert = [NSAlert alertWithError:error];
		[alert runModal];
		[error release];
	}
}

- (IBAction)addMusic:(id)sender
{
	NSOpenPanel *panel = [[NSOpenPanel openPanel] retain];
	NSMutableArray *supportedUTIs = [NSMutableArray array];
	[supportedUTIs addObject:@"com.quadmation.playerpro.madk"];
	int i = 0;
	@autoreleasepool {
		for (i = 0; i < MADLib->TotalPlug; i++) {
			NSArray *tempArray = [NSArray arrayWithArray:(id)MADLib->ThePlug[i].UTItypes];
			[supportedUTIs addObjectsFromArray:tempArray];
		}
	}
	[panel setAllowsMultipleSelection:NO];
	[panel setAllowedFileTypes:supportedUTIs];
	if([panel runModal] == NSFileHandlingPanelOKButton)
	{
		[self willChangeValueForKey:@"musicList"];
		[musicList addMusicURL:[panel URL]];
		[self didChangeValueForKey:@"musicList"];
	}
	
	[panel release];
}

- (IBAction)removeSelectedMusic:(id)sender {
#if 0
	NSIndexSet *selMusic = [tableView selectedRowIndexes];
	NSInteger i = 0;
	NSInteger selIndexes = [selMusic count];
	NSUInteger *indexArray = malloc(sizeof(NSUInteger) * selIndexes);
	NSRange theRange;
	theRange.location = 0;
	theRange.length = NSUIntegerMax;
	[selMusic getIndexes:indexArray maxCount:selIndexes inIndexRange:&theRange];
	[self willChangeValueForKey:@"musicList"];
	for (i = selIndexes - 1; i >= 0 ; i--) {
		[musicList removeObjectInMusicListAtIndex:indexArray[i]];
	}
	[self didChangeValueForKey:@"musicList"];
	free(indexArray);
#else
	[self willChangeValueForKey:@"musicList"];
	[musicList removeObjectInMusicListAtIndex:[tableView selectedRow]];
	[self didChangeValueForKey:@"musicList"];
#endif
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

- (void)digitalEditorPreferencesDidChange:(NSNotification *)notification
{
	
}

- (IBAction)saveMusicList:(id)sender {
	NSSavePanel *savePanel = [[NSSavePanel savePanel] retain];
	[savePanel setAllowedFileTypes:[NSArray arrayWithObject:@"net.sourceforge.playerpro.musiclist"]];
	[savePanel setCanCreateDirectories:YES];
	[savePanel setCanSelectHiddenExtension:YES];
	if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
		[musicList saveMusicListToURL:[savePanel URL]];
	}
	[savePanel release];
}

- (IBAction)fastForwardButtonPressed:(id)sender
{
    
}

- (IBAction)loopButtonPressed:(id)sender
{
    
}

- (IBAction)nextButtonPressed:(id)sender
{
    
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

static NSString * const doubleDash = @"--";

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{

	NSInteger selected = [tableView selectedRow];
	if (selected < 0) {
		goto badMus;
	}
	NSURL *musicURL = [musicList URLAtIndex:selected];
	PPInfoRec theInfo;
	char info[5] = {0};
	if(MADMusicIdentifyCFURL(MADLib, info, (CFURLRef)musicURL) != noErr) goto badMus;
	if(MADMusicInfoCFURL(MADLib, info, (CFURLRef)musicURL, &theInfo) != noErr) goto badMus;
	[fileName setTitleWithMnemonic:[musicURL lastPathComponent]];
	[internalName setTitleWithMnemonic:[NSString stringWithCString:theInfo.internalFileName encoding:NSMacOSRomanStringEncoding]];
	[fileSize setTitleWithMnemonic:[NSString stringWithFormat:@"%.2f kiB", theInfo.fileSize / 1024.0]];
	[musicInstrument setTitleWithMnemonic:[NSString stringWithFormat:@"%d", theInfo.totalInstruments]];
	[musicPatterns setTitleWithMnemonic:[NSString stringWithFormat:@"%d", theInfo.totalPatterns]];
	[musicPlugType setTitleWithMnemonic:[NSString stringWithCString:theInfo.formatDescription encoding:NSMacOSRomanStringEncoding]];
	{
		char sig[5] = {0};
		OSType2Ptr(theInfo.signature, sig);
		[musicSignature setTitleWithMnemonic:[NSString stringWithCString:sig encoding:NSMacOSRomanStringEncoding]];
	}
	[fileLocation setTitleWithMnemonic:[musicURL path]];
	return;
	
badMus:
	
	[fileName setTitleWithMnemonic:doubleDash];
	[internalName setTitleWithMnemonic:doubleDash];
	[fileSize setTitleWithMnemonic:doubleDash];
	[musicInstrument setTitleWithMnemonic:doubleDash];
	[musicPatterns setTitleWithMnemonic:doubleDash];
	[musicPlugType setTitleWithMnemonic:doubleDash];
	[musicSignature setTitleWithMnemonic:doubleDash];
	[fileLocation setTitleWithMnemonic:doubleDash];

	
}

- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename
{
	NSError *err = nil;
	NSWorkspace *sharedWorkspace = [NSWorkspace sharedWorkspace];
	NSString *utiFile = [sharedWorkspace typeOfFile:filename error:&err];
	if (err) {
		NSRunAlertPanel(@"Error opening file", [NSString stringWithFormat:@"Unable to open %@: %@", [filename lastPathComponent], [err localizedFailureReason]], nil, nil, nil);
		return NO;
	}
	
	if([sharedWorkspace type:utiFile conformsToType:@"net.sourceforge.playerpro.tracker"])
	{
		[self willChangeValueForKey:@"musicList"];
		[musicList addMusicURL:[NSURL fileURLWithPath:filename]];
		[self didChangeValueForKey:@"musicList"];
		return YES;
	} else if ([sharedWorkspace type:utiFile conformsToType:@"net.sourceforge.playerpro.musiclist"])
	{
		[self willChangeValueForKey:@"musicList"];
		[musicList loadMusicListAtURL:[NSURL fileURLWithPath:filename]];
		[self didChangeValueForKey:@"musicList"];
		return YES;
	} else if ([sharedWorkspace type:utiFile conformsToType:@"net.sourceforge.playerpro.stcfmusiclist"])
	{
		[self willChangeValueForKey:@"musicList"];
		[musicList loadOldMusicListAtURL:[NSURL fileURLWithPath:filename]];
		[self didChangeValueForKey:@"musicList"];
		return YES;
	}
	return NO;
}

@end
