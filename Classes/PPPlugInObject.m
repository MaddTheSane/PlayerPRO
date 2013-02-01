//
//  PPPlugInObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/27/13.
//
//

#import "PPPlugInObject.h"
#import "PPPlugInCommon.h"
#include <PlayerPROCore/RDriverInt.h>
#import "ARCBridge.h"

void **GetCOMPlugInterface(CFBundleRef tempBundleRef, CFUUIDRef TypeUUID, CFUUIDRef InterfaceUUID)
{
	CFArrayRef	factories = NULL;
	Boolean		foundInterface = FALSE;
	void		**formatPlugA = NULL;
	
	CFPlugInRef plugToTest = CFBundleGetPlugIn(tempBundleRef);
	
	if (!plugToTest) {
		return NULL;
	}
	
	//  See if this plug-in implements the Test type.
	factories	= CFPlugInFindFactoriesForPlugInTypeInPlugIn( TypeUUID, plugToTest );
	
	if ( factories != NULL )
	{
		CFIndex	factoryCount, index;
		
		factoryCount	= CFArrayGetCount( factories );
		if ( factoryCount > 0 )
		{
			for ( index = 0 ; (index < factoryCount) && (foundInterface == false) ; index++ )
			{
				CFUUIDRef	factoryID;
				
				//  Get the factory ID for the first location in the array of IDs.
				factoryID = (CFUUIDRef) CFArrayGetValueAtIndex( factories, index );
				if ( factoryID )
				{
					IUnknownVTbl **iunknown = NULL;
					
					//  Use the factory ID to get an IUnknown interface. Here the plug-in code is loaded.
					iunknown	= (IUnknownVTbl **) CFPlugInInstanceCreate( kCFAllocatorDefault, factoryID, TypeUUID );
					
					if ( iunknown )
					{
						//  If this is an IUnknown interface, query for the test interface.
						(*iunknown)->QueryInterface( iunknown, CFUUIDGetUUIDBytes( InterfaceUUID ), (LPVOID *)( &formatPlugA ) );
						
						// Now we are done with IUnknown
						(*iunknown)->Release( iunknown );
						
						if ( formatPlugA )
						{
							//	We found the interface we need
							foundInterface	= true;
						}
					}
				}
			}
		}
		else {
			//Clang says that we aren't supposed to release, but Apple's sample code does release
			//Trusting the sample code until further notice
			CFRelease(factories); factories = NULL;
			return NULL;
		}
	}
	else {
		return NULL;
	}
	//Clang says that we aren't supposed to release, but Apple's sample code does release
	//Trusting the sample code until further notice
	CFRelease(factories); factories = NULL;
	
	return formatPlugA;
}

NSArray *DefaultPlugInLocations()
{
	static NSArray *immPlugLocs = nil;
	if (immPlugLocs == nil) {
		@autoreleasepool {
			NSMutableArray *plugLocs = [NSMutableArray arrayWithCapacity:3];

			NSFileManager *fm = [NSFileManager defaultManager];
			[plugLocs addObject:[[NSBundle mainBundle] builtInPlugInsURL]];
			
			[plugLocs addObject:[[[fm URLForDirectory:NSApplicationSupportDirectory inDomain:NSLocalDomainMask appropriateForURL:nil create:NO error:NULL] URLByAppendingPathComponent:@"PlayerPRO"] URLByAppendingPathComponent:@"Plugins"]];
			
			//User plugins
			[plugLocs addObject:[[[fm URLForDirectory:NSApplicationSupportDirectory inDomain:NSUserDomainMask appropriateForURL:nil create:NO error:NULL] URLByAppendingPathComponent:@"PlayerPRO"] URLByAppendingPathComponent:@"Plugins"]];
			
			immPlugLocs = [[NSArray alloc] initWithArray:plugLocs];
		}
	}
	
	return immPlugLocs;
}

OSErr inMADPlaySoundData( MADDriverRec *theRec, Ptr soundPtr, long size, SInt32 channel, SInt32 note, SInt32 amplitude, long loopBeg, long loopSize, unsigned int rate, Boolean stereo)
{
	OSErr iErr = MADPlaySoundData( theRec, soundPtr, size, channel, note, amplitude, 0, 0, rate, stereo);
	Boolean	continueLoop;
	
	if( iErr == noErr)
	{
		continueLoop = true;
		while( continueLoop)
		{
			//GetKeys( km);
			
			if( theRec->chan[ channel].samplePtr == NULL) continueLoop = false;
			//if( MADIsPressed( (unsigned char*) km, 0x37) && MADIsPressed( (unsigned char*) km, 0x2F)) continueLoop = false;
			//if( Button()) continueLoop = false;
			//DoGlobalNull();
		}
		
		if( theRec->chan[ channel].samplePtr != NULL)
		{
			theRec->chan[ channel].curPtr 		= theRec->chan[ channel].maxPtr;
			theRec->chan[ channel].samplePtr	= NULL;
			theRec->chan[ channel].lAC			= 0;
			theRec->chan[ channel].loopBeg		= 0;
			theRec->chan[ channel].loopSize 	= 0;
		}
	}
	
	return iErr;
}


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
