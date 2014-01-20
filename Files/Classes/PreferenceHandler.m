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
	  nil]];
	
	
	[pool drain];
}

void ReadCFPreferences()
{
	NSAutoreleasePool *pool = [NSAutoreleasePool new];

	[pool drain];
}

void WriteCFPreferences()
{
	NSAutoreleasePool *pool = [NSAutoreleasePool new];

	
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
