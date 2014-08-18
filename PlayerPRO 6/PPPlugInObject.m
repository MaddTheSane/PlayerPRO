//
//  PPPlugInObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/27/13.
//
//

#import "PPPlugInObject.h"
#import "PPPlugInCommon.h"
//#include <PlayerPROCore/MADDriver.h>

void **GetCOMPlugInterface(CFBundleRef tempBundleRef, CFUUIDRef TypeUUID, CFUUIDRef InterfaceUUID)
{
	CFArrayRef	factories		= NULL;
	Boolean		foundInterface	= FALSE;
	void		**formatPlugA	= NULL;
	
	CFPlugInRef plugToTest = CFBundleGetPlugIn(tempBundleRef);
	
	if (!plugToTest)
		return NULL;
	
	//  See if this plug-in implements the Test type.
	factories = CFPlugInFindFactoriesForPlugInTypeInPlugIn(TypeUUID, plugToTest);
	
	if (factories != NULL) {
		CFIndex	factoryCount, index;
		
		factoryCount = CFArrayGetCount(factories);
		if (factoryCount > 0) {
			for (index = 0 ; (index < factoryCount) && (foundInterface == false) ; index++) {
				CFUUIDRef factoryID;
				
				//  Get the factory ID for the first location in the array of IDs.
				factoryID = (CFUUIDRef) CFArrayGetValueAtIndex(factories, index);
				if (factoryID) {
					IUnknownVTbl **iunknown = NULL;
					
					//  Use the factory ID to get an IUnknown interface. Here the plug-in code is loaded.
					iunknown = (IUnknownVTbl **)CFPlugInInstanceCreate(kCFAllocatorDefault, factoryID, TypeUUID);
					
					if (iunknown) {
						//  If this is an IUnknown interface, query for the test interface.
						(*iunknown)->QueryInterface(iunknown, CFUUIDGetUUIDBytes(InterfaceUUID), (LPVOID *)(&formatPlugA));
						
						// Now we are done with IUnknown
						(*iunknown)->Release(iunknown);
						
						if (formatPlugA) {
							// We found the interface we need
							foundInterface = true;
						}
					}
				}
			}
		} else {
			//You can ignore the Clang static warning of incorrect decrement here.
			CFRelease(factories);
			return NULL;
		}
	} else
		return NULL;
	
	//You can ignore the Clang static warning of incorrect decrement here.
	CFRelease(factories);
	
	return formatPlugA;
}

NSArray *DefaultPlugInLocations()
{
	static NSArray *immPlugLocs;
	if (immPlugLocs == nil) {
		NSMutableArray *plugLocs = [[NSMutableArray alloc] initWithCapacity:3];
		[plugLocs addObject:[[NSBundle mainBundle] builtInPlugInsURL]];
		// Disregard files in the system domain: We wouldn't install anything there.
		NSArray *fmLocs = [[NSFileManager defaultManager] URLsForDirectory:NSApplicationSupportDirectory inDomains:NSAllDomainsMask & ~NSSystemDomainMask];
		for (NSURL *aURL in fmLocs) {
			[plugLocs addObject:[NSURL fileURLWithPathComponents:@[[aURL path], @"PlayerPRO", @"Plugins"]]];
		}
		
		immPlugLocs = [[NSArray alloc] initWithArray:plugLocs];
	}
	
	return immPlugLocs;
}

OSErr inMADPlaySoundData(MADDriverRec *theRec, Ptr soundPtr, long size, int channel, int note, int amplitude, long loopBeg, long loopSize, unsigned int rate, Boolean stereo)
{
	OSErr iErr = MADPlaySoundData(theRec, soundPtr, size, channel, note, amplitude, 0, 0, rate, stereo);
	Boolean	continueLoop;
	
	if (iErr == MADNoErr) {
		continueLoop = true;
		while (continueLoop) {
			//GetKeys(km);
			
			if (MADDriverChannelIsDonePlaying(theRec, channel))
				continueLoop = false;
			//if (MADIsPressed((unsigned char*) km, 0x37) && MADIsPressed((unsigned char*) km, 0x2F)) continueLoop = false;
			//if (Button()) continueLoop = false;
			//DoGlobalNull();
		}
#if 0
		if (theRec->chan[channel].samplePtr != NULL) {
			theRec->chan[channel].curPtr 	= theRec->chan[channel].maxPtr;
			theRec->chan[channel].samplePtr	= NULL;
			theRec->chan[channel].lAC		= 0;
			theRec->chan[channel].loopBeg	= 0;
			theRec->chan[channel].loopSize 	= 0;
		}
#endif
	}
	
	return iErr;
}

@interface PPPlugInObject ()
@property (readwrite, copy) NSString *menuName;
@property (readwrite, copy) NSString *authorString;
@property (readwrite, strong) NSBundle *file;
@property (readwrite) OSType type;
@property (readwrite) UInt32 version;
@end

@implementation PPPlugInObject
@synthesize type;
@synthesize version;

- (id)init
{
	[self doesNotRecognizeSelector:_cmd];
	return nil;
}

- (instancetype)initWithBundle:(NSBundle *)aBund
{
	if (self = [super init]) {
		CFBundleRef cfBundle = CFBundleCreate(kCFAllocatorDefault, (__bridge CFURLRef)[aBund bundleURL]);
		
		self.version = CFBundleGetVersionNumber(cfBundle);
		CFRelease(cfBundle);
		
		NSMutableDictionary *tempDict = [[aBund infoDictionary] mutableCopy];
		[tempDict addEntriesFromDictionary:[aBund localizedInfoDictionary]];
		id DictionaryTemp = [tempDict valueForKey:(__bridge NSString*)kMadPlugMenuNameKey];
		if ([DictionaryTemp isKindOfClass:[NSString class]]) {
			self.menuName = [DictionaryTemp copy];
		} else
			return nil;
		
		DictionaryTemp = [tempDict valueForKey:(__bridge NSString*)kMadPlugAuthorNameKey];
		if (DictionaryTemp) {
			if ([DictionaryTemp isKindOfClass:[NSString class]]) {
				self.authorString = [DictionaryTemp copy];
			} else {
				self.authorString = NSLocalizedString(@"No Author", @"no author");
			}
		} else {
			self.authorString = NSLocalizedString(@"No Author", @"no author");
		}
		
		self.file = aBund;
	}
	return self;
}

- (NSString *)description
{
	char typ[5];
	OSType2Ptr(type, typ);
	return [NSString stringWithFormat:@"%@ - type \"%@\" Location: %@", _menuName, [NSString stringWithCString:typ encoding:NSMacOSRomanStringEncoding], [_file bundlePath]];
}

@end
