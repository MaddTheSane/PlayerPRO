//
//  PreferenceHandler.m
//  PPMacho
//
//  Created by C.W. Betts on 1/19/14.
//
//

#import <Cocoa/Cocoa.h>
#import "NSColor+PPPreferences.h"
#include "UserDefaultKeys.h"
#include "PreferenceHandler.h"
#include "Shuddup.h"

#define makeNSRGB(red1, green1, blue1) [NSColor colorWithCalibratedRed:red1 / (CGFloat)USHRT_MAX green:green1 / (CGFloat)USHRT_MAX blue:blue1 / (CGFloat)USHRT_MAX alpha:1]

Boolean CFPreferencesHaveBeenSet()
{
	BOOL isSet = NO;
	NSAutoreleasePool *pool = [NSAutoreleasePool new];
	isSet = [[NSUserDefaults standardUserDefaults] boolForKey:(NSString*)PPPreferencesSet];
	[pool drain];
	
	return isSet;
}

void RegisterCFDefaults()
{
	NSAutoreleasePool *pool = [NSAutoreleasePool new];
	NSArray *pianoArray = [[NSArray alloc] initWithObjects:
						   [NSNumber numberWithShort:-4400],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:27400],
						   [NSNumber numberWithShort:8824],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:9024],
						   [NSNumber numberWithShort:-6],
						   [NSNumber numberWithShort:12328],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:2232],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:886],
						   [NSNumber numberWithShort:2048],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:26118],
						   [NSNumber numberWithShort:2296],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:886],
						   [NSNumber numberWithShort:28672],
						   [NSNumber numberWithShort:24576],
						   [NSNumber numberWithShort:568],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:-4446],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:28178],
						   [NSNumber numberWithShort:26374],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-490],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:9631],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:21314],
						   [NSNumber numberWithShort:9281],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:8722],
						   [NSNumber numberWithShort:24818],
						   [NSNumber numberWithShort:33],
						   [NSNumber numberWithShort:24],
						   [NSNumber numberWithShort:25],
						   [NSNumber numberWithShort:26],
						   [NSNumber numberWithShort:27],
						   [NSNumber numberWithShort:28],
						   [NSNumber numberWithShort:29],
						   [NSNumber numberWithShort:30],
						   [NSNumber numberWithShort:31],
						   [NSNumber numberWithShort:32],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:70],
						   [NSNumber numberWithShort:83],
						   [NSNumber numberWithShort:81],
						   [NSNumber numberWithShort:72],
						   [NSNumber numberWithShort:62],
						   [NSNumber numberWithShort:73],
						   [NSNumber numberWithShort:74],
						   [NSNumber numberWithShort:75],
						   [NSNumber numberWithShort:67],
						   [NSNumber numberWithShort:76],
						   [NSNumber numberWithShort:77],
						   [NSNumber numberWithShort:78],
						   [NSNumber numberWithShort:85],
						   [NSNumber numberWithShort:84],
						   [NSNumber numberWithShort:68],
						   [NSNumber numberWithShort:69],
						   [NSNumber numberWithShort:60],
						   [NSNumber numberWithShort:63],
						   [NSNumber numberWithShort:71],
						   [NSNumber numberWithShort:64],
						   [NSNumber numberWithShort:66],
						   [NSNumber numberWithShort:82],
						   [NSNumber numberWithShort:61],
						   [NSNumber numberWithShort:80],
						   [NSNumber numberWithShort:79],
						   [NSNumber numberWithShort:65],
						   [NSNumber numberWithShort:26196],
						   [NSNumber numberWithShort:12840],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:28238],
						   [NSNumber numberWithShort:27908],
						   [NSNumber numberWithShort:29186],
						   [NSNumber numberWithShort:44],
						   [NSNumber numberWithShort:57],
						   [NSNumber numberWithShort:55],
						   [NSNumber numberWithShort:46],
						   [NSNumber numberWithShort:36],
						   [NSNumber numberWithShort:47],
						   [NSNumber numberWithShort:48],
						   [NSNumber numberWithShort:49],
						   [NSNumber numberWithShort:41],
						   [NSNumber numberWithShort:50],
						   [NSNumber numberWithShort:51],
						   [NSNumber numberWithShort:52],
						   [NSNumber numberWithShort:59],
						   [NSNumber numberWithShort:58],
						   [NSNumber numberWithShort:42],
						   [NSNumber numberWithShort:43],
						   [NSNumber numberWithShort:34],
						   [NSNumber numberWithShort:37],
						   [NSNumber numberWithShort:45],
						   [NSNumber numberWithShort:38],
						   [NSNumber numberWithShort:40],
						   [NSNumber numberWithShort:56],
						   [NSNumber numberWithShort:35],
						   [NSNumber numberWithShort:54],
						   [NSNumber numberWithShort:53],
						   [NSNumber numberWithShort:39],
						   [NSNumber numberWithShort:10724],
						   [NSNumber numberWithShort:26126],
						   [NSNumber numberWithShort:12329],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:19678],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:12608],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:24580],
						   [NSNumber numberWithShort:19678],
						   [NSNumber numberWithShort:768],
						   [NSNumber numberWithShort:19011],
						   [NSNumber numberWithShort:26118],
						   [NSNumber numberWithShort:19048],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:27910],
						   [NSNumber numberWithShort:12650],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:406],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:5632],
						   [NSNumber numberWithShort:26128],
						   [NSNumber numberWithShort:12330],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-28312],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:12330],
						   [NSNumber numberWithShort:162],
						   [NSNumber numberWithShort:-28312],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:2088],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:26138],
						   [NSNumber numberWithShort:12348],
						   [NSNumber numberWithShort:31744],
						   [NSNumber numberWithShort:-20376],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:25102],
						   [NSNumber numberWithShort:12608],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-20376],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:25092],
						   [NSNumber numberWithShort:12608],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:24678],
						   [NSNumber numberWithShort:12650],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:12650],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:12650],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:12650],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:12650],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:17064],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:12650],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:17064],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:12668],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:5526],
						   [NSNumber numberWithShort:26174],
						   [NSNumber numberWithShort:8554],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:12650],
						   [NSNumber numberWithShort:98],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:8554],
						   [NSNumber numberWithShort:100],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:8554],
						   [NSNumber numberWithShort:114],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:8554],
						   [NSNumber numberWithShort:118],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:12040],
						   [NSNumber numberWithShort:17384],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:16874],
						   [NSNumber numberWithShort:122],
						   [NSNumber numberWithShort:28704],
						   [NSNumber numberWithShort:-24530],
						   [NSNumber numberWithShort:8287],
						   [NSNumber numberWithShort:8650],
						   [NSNumber numberWithShort:1006],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:5470],
						   [NSNumber numberWithShort:26118],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:-920],
						   [NSNumber numberWithShort:26114],
						   [NSNumber numberWithShort:28672],
						   [NSNumber numberWithShort:24576],
						   [NSNumber numberWithShort:-4686],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:-4870],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:-900],
						   [NSNumber numberWithShort:26214],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:156],
						   [NSNumber numberWithShort:26208],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:4394],
						   [NSNumber numberWithShort:26202],
						   [NSNumber numberWithShort:9576],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:9576],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:12328],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:576],
						   [NSNumber numberWithShort:-32768],
						   [NSNumber numberWithShort:12842],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:577],
						   [NSNumber numberWithShort:32767],
						   [NSNumber numberWithShort:-32192],
						   [NSNumber numberWithShort:13633],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:5396],
						   [NSNumber numberWithShort:26146],
						   [NSNumber numberWithShort:9576],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:9576],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:13672],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:98],
						   [NSNumber numberWithShort:12040],
						   [NSNumber numberWithShort:16872],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:17386],
						   [NSNumber numberWithShort:122],
						   [NSNumber numberWithShort:28704],
						   [NSNumber numberWithShort:-24530],
						   [NSNumber numberWithShort:8287],
						   [NSNumber numberWithShort:8232],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:26372],
						   [NSNumber numberWithShort:257],
						   [NSNumber numberWithShort:257],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:259],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:219],
						   [NSNumber numberWithShort:196],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:148],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:102],
						   nil];
	
	[[NSUserDefaults standardUserDefaults]
	 registerDefaults:
	 [NSDictionary dictionaryWithObjectsAndKeys:
	  [NSNumber numberWithBool:YES], PPPreferencesSet,
	  [NSNumber numberWithBool:YES], PPMAddExtension,
	  [NSNumber numberWithBool:YES], PPMMadCompression,
	  [NSNumber numberWithBool:YES], PPAutoCreator,
	  @"State 1", PPWindowName1,
	  @"State 2", PPWindowName2,
	  @"State 3", PPWindowName3,
	  [NSNumber numberWithInt:0], PPStartNumber,
	  pianoArray, PPPianoKeys,
	  [NSNumber numberWithShort:1], PPLoopType,
	  [NSNumber numberWithShort:3], PPVolumeLevel,
	  [NSNumber numberWithLong:rate44khz], PPSoundOutRate,
	  [NSNumber numberWithShort:SoundManagerDriver], PPSoundDriver,
	  [NSNumber numberWithShort:16], PPSoundOutBits,
	  [NSNumber numberWithBool:NO], PPSurroundToggle,
	  [NSNumber numberWithLong:NO], PPReverbToggle,
	  [NSNumber numberWithLong:25], PPReverbAmount,
	  [NSNumber numberWithLong:60], PPReverbStrength,
	  [NSNumber numberWithLong:1], PPOversamplingAmount,
	  [NSNumber numberWithInt:0], PPStereoDelayAmount,
	  [NSNumber numberWithBool:NO], PPStereoDelayToggle,
	  [NSNumber numberWithBool:NO], PPOversamplingToggle,
	  [NSNumber numberWithInt:1], PPOversamplingAmount,
	  [makeNSRGB(65535, 65535, 39321) PPencodeColor], PPDEMarkerColorPref,
	  [NSNumber numberWithBool:NO], PPCEShowNotesLen,
	  [NSNumber numberWithBool:NO], PPCEShowMarkers,
	  [NSNumber numberWithShort:130], PPCETrackHeight,
	  [NSNumber numberWithShort:4], PPCETempoNum,
	  [NSNumber numberWithShort:4], PPCETempoUnit,
	  
	  [makeNSRGB(61166, 0, 0) PPencodeColor],PPCColor1,
	  [makeNSRGB(35980, 48316, 7196) PPencodeColor], PPCColor2,
	  [makeNSRGB(13107, 65535, 65535) PPencodeColor], PPCColor3,
	  [[NSColor yellowColor] PPencodeColor], PPCColor4,
	  [makeNSRGB(24672, 51914, 36494) PPencodeColor], PPCColor5,
	  [makeNSRGB(13107, 65535, 65535) PPencodeColor], PPCColor6,
	  [makeNSRGB(26214, 52428, 65535) PPencodeColor], PPCColor7,
	  [makeNSRGB(65535, 25428, 65535) PPencodeColor], PPCColor8,
	  [makeNSRGB(52428, 39321, 26214) PPencodeColor], PPCColor9,
	  [makeNSRGB(13107, 26214, 13107) PPencodeColor], PPCColor10,
	  [makeNSRGB(65535, 26214, 65535) PPencodeColor], PPCColor11,
	  [makeNSRGB(65535, 39321, 0) PPencodeColor], PPCColor12,
	  [makeNSRGB(52428, 65535, 52428) PPencodeColor],PPCColor13,
	  [makeNSRGB(39321, 0, 26214) PPencodeColor], PPCColor14,
	  [makeNSRGB(26214, 52428, 0) PPencodeColor], PPCColor15,
	  [makeNSRGB(65535, 39321, 65535) PPencodeColor], PPCColor16,
	  [makeNSRGB(52428, 65535, 26214) PPencodeColor], PPCColor17,
	  [makeNSRGB(26214, 0, 39321) PPencodeColor], PPCColor18,
	  [makeNSRGB(52428, 0, 39321) PPencodeColor], PPCColor19,
	  [makeNSRGB(52428, 39321, 65535) PPencodeColor], PPCColor20,
	  [makeNSRGB(26214, 52428, 26214) PPencodeColor], PPCColor21,
	  [makeNSRGB(52428, 52428, 13107) PPencodeColor], PPCColor22,
	  [makeNSRGB(39321, 52428, 52428) PPencodeColor], PPCColor23,
	  [makeNSRGB(26214, 13109, 26214) PPencodeColor], PPCColor24,
	  [makeNSRGB(52428, 39321, 52428) PPencodeColor], PPCColor25,
	  [makeNSRGB(13107, 65535, 65535) PPencodeColor], PPCColor26,
	  [makeNSRGB(13107, 65535, 26214) PPencodeColor], PPCColor27,
	  [makeNSRGB(26214, 65535, 13107) PPencodeColor], PPCColor28,
	  [makeNSRGB(52428, 39321, 52428) PPencodeColor], PPCColor29,
	  [makeNSRGB(52428, 39321, 13107) PPencodeColor], PPCColor30,
	  [makeNSRGB(52428, 52428, 13107) PPencodeColor], PPCColor31,
	  [makeNSRGB(65535, 52428, 0) PPencodeColor], PPCColor32,
	  [makeNSRGB(61166, 0, 0) PPencodeColor], PPCColor33,
	  [makeNSRGB(0, 65535, 26214) PPencodeColor], PPCColor34,
	  [[NSColor cyanColor] PPencodeColor], PPCColor35,
	  [[NSColor yellowColor] PPencodeColor], PPCColor36,
	  [[NSColor greenColor] PPencodeColor], PPCColor37,
	  [makeNSRGB(13107, 52428, 65535) PPencodeColor], PPCColor38,
	  [makeNSRGB(39321, 52428, 39321) PPencodeColor], PPCColor39,
	  [makeNSRGB(65535, 39321, 26214) PPencodeColor], PPCColor40,
	  [makeNSRGB(39321, 65535, 65535) PPencodeColor], PPCColor41,
	  [makeNSRGB(26214, 65535, 65535) PPencodeColor], PPCColor42,
	  [makeNSRGB(65535, 39321, 65535) PPencodeColor], PPCColor43,
	  [makeNSRGB(0, 21845, 0) PPencodeColor], PPCColor44,
	  [makeNSRGB(52428, 65535, 52428) PPencodeColor], PPCColor45,
	  [makeNSRGB(26214, 0, 0) PPencodeColor], PPCColor46,
	  [makeNSRGB(13107, 65535, 26214) PPencodeColor], PPCColor47,
	  [makeNSRGB(65535, 65535, 26214) PPencodeColor], PPCColor48,
	  [makeNSRGB(52428, 0, 0) PPencodeColor], PPCColor49,
	  [makeNSRGB(39321, 0, 13107) PPencodeColor], PPCColor50,
	  [makeNSRGB(39321, 65535, 39321) PPencodeColor], PPCColor51,
	  [makeNSRGB(39321, 52428, 52428) PPencodeColor], PPCColor52,
	  [makeNSRGB(52428, 52428, 0) PPencodeColor], PPCColor53,
	  [makeNSRGB(52428, 52428, 13107) PPencodeColor], PPCColor54,
	  [makeNSRGB(65535, 39321, 65535) PPencodeColor], PPCColor55,
	  [[NSColor redColor] PPencodeColor], PPCColor56,
	  [makeNSRGB(65535, 0, 26214) PPencodeColor], PPCColor57,
	  [makeNSRGB(26214, 65535, 65535) PPencodeColor], PPCColor58,
	  [makeNSRGB(26214, 65535, 52428) PPencodeColor], PPCColor59,
	  [makeNSRGB(39321, 52428, 26214) PPencodeColor], PPCColor60,
	  [makeNSRGB(39321, 52428, 39321) PPencodeColor], PPCColor61,
	  [makeNSRGB(39321, 52428, 26214) PPencodeColor], PPCColor62,
	  [makeNSRGB(52428, 52428, 26214) PPencodeColor], PPCColor63,
	  [makeNSRGB(52428, 52428, 39321) PPencodeColor], PPCColor64,
	  [makeNSRGB(61166, 0, 0) PPencodeColor], PPCColor65,
	  [makeNSRGB(0, 65535, 26214) PPencodeColor], PPCColor66,
	  [makeNSRGB(0, 65535, 65535) PPencodeColor], PPCColor67,
	  [makeNSRGB(65535, 65535, 0) PPencodeColor], PPCColor68,
	  [[NSColor greenColor] PPencodeColor], PPCColor69,
	  [makeNSRGB(13107, 52428, 65535) PPencodeColor], PPCColor70,
	  [makeNSRGB(39321, 52428, 39321) PPencodeColor], PPCColor71,
	  [makeNSRGB(65535, 39321, 26214) PPencodeColor], PPCColor72,
	  [makeNSRGB(39321, 65535, 65535) PPencodeColor], PPCColor73,
	  [makeNSRGB(26214, 65535, 65535) PPencodeColor], PPCColor74,
	  [makeNSRGB(65535, 39321, 65535) PPencodeColor], PPCColor75,
	  [makeNSRGB(0, 21845, 0) PPencodeColor], PPCColor76,
	  [makeNSRGB(52428, 65535, 52428) PPencodeColor], PPCColor77,
	  [makeNSRGB(26214, 0, 0) PPencodeColor], PPCColor78,
	  [makeNSRGB(13107, 65535, 26214) PPencodeColor], PPCColor79,
	  [makeNSRGB(65535, 65535, 26214) PPencodeColor], PPCColor80,
	  [makeNSRGB(52428, 0, 0) PPencodeColor], PPCColor81,
	  [makeNSRGB(39321, 0, 13107) PPencodeColor], PPCColor82,
	  [makeNSRGB(39321, 65535, 39321) PPencodeColor], PPCColor83,
	  [makeNSRGB(39321, 52428, 52428) PPencodeColor], PPCColor84,
	  [makeNSRGB(52428, 52428, 0) PPencodeColor], PPCColor85,
	  [makeNSRGB(52428, 52428, 13107) PPencodeColor], PPCColor86,
	  [makeNSRGB(65535, 39321, 65535) PPencodeColor], PPCColor87,
	  [[NSColor redColor] PPencodeColor], PPCColor88,
	  [makeNSRGB(39321, 0, 26214) PPencodeColor], PPCColor89,
	  [makeNSRGB(26214, 65535, 65535) PPencodeColor], PPCColor90,
	  [makeNSRGB(26214, 65535, 52428) PPencodeColor], PPCColor91,
	  [makeNSRGB(39321, 52428, 26214) PPencodeColor], PPCColor92,
	  [makeNSRGB(39321, 52428, 39321) PPencodeColor], PPCColor93,
	  [makeNSRGB(39321, 52428, 26214) PPencodeColor], PPCColor94,
	  [makeNSRGB(52428, 52428, 26214) PPencodeColor], PPCColor95,
	  [makeNSRGB(52428, 52428, 39321) PPencodeColor], PPCColor96,
	  nil]];
	
	[pianoArray release];
	[pool drain];
}

void ResetCFPreferences()
{
	NSAutoreleasePool *pool = [NSAutoreleasePool new];
	NSString *appDomain = [[NSBundle mainBundle] bundleIdentifier];
	[[NSUserDefaults standardUserDefaults] removePersistentDomainForName:appDomain];
	[pool drain];
}

void ReadCFPreferences()
{
	NSAutoreleasePool *pool = [NSAutoreleasePool new];
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSString *tempStr = nil;
	NSArray *tempArray = nil;
	int i;
	memset(&thePrefs, 0, sizeof(Prefs));
	thePrefs.addExtension = [defaults boolForKey:(NSString*)PPMAddExtension];
	thePrefs.MADCompression = [defaults boolForKey:(NSString*)PPMMadCompression];
	thePrefs.OscilloLine = [defaults boolForKey:(NSString*)PPMOscilloscopeDrawLines];
#define PPCOLOR(val) ReadCFPreferencesWithQDColor(PPCColor ## val, &thePrefs.tracksColor[val - 1])
	PPCOLORPOPULATE();
#undef PPCOLOR
#if defined(powerc) || defined (__powerc) || defined(__ppc__)
	thePrefs.PPCMachine = true;
#else
	thePrefs.PPCMachine = false;
#endif
	tempStr = [defaults stringForKey:(NSString*)PPWindowName1];
	CFStringGetPascalString((CFStringRef)tempStr, thePrefs.WinNames[0], sizeof(thePrefs.WinNames[0]), kCFStringEncodingMacRoman);
	tempStr = [defaults stringForKey:(NSString*)PPWindowName2];
	CFStringGetPascalString((CFStringRef)tempStr, thePrefs.WinNames[1], sizeof(thePrefs.WinNames[0]), kCFStringEncodingMacRoman);
	tempStr = [defaults stringForKey:(NSString*)PPWindowName3];
	CFStringGetPascalString((CFStringRef)tempStr, thePrefs.WinNames[2], sizeof(thePrefs.WinNames[0]), kCFStringEncodingMacRoman);
	tempStr = nil;
	thePrefs.NoStart = [defaults integerForKey:(NSString*)PPStartNumber];
	tempArray = [defaults arrayForKey:(NSString*)PPPianoKeys];
	for (i = 0; i < 300; i++) {
		thePrefs.PianoKey[i] = [[tempArray objectAtIndex:i] shortValue];
	}
	thePrefs.LoopType = [defaults integerForKey:(NSString*)PPLoopType];
	thePrefs.volumeLevel = [defaults integerForKey:(NSString*)PPVolumeLevel];
	thePrefs.Compressor = 'NONE';
	thePrefs.outPutMode = thePrefs.DirectDriverType.outPutMode = DeluxeStereoOutPut;	// force DeluxeStereoOutPut
	thePrefs.channelNumber = 2;
	thePrefs.DirectDriverType.numChn = 4;
	thePrefs.DirectDriverType.TickRemover = thePrefs.TickRemover = true;
	
	thePrefs.Reverb = thePrefs.DirectDriverType.Reverb = [defaults boolForKey:(NSString*)PPReverbToggle];
	thePrefs.ReverbSize = thePrefs.DirectDriverType.ReverbSize = [defaults integerForKey:(NSString*)PPReverbAmount];
	thePrefs.ReverbStrength = thePrefs.DirectDriverType.ReverbStrength = [defaults integerForKey:(NSString*)PPReverbStrength];
	thePrefs.outPutRate = thePrefs.FrequenceSpeed = thePrefs.DirectDriverType.outPutRate = [defaults integerForKey:(NSString*)PPSoundOutRate];
	thePrefs.outPutBits = thePrefs.amplitude = thePrefs.DirectDriverType.outPutBits = [defaults integerForKey:(NSString*)PPSoundOutBits];
	thePrefs.driverMode = thePrefs.DirectDriverType.driverMode = [defaults integerForKey:(NSString*)PPSoundDriver];
	thePrefs.surround = thePrefs.DirectDriverType.surround = [defaults boolForKey:(NSString*)PPSurroundToggle];
	if ([defaults boolForKey:(NSString*)PPStereoDelayToggle]) {
		thePrefs.MicroDelaySize = thePrefs.DirectDriverType.MicroDelaySize = [defaults integerForKey:(NSString*)PPStereoDelayAmount];
	} else {
		thePrefs.MicroDelaySize = thePrefs.DirectDriverType.MicroDelaySize = 0;
	}
	
	if ([defaults boolForKey:(NSString *)PPOversamplingToggle]) {
		thePrefs.oversampling = thePrefs.DirectDriverType.oversampling = [defaults integerForKey:(NSString *)PPOversamplingAmount];
	} else {
		thePrefs.oversampling = thePrefs.DirectDriverType.oversampling = 1;
	}
	
	ReadCFPreferencesWithQDColor(PPDEMarkerColorPref, &thePrefs.yellC);
	thePrefs.StaffShowAllNotes = [defaults boolForKey:(NSString*)PPCEShowMarkers];
	thePrefs.StaffShowLength = [defaults boolForKey:(NSString*)PPCEShowNotesLen];
	thePrefs.TempsNum = [defaults integerForKey:(NSString*)PPCETempoNum];
	thePrefs.TempsUnit = [defaults integerForKey:(NSString*)PPCETempoUnit];
	thePrefs.TrackHeight = [defaults integerForKey:(NSString*)PPCETrackHeight];
	
	[pool drain];
}

void WriteCFPreferences()
{
	NSAutoreleasePool *pool = [NSAutoreleasePool new];
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSMutableArray *tmpMutable = nil;
	int i;
	[defaults setBool:thePrefs.addExtension forKey:(NSString*)PPMAddExtension];
	[defaults setBool:thePrefs.MADCompression forKey:(NSString*)PPMMadCompression];
#define PPCOLOR(val) WriteCFPreferencesWithQDColor(PPCColor ## val, thePrefs.tracksColor[val - 1])
	PPCOLORPOPULATE();
#undef PPCOLOR
	[defaults setObject:[(NSString*)CFStringCreateWithPascalString(kCFAllocatorDefault, thePrefs.WinNames[0], kCFStringEncodingMacRoman) autorelease] forKey:(NSString*)PPWindowName1];
	[defaults setObject:[(NSString*)CFStringCreateWithPascalString(kCFAllocatorDefault, thePrefs.WinNames[1], kCFStringEncodingMacRoman) autorelease] forKey:(NSString*)PPWindowName2];
	[defaults setObject:[(NSString*)CFStringCreateWithPascalString(kCFAllocatorDefault, thePrefs.WinNames[2], kCFStringEncodingMacRoman) autorelease] forKey:(NSString*)PPWindowName3];
	[defaults setInteger:thePrefs.NoStart forKey:(NSString*)PPStartNumber];
	tmpMutable = [[NSMutableArray alloc] initWithCapacity:300];
	for (i = 0; i < 300; i++) {
		[tmpMutable addObject:[NSNumber numberWithShort:thePrefs.PianoKey[i]]];
	}
	[defaults setObject:tmpMutable forKey:(NSString*)PPPianoKeys];
	[tmpMutable release];
	tmpMutable = nil;
	[defaults setInteger:thePrefs.LoopType forKey:(NSString*)PPLoopType];
	[defaults setInteger:thePrefs.volumeLevel forKey:(NSString*)PPVolumeLevel];
	[defaults setInteger:thePrefs.ReverbStrength forKey:(NSString*)PPReverbStrength];
	[defaults setInteger:thePrefs.ReverbSize forKey:(NSString*)PPReverbAmount];
	[defaults setBool:thePrefs.Reverb forKey:(NSString*)PPReverbToggle];
	[defaults setInteger:thePrefs.outPutRate forKey:(NSString*)PPSoundOutRate];
	[defaults setInteger:thePrefs.outPutBits forKey:(NSString*)PPSoundOutBits];
	[defaults setInteger:thePrefs.driverMode forKey:(NSString*)PPSoundDriver];
	[defaults setBool:thePrefs.surround forKey:(NSString*)PPSurroundToggle];
	if (thePrefs.MicroDelaySize > 0) {
		[defaults setBool:YES forKey:(NSString*)PPStereoDelayToggle];
	} else {
		[defaults setBool:NO forKey:(NSString*)PPStereoDelayToggle];
	}
	[defaults setInteger:thePrefs.MicroDelaySize forKey:(NSString*)PPStereoDelayAmount];

	if (thePrefs.oversampling > 1) {
		[defaults setBool:YES forKey:(NSString *)PPOversamplingToggle];
	} else {
		[defaults setBool:NO forKey:(NSString *)PPOversamplingToggle];
	}
	[defaults setInteger:thePrefs.oversampling forKey:(NSString *)PPOversamplingAmount];
	
	WriteCFPreferencesWithQDColor(PPDEMarkerColorPref, thePrefs.yellC);
	[defaults setBool:thePrefs.StaffShowAllNotes forKey:(NSString*)PPCEShowMarkers];
	[defaults setBool:thePrefs.StaffShowLength forKey:(NSString*)PPCEShowNotesLen];
	[defaults setInteger:thePrefs.TempsNum forKey:(NSString*)PPCETempoNum];
	[defaults setInteger:thePrefs.TempsUnit forKey:(NSString*)PPCETempoUnit];
	[defaults setInteger:thePrefs.TrackHeight forKey:(NSString*)PPCETrackHeight];
	
	
	[defaults synchronize];
	
	[pool drain];
}

void WriteCFPreferencesWithQDColor(CFStringRef valName, RGBColor valVal)
{
	NSAutoreleasePool *pool = [NSAutoreleasePool new];
	[[NSUserDefaults standardUserDefaults] setObject:[[NSColor PPColorFromQDColor:valVal] PPencodeColor] forKey:(NSString*)valName];
	[pool drain];
}

Boolean ReadCFPreferencesWithQDColor(CFStringRef valName, RGBColor *valVal)
{
	NSAutoreleasePool *pool = [NSAutoreleasePool new];
	NSData *colorDat = [[NSUserDefaults standardUserDefaults] valueForKey:(NSString*)valName];
	if (colorDat) {
		NSColor *colorClass = [NSColor PPDecodeColorWithData:colorDat];
		if (colorClass) {
			*valVal = [colorClass PPQDColor];
			[pool drain];
			return true;
		}
	}
	[pool drain];
	return false;
}
