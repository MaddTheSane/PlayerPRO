//
//  PPTimeFormatter.m
//  PPMacho
//
//  Created by C.W. Betts on 12/10/12.
//
//

#include <stdio.h>
#import "PPTimeFormatter.h"
#import <Foundation/Foundation.h>

@implementation PPTimeFormatter

- (NSString *)stringForObjectValue:(id)obj
{
	if ([obj isKindOfClass:[NSNumber class]] ) {
		NSInteger theVal = [obj integerValue];
		NSInteger fullVal = labs(theVal);
		BOOL isNeg = theVal < 0;
		int seconds = (fullVal/60) % 60;
		int minutes = (fullVal/(60 * 60)) % 60;
		if (fullVal / (60*60*60) > 0) {
			int hours = (int)(fullVal/(60 * 60 * 60));
			return [NSString stringWithFormat:@"%s%d:%02d:%02d", isNeg ? "-": "", hours, minutes, seconds];
		} else {
			return [NSString stringWithFormat:@"%s%d:%02d", isNeg ? "-": "", minutes, seconds];
		}
	} else return nil;
}

- (BOOL)getObjectValue:(out id *)obj forString:(NSString *)string errorDescription:(out NSString **)error
{
	int seconds = 0, minutes = 0, hours = 0;
	NSInteger returnValue = 0;
	BOOL isNeg = NO;
	if ([string hasPrefix:@"-"]) {
		isNeg = YES;
		string = [string substringFromIndex:1];
	}
	if (sscanf([string UTF8String], "%d:%d:%d", &hours, &minutes, &seconds) != 3) {
		hours = 0;
		if (sscanf([string UTF8String], "%d:%d", &minutes, &seconds) != 2) {
			if (error) {
				*error = @"Invalid format";
			}
			return NO;
		}
	}
	returnValue = (((hours * 60 + minutes) * 60) + seconds) * 60 * (isNeg ? -1 : 1);
	*obj = @(returnValue);
	return YES;
}

@end
