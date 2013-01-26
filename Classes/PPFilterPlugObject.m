//
//  PPFilterPlugObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/22/13.
//
//

#import "PPFilterPlugObject.h"
#import "PPPlugInCommon.h"

#define PPFilterLoadPlug(theBundle) (PPFiltersPlugin**)GetCOMPlugInterface(theBundle, kPlayerPROFiltersPlugTypeID, kPlayerPROFiltersPlugInterfaceID)

@implementation PPFilterPlugObject

@synthesize authorString;
@synthesize file;
@synthesize menuName;
@synthesize type;
@synthesize version;

- (BOOL)isEqual:(id)object
{
	if (self == object) {
		return YES;
	}
	if ([object isKindOfClass:[PPFilterPlugObject class]]) {
		if (![menuName isEqualToString:[object menuName]]) {
			return NO;
		} else if (![file isEqual:[object file]]) {
			return NO;
			//Ignore version, authorstring and type
		} else return YES;
	}
	return NO;
}

- (NSUInteger)hash
{
	return [menuName hash];
}

- (id)initWithBundle:(NSBundle *)aBund
{
	if (self = [super init]) {
		{
			NSURL *bundleURL = [aBund bundleURL];
			CFBundleRef cfBundle = CFBundleCreate(kCFAllocatorDefault, BRIDGE(CFURLRef, bundleURL));
			
			plugData = PPFilterLoadPlug(cfBundle);
			if (!plugData) {
				CFRelease(cfBundle);
				AUTORELEASEOBJNORETURN(self);
				return nil;
			}
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
		
		type = 'PLug';
		
		RELEASEOBJ(tempDict);
		
		file = RETAINOBJ(aBund);

	}
	return self;
}

- (OSErr)callPluginWithData:(sData *)theData selectionStart:(long) SelectionStart selectionEnd:(long) SelectionEnd plugInInfo:(PPInfoPlug *)thePPInfoPlug stereoMode:(short)stereoMode
{
	CFBundleRef tempBund = CFBundleCreate(kCFAllocatorDefault, BRIDGE(CFURLRef,[file bundleURL]));
	CFBundleRefNum refNum = CFBundleOpenBundleResourceMap(tempBund);
	
	OSErr iErr = (*plugData)->FiltersMain(plugData, theData, SelectionStart, SelectionEnd, thePPInfoPlug, stereoMode);
	
	CFBundleCloseBundleResourceMap(tempBund, refNum);
	CFRelease(tempBund);
	return iErr;
}

- (void)dealloc
{
	RELEASEOBJ(authorString);
	RELEASEOBJ(file);
	RELEASEOBJ(menuName);
	if (plugData) {
		(*plugData)->Release(plugData);
	}
	
	SUPERDEALLOC;
}

@end
