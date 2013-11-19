#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFPlugInCOM.h>
#include <CoreServices/CoreServices.h>
#include "GetMetadataForFile.h"

// -----------------------------------------------------------------------------
//	constants
// -----------------------------------------------------------------------------

#define PLUGIN_ID "DF098C69-8463-40CD-BD50-A964BBCEFDFE"

//
// Below is the generic glue code for all plug-ins.
//


// -----------------------------------------------------------------------------
//	typedefs
// -----------------------------------------------------------------------------
			   
// The layout for an instance of MetaDataImporterPlugIn 
typedef struct __MetadataImporterPluginType {
	MDImporterInterfaceStruct *conduitInterface;
	CFUUIDRef                 factoryID;
	UInt32                    refCount;
} MetadataImporterPluginType;

// -----------------------------------------------------------------------------
//	prototypes
// -----------------------------------------------------------------------------
//	Forward declaration for the IUnknown implementation.
//

static MetadataImporterPluginType	*AllocMetadataImporterPluginType(CFUUIDRef inFactoryID);
static void							DeallocMetadataImporterPluginType(MetadataImporterPluginType *thisInstance);
static HRESULT						MetadataImporterQueryInterface(void *thisInstance,REFIID iid,LPVOID *ppv);
extern void							*PPMetadataImporterPluginFactory(CFAllocatorRef allocator,CFUUIDRef typeID) __attribute__((visibility ("default")));
static ULONG						MetadataImporterPluginAddRef(void *thisInstance);
static ULONG						MetadataImporterPluginRelease(void *thisInstance);

static Boolean GetMetadataForFile(void* thisInterface,
								  CFMutableDictionaryRef attributes,
								  CFStringRef contentTypeUTI,
								  CFStringRef pathToFile)
{
	Boolean isGood = FALSE;
	CFURLRef theURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, pathToFile, kCFURLPOSIXPathStyle, UTTypeConformsTo(contentTypeUTI, CFSTR("net.sourceforge.playerpro.mad-bundle")));
	if (theURL) {
		isGood = GetMetadataForURL(thisInterface, attributes, contentTypeUTI, theURL);
		CFRelease(theURL);
	}
	return isGood;
	
}

// -----------------------------------------------------------------------------
//	testInterfaceFtbl	definition
// -----------------------------------------------------------------------------
//	The TestInterface function table.
//

static MDImporterInterfaceStruct testInterfaceFtbl = {
	NULL,
	MetadataImporterQueryInterface,
	MetadataImporterPluginAddRef,
	MetadataImporterPluginRelease,
	GetMetadataForFile
};


// -----------------------------------------------------------------------------
//	AllocMetadataImporterPluginType
// -----------------------------------------------------------------------------
//	Utility function that allocates a new instance.
//      You can do some initial setup for the importer here if you wish
//      like allocating globals etc...
//
static MetadataImporterPluginType *AllocMetadataImporterPluginType(CFUUIDRef inFactoryID)
{
	MetadataImporterPluginType *theNewInstance;
	
	theNewInstance = (MetadataImporterPluginType *)malloc(sizeof(MetadataImporterPluginType));
	memset(theNewInstance, 0, sizeof(MetadataImporterPluginType));
	
	/* Point to the function table */
	theNewInstance->conduitInterface = &testInterfaceFtbl;
	
	/*  Retain and keep an open instance refcount for each factory. */
	theNewInstance->factoryID = CFRetain(inFactoryID);
	CFPlugInAddInstanceForFactory(inFactoryID);
	
	/* This function returns the IUnknown interface so set the refCount to one. */
	theNewInstance->refCount = 1;
	return theNewInstance;
}

// -----------------------------------------------------------------------------
//	DeallocMetadataImporterPluginType
// -----------------------------------------------------------------------------
//	Utility function that deallocates the instance when
//	the refCount goes to zero.
//      In the current implementation importer interfaces are never deallocated
//      but implement this as this might change in the future
//
static void DeallocMetadataImporterPluginType(MetadataImporterPluginType *thisInstance)
{
    CFUUIDRef theFactoryID;

    theFactoryID = thisInstance->factoryID;
    free(thisInstance);
    if (theFactoryID){
        CFPlugInRemoveInstanceForFactory(theFactoryID);
        CFRelease(theFactoryID);
    }
}

// -----------------------------------------------------------------------------
//	MetadataImporterQueryInterface
// -----------------------------------------------------------------------------
//	Implementation of the IUnknown QueryInterface function.
//
static HRESULT MetadataImporterQueryInterface(void *thisInstance, REFIID iid, LPVOID *ppv)
{
    CFUUIDRef interfaceID;
	
    interfaceID = CFUUIDCreateFromUUIDBytes(kCFAllocatorDefault, iid);
	
    if (CFEqual(interfaceID, kMDImporterInterfaceID)){
		/* If the right interface was requested, bump the ref count,
		 * set the ppv parameter equal to the instance, and
		 * return good status.
		 */
        ((MetadataImporterPluginType*)thisInstance)->conduitInterface->AddRef(thisInstance);
        *ppv = thisInstance;
        CFRelease(interfaceID);
        return S_OK;
	} else if (CFEqual(interfaceID, kMDImporterURLInterfaceID)) {
		/* If the right interface was requested, bump the ref count,
		 * set the ppv parameter equal to the instance, and
		 * return good status.
		 */
		((MDImporterURLInterfaceStruct *)((MetadataImporterPluginType *)thisInstance)->conduitInterface)->ImporterImportURLData = GetMetadataForURL;
		((MDImporterURLInterfaceStruct *)((MetadataImporterPluginType*)thisInstance)->conduitInterface)->AddRef(thisInstance);
		*ppv = thisInstance;
		CFRelease(interfaceID);
		return S_OK;
	} else if (CFEqual(interfaceID, IUnknownUUID)){
		/* If the IUnknown interface was requested, same as above. */
		((MetadataImporterPluginType*)thisInstance )->conduitInterface->AddRef(thisInstance);
		*ppv = thisInstance;
		CFRelease(interfaceID);
		return S_OK;
	}else{
		/* Requested interface unknown, bail with error. */
		*ppv = NULL;
		CFRelease(interfaceID);
		return E_NOINTERFACE;
	}
}

// -----------------------------------------------------------------------------
//	MetadataImporterPluginAddRef
// -----------------------------------------------------------------------------
//	Implementation of reference counting for this type. Whenever an interface
//	is requested, bump the refCount for the instance. NOTE: returning the
//	refcount is a convention but is not required so don't rely on it.
//
static ULONG MetadataImporterPluginAddRef(void *thisInstance)
{
    ((MetadataImporterPluginType *)thisInstance )->refCount += 1;
    return ((MetadataImporterPluginType*) thisInstance)->refCount;
}

// -----------------------------------------------------------------------------
// MetadataImporterPluginRelease
// -----------------------------------------------------------------------------
//	When an interface is released, decrement the refCount.
//	If the refCount goes to zero, deallocate the instance.
//
static ULONG MetadataImporterPluginRelease(void *thisInstance)
{
    ((MetadataImporterPluginType*)thisInstance)->refCount -= 1;
    if (((MetadataImporterPluginType*)thisInstance)->refCount == 0){
        DeallocMetadataImporterPluginType((MetadataImporterPluginType*)thisInstance );
        return 0;
    }else{
        return ((MetadataImporterPluginType*) thisInstance )->refCount;
    }
}

// -----------------------------------------------------------------------------
//	PPMetadataImporterPluginFactory
// -----------------------------------------------------------------------------
//	Implementation of the factory function for this type.
//
void *PPMetadataImporterPluginFactory(CFAllocatorRef allocator, CFUUIDRef typeID)
{
	MetadataImporterPluginType	*result;
	CFUUIDRef					uuid;
	
	/* If correct type is being requested, allocate an
	 * instance of TestType and return the IUnknown interface.
	 */
	
	if (CFEqual(typeID, kMDImporterTypeID)){
		uuid = CFUUIDCreateFromString(kCFAllocatorDefault, CFSTR(PLUGIN_ID));
		result = AllocMetadataImporterPluginType(uuid);
		CFRelease(uuid);
		return result;
	}
	/* If the requested type is incorrect, return NULL. */
	return NULL;
}
