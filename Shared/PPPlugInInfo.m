//
//  PPPlugInInfo.m
//  PPMacho
//
//  Created by C.W. Betts on 12/31/12.
//
//

#import "PPPlugInInfo.h"

@interface PPPlugInInfo ()
@property (readwrite, copy) NSString	*plugName;
@property (readwrite, copy) NSString	*authorName;
@property (readwrite, copy) NSString	*plugType;
@property (readwrite, copy) NSString	*plugCopyright;
@property (readwrite, strong) NSURL		*plugURL;
@end

@implementation PPPlugInInfo
@synthesize authorName;
@synthesize plugName;
@synthesize plugType;
@synthesize plugURL;
@synthesize plugCopyright;

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
		NSString *tmpCopyRight = [[NSBundle bundleWithURL:pu] infoDictionary][@"NSHumanReadableCopyright"];
		self.plugName = pn;
		self.authorName = aut;
		self.plugType = pt;
		self.plugURL = pu;
		if (tmpCopyRight) {
			self.plugCopyright = tmpCopyRight;
		} else {
			self.plugCopyright = @"No Copyright Info available";
		}
	}
	return self;
}

- (NSString*)description
{
	return [NSString stringWithFormat:@"%@: %@ (%@), %@", plugName, authorName, plugCopyright, plugType];
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
		} else
			return YES;
	} else
		return NO;
}

- (NSUInteger)hash
{
	return [plugName hash] ^ [plugType hash] ^ [authorName hash] ^ [plugCopyright hash];
}

@end
