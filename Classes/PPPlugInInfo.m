//
//  PPPlugInInfo.m
//  PPMacho
//
//  Created by C.W. Betts on 12/31/12.
//
//

#import "PPPlugInInfo.h"

@interface PPPlugInInfo ()
@property (readwrite, copy)		NSString *plugName;
@property (readwrite, copy)		NSString *authorName;
@property (readwrite, copy)		NSString *plugType;
@property (readwrite, retain)	NSURL    *plugURL;
@end

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
		self.plugName = pn;
		self.authorName = aut;
		self.plugType = pt;
		self.plugURL = pu;
	}
	return self;
}

- (NSString*)description
{
	return [NSString stringWithFormat:@"%@: %@, %@", plugName, authorName, plugType];
}

- (BOOL)isEqual:(id)object
{
	if (!object) {
		return NO;
	}
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
	self.plugName = nil;
	self.authorName = nil;
	self.plugType = nil;
	self.plugURL = nil;
	
	[super dealloc];
}
#endif


@end
