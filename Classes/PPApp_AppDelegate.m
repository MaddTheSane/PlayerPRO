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
#include <PlayerPROCore/RDriverInt.h>

static NSString * const kMusicListKVO = @"musicList";

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
- (BOOL)loadMusicURL:(NSURL*)musicToLoad error:(NSError *__autoreleasing*)theErr;

@end

@implementation PPApp_AppDelegate

@synthesize paused;

- (BOOL)loadMusicFromCurrentlyPlayingIndexWithError:(NSError *__autoreleasing*)theErr
{
	return [self loadMusicURL:[musicList URLAtIndex:currentlyPlayingIndex] error:theErr];
}

- (void)addMusicToMusicList:(NSURL* )theURL
{
	[self willChangeValueForKey:kMusicListKVO];
	[musicList addMusicURL:theURL];
	[self didChangeValueForKey:kMusicListKVO];
	if ([[NSUserDefaults standardUserDefaults] boolForKey:PPLoadMusicAtMusicLoad]) {
		currentlyPlayingIndex = [musicList countOfMusicList] - 1;
		[self selectCurrentlyPlayingMusic];
		NSError *err;
		if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err]) {
			NSAlert *errAlert = [NSAlert alertWithError:err];
			[errAlert runModal];
		}
	}
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
															 [NSNumber numberWithInt:25], PPReverbAmount,
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
	RELEASEOBJ(tempList);
}

- (IBAction)showMusicList:(id)sender
{
    [window makeKeyAndOrderFront:sender];
}

- (void)selectCurrentlyPlayingMusic
{
	NSIndexSet *idx = [[NSIndexSet alloc] initWithIndex:currentlyPlayingIndex];
	[tableView selectRowIndexes:idx byExtendingSelection:NO];
	[tableView scrollRowToVisible:currentlyPlayingIndex];
	RELEASEOBJ(idx);
}

- (void)songIsDonePlaying
{
	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	switch ([userDefaults integerForKey:PPAfterPlayingMusic]) {
		case PPStopPlaying:
		default:
			MADStopMusic(MADDriver);
			MADCleanDriver(MADDriver);
			if ([userDefaults boolForKey:PPLoopMusicWhenDone]) {
				MADSetMusicStatus(MADDriver, 0, 100, 0);
			}
			break;
			
		case PPLoopMusic:
			MADSetMusicStatus(MADDriver, 0, 100, 0);
			break;
			
		case PPLoadNext:
		{
			if ([musicList countOfMusicList] > ++currentlyPlayingIndex) {
				[self selectCurrentlyPlayingMusic];
				NSError *err = nil;
				if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err])
				{
					NSAlert *alert = [NSAlert alertWithError:err];
					[alert runModal];
				}
			} else {
				if ([userDefaults boolForKey:PPLoopMusicWhenDone]) {
					currentlyPlayingIndex = 0;
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
					if ([userDefaults boolForKey:PPLoopMusicWhenDone]) {
						MADSetMusicStatus(MADDriver, 0, 100, 0);
					}
				}
			}
		}
			break;
			
		case PPLoadRandom:
		{
			currentlyPlayingIndex = random() % [musicList countOfMusicList];
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
		if (fT == cT) {
			[self songIsDonePlaying];
			MADGetMusicStatus(MADDriver, &fT, &cT);
		}
		[songPos setDoubleValue:cT];
		[songCurTime setIntegerValue:cT];
	}
}

- (BOOL)loadMusicURL:(NSURL*)musicToLoad error:(NSError *__autoreleasing*)theErr
{
	if (Music != NULL) {
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
		[songLabel setTitleWithMnemonic:[NSString stringWithCString:Music->header->name encoding:NSMacOSRomanStringEncoding]];
		[songTotalTime setIntegerValue:fT];
	}

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
				break;
				
			case YES:
				[pauseButton setState:NSOnState];
				break;
		}
	}
	
	//[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
}

@synthesize window;

- (void)doubleClickMusicList
{
	NSError *err = nil;
	currentlyPlayingIndex = [tableView selectedRow];
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
	[defaultCenter addObserver:self selector:@selector(digitalEditorPreferencesDidChange:) name:PPDigitalEditorPrefrencesDidChange object:nil];
	
	[self MADDriverWithPreferences];
	Music = CreateFreeMADK();
	MADAttachDriverToMusic(MADDriver, Music, NULL);
	instrumentImporter = [[PPInstrumentImporter alloc] initWithMusic:&Music];
	instrumentController = [[PPInstrumentWindowController alloc] init];
	instrumentController.importer = instrumentImporter;
	instrumentController.curMusic = &Music;
	
	NSInteger i;
	for (i = 0; i < [instrumentImporter plugInCount]; i++) {
		PPInstrumentImporterObject *obj = [instrumentImporter plugInAtIndex:i];
		if ([obj mode] == MADPlugImportExport || [obj mode] == MADPlugExport) {
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:obj.menuName action:@selector(exportInstrument:) keyEquivalent:@""];
			[mi setTag:i];
			[mi setTarget:instrumentController];
			[instrumentExportMenu addItem:mi];
			RELEASEOBJ(mi);
		}
	}
	
	{
		NSMutableArray *tmpArray = [NSMutableArray array];
		for (i = 0; i < MADLib->TotalPlug ; i++) {
			//PlugInfo *tmpPlug = &MADLib->ThePlug[i];
			PPPlugInInfo *tmpInfo = [[PPPlugInInfo alloc] initWithPlugName:BRIDGE(NSString*,MADLib->ThePlug[i].MenuName) author:BRIDGE(NSString*,MADLib->ThePlug[i].AuthorString) plugType:NSLocalizedString(@"Tracker", @"Tracker plug-in name")];
			[tmpArray addObject:tmpInfo];
			RELEASEOBJ(tmpInfo);
		}
		
		for (i = 0; i < [instrumentImporter plugInCount]; i++) {
			PPInstrumentImporterObject *obj = [instrumentImporter plugInAtIndex:i];
			PPPlugInInfo *tmpInfo = [[PPPlugInInfo alloc] initWithPlugName:obj.menuName author:obj.authorString plugType:NSLocalizedString(@"Instrument", @"Instrument plug-in name")];
			[tmpArray addObject:tmpInfo];
			RELEASEOBJ(tmpInfo);
		}

		[tmpArray sortUsingComparator:^NSComparisonResult(id obj1, id obj2) {
			NSString *menuNam1 = [obj1 plugName];
			NSString *menuNam2 = [obj2 plugName];
			NSComparisonResult res = [menuNam1 localizedStandardCompare:menuNam2];
			return res;
		}];
		
		plugInInfos = [[NSArray alloc] initWithArray:tmpArray];
		for (i = 0; i < [plugInInfos count]; i++) {
			PPPlugInInfo *pi = [plugInInfos objectAtIndex:i];
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:pi.plugName action:@selector(showPlugInInfo:) keyEquivalent:@""];
			[mi setTag:i];
			[mi setTarget:self];
			[aboutPlugInMenu addItem:mi];
			RELEASEOBJ(mi);
		}
	}
	
	timeChecker = [[NSTimer alloc] initWithFireDate:[NSDate dateWithTimeIntervalSinceNow:0] interval:1/4.0 target:self selector:@selector(updateMusicStats:) userInfo:nil repeats:YES];
	[[NSRunLoop mainRunLoop] addTimer:timeChecker forMode:NSDefaultRunLoopMode];
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
	[timeChecker invalidate];

	[self removeObserver:self forKeyPath:@"paused"];
	
	RELEASEOBJ(instrumentImporter);
	instrumentImporter = nil;
	
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
	
	[super dealloc];
}
#endif

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

- (IBAction)sortMusicList:(id)sender
{
	[self willChangeValueForKey:kMusicListKVO];
	[musicList sortMusicList];
	[self didChangeValueForKey:kMusicListKVO];
}

- (IBAction)playSelectedMusic:(id)sender
{
	NSError *error = nil;
	currentlyPlayingIndex = [tableView selectedRow];
	if ([self loadMusicFromCurrentlyPlayingIndexWithError:&error] == NO)
	{
		NSAlert *alert = [NSAlert alertWithError:error];
		[alert runModal];
	}
}

- (IBAction)addMusic:(id)sender
{
	NSOpenPanel *panel = RETAINOBJ([NSOpenPanel openPanel]);
	NSMutableArray *supportedUTIs = [NSMutableArray arrayWithObject:@"com.quadmation.playerpro.madk"];
	int i = 0;
	@autoreleasepool {
		for (i = 0; i < MADLib->TotalPlug; i++) {
			NSArray *tempArray = [NSArray arrayWithArray:BRIDGE(NSArray*, MADLib->ThePlug[i].UTItypes)];
			[supportedUTIs addObjectsFromArray:tempArray];
		}
	}
	[panel setAllowsMultipleSelection:NO];
	[panel setAllowedFileTypes:supportedUTIs];
	if([panel runModal] == NSFileHandlingPanelOKButton)
	{
		[self addMusicToMusicList:[panel URL]];
	}
	
	RELEASEOBJ(panel);
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
	[self willChangeValueForKey:kMusicListKVO];
	[musicList removeObjectInMusicListAtIndex:[tableView selectedRow]];
	[self didChangeValueForKey:kMusicListKVO];
#endif
}

- (IBAction)clearMusicList:(id)sender
{
	[self willChangeValueForKey:kMusicListKVO];
	[musicList clearMusicList];
	[self didChangeValueForKey:kMusicListKVO];
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

- (IBAction)openFile:(id)sender {
	NSOpenPanel *panel = RETAINOBJ([NSOpenPanel openPanel]);
	NSMutableArray *supportedUTIs = [NSMutableArray arrayWithObjects:@"com.quadmation.playerpro.madk", @"net.sourceforge.playerpro.musiclist", @"net.sourceforge.playerpro.stcfmusiclist", nil];
	int i = 0;
	NSMutableDictionary *trackerDict = [NSMutableDictionary dictionaryWithObject:[NSArray arrayWithObject:@"com.quadmation.playerpro.madk"] forKey:@"MADK Tracker"];
	NSDictionary *playlistDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSArray arrayWithObject:@"net.sourceforge.playerpro.musiclist"], @"PlayerPRO Music List", [NSArray arrayWithObject:@"net.sourceforge.playerpro.stcfmusiclist"], @"PlayerPRO Old Music List", nil];
	@autoreleasepool {
		for (i = 0; i < MADLib->TotalPlug; i++) {
			NSArray *tempArray = [NSArray arrayWithArray:BRIDGE(NSArray*, MADLib->ThePlug[i].UTItypes)];
			[supportedUTIs addObjectsFromArray:tempArray];
			NSString *menuName = [NSString stringWithString:BRIDGE(NSString*, MADLib->ThePlug[i].MenuName)];
			[trackerDict setObject:tempArray forKey:menuName];
		}
	}
	
	[panel setAllowsMultipleSelection:NO];
	[panel setAllowedFileTypes:supportedUTIs];
	OpenPanelViewController *av = [[OpenPanelViewController alloc] initWithOpenPanel:panel trackerDictionary:trackerDict playlistDictionary:playlistDict];
	[panel setAccessoryView:[av view]];
	if([panel runModal] == NSFileHandlingPanelOKButton)
	{
		//[self addMusicToMusicList:[panel URL]];
		NSURL *panelURL = [panel URL];
		NSString *filename = [panelURL path];
		NSError *err = nil;
		NSWorkspace *sharedWorkspace = [NSWorkspace sharedWorkspace];
		NSString *utiFile = [sharedWorkspace typeOfFile:filename error:&err];
		if (err) {
			NSRunAlertPanel(@"Error opening file", [NSString stringWithFormat:@"Unable to open %@: %@", [filename lastPathComponent], [err localizedFailureReason]], nil, nil, nil);
			RELEASEOBJ(panel);
			RELEASEOBJ(av);
			return;
		}
		if ([sharedWorkspace type:utiFile conformsToType:@"net.sourceforge.playerpro.tracker"]) {
			[self addMusicToMusicList:panelURL];
		}else if ([sharedWorkspace type:utiFile conformsToType:@"net.sourceforge.playerpro.musiclist"]) {
			[self willChangeValueForKey:kMusicListKVO];
			[musicList loadMusicListAtURL:panelURL];
			[self didChangeValueForKey:kMusicListKVO];
			if ([[NSUserDefaults standardUserDefaults] boolForKey:PPLoadMusicAtListLoad] && [musicList countOfMusicList] > 0) {
				NSError *err = nil;
				currentlyPlayingIndex = 0;
				if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err])
				{
					NSAlert *alert = [NSAlert alertWithError:err];
					[alert runModal];
				}
			}
		} else if ([sharedWorkspace type:utiFile conformsToType:@"net.sourceforge.playerpro.stcfmusiclist"]) {
			[self willChangeValueForKey:kMusicListKVO];
			[musicList loadOldMusicListAtURL:panelURL];
			[self didChangeValueForKey:kMusicListKVO];
			if ([[NSUserDefaults standardUserDefaults] boolForKey:PPLoadMusicAtListLoad] && [musicList countOfMusicList] > 0) {
				NSError *err = nil;
				currentlyPlayingIndex = 0;
				if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err])
				{
					NSAlert *alert = [NSAlert alertWithError:err];
					[alert runModal];
				}
			}

		}
	}
	RELEASEOBJ(panel);
	RELEASEOBJ(av);
}

- (IBAction)saveMusicList:(id)sender {
	NSSavePanel *savePanel = RETAINOBJ([NSSavePanel savePanel]);
	[savePanel setAllowedFileTypes:[NSArray arrayWithObject:@"net.sourceforge.playerpro.musiclist"]];
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

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
	static NSString * const doubleDash = @"--";

	NSInteger selected = [tableView selectedRow];
	do {
		if (selected < 0)
			break;
		
		NSURL *musicURL = [musicList URLAtIndex:selected];
		PPInfoRec theInfo;
		char info[5] = {0};
		if(MADMusicIdentifyCFURL(MADLib, info, BRIDGE(CFURLRef, musicURL)) != noErr) break;
		if(MADMusicInfoCFURL(MADLib, info, BRIDGE(CFURLRef, musicURL), &theInfo) != noErr) break;
		[fileName setTitleWithMnemonic:[musicURL lastPathComponent]];
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
		[self addMusicToMusicList:[NSURL fileURLWithPath:filename]];
		return YES;
	}
	else if ([sharedWorkspace type:utiFile conformsToType:@"net.sourceforge.playerpro.musiclist"])
	{
		[self willChangeValueForKey:kMusicListKVO];
		[musicList loadMusicListAtURL:[NSURL fileURLWithPath:filename]];
		[self didChangeValueForKey:kMusicListKVO];
		if ([[NSUserDefaults standardUserDefaults] boolForKey:PPLoadMusicAtListLoad] && [musicList countOfMusicList] > 0) {
			NSError *err = nil;
			currentlyPlayingIndex = 0;
			if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err])
			{
				NSAlert *alert = [NSAlert alertWithError:err];
				[alert runModal];
			}
		}
		return YES;
	}
	else if ([sharedWorkspace type:utiFile conformsToType:@"net.sourceforge.playerpro.stcfmusiclist"])
	{
		[self willChangeValueForKey:kMusicListKVO];
		[musicList loadOldMusicListAtURL:[NSURL fileURLWithPath:filename]];
		[self didChangeValueForKey:kMusicListKVO];
		if ([[NSUserDefaults standardUserDefaults] boolForKey:PPLoadMusicAtListLoad] && [musicList countOfMusicList] > 0) {
			NSError *err = nil;
			currentlyPlayingIndex = 0;
			if (![self loadMusicFromCurrentlyPlayingIndexWithError:&err])
			{
				NSAlert *alert = [NSAlert alertWithError:err];
				[alert runModal];
			}
		}
		return YES;
	}
	return NO;
}

@end
