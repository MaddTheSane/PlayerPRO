//
//  PPInstrumentImporterObject.m
//  PPMacho
//
//  Created by C.W. Betts on 12/27/12.
//
//

#define ISUSINGPPPLUG 1
#import "PPInstrumentImporterObject.h"
#import "ARCBridge.h"

NSString * const kMadPlugIsSampleKey = @"MADPlugIsSample";

#define PPINLoadPlug(theBundle) (PPInstrumentPlugin**)GetCOMPlugInterface(theBundle, kPlayerPROInstrumentPlugTypeID, kPlayerPROInstrumentPlugInterfaceID)

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

static inline void NSStringToOSType(NSString *CFstri, OSType *theOSType)
{
	char * thecOSType = (char*)[CFstri cStringUsingEncoding:NSMacOSRomanStringEncoding];
	
	*theOSType = Ptr2OSType(thecOSType);
}

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

- (id)init
{
	[self doesNotRecognizeSelector:_cmd];
	return nil;
}

- (NSString*)description
{
	char typeString[5] = {0};
	OSType2Ptr(type, typeString);
	return [NSString stringWithFormat:@"%@ - %@ Sample: %@ Type: %@ UTIs: %@", menuName, [file bundlePath], isSamp ? @"YES": @"NO", [NSString stringWithCString:typeString encoding:NSMacOSRomanStringEncoding] ,[UTITypes description]];
}

- (id)initWithBundle:(NSBundle *)tempBundle
{
	if (self = [super init]) {
		{
			NSURL *tempBundleRef = [tempBundle bundleURL];
			
			CFBundleRef tempCFBundle = CFBundleCreate(kCFAllocatorDefault, BRIDGE(CFURLRef, tempBundleRef));
			
			xxxx = PPINLoadPlug(tempCFBundle);
			
			if (!xxxx) {
				CFRelease(tempCFBundle);
				AUTORELEASEOBJNORETURN(self);
				return nil;
			}
			
			//TODO: Cocoa function of this?
			version = CFBundleGetVersionNumber(tempCFBundle);
			CFRelease(tempCFBundle);
		}
		
		NSMutableDictionary *tempDict = [[tempBundle infoDictionary] mutableCopy];
		[tempDict addEntriesFromDictionary:[tempBundle localizedInfoDictionary]];
		id DictionaryTemp = [tempDict valueForKey:BRIDGE(NSString*, kMadPlugMenuNameKey)];
		if ([DictionaryTemp isKindOfClass:[NSString class]]) {
			menuName = [[NSString alloc] initWithString:DictionaryTemp];
		} else {
			RELEASEOBJ(tempDict);
			AUTORELEASEOBJNORETURN(self);
			return nil;
		}
		DictionaryTemp = [tempDict valueForKey:BRIDGE(NSString*, kMadPlugAuthorNameKey)];
		if ([DictionaryTemp isKindOfClass:[NSString class]]) {
			authorString = [[NSString alloc] initWithString:DictionaryTemp];
		} else {
			authorString = @"No author";
		}
		
		
		DictionaryTemp = [tempDict valueForKey:BRIDGE(NSString*, kMadPlugUTITypesKey)];
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
		
		DictionaryTemp = [tempDict valueForKey:BRIDGE(NSString*, kMadPlugTypeKey)];
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
			canImportValue = [tempDict valueForKey:BRIDGE(NSString*, kMadPlugDoesImport)];
			canExportValue = [tempDict valueForKey:BRIDGE(NSString*, kMadPlugDoesExport)];
			if (canImportValue || canExportValue) {
				MADPlugCapabilities capabilities = PPMADCanDoNothing;
				if (canImportValue) {
					if([canImportValue boolValue])
						capabilities = PPMADCanImport;
				}
				if (canExportValue) {
					if([canExportValue boolValue])
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
				DictionaryTemp = [tempDict valueForKey:BRIDGE(NSString*, kMadPlugModeKey)];
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
	CFBundleRef tempRef = CFBundleCreate(kCFAllocatorDefault, BRIDGE(CFURLRef, bundleURL));
	
	CFBundleRefNum fileID = CFBundleOpenBundleResourceMap(tempRef);
	
	OSErr returnType = (*xxxx)->InstrMain(xxxx, imporexp, insData, sdataref, insSamp, BRIDGE(CFURLRef, fileToImport), plugInfo);
	
	CFBundleCloseBundleResourceMap(tempRef, fileID);
	CFRelease(tempRef);
	
	return returnType;
}

- (OSErr)importInstrument:(NSURL *)fileToImport instrumentDataReference:(InstrData*)insData sampleDataReference:(sData**)sdataref instrumentSample:(short*)insSamp function:(OSType)imporexp
{
	return [self importInstrument:fileToImport instrumentDataReference:insData sampleDataReference:sdataref instrumentSample:insSamp function:imporexp plugInfo:NULL];
}

@end
