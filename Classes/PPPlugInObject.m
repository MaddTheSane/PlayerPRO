//
//  PPPlugInObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/27/13.
//
//

#import "PPPlugInObject.h"

@implementation PPPlugInObject

@synthesize type;
@synthesize menuName;
@synthesize authorString;
@synthesize file;
@synthesize version;

- (id)init
{
	[self doesNotRecognizeSelector:_cmd];
	AUTORELEASEOBJNORETURN(self);
	return nil;
}

- (id)initWithBundle:(NSBundle *)aBund
{
	if (self = [super init]) {
		{
			NSURL *bundleURL = [aBund bundleURL];
			CFBundleRef cfBundle = CFBundleCreate(kCFAllocatorDefault, BRIDGE(CFURLRef, bundleURL));
			
			version = CFBundleGetVersionNumber(cfBundle);
			CFRelease(cfBundle);
		}
		
		NSMutableDictionary *tempDict = [[aBund infoDictionary] mutableCopy];
		[tempDict addEntriesFromDictionary:[aBund localizedInfoDictionary]];
		id DictionaryTemp = [tempDict valueForKey:BRIDGE(NSString*, kMadPlugMenuNameKey)];
		if ([DictionaryTemp isKindOfClass:[NSString class]]) {
			menuName = [[NSString alloc] initWithString:DictionaryTemp];
		} else {
			RELEASEOBJ(tempDict);
			AUTORELEASEOBJNORETURN(self);
			return nil;
		}
		DictionaryTemp = [tempDict valueForKey:BRIDGE(NSString*, kMadPlugAuthorNameKey)];
		if (DictionaryTemp) {
			if ([DictionaryTemp isKindOfClass:[NSString class]]) {
				authorString = [[NSString alloc] initWithString:DictionaryTemp];
			} else {
				authorString = [NSLocalizedString(@"No Author", @"no author") copy];
			}
		} else {
			authorString = [NSLocalizedString(@"No Author", @"no author") copy];
		}
		
		RELEASEOBJ(tempDict);
		
		file = RETAINOBJ(aBund);
	}
	return self;
}

- (NSString *)description
{
	char typ[5];
	OSType2Ptr(type, typ);
	return [NSString stringWithFormat:@"%@ - type \"%@\" Location: %@", menuName, [NSString stringWithCString:typ encoding:NSMacOSRomanStringEncoding], [file bundlePath]];
}

#if !__has_feature(objc_arc)
- (void)dealloc
{
	[authorString release];
	[file release];
	[menuName release];
	
	[super dealloc];
}
#endif


@end
