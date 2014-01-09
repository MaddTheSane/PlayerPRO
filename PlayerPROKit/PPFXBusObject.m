//
//  PPFXBusObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/8/14.
//
//

#import "PPFXBusObject.h"

@implementation PPFXBusObject
@synthesize theBus;

- (id)initWithFXBus:(FXBus *)set
{
	if (self = [super init]) {
		if (!set) {
			return nil;
		}
		theBus = *set;
	}
	return self;
}

@end
