//
//  NSColor+PPPreferences.m
//  PPMacho
//
//  Created by C.W. Betts on 7/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "NSColor+PPPreferences.h"
#import <Foundation/NSArchiver.h>

@implementation NSColor (PPPreferences)

- (NSData *)PPencodeColor
{
	return [NSKeyedArchiver archivedDataWithRootObject:self];
}

+ (NSColor *)PPDecodeColorWithData:(NSData *)theData
{
	return [NSKeyedUnarchiver unarchiveObjectWithData:theData];
}

+ (NSColor*)PPColorFromQDColor:(RGBColor)qdColor
{
	return [self colorWithCalibratedRed:qdColor.red / (CGFloat)USHRT_MAX green:qdColor.green / (CGFloat)USHRT_MAX blue:qdColor.blue / (CGFloat)USHRT_MAX alpha:1];
}

- (RGBColor)PPQDColor
{
	RGBColor toRet;
	CGFloat CGRed, CGGreen, CGBlue;
	NSColor *calColor;
	if ([[self colorSpaceName] isEqualToString:NSCalibratedRGBColorSpace])
		calColor = self;
	else
		calColor = [self colorUsingColorSpaceName:NSCalibratedRGBColorSpace];
	[calColor getRed:&CGRed green:&CGGreen blue:&CGBlue alpha:NULL];
	
	toRet.red = CGRed * USHRT_MAX;
	toRet.green = CGGreen * USHRT_MAX;
	toRet.blue = CGBlue * USHRT_MAX;
	
	return toRet;
}

@end
