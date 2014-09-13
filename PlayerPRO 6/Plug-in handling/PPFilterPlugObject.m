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

@interface PPFilterPlugObject ()
@property PPFiltersPlugin **plugData;
@end

@implementation PPFilterPlugObject
@synthesize plugData;

- (BOOL)isEqual:(id)object
{
	if (!object) {
		return NO;
	}
	
	if (self == object) {
		return YES;
	}
	
	if ([object isKindOfClass:[PPFilterPlugObject class]]) {
		if (![self.menuName isEqualToString:[object menuName]]) {
			return NO;
		} else if (![self.file isEqual:[object file]]) {
			return NO;
			//Ignore version, authorstring and type
		} else return YES;
	}
	return NO;
}

- (NSUInteger)hash
{
	return [self.menuName hash];
}

- (instancetype)initWithBundle:(NSBundle *)aBund
{
	if (self = [super initWithBundle:aBund]) {
		{
			NSArray *archs = aBund.executableArchitectures;
			BOOL hasArch = NO;
			for (NSNumber *arch in archs) {
				if ([arch isEqual:@(NSBundleExecutableArchitectureX86_64)]) {
					hasArch = YES;
					break;
				}
			}
			
			if (!hasArch) {
				return nil;
			}
		}
		
		NSURL *bundleURL = [aBund bundleURL];
		CFBundleRef cfBundle = CFBundleCreate(kCFAllocatorDefault, (__bridge CFURLRef)bundleURL);
		
		plugData = PPFilterLoadPlug(cfBundle);
		CFRelease(cfBundle);

		if (!plugData)
			return nil;
		
		type = 'PLug';
	}
	return self;
}

- (OSErr)callPluginWithData:(sData *)theData selectionStart:(long) SelectionStart selectionEnd:(long) SelectionEnd plugInInfo:(PPInfoPlug *)thePPInfoPlug stereoMode:(short)stereoMode
{
	CFBundleRef tempBund = CFBundleCreate(kCFAllocatorDefault, (__bridge CFURLRef)[self.file bundleURL]);
	CFBundleRefNum refNum = CFBundleOpenBundleResourceMap(tempBund);
	
	OSErr iErr = (*plugData)->FiltersMain(plugData, theData, SelectionStart, SelectionEnd, thePPInfoPlug, stereoMode);
	
	CFBundleCloseBundleResourceMap(tempBund, refNum);
	CFRelease(tempBund);
	return iErr;
}

- (void)dealloc
{
	if (plugData)
		(*plugData)->Release(plugData);
}

@end
