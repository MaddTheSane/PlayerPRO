//
//  PPPatternObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/17/13.
//
//

#import "PPPatternObject.h"

@implementation PPPatternObject

- (id)initWithMusic:(MADMusic *)mus patternAtIndex:(short)ptnIdx
{
	if (self = [super init]) {
		if (!mus) {
			return nil;
		}
		curMusic = mus;
		index = ptnIdx;
		
	}
	return self;
}

@end
