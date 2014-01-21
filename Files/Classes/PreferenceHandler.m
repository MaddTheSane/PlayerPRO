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
#if 0
	tempArray = [defaults arrayForKey:PPPianoKeys];
	for (i = 0; i < 300; i++) {
		thePrefs.PianoKey[i] = [[tempArray objectAtIndex:i] shortValue];
	}
#endif
	
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
