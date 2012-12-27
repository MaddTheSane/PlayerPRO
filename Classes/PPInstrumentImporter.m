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

void **GetCOMPlugInterface(CFPlugInRef plugToTest, CFUUIDRef TypeUUID, CFUUIDRef InterfaceUUID)
{
	CFArrayRef	factories = NULL;
	Boolean		foundInterface = FALSE;
	void		**formatPlugA = NULL;
	
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
		xxxx = PPINLoadPlug((__bridge CFPlugInRef)(tempBundle));
		
		if (!xxxx) {
			AUTORELEASEOBJNORETURN(self);
			return nil;
		}
		
		//TODO: Cocoa function of this?
		version = CFBundleGetVersionNumber((__bridge CFBundleRef)(tempBundle));
		
		NSDictionary *tempDict = [tempBundle localizedInfoDictionary];
		id DictionaryTemp = [tempDict valueForKey:(__bridge NSString *)(kMadPlugMenuNameKey)];
		if ([DictionaryTemp isKindOfClass:[NSString class]]) {
			menuName = [DictionaryTemp copy];
		} else {
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
			UTITypes = RETAINOBJ(DictionaryTemp);
		} else if ([DictionaryTemp isKindOfClass:[NSString class]]) {
			NSString *tempStr = [NSString stringWithString:DictionaryTemp];
			UTITypes = [[NSArray alloc] initWithObjects:tempStr, nil];
		} else {
			AUTORELEASEOBJNORETURN(self);
			return nil;
		}
		
		DictionaryTemp = [tempDict valueForKey:kMadPlugIsSampleKey];
		if ([DictionaryTemp isKindOfClass:[NSNumber class]]) {
			isSamp = [(NSNumber*)DictionaryTemp boolValue];
		} else if ([DictionaryTemp isKindOfClass:[NSString class]]){
			isSamp = [(NSString*)DictionaryTemp boolValue];
		} else {
			AUTORELEASEOBJNORETURN(self);
			return nil;
		}
		
		DictionaryTemp = [tempDict valueForKey:(__bridge NSString *)(kMadPlugTypeKey)];
		if ([DictionaryTemp isKindOfClass:[NSString class]]) {
			NSStringToOSType(DictionaryTemp, &type);
		} else if([DictionaryTemp isKindOfClass:[NSNumber class]]) {
			type = [(NSNumber*)DictionaryTemp unsignedIntValue];
		} else {
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
					AUTORELEASEOBJNORETURN(self);
					return nil;
				}
			}
		}

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
	CFBundleRefNum fileID = CFBundleOpenBundleResourceMap( (__bridge CFBundleRef)(file));

	OSErr returnType = (*xxxx)->InstrMain(imporexp,insData,sdataref,insSamp,(__bridge CFURLRef)(fileToImport),plugInfo);
	
	CFBundleCloseBundleResourceMap((__bridge CFBundleRef)(file), fileID);
	
	return returnType;
}

- (OSErr)importInstrument:(NSURL *)fileToImport instrumentDataReference:(InstrData*)insData sampleDataReference:(sData**)sdataref instrumentSample:(short*)insSamp function:(OSType)imporexp
{
	return [self importInstrument:fileToImport instrumentDataReference:insData sampleDataReference:sdataref instrumentSample:insSamp function:imporexp plugInfo:NULL];
}

@end

@implementation PPInstrumentImporter
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
				*theType = MADPlugSampleImporter;
			} else {
				*theType = MADPlugInstrumentImporter;
			}
			return YES;
		}
	}
	*theType = MADPlugNonePlug;
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
