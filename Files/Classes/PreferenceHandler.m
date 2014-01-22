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

#define	VERSION			0x0592

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
	NSString *tempStr;
	NSArray *tempArray;
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
	thePrefs.NoStart = [defaults integerForKey:(NSString*)PPStartNumber];
	tempArray = [defaults arrayForKey:(NSString*)PPPianoKeys];
	for (i = 0; i < 300; i++) {
		thePrefs.PianoKey[i] = [[tempArray objectAtIndex:i] shortValue];
	}
	
	[pool drain];
}

void WriteCFPreferences()
{
	NSAutoreleasePool *pool = [NSAutoreleasePool new];
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSMutableArray *tmpMutable;
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
