//
//  NSColor+PPPreferences.m
//  PPMacho
//
//  Created by C.W. Betts on 7/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "NSColor+PPPreferences.h"


@implementation NSColor (PPPreferences)

- (NSData *)PPencodeColor
{
	return [NSKeyedArchiver archivedDataWithRootObject:self];
}

+ (NSColor *)PPDecodeColorWithData:(NSData *)theData
{
	return [NSKeyedUnarchiver unarchiveObjectWithData:theData];

}

@end
