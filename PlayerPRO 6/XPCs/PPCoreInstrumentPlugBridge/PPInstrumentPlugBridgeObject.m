//
//  PPInstrumentPlugBridgeObject.m
//  PPMacho
//
//  Created by C.W. Betts on 9/20/14.
//
//

#import "PPInstrumentPlugBridgeObject.h"
#import "ARCBridge.h"

@interface PPInstrumentPlugBridgeObject ()
@property (retain, readwrite) NSBundle *bundleFile;
@end

@implementation PPInstrumentPlugBridgeObject
@synthesize xxxx;
@synthesize bundleFile = _bundleFile;

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
	
#if !__has_feature(objc_arc)
	self.bundleFile = nil;
	
	[super dealloc];
#endif
}

@end
