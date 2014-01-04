//
//  PPDigitalPlugInObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/1/13.
//
//

#import "PPDigitalPlugInObject.h"
#import "PPPlugInCommon.h"

#define PPDGLoadPlug(theBundle) (PPDigitalPlugin**)GetCOMPlugInterface(theBundle, kPlayerPRODigitalPlugTypeID, kPlayerPRODigitalPlugInterfaceID)

@interface PPDigitalPlugInObject ()
@property PPDigitalPlugin **plugCode;
@end

@implementation PPDigitalPlugInObject
@synthesize plugCode;

- (BOOL)isEqual:(id)object
{
	if (!object) {
		return NO;
	}
	if (self == object) {
		return YES;
	}
	if ([object isKindOfClass:[PPDigitalPlugInObject class]]) {
		if (![self.menuName isEqualToString:[object menuName]]) {
			return NO;
		} else if (![self.file isEqual:[object file]]) {
			return NO;
			//Ignore version, authorstring and type
		} else
			return YES;
	}
	return NO;
}

- (NSUInteger)hash
{
	return [self.menuName hash];
}

- (instancetype)initWithBundle:(NSBundle*)toInit
{
	if (self = [super initWithBundle:toInit]) {
		NSURL *bundleURL = [toInit bundleURL];
		CFBundleRef tempBundle = CFBundleCreate(kCFAllocatorDefault, (__bridge CFURLRef) bundleURL);
		
		plugCode = PPDGLoadPlug(tempBundle);
		CFRelease(tempBundle);

		if (!plugCode)
			return nil;
		
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
	NSURL *tempURL = [self.file bundleURL];
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
