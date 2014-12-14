//
//  PPPlugInObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/27/13.
//
//

#import "PPPlugInObject.h"
#import "PPPlugInCommon.h"

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
		if (theRec->base.chan[channel].samplePtr != NULL) {
			theRec->base.chan[channel].curPtr 	= theRec->base.chan[channel].maxPtr;
			theRec->base.chan[channel].samplePtr	= NULL;
			theRec->base.chan[channel].lAC		= 0;
			theRec->base.chan[channel].loopBeg	= 0;
			theRec->base.chan[channel].loopSize 	= 0;
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

- (instancetype)init
{
	[self doesNotRecognizeSelector:_cmd];
	return nil;
}

- (NSUInteger)hash
{
	return self.menuName.hash ^ self.authorString.hash ^ self.file.hash ^ type ^ version;
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
		self.type = '    ';
	}
	return self;
}

- (NSString *)description
{
	NSString *typeString = CFBridgingRelease(UTCreateStringForOSType(type));
	return [NSString stringWithFormat:@"%@ - type \"%@\" Location: %@", _menuName, typeString, [_file bundlePath]];
}

@end
