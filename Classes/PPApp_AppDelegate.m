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
#import "PPDigitalPlugHandler.h"
#import "PPDigitalPlugInObject.h"
#import "PPFilterPlugHandler.h"
#import "PPFilterPlugObject.h"
#import "PPDocument.h"
#import "PPInstrumentViewController.h"
#import <PlayerPROKit/PlayerPROKit.h>
#include <PlayerPROCore/RDriverInt.h>

#define kUnresolvableFile @"Unresolvable files"
#define kUnresolvableFileDescription @"There were %lu file(s) that were unable to be resolved."

__weak PPLibrary *globalMadLib = nil;

#define PPPRIVINLINE static __inline__ __attribute__((always_inline))

PPPRIVINLINE NSColor *makeNSRGB(unsigned short red, unsigned short green, unsigned short blue)
{
	return [NSColor colorWithCalibratedRed:red / (CGFloat)USHRT_MAX green:green / (CGFloat)USHRT_MAX blue:blue / (CGFloat)USHRT_MAX alpha:1];
}

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
@property (nonatomic, strong) PPLibrary *madLib;
@end

@implementation PPApp_AppDelegate
@synthesize window;
@synthesize madLib;
- (void)setMadLib:(PPLibrary *)madLibb
{
	[self willChangeValueForKey:@"madLib"];
	madLib = madLibb;
	globalMadLib = madLib;
	[self didChangeValueForKey:@"madLib"];
}


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
	static dispatch_once_t oncePrefToken;
	dispatch_once(&oncePrefToken, ^{\
		[[NSUserDefaults standardUserDefaults]
		 registerDefaults:@{PPSoundOutBits: @16,
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
							PPDEMarkerColorPref: [makeNSRGB(65535, 65535, 39321) PPencodeColor],
							PPDEMouseClickControlPref: @YES,
							PPDEMouseClickShiftPref: @NO,
							PPDEMouseClickCommandPref: @NO,
							PPDEMouseClickOptionPref: @NO,
							PPDELineHeightNormal: @YES,
							PPDEMusicTraceOn: @YES,
							PPDEPatternWrappingPartition: @YES,
							PPDEDragAsPcmd: @YES,
							
							PPCColor1: [[NSColor colorWithCalibratedRed:(CGFloat)61166 / (CGFloat)(USHRT_MAX) green:0 blue:0 alpha:1] PPencodeColor],
							PPCColor2: [[NSColor colorWithCalibratedRed:(CGFloat)35980 / (CGFloat)(USHRT_MAX) green:(CGFloat)48316 / (CGFloat)(USHRT_MAX) blue:(CGFloat)7196 / (CGFloat)(USHRT_MAX) alpha:1] PPencodeColor],
							PPCColor3: [[NSColor colorWithCalibratedRed:(CGFloat)13107 / (CGFloat)(USHRT_MAX) green:1 blue:1 alpha:1] PPencodeColor],
							PPCColor4: [[NSColor colorWithCalibratedRed:1 green:1 blue:0 alpha:1] PPencodeColor],
							PPCColor5: [[NSColor colorWithCalibratedRed:(CGFloat)24672 / (CGFloat)(USHRT_MAX) green:(CGFloat)51914 / (CGFloat)(USHRT_MAX) blue:(CGFloat)36494 / (CGFloat)(USHRT_MAX) alpha:1] PPencodeColor],
							PPCColor6: [[NSColor colorWithCalibratedRed:(CGFloat)13107 / (CGFloat)(USHRT_MAX) green:1 blue:1 alpha:1] PPencodeColor],
							PPCColor7: [[NSColor colorWithCalibratedRed:(CGFloat)26214 / (CGFloat)(USHRT_MAX) green:(CGFloat)52428 / (CGFloat)(USHRT_MAX) blue:1 alpha:1] PPencodeColor],
							PPCColor8: [[NSColor colorWithCalibratedRed:1 green:(CGFloat)25428 / (CGFloat)(USHRT_MAX) blue:1 alpha:1] PPencodeColor],
							
							PPCColor9: [[NSColor colorWithCalibratedRed:(CGFloat)52428 / (CGFloat)(USHRT_MAX) green:(CGFloat)39321 / (CGFloat)(USHRT_MAX) blue:(CGFloat)26214 / (CGFloat)(USHRT_MAX) alpha:1] PPencodeColor],
							PPCColor10: [[NSColor colorWithCalibratedRed:(CGFloat)13107 / (CGFloat)(USHRT_MAX) green:(CGFloat)26214 / (CGFloat)(USHRT_MAX) blue:(CGFloat)13107 / (CGFloat)(USHRT_MAX) alpha:1] PPencodeColor],
							PPCColor11: [[NSColor colorWithCalibratedRed:1 green:(CGFloat)26214 / (CGFloat)(USHRT_MAX) blue:1 alpha:1] PPencodeColor],
							PPCColor12: [[NSColor colorWithCalibratedRed:1 green:(CGFloat)39321 / (CGFloat)(USHRT_MAX) blue:0 alpha:1] PPencodeColor],
							PPCColor13: [[NSColor colorWithCalibratedRed:(CGFloat)52428 / (CGFloat)(USHRT_MAX) green:1 blue:(CGFloat)52428 / (CGFloat)(USHRT_MAX) alpha:1] PPencodeColor],
							PPCColor14: [[NSColor colorWithCalibratedRed:(CGFloat)39321 / (CGFloat)(USHRT_MAX) green:0 blue:(CGFloat)26214 / (CGFloat)(USHRT_MAX) alpha:1] PPencodeColor],
							PPCColor15: [[NSColor colorWithCalibratedRed:(CGFloat)26214 / (CGFloat)(USHRT_MAX) green:(CGFloat)52428 / (CGFloat)(USHRT_MAX) blue:0 alpha:1] PPencodeColor],
							PPCColor16: [[NSColor colorWithCalibratedRed:1 green:(CGFloat)39321 / (CGFloat)(USHRT_MAX) blue:1 alpha:1] PPencodeColor],
							PPCColor17: [[NSColor colorWithCalibratedRed:(CGFloat)52428 / (CGFloat)(USHRT_MAX) green:1 blue:(CGFloat)26214 / (CGFloat)(USHRT_MAX) alpha:1] PPencodeColor],
							PPCColor18: [[NSColor colorWithCalibratedRed:(CGFloat)26214 / (CGFloat)(USHRT_MAX) green:0 blue:(CGFloat)39321 / (CGFloat)(USHRT_MAX) alpha:1] PPencodeColor],
							PPCColor19: [[NSColor colorWithCalibratedRed:(CGFloat)52428 / (CGFloat)(USHRT_MAX) green:0 blue:(CGFloat)39321 / (CGFloat)(USHRT_MAX) alpha:1] PPencodeColor],
							PPCColor20: [[NSColor colorWithCalibratedRed:(CGFloat)52428 / (CGFloat)(USHRT_MAX) green:(CGFloat)39321 / (CGFloat)(USHRT_MAX) blue:1 alpha:1] PPencodeColor],
							PPCColor21: [[NSColor colorWithCalibratedRed:(CGFloat)26214 / (CGFloat)(USHRT_MAX) green:(CGFloat)52428 / (CGFloat)(USHRT_MAX) blue:(CGFloat)26214 / (CGFloat)(USHRT_MAX) alpha:1] PPencodeColor],
							PPCColor22: [[NSColor colorWithCalibratedRed:(CGFloat)52428 / (CGFloat)(USHRT_MAX) green:(CGFloat)52428 / (CGFloat)(USHRT_MAX) blue:(CGFloat)13107 / (CGFloat)(USHRT_MAX) alpha:1] PPencodeColor],
							PPCColor23: [[NSColor colorWithCalibratedRed:(CGFloat)39321 / (CGFloat)(USHRT_MAX) green:(CGFloat)52428 / (CGFloat)(USHRT_MAX) blue:(CGFloat)52428 / (CGFloat)(USHRT_MAX) alpha:1] PPencodeColor],
							PPCColor24: [[NSColor colorWithCalibratedRed:(CGFloat)26214 / (CGFloat)(USHRT_MAX) green:(CGFloat)13109 / (CGFloat)(USHRT_MAX) blue:(CGFloat)26214 / (CGFloat)(USHRT_MAX) alpha:1] PPencodeColor],
							PPCColor25: [[NSColor colorWithCalibratedRed:(CGFloat)52428 / (CGFloat)(USHRT_MAX) green:(CGFloat)39321 / (CGFloat)(USHRT_MAX) blue:(CGFloat)52428 / (CGFloat)(USHRT_MAX) alpha:1] PPencodeColor],
							PPCColor26: [[NSColor colorWithCalibratedRed:(CGFloat)13107 / (CGFloat)(USHRT_MAX) green:1 blue:1 alpha:1] PPencodeColor],
							PPCColor27: [[NSColor colorWithCalibratedRed:(CGFloat)13107 / (CGFloat)(USHRT_MAX) green:1 blue:(CGFloat)26214 / (CGFloat)(USHRT_MAX) alpha:1] PPencodeColor],
							PPCColor28: [[NSColor colorWithCalibratedRed:(CGFloat)26214 / (CGFloat)(USHRT_MAX) green:1 blue:(CGFloat)13107 / (CGFloat)(USHRT_MAX) alpha:1] PPencodeColor],
							PPCColor29: [[NSColor colorWithCalibratedRed:(CGFloat)52428 / (CGFloat)(USHRT_MAX) green:(CGFloat)39321 / (CGFloat)(USHRT_MAX) blue:(CGFloat)52428 / (CGFloat)(USHRT_MAX) alpha:1] PPencodeColor],
							PPCColor30: [[NSColor colorWithCalibratedRed:(CGFloat)52428 / (CGFloat)(USHRT_MAX) green:(CGFloat)39321 / (CGFloat)(USHRT_MAX) blue:(CGFloat)13107 / (CGFloat)(USHRT_MAX) alpha:1] PPencodeColor],
							PPCColor31: [makeNSRGB(52428, 52428, 13107) PPencodeColor],
							PPCColor32: [makeNSRGB(65535, 52428, 0) PPencodeColor],
							PPCColor33: [makeNSRGB(61166, 0, 0) PPencodeColor],
							PPCColor34: [makeNSRGB(0, 65535, 26214) PPencodeColor],
							PPCColor35: [[NSColor cyanColor] PPencodeColor],
							PPCColor36: [[NSColor yellowColor] PPencodeColor],
							PPCColor37: [[NSColor greenColor] PPencodeColor],
							PPCColor38: [makeNSRGB(13107, 52428, 65535) PPencodeColor],
							PPCColor39: [makeNSRGB(39321, 52428, 39321) PPencodeColor],
							PPCColor40: [makeNSRGB(65535, 39321, 26214) PPencodeColor],
							PPCColor41: [makeNSRGB(39321, 65535, 65535) PPencodeColor],
							PPCColor42: [makeNSRGB(26214, 65535, 65535) PPencodeColor],
							PPCColor43: [makeNSRGB(65535, 39321, 65535) PPencodeColor],
							PPCColor44: [makeNSRGB(0, 21845, 0) PPencodeColor],
							PPCColor45: [makeNSRGB(52428, 65535, 52428) PPencodeColor],
							PPCColor46: [makeNSRGB(26214, 0, 0) PPencodeColor],
							PPCColor47: [makeNSRGB(13107, 65535, 26214) PPencodeColor],
							PPCColor48: [makeNSRGB(65535, 65535, 26214) PPencodeColor],
							PPCColor49: [makeNSRGB(52428, 0, 0) PPencodeColor],
							PPCColor50: [makeNSRGB(39321, 0, 13107) PPencodeColor],
							PPCColor51: [makeNSRGB(39321, 65535, 39321) PPencodeColor],
							PPCColor52: [makeNSRGB(39321, 52428, 52428) PPencodeColor],
							PPCColor53: [makeNSRGB(52428, 52428, 0) PPencodeColor],
							PPCColor54: [makeNSRGB(52428, 52428, 13107) PPencodeColor],
							PPCColor55: [makeNSRGB(65535, 39321, 65535) PPencodeColor],
							PPCColor56: [[NSColor redColor] PPencodeColor],
							PPCColor57: [makeNSRGB(65535, 0, 26214) PPencodeColor],
							PPCColor58: [makeNSRGB(26214, 65535, 65535) PPencodeColor],
							PPCColor59: [makeNSRGB(26214, 65535, 52428) PPencodeColor],
							PPCColor60: [makeNSRGB(39321, 52428, 26214) PPencodeColor],
							PPCColor61: [makeNSRGB(39321, 52428, 39321) PPencodeColor],
							PPCColor62: [makeNSRGB(39321, 52428, 26214) PPencodeColor],
							PPCColor63: [makeNSRGB(52428, 52428, 26214) PPencodeColor],
							PPCColor64: [makeNSRGB(52428, 52428, 39321) PPencodeColor],
							PPCColor65: [makeNSRGB(61166, 0, 0) PPencodeColor],
							PPCColor66: [makeNSRGB(0, 65535, 26214) PPencodeColor],
							PPCColor67: [makeNSRGB(0, 65535, 65535) PPencodeColor],
							PPCColor68: [makeNSRGB(65535, 65535, 0) PPencodeColor],
							PPCColor69: [[NSColor greenColor] PPencodeColor],
							PPCColor70: [makeNSRGB(13107, 52428, 65535) PPencodeColor],
							PPCColor71: [makeNSRGB(39321, 52428, 39321) PPencodeColor],
							PPCColor72: [makeNSRGB(65535, 39321, 26214) PPencodeColor],
							PPCColor73: [makeNSRGB(39321, 65535, 65535) PPencodeColor],
							PPCColor74: [makeNSRGB(26214, 65535, 65535) PPencodeColor],
							PPCColor75: [makeNSRGB(65535, 39321, 65535) PPencodeColor],
							PPCColor76: [makeNSRGB(0, 21845, 0) PPencodeColor],
							PPCColor77: [makeNSRGB(52428, 65535, 52428) PPencodeColor],
							PPCColor78: [makeNSRGB(26214, 0, 0) PPencodeColor],
							PPCColor79: [makeNSRGB(13107, 65535, 26214) PPencodeColor],
							PPCColor80: [makeNSRGB(65535, 65535, 26214) PPencodeColor],
							PPCColor81: [makeNSRGB(52428, 0, 0) PPencodeColor],
							PPCColor82: [makeNSRGB(39321, 0, 13107) PPencodeColor],
							PPCColor83: [makeNSRGB(39321, 65535, 39321) PPencodeColor],
							PPCColor84: [makeNSRGB(39321, 52428, 52428) PPencodeColor],
							PPCColor85: [makeNSRGB(52428, 52428, 0) PPencodeColor],
							PPCColor86: [makeNSRGB(52428, 52428, 13107) PPencodeColor],
							PPCColor87: [makeNSRGB(65535, 39321, 65535) PPencodeColor],
							PPCColor88: [[NSColor redColor] PPencodeColor],
							PPCColor89: [makeNSRGB(39321, 0, 26214) PPencodeColor],
							PPCColor90: [makeNSRGB(26214, 65535, 65535) PPencodeColor],
							PPCColor91: [makeNSRGB(26214, 65535, 52428) PPencodeColor],
							PPCColor92: [makeNSRGB(39321, 52428, 26214) PPencodeColor],
							PPCColor93: [makeNSRGB(39321, 52428, 39321) PPencodeColor],
							PPCColor94: [makeNSRGB(39321, 52428, 26214) PPencodeColor],
							PPCColor95: [makeNSRGB(52428, 52428, 26214) PPencodeColor],
							PPCColor96: [makeNSRGB(52428, 52428, 39321) PPencodeColor],
							
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
	});
}

- (IBAction)showPreferences:(id)sender
{
    if (!preferences) {
		preferences = [[PPPreferences alloc] init];
	}
	[[preferences window] center];
	[preferences showWindow:sender];
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
	
	for (PPDigitalPlugInObject *obj in digitalHandler) {
		PPPlugInInfo *tmpInfo = [[PPPlugInInfo alloc] initWithPlugName:obj.menuName author:obj.authorString plugType:NSLocalizedString(@"DigitalPlugName", @"Digital plug-in name") plugURL:[[obj file] bundleURL]];
		if (![plugInInfos containsObject:tmpInfo]) {
			[plugInInfos addObject:tmpInfo];
		}
	}
	
	for (PPFilterPlugObject *obj in filterHandler) {
		PPPlugInInfo *tmpInfo = [[PPPlugInInfo alloc] initWithPlugName:obj.menuName author:obj.authorString plugType:NSLocalizedString(@"FilterPlugName", @"Filter plug-in name") plugURL:[[obj file] bundleURL]];
		if (![plugInInfos containsObject:tmpInfo]) {
			[plugInInfos addObject:tmpInfo];
		}
	}
	
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

- (void)addExportObject:(PPExportObject *)expObj
{
	
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	isQuitting = NO;
	srandom(time(NULL) & 0xffffffff);
	PPRegisterDebugFunc(CocoaDebugStr);
	self.madLib = [[PPLibrary alloc] init];

	//[self addObserver:self forKeyPath:@"paused" options:NSKeyValueObservingOptionNew context:NULL];
	//self.paused = YES;
	
	filterHandler = [[PPFilterPlugHandler alloc] init];
	digitalHandler = [[PPDigitalPlugHandler alloc] init];
	instrumentImporter = [[PPInstrumentPlugHandler alloc] init];
	NSInteger i;
	for (i = 0; i < [instrumentImporter plugInCount]; i++) {
		PPInstrumentImporterObject *obj = [instrumentImporter plugInAtIndex:i];
		if (obj.mode == MADPlugImportExport || obj.mode == MADPlugExport) {
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:obj.menuName action:@selector(exportInstrument:) keyEquivalent:@""];
			[mi setTag:i];
			[mi setTarget:nil];
			[instrumentExportMenu addItem:mi];
		}
	}
	
	for (i = 0; i < [madLib pluginCount]; i++) {
		PPLibraryObject *obj = [madLib pluginAtIndex:i];
		if (obj.canExport) {
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:[NSString stringWithFormat:@"%@...", obj.menuName] action:@selector(exportMusicAs:) keyEquivalent:@""];
			[mi setTag:i];
			[mi setTarget:nil];
			[musicExportMenu addItem:mi];
		}
	}
	
	plugInInfos = [[NSMutableArray alloc] init];
	[self updatePlugInInfoMenu];
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
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
					PPMusicObjectWrapper *theWrap = [[PPMusicObjectWrapper alloc] initWithURL:theURL library:madLib];
					PPDocument *theDoc = [[PPDocument alloc] init];
					[theDoc importMusicObjectWrapper:theWrap];
					
					[self addDocument:theDoc];
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

- (id)openUntitledDocumentAndDisplay:(BOOL)displayDocument error:(NSError **)outError;
{
	PPDocument *theDoc = [[PPDocument alloc] init];
	[theDoc importMusicObjectWrapper:[[PPMusicObjectWrapper alloc] init]];
	
	return theDoc;
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
