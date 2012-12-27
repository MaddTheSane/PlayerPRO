//
//  PPInstrumentImporter.m
//  PPMacho
//
//  Created by C.W. Betts on 12/26/12.
//
//

#define ISUSINGPPPLUG 1
#import "PPInstrumentImporter.h"
#import "ARCBridge.h"
#import "PPPlugInCommon.h"

NSString * const kMadPlugIsSampleKey = @"MADPlugIsSample";

#define PPINLoadPlug(theBundle) (PPInstrumentPlugin**)GetCOMPlugInterface(theBundle, kPlayerPROInstrumentPlugTypeID, kPlayerPROInstrumentPlugInterfaceID)

void **GetCOMPlugInterface(CFBundleRef tempBundleRef, CFUUIDRef TypeUUID, CFUUIDRef InterfaceUUID)
{
	CFArrayRef	factories = NULL;
	Boolean		foundInterface = FALSE;
	void		**formatPlugA = NULL;
	
	CFPlugInRef plugToTest = CFBundleGetPlugIn(tempBundleRef);
	
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
			CFRelease(factories); factories = NULL;
			return NULL;
		}
	}
	else {
		return NULL;
	}
	CFRelease(factories); factories = NULL;
	
	return formatPlugA;
}

static void NSStringToOSType(NSString *CFstri, OSType *theOSType)
{
	char * thecOSType = (char*)[CFstri cStringUsingEncoding:NSMacOSRomanStringEncoding];
	
	*theOSType = Ptr2OSType(thecOSType);
}

@interface PPInstrumentImporterObject : NSObject
{
	PPInstrumentPlugin	**xxxx;
	NSString			*menuName;
	NSString			*authorString;
	NSArray				*UTITypes;
	NSBundle			*file;
	OSType				type;
	OSType				mode;
	BOOL				isSamp;
	UInt32				version;
}

@property (readonly) NSString *menuName;
@property (readonly) NSString *authorString;
@property (readonly) NSArray *UTITypes;
@property (readonly) NSBundle *file;
@property (readonly) OSType type;
@property (readonly) OSType mode;
@property (readonly) BOOL isSamp;
@property (readonly) UInt32 version;

- (id)initWithBundle:(NSBundle *)theBund;
- (OSErr)importInstrument:(NSURL *)fileToImport instrumentDataReference:(InstrData*)insData sampleDataReference:(sData**)sdataref instrumentSample:(short*)insNum function:(OSType)imporexp;
- (OSErr)importInstrument:(NSURL *)fileToImport instrumentDataReference:(InstrData*)insData sampleDataReference:(sData**)sdataref instrumentSample:(short*)insSamp function:(OSType)imporexp plugInfo:(PPInfoPlug*)plugInfo;

@end

@implementation PPInstrumentImporterObject

@synthesize menuName;
@synthesize authorString;
@synthesize UTITypes;
@synthesize file;
@synthesize type;
@synthesize mode;
@synthesize isSamp;
@synthesize version;

typedef enum _MADPlugCapabilities {
	PPMADCanDoNothing = 0,
	PPMADCanImport = 1,
	PPMADCanExport = 2,
	PPMADCanDoBoth = PPMADCanImport | PPMADCanExport
} MADPlugCapabilities;


- (id)initWithBundle:(NSBundle *)tempBundle
{
	if (self = [super init]) {
		{
			NSURL *tempBundleRef = [tempBundle bundleURL];
			
			CFBundleRef tempCFBundle = CFBundleCreate(kCFAllocatorDefault, (__bridge CFURLRef)(tempBundleRef));
			
			xxxx = PPINLoadPlug(tempCFBundle);
			
			if (!xxxx) {
				CFRelease(tempCFBundle);
				AUTORELEASEOBJNORETURN(self);
				return nil;
			}
			
			//TODO: Cocoa function of this?
			version = CFBundleGetVersionNumber(tempCFBundle);
			CFRelease(tempCFBundle);
			tempCFBundle = NULL;
		}
		
		NSMutableDictionary *tempDict = [[tempBundle infoDictionary] mutableCopy];
		[tempDict addEntriesFromDictionary:[tempBundle localizedInfoDictionary]];
		id DictionaryTemp = [tempDict valueForKey:(__bridge NSString *)(kMadPlugMenuNameKey)];
		if ([DictionaryTemp isKindOfClass:[NSString class]]) {
			menuName = [DictionaryTemp copy];
		} else {
			RELEASEOBJ(tempDict);
			AUTORELEASEOBJNORETURN(self);
			return nil;
		}
		DictionaryTemp = [tempDict valueForKey:(__bridge NSString *)(kMadPlugAuthorNameKey)];
		if ([DictionaryTemp isKindOfClass:[NSString class]]) {
			authorString = [DictionaryTemp copy];
		} else {
			authorString = @"No author";
		}
		
		
		DictionaryTemp = [tempDict valueForKey:(__bridge NSString *)(kMadPlugUTITypesKey)];
		if ([DictionaryTemp isKindOfClass:[NSArray class]]) {
			UTITypes = [[NSArray alloc] initWithArray:DictionaryTemp];
		} else if ([DictionaryTemp isKindOfClass:[NSString class]]) {
			NSString *tempStr = [NSString stringWithString:DictionaryTemp];
			UTITypes = [[NSArray alloc] initWithObjects:tempStr, nil];
		} else {
			RELEASEOBJ(tempDict);
			AUTORELEASEOBJNORETURN(self);
			return nil;
		}
		
		DictionaryTemp = [tempDict valueForKey:kMadPlugIsSampleKey];
		if ([DictionaryTemp isKindOfClass:[NSNumber class]]) {
			isSamp = [(NSNumber*)DictionaryTemp boolValue];
		} else if ([DictionaryTemp isKindOfClass:[NSString class]]){
			isSamp = [(NSString*)DictionaryTemp boolValue];
		} else {
			RELEASEOBJ(tempDict);
			AUTORELEASEOBJNORETURN(self);
			return nil;
		}
		
		DictionaryTemp = [tempDict valueForKey:(__bridge NSString *)(kMadPlugTypeKey)];
		if ([DictionaryTemp isKindOfClass:[NSString class]]) {
			NSStringToOSType(DictionaryTemp, &type);
		} else if([DictionaryTemp isKindOfClass:[NSNumber class]]) {
			type = [(NSNumber*)DictionaryTemp unsignedIntValue];
		} else {
			RELEASEOBJ(tempDict);
			AUTORELEASEOBJNORETURN(self);
			return nil;
		}
		{
			id canImportValue = nil, canExportValue = nil;
			BOOL canImport = NO, canExport = NO;
			canImportValue = [tempDict valueForKey:(__bridge NSString *)(kMadPlugDoesImport)];
			canExportValue = [tempDict valueForKey:(__bridge NSString *)(kMadPlugDoesExport)];
			if (canImportValue || canExportValue) {
				MADPlugCapabilities capabilities = PPMADCanDoNothing;
				if (canImportValue) {
					canImport = [canImportValue boolValue];
				}
				if (canExportValue) {
					canExport = [canExportValue boolValue];
				}
				if (canImport) {
					capabilities = PPMADCanImport;
				}
				if (canExport) {
					capabilities |= PPMADCanExport;
				}
				
				switch (capabilities) {
					case PPMADCanImport:
						mode = MADPlugImport;
						break;
						
					case PPMADCanExport:
						mode = MADPlugExport;
						break;
						
					case PPMADCanDoBoth:
						mode = MADPlugImportExport;
						break;
						
					default:
						RELEASEOBJ(tempDict);
						AUTORELEASEOBJNORETURN(self);
						return nil;
						break;
				}
			} else {
				DictionaryTemp = [tempDict valueForKey:(__bridge NSString *)(kMadPlugModeKey)];
				if ([DictionaryTemp isKindOfClass:[NSString class]]) {
					NSStringToOSType(DictionaryTemp, &mode);
				} else if([DictionaryTemp isKindOfClass:[NSNumber class]]) {
					mode = [(NSNumber*)DictionaryTemp unsignedIntValue];
				} else {
					RELEASEOBJ(tempDict);
					AUTORELEASEOBJNORETURN(self);
					return nil;
				}
			}
		}

		RELEASEOBJ(tempDict);
		file = RETAINOBJ(tempBundle);
	}
	return self;
}

- (void)dealloc
{
	RELEASEOBJ(menuName);
	RELEASEOBJ(authorString);
	RELEASEOBJ(UTITypes);
	RELEASEOBJ(file);
	
	if (xxxx) {
		(*xxxx)->Release(xxxx);
	}
	
	SUPERDEALLOC;
}

- (OSErr)importInstrument:(NSURL *)fileToImport instrumentDataReference:(InstrData*)insData sampleDataReference:(sData**)sdataref instrumentSample:(short*)insSamp function:(OSType)imporexp plugInfo:(PPInfoPlug*)plugInfo
{
	NSURL *bundleURL = [file bundleURL];
	CFBundleRef tempRef = CFBundleCreate(kCFAllocatorDefault, (__bridge CFURLRef)(bundleURL));
	
	CFBundleRefNum fileID = CFBundleOpenBundleResourceMap(tempRef);

	OSErr returnType = (*xxxx)->InstrMain(imporexp,insData,sdataref,insSamp,(__bridge CFURLRef)(fileToImport),plugInfo);
	
	CFBundleCloseBundleResourceMap(tempRef, fileID);
	CFRelease(tempRef);
	
	return returnType;
}

- (OSErr)importInstrument:(NSURL *)fileToImport instrumentDataReference:(InstrData*)insData sampleDataReference:(sData**)sdataref instrumentSample:(short*)insSamp function:(OSType)imporexp
{
	return [self importInstrument:fileToImport instrumentDataReference:insData sampleDataReference:sdataref instrumentSample:insSamp function:imporexp plugInfo:NULL];
}

@end

@implementation PPInstrumentImporter

- (id)initWithMusic:(MADMusic**)theMus
{
	if (self = [super init]) {
		curMusic = theMus;
		instrumentIEArray = [[NSMutableArray alloc] initWithCapacity:20];
		//[NSBundle]
		NSMutableArray *plugLocs = [NSMutableArray arrayWithCapacity:3];
		NSFileManager *fm = [NSFileManager defaultManager];
		
		[plugLocs addObject:[[NSBundle mainBundle] builtInPlugInsURL]];
		
		[plugLocs addObject:[[[fm URLForDirectory:NSApplicationSupportDirectory inDomain:NSLocalDomainMask appropriateForURL:nil create:NO error:NULL] URLByAppendingPathComponent:@"PlayerPRO"] URLByAppendingPathComponent:@"Plugins"]];
		
		//User plugins
		[plugLocs addObject:[[[fm URLForDirectory:NSApplicationSupportDirectory inDomain:NSUserDomainMask appropriateForURL:nil create:NO error:NULL] URLByAppendingPathComponent:@"PlayerPRO"] URLByAppendingPathComponent:@"Plugins"]];
		
		NSInteger PlugLocNums = [plugLocs count], i, x, y;
		
		for (i=0; i < PlugLocNums; i++) {
			CFIndex		PlugNums;
			CFArrayRef	somePlugs;
			NSURL		*aPlugLoc;
			aPlugLoc = [plugLocs objectAtIndex:i];
			somePlugs = CFBundleCreateBundlesFromDirectory(kCFAllocatorDefault, (__bridge CFURLRef)(aPlugLoc), NULL);
			PlugNums = CFArrayGetCount( somePlugs );
			if (PlugNums > 0) {
				for (x = 0; x < PlugNums; x++) {
					CFBundleRef tempBundleRef = (CFBundleRef)CFArrayGetValueAtIndex(somePlugs, x);
					CFURLRef BundleURL = CFBundleCopyBundleURL(tempBundleRef);
					NSBundle *tempBundle = [NSBundle bundleWithURL:CFBridgingRelease(BundleURL)];
					PPInstrumentImporterObject *tempObj = [[PPInstrumentImporterObject alloc] initWithBundle:tempBundle];
					CFRelease(tempBundleRef);
					if (tempObj) {
						for (y = 0; y < [instrumentIEArray count]; y++) {
							PPInstrumentImporterObject *toComp = [instrumentIEArray objectAtIndex:y];
							if (toComp.type == tempObj.type) {
								if (toComp.version < tempObj.version) {
									[instrumentIEArray replaceObjectAtIndex:y withObject:tempObj];
									RELEASEOBJ(tempObj);
									tempObj = nil;
									break;
								}
							}
						}
						if (tempObj) {
							[instrumentIEArray addObject:tempObj];
							RELEASEOBJ(tempObj);
						}
					}
				}
			}
			CFRelease(somePlugs);
		}


	}
	return self;
}

- (NSInteger)plugInCount
{
	return [instrumentIEArray count];
}

- (OSErr)callInstumentPlugIn:(PPInstrumentImporterObject*)thePlug order:(OSType)theOrd instrument:(short)ins sample:(short*)samp URL:(NSURL*)theURL
{
	return [thePlug importInstrument:theURL instrumentDataReference:&(*curMusic)->fid[ ins] sampleDataReference:&(*curMusic)->sample[ (*curMusic)->fid[ ins].firstSample] instrumentSample:samp function:theOrd];
}

- (OSErr)exportInstrumentOfType:(OSType)theType instrument:(short)ins sample:(short*)samp URL:(NSURL*)theURL
{
	for (PPInstrumentImporterObject *obj in instrumentIEArray) {
		if (theType == obj.type) {
			return [self callInstumentPlugIn:obj order:MADPlugExport instrument:ins sample:samp URL:theURL];
		}
	}
	return MADCannotFindPlug;
}

- (OSErr)importInstrumentOfType:(OSType)theType instrument:(short)ins sample:(short*)samp URL:(NSURL*)theURL
{
	for (PPInstrumentImporterObject *obj in instrumentIEArray) {
		if (theType == obj.type) {
			return [self callInstumentPlugIn:obj order:MADPlugImport instrument:ins sample:samp URL:theURL];
		}
	}
	return MADCannotFindPlug;
}

- (OSErr)testInstrumentFile:(NSURL *)toTest type:(OSType)theType
{
	for (PPInstrumentImporterObject *obj in instrumentIEArray) {
		if (theType == obj.type) {
			short temp;
			return [self callInstumentPlugIn:obj order:MADPlugTest instrument:0 sample:&temp URL:toTest];
		}
	}
	return MADCannotFindPlug;
}

- (OSErr)identifyInstrumentFile:(NSURL*)ref type:(OSType*)outType
{
	for (PPInstrumentImporterObject *obj in instrumentIEArray) {
		short temp;
		if ([self callInstumentPlugIn:obj order:MADPlugTest instrument:0 sample:&temp URL:ref] == noErr) {
			if (outType) {
				*outType = obj.type;
			}
			return noErr;
		}
	}
	if (outType) {
		*outType = '!!!!';
	}
	return MADCannotFindPlug;
}

- (BOOL)isPlugAvailable:(OSType)kind type:(OSType*)theType
{
	for (PPInstrumentImporterObject *obj in instrumentIEArray) {
		if (kind == obj.type) {
			if (obj.isSamp) {
				if (theType) {
					*theType = MADPlugSampleImporter;
				}
			} else {
				if (theType) {
					*theType = MADPlugInstrumentImporter;
				}
			}
			return YES;
		}
	}
	if (theType) {
		*theType = MADPlugNonePlug;
	}
	return NO;
}

#if !__has_feature(objc_arc)
- (void)dealloc
{
	[instrumentIEArray release];
	
	[super dealloc];
}
#endif

@end
