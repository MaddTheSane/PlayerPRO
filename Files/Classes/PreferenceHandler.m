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
	  nil]];
	
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
	memset(&thePrefs, 0, sizeof(Prefs));
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSString *tempStr;
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

	[pool drain];
}

void WriteCFPreferences()
{
	NSAutoreleasePool *pool = [NSAutoreleasePool new];
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setBool:thePrefs.addExtension forKey:(NSString*)PPMAddExtension];
	[defaults setBool:thePrefs.MADCompression forKey:(NSString*)PPMMadCompression];
#define PPCOLOR(val) WriteCFPreferencesWithQDColor(PPCColor ## val, thePrefs.tracksColor[val - 1])
	PPCOLORPOPULATE();
#undef PPCOLOR
	[defaults setObject:[(NSString*)CFStringCreateWithPascalString(kCFAllocatorDefault, thePrefs.WinNames[0], kCFStringEncodingMacRoman) autorelease] forKey:(NSString*)PPWindowName1];
	[defaults setObject:[(NSString*)CFStringCreateWithPascalString(kCFAllocatorDefault, thePrefs.WinNames[1], kCFStringEncodingMacRoman) autorelease] forKey:(NSString*)PPWindowName2];
	[defaults setObject:[(NSString*)CFStringCreateWithPascalString(kCFAllocatorDefault, thePrefs.WinNames[2], kCFStringEncodingMacRoman) autorelease] forKey:(NSString*)PPWindowName3];
	
	
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
