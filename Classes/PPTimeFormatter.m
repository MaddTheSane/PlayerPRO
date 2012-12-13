//
//  PPTimeFormatter.m
//  PPMacho
//
//  Created by C.W. Betts on 12/10/12.
//
//

#import "PPTimeFormatter.h"

@implementation PPTimeFormatter

- (NSString *)stringForObjectValue:(id)obj
{
	if ([obj isKindOfClass:[NSNumber class]] ) {
		NSInteger theVal = [obj integerValue];
		int seconds = (theVal/60) % 60;
		int minutes = (theVal/(60 * 60)) % 60;
		if (theVal / (60*60*60) > 1) {
			int hours = (theVal/(60 * 60 * 60));
			return [NSString stringWithFormat:@"%d:%02d:%02d", hours, minutes, seconds];
		} else {
			return [NSString stringWithFormat:@"%d:%02d", minutes, seconds];

		}
	} else return nil;
}

- (BOOL)getObjectValue:(out id *)obj forString:(NSString *)string errorDescription:(out NSString **)error
{
	int seconds = 0, minutes = 0, hours = 0;
	if(sscanf([string UTF8String], "%d:%d:%d", &hours, &minutes, &seconds) != 3)
	{
		hours = 0;
		if(sscanf([string UTF8String], "%d:%d", &minutes, &seconds) != 2)
		{
			*error = @"Invalid format";
			return NO;
		}
	}
	NSInteger returnValue = (((hours * 60 + minutes) * 60) + seconds) * 60;
	//[obj setIntegerValue:returnValue];
	*obj = [[NSNumber alloc] initWithInteger:returnValue];
	return YES;
}

@end
