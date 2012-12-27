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
