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
@synthesize plugType;

- (id)initWithPlugName:(NSString*)pn author:(NSString*)aut
{
	return [self initWithPlugName:pn author:aut plugType:@"Unknown"];
}

- (id)initWithPlugName:(NSString*)pn author:(NSString*)aut plugType:(NSString*)pt
{
	if (self = [super init]) {
		plugName = [[NSString alloc] initWithString:pn];
		authorName = [[NSString alloc] initWithString:aut];
		plugType = [[NSString alloc] initWithString:pt];
	}
	return self;
}

- (NSString*)description
{
	return [NSString stringWithFormat:@"%@: %@, %@", plugName, authorName, plugType];
}

- (BOOL)isEqual:(id)object
{
	if (object == self) {
		return YES;
	}
	if ([object isKindOfClass:[PPPlugInInfo class]]) {
		if (![plugName isEqualToString:[object plugName] ]) {
			return NO;
		} else if (![authorName isEqualToString:[object authorName]]) {
			return NO;
		} else if (![plugType isEqualToString:[object plugType]]) {
			return NO;
		} else return YES;
	} else return NO;
	
}

#if !__has_feature(objc_arc)
- (void)dealloc
{
	[plugName release];
	[authorName release];
	[plugType release];
	
	[super dealloc];
}
#endif


@end
