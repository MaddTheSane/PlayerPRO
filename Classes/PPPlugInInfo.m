//
//  PPPlugInInfo.m
//  PPMacho
//
//  Created by C.W. Betts on 12/31/12.
//
//

#import "PPPlugInInfo.h"

@implementation PPPlugInInfo

@synthesize authorName;
@synthesize plugName;

- (id)initWithPlugName:(NSString*)pn author:(NSString*)aut
{
	if (self = [super init]) {
		plugName = [[NSString alloc] initWithString:pn];
		authorName = [[NSString alloc] initWithString:aut];
	}
	return self;
}

- (NSString*)description
{
	return [NSString stringWithFormat:@"%@ %@", plugName, authorName];
}

#if !__has_feature(objc_arc)
- (void)dealloc
{
	[plugName release];
	[authorName release];
	
	[super dealloc];
}
#endif


@end
