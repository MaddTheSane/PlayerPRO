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

@synthesize file;
@synthesize authorString;
@synthesize menuName;
@synthesize type;
@synthesize version;

- (BOOL)isEqual:(id)object
{
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


#define PPDGLoadPlug(theBundle) (PPDigitalPlugin**)GetCOMPlugInterface(theBundle, kPlayerPRODigitalPlugTypeID, kPlayerPRODigitalPlugInterfaceID)

- (id)initWithBundle:(NSBundle*)toInit
{
	if (self = [super init]) {
		{
			NSURL *bundleURL = [toInit bundleURL];
			CFBundleRef tempBundle = CFBundleCreate(kCFAllocatorDefault, BRIDGE(CFURLRef, bundleURL));
			
			plugCode = PPDGLoadPlug(tempBundle);
			if (!plugCode) {
				CFRelease(tempBundle);
				AUTORELEASEOBJNORETURN(self);
				return nil;
			}
			
			version = CFBundleGetVersionNumber(tempBundle);
			CFRelease(tempBundle);
		}
		NSMutableDictionary *tempDict = [[toInit infoDictionary] mutableCopy];
		[tempDict addEntriesFromDictionary:[toInit localizedInfoDictionary]];
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
		
		type = 'PPDG';

		RELEASEOBJ(tempDict);
		
		file = RETAINOBJ(toInit);
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
	CFBundleRef tempBundle = CFBundleCreate(kCFAllocatorDefault, BRIDGE(CFURLRef, tempURL));
	
	CFBundleRefNum resFileNum = CFBundleOpenBundleResourceMap(tempBundle);
	
	OSErr iErr = (*plugCode)->MyProcPtr(plugCode, myPcmd, pi);
	
	CFBundleCloseBundleResourceMap(tempBundle, resFileNum);
	CFRelease(tempBundle);
	
	return iErr;
}

- (void)dealloc
{
	RELEASEOBJ(menuName);
	RELEASEOBJ(authorString);
	RELEASEOBJ(file);
	
	if (plugCode) {
		(*plugCode)->Release(plugCode);
	}
	
	SUPERDEALLOC;
}

@end
