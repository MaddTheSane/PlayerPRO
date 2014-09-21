//
//  PPInstrumentPlugBridgeObject.m
//  PPMacho
//
//  Created by C.W. Betts on 9/20/14.
//
//

#import "PPInstrumentPlugBridgeObject.h"

@interface PPInstrumentPlugBridgeObject ()
@property (strong, readwrite) NSBundle *bundleFile;
@end

@implementation PPInstrumentPlugBridgeObject
@synthesize xxxx;

- (instancetype)init
{
	return nil;
}

- (instancetype)initWithBundle:(NSBundle *)tempBundle
{
	if (self = [super init]) {
		
	}
	return self;
}

- (void)dealloc
{
	if (xxxx) {
		(*xxxx)->Release(xxxx);
	}
}

@end
