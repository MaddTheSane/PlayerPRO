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
@synthesize plugURL;

- (id)initWithPlugName:(NSString*)pn author:(NSString*)aut
{
	return [self initWithPlugName:pn author:aut plugType:@"Unknown" plugURL:[[NSBundle mainBundle] bundleURL]];
}

- (id)initWithPlugName:(NSString*)pn author:(NSString*)aut plugType:(NSString*)pt
{
	return [self initWithPlugName:pn author:aut plugType:pt plugURL:[[NSBundle mainBundle] bundleURL]];
}

- (id)initWithPlugName:(NSString*)pn author:(NSString*)aut plugType:(NSString*)pt plugURL:(NSURL*)pu;
{
	if (self = [super init]) {
		plugName = [pn copy];
		authorName = [aut copy];
		plugType = [pt copy];
		plugURL = [pu copy];
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

- (NSUInteger)hash
{
	return [plugName hash] ^ [plugType hash] ^ [authorName hash];
}

#if !__has_feature(objc_arc)
- (void)dealloc
{
	[plugName release];
	[authorName release];
	[plugType release];
	[plugURL release];
	
	[super dealloc];
}
#endif


@end
