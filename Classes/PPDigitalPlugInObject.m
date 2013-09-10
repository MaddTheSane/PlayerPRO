//
//  PPDigitalPlugInObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/1/13.
//
//

#import "PPDigitalPlugInObject.h"
#import "PPPlugInCommon.h"

@implementation PPDigitalPlugInObject

- (BOOL)isEqual:(id)object
{
	if (!object) {
		return NO;
	}
	if (self == object) {
		return YES;
	}
	if ([object isKindOfClass:[PPDigitalPlugInObject class]]) {
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

#define PPDGLoadPlug(theBundle) (PPDigitalPlugin**)GetCOMPlugInterface(theBundle, kPlayerPRODigitalPlugTypeID, kPlayerPRODigitalPlugInterfaceID)

- (id)initWithBundle:(NSBundle*)toInit
{
	if (self = [super initWithBundle:toInit]) {
		NSURL *bundleURL = [toInit bundleURL];
		CFBundleRef tempBundle = CFBundleCreate(kCFAllocatorDefault, (__bridge CFURLRef) bundleURL);
		
		plugCode = PPDGLoadPlug(tempBundle);
		CFRelease(tempBundle);

		if (!plugCode) {
			return nil;
		}
		
		type = 'PPDG';
	}
	return self;
}

- (OSErr)callWithPcmd:(Pcmd*)myPcmd
{
	PPInfoPlug theInfo = {0};
	
	return [self callWithPcmd:myPcmd plugInfo:&theInfo];
}

- (OSErr)callWithPcmd:(Pcmd*)myPcmd plugInfo:(PPInfoPlug*)pi
{
	NSURL *tempURL = [file bundleURL];
	CFBundleRef tempBundle = CFBundleCreate(kCFAllocatorDefault, (__bridge CFURLRef) tempURL);
	
	CFBundleRefNum resFileNum = CFBundleOpenBundleResourceMap(tempBundle);
	
	OSErr iErr = (*plugCode)->MyProcPtr(plugCode, myPcmd, pi);
	
	CFBundleCloseBundleResourceMap(tempBundle, resFileNum);
	CFRelease(tempBundle);
	
	return iErr;
}

- (void)dealloc
{
	if (plugCode) {
		(*plugCode)->Release(plugCode);
	}
}

@end
