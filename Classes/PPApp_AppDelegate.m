//
//  PPApp_AppDelegate.m
//  PPMacho
//
//  Created by C.W. Betts on 7/12/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "PPApp_AppDelegate.h"
#import "PPPreferences.h"
#import "UserDefaultKeys.h"
#import "NSColor+PPPreferences.h"
#import "PPErrors.h"
#import "OpenPanelViewController.h"
#import "PPInstrumentPlugHandler.h"
#import "PPInstrumentImporterObject.h"
#import "PPPlugInInfo.h"
#import "PPPlugInInfoController.h"
#import "PPDigitalPlugInHandler.h"
#import "PPDigitalPlugInObject.h"
#import "PPFilterPlugHandler.h"
#import "PPFilterPlugObject.h"
#import <PlayerPROKit/PlayerPROKit.h>
#include <PlayerPROCore/RDriverInt.h>

#define kUnresolvableFile @"Unresolvable files"
#define kUnresolvableFileDescription @"There were %lu file(s) that were unable to be resolved."

static void CocoaDebugStr( short line, Ptr file, Ptr text)
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
@property (readonly, strong) NSDictionary *trackerDict;
@property (readwrite, strong) PPLibrary *madLib;
@end

@implementation PPApp_AppDelegate
@synthesize window;
@synthesize madLib;

@synthesize trackerDict = _trackerDict;
- (NSDictionary *)trackerDict
{
	if (!_trackerDict) {
		NSMutableDictionary *trackerDict = [NSMutableDictionary dictionaryWithDictionary:@{
											NSLocalizedStringWithDefaultValue(@"PPMADKFile", @"InfoPlist",
																			  [NSBundle mainBundle],
																			  @"MADK Tracker", @"MADK Tracker") : @[MADNativeUTI],
											NSLocalizedString(@"Generic MAD tracker", @"Generic MAD tracker"): @[MADGenericUTI] /*,
											NSLocalizedString(@"MAD Package", @"MAD Package"):@[MADPackageUTI]*/}];
		for (PPLibraryObject *obj in madLib) {
			trackerDict[obj.menuName] = obj.UTItypes;
		}
		
		_trackerDict = [[NSDictionary alloc] initWithDictionary:trackerDict];
	}
	
	return _trackerDict;
}

+ (void)initialize
{
	[[NSUserDefaults standardUserDefaults] registerDefaults:@{
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
	 
										 PPDEShowInstrument: @YES,
											   PPDEShowNote: @YES,
											 PPDEShowEffect: @YES,
										   PPDEShowArgument: @YES,
											 PPDEShowVolume: @YES,
											PPDEShowMarkers: @YES,
									   PPDEMarkerOffsetPref: @0,
										 PPDEMarkerLoopPref: @3,
										PPDEMarkerColorPref: [[NSColor yellowColor] PPencodeColor],
								  PPDEMouseClickControlPref: @YES,
									PPDEMouseClickShiftPref: @NO,
								  PPDEMouseClickCommandPref: @NO,
								   PPDEMouseClickOptionPref: @NO,
									   PPDELineHeightNormal: @YES,
										   PPDEMusicTraceOn: @YES,
							   PPDEPatternWrappingPartition: @YES,
											 PPDEDragAsPcmd: @YES,
	 
												  PPCColor1: [[NSColor redColor] PPencodeColor],
												  PPCColor2: [[NSColor greenColor] PPencodeColor],
												  PPCColor3: [[NSColor blueColor] PPencodeColor],
												  PPCColor4: [[NSColor cyanColor] PPencodeColor],
												  PPCColor5: [[NSColor yellowColor] PPencodeColor],
												  PPCColor6: [[NSColor magentaColor] PPencodeColor],
												  PPCColor7: [[NSColor orangeColor] PPencodeColor],
												  PPCColor8: [[NSColor purpleColor] PPencodeColor],
	 
										 PPBEMarkersEnabled: @YES,
										  PPBEMarkersOffset: @0,
											PPBEMarkersLoop: @3,
										  PPBEOctaveMarkers: @YES,
										PPBENotesProjection: @NO,
	 
											PPMAddExtension: @YES,
										  PPMMadCompression: @YES,
									PPMNoLoadMixerFromFiles: @NO,
								   PPMOscilloscopeDrawLines: @YES,
	 
										   PPCEShowNotesLen: @NO,
											PPCEShowMarkers: @YES,
										   PPCEMarkerOffset: @0,
											 PPCEMarkerLoop: @3,
											   PPCETempoNum: @4,
											  PPCETempoUnit: @4,
											PPCETrackHeight: @130}];
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
    //[instrumentController exportInstrument:sender];
}

- (IBAction)showInstrumentsList:(id)sender
{
    //[instrumentController showWindow:sender];
}

- (IBAction)showPlugInInfo:(id)sender
{
	PPPlugInInfo *inf = plugInInfos[[sender tag]];
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
	
	for (PPLibraryObject *obj in madLib) {
		PPPlugInInfo *tmpInfo = [[PPPlugInInfo alloc] initWithPlugName:obj.menuName author:obj.authorString plugType:NSLocalizedString(@"TrackerPlugName", @"Tracker plug-in name") plugURL:[obj.plugFile bundleURL]];
		if (![plugInInfos containsObject:tmpInfo]) {
			[plugInInfos addObject:tmpInfo];
		}
	}
	
	for (PPInstrumentImporterObject *obj in instrumentImporter) {
		PPPlugInInfo *tmpInfo = [[PPPlugInInfo alloc] initWithPlugName:obj.menuName author:obj.authorString plugType:NSLocalizedString(@"InstrumentPlugName", @"Instrument plug-in name") plugURL:[[obj file] bundleURL]];
		if (![plugInInfos containsObject:tmpInfo]) {
			[plugInInfos addObject:tmpInfo];
		}
	}
	
#if 0
	for (PPDigitalPlugInObject *obj in digitalHandler) {
		PPPlugInInfo *tmpInfo = [[PPPlugInInfo alloc] initWithPlugName:obj.menuName author:obj.authorString plugType:NSLocalizedString(@"DigitalPlugName", @"Digital plug-in name") plugURL:[[obj file] bundleURL]];
		if (![plugInInfos containsObject:tmpInfo]) {
			[plugInInfos addObject:tmpInfo];
		}
		RELEASEOBJ(tmpInfo);
	}
	
	for (PPFilterPlugObject *obj in filterHandler) {
		PPPlugInInfo *tmpInfo = [[PPPlugInInfo alloc] initWithPlugName:obj.menuName author:obj.authorString plugType:NSLocalizedString(@"FilterPlugName", @"Filter plug-in name") plugURL:[[obj file] bundleURL]];
		if (![plugInInfos containsObject:tmpInfo]) {
			[plugInInfos addObject:tmpInfo];
		}
		RELEASEOBJ(tmpInfo);
	}
#endif
	
	[plugInInfos sortWithOptions:NSSortConcurrent usingComparator:^NSComparisonResult(id obj1, id obj2) {
		NSString *menuNam1 = [obj1 plugName];
		NSString *menuNam2 = [obj2 plugName];
		NSComparisonResult res = [menuNam1 localizedStandardCompare:menuNam2];
		return res;
	}];
	
	[aboutPlugInMenu removeAllItems];
	
	for (i = 0; i < [plugInInfos count]; i++) {
		PPPlugInInfo *pi = plugInInfos[i];
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
	self.madLib = [[PPLibrary alloc] init];

	[self addObserver:self forKeyPath:@"paused" options:NSKeyValueObservingOptionNew context:NULL];
	//self.paused = YES;
	
	NSNotificationCenter *defaultCenter = [NSNotificationCenter defaultCenter];
	[defaultCenter addObserver:self selector:@selector(soundPreferencesDidChange:) name:PPSoundPreferencesDidChange object:nil];
	instrumentImporter = [[PPInstrumentPlugHandler alloc] init];
	NSInteger i;
	for (i = 0; i < [instrumentImporter plugInCount]; i++) {
		PPInstrumentImporterObject *obj = [instrumentImporter plugInAtIndex:i];
		if (obj.mode == MADPlugImportExport || obj.mode == MADPlugExport) {
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:obj.menuName action:@selector(exportInstrument:) keyEquivalent:@""];
			[mi setTag:i];
			[mi setTarget:self];
			[instrumentExportMenu addItem:mi];
		}
	}
	
	for (i = 0; i < [madLib pluginCount]; i++) {
		PPLibraryObject *obj = [madLib pluginAtIndex:i];
		if (obj.canExport) {
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:[NSString stringWithFormat:@"%@...", obj.menuName] action:@selector(exportMusicAs:) keyEquivalent:@""];
			[mi setTag:i];
			[mi setTarget:self];
			[musicExportMenu addItem:mi];
		}
	}
	
	plugInInfos = [[NSMutableArray alloc] init];
	[self updatePlugInInfoMenu];
}

#if 0
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

}

- (IBAction)saveInstrumentList:(id)sender
{
	MADBeginExport(madDriver);
	NSSavePanel *savePanel = RETAINOBJ([NSSavePanel savePanel]);
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
			RELEASEOBJ(theErr);
		}
	}
	
	MADEndExport(madDriver);
	RELEASEOBJ(savePanel);
}
#endif

- (IBAction)showBoxEditor:(id)sender
{
    
}

- (IBAction)showClassicEditor:(id)sender
{
    
}

- (IBAction)showDigitalEditor:(id)sender
{
    
}

- (BOOL)handleFile:(NSURL *)theURL ofType:(NSString *)theUTI
{
	NSWorkspace *sharedWorkspace = [NSWorkspace sharedWorkspace];
	if ([sharedWorkspace type:theUTI conformsToType:MADPackageUTI]) {
		// Document controller should automatically handle this.
		return YES;
	} else if ([theUTI isEqualToString:MADGenericUTI]) {
		NSInteger retVal = NSRunInformationalAlertPanel(NSLocalizedString(@"Invalid Extension", @"Invalid extension"), NSLocalizedString(@"The file %@ is identified as as a generic MAD tracker, and not a specific one. Renaming it will fix this. Do you want to rename the file extension?", @"Invalid extension description"), NSLocalizedString(@"Rename", @"rename file"), NSLocalizedString(@"Open", @"Open a file"), NSLocalizedString(@"Cancel", @"Cancel"), [theURL lastPathComponent]);
		switch (retVal) {
			case NSAlertDefaultReturn:
			{
				PPInfoRec rec;
				char ostype[5] = {0};
				//theURL = [theURL fileReferenceURL];
				if ([madLib identifyFileAtURL:theURL type:ostype] != noErr || [madLib getInformationFromFileAtURL:theURL type:ostype info:&rec] != noErr) {
					NSRunCriticalAlertPanel(NSLocalizedString(@"Unknown File", @"unknown file"), NSLocalizedString(@"The file type could not be identified.", @"Unidentified file"), nil, nil, nil);
					return NO;
				}
				OSType2Ptr(rec.signature, ostype);

				NSURL *tmpURL = [theURL URLByDeletingPathExtension];
				tmpURL = [tmpURL URLByAppendingPathExtension:[[NSString stringWithCString:ostype encoding:NSMacOSRomanStringEncoding] lowercaseString]];
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
#if 0
	if ([sharedWorkspace type:theUTI conformsToType:PPPCMDUTI]) {
		OSErr theOSErr = [patternHandler importPcmdFromURL:theURL];
		if (theOSErr != noErr) {
			NSError *theErr = CreateErrorFromMADErrorType(theOSErr);
			[[NSAlert alertWithError:theErr] runModal];
			RELEASEOBJ(theErr);
			return NO;
		}
		return YES;
	} else if([sharedWorkspace type:theUTI conformsToType:PPInstrumentListUTI]) {
		NSError *err = nil;
		if (![instrumentController importInstrumentListFromURL:theURL error:&err]) {
			[[NSAlert alertWithError:err] runModal];
		} else return YES;
	} else
#endif
	{
		{
			//TODO: check for valid extension.
			NSDictionary *tracDict = self.trackerDict;
			NSMutableArray *trackerUTIs = [NSMutableArray array];
			for (NSString *key in tracDict) {
				[trackerUTIs addObjectsFromArray:tracDict[key]];
			}
			for (NSString *aUTI in trackerUTIs) {
				if([sharedWorkspace type:theUTI conformsToType:aUTI])
				{
					return YES;
				}
			}
		}
		{
			NSMutableArray *instrumentArray = [NSMutableArray array];
			for (PPInstrumentImporterObject *obj in instrumentImporter) {
				[instrumentArray addObjectsFromArray:obj.UTITypes];
			}
			
			for (NSString *aUTI in instrumentArray) {
				if ([sharedWorkspace type:theUTI conformsToType:aUTI]) {
#if 0
					if ([instrumentController isWindowLoaded]) {
						NSError *theErr = nil;
						if (![instrumentController importSampleFromURL:theURL makeUserSelectInstrument:YES error:&theErr])
						{
							[[NSAlert alertWithError:theErr] runModal];
							return NO;
						}
						return YES;
					} else return NO;
#endif
				}
			}
		}
	}
	return NO;
}

- (IBAction)openFile:(id)sender {
	
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	NSDictionary *trackerDict = self.trackerDict;
	
	NSMutableDictionary *samplesDict = nil;
	NSInteger plugCount = [instrumentImporter plugInCount];
	samplesDict = [[NSMutableDictionary alloc] initWithCapacity:plugCount];
	for (PPInstrumentImporterObject *obj in instrumentImporter) {
		NSArray *tmpArray = obj.UTITypes;
		samplesDict[obj.menuName] = tmpArray;
	}
	
	NSDictionary *otherDict = @{@"PCMD": @[PPPCMDUTI], @"Instrument List": @[PPInstrumentListUTI]};
	
	OpenPanelViewController *av = [[OpenPanelViewController alloc] initWithOpenPanel:panel trackerDictionary:trackerDict playlistDictionary:nil instrumentDictionary:samplesDict additionalDictionary:otherDict];
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

@end
