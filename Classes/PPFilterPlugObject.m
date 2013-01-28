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
	if (self = [super initWithBundle:aBund]) {
		NSURL *bundleURL = [aBund bundleURL];
		CFBundleRef cfBundle = CFBundleCreate(kCFAllocatorDefault, BRIDGE(CFURLRef, bundleURL));
		
		plugData = PPFilterLoadPlug(cfBundle);
		if (!plugData) {
			CFRelease(cfBundle);
			AUTORELEASEOBJNORETURN(self);
			return nil;
		}
		CFRelease(cfBundle);
		type = 'PLug';
		
		RELEASEOBJ(tempDict);
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
	if (plugData) {
		(*plugData)->Release(plugData);
	}
	
	SUPERDEALLOC;
}

@end
