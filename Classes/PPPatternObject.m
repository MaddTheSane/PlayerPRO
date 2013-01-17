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
			AUTORELEASEOBJNORETURN(self);
			return nil;
		}
		curMusic = mus;
		index = ptnIdx;
		
	}
	return self;
}

#if !__has_feature(objc_arc)
- (void)dealloc
{
	[commands release];
	
	[super dealloc];
}

#endif

@end
