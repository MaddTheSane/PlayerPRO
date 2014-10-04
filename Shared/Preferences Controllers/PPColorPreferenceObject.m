//
//  PPColorPreferenceObject.m
//  PPMacho
//
//  Created by C.W. Betts on 8/21/14.
//
//

#import "PPColorPreferenceObject.h"

@interface PPColorPreferenceObject ()
@property (readwrite, setter=privateSetIndex:) NSInteger colorIndex;
@end

@implementation PPColorPreferenceObject

- (instancetype)initWithColor:(NSColor*)aColor index:(NSInteger)idx
{
	if (self = [super init]) {
		self.color = aColor;
		self.colorIndex = idx;
	}
	return self;
}

@end
