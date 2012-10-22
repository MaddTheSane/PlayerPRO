/*
 *  CFPlugin-bridge.c
 *  PlayerPROCore
 *
 *  Created by C.W. Betts on 7/23/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

//This file is to be included at the end of an import-export plugin.
//This is a bridge for old-style PlayerPRO import-export plugins.
//the following needs to be defined before this file is included:
/*
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorDefault, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46) //The UUID of the plugin.  MUST match the one in the Info.plist file
#define PLUGINFACTORY SixSixNineFactory //The factory name as defined in the Info.plist file
 */

//Please make sure that <PlayerPROCore/PlayerPROCore.h> is already included

typedef struct _CFImpExpPlugType {
	MADFileFormatPlugin *_PPROCFPlugFormat;
	CFUUIDRef _factoryID;
	UInt32 _refCount;
} CFImpExpPlugType;

static void _deallocCFImpExpPlugType( CFImpExpPlugType *myInstance );


static HRESULT CFImpExpPlugQueryInterface( void *myInstance, REFIID iid, LPVOID *ppv )
{
    //  Create a CoreFoundation UUIDRef for the requested interface.
	
    CFUUIDRef interfaceID = CFUUIDCreateFromUUIDBytes( kCFAllocatorDefault, iid );
	
    // Test the requested ID against the valid interfaces.
	
    if( CFEqual( interfaceID, kPlayerPROModFormatInterfaceID ) ) 
	{
		
        //  If the TestInterface was requested, bump the ref count, set the ppv parameter
        //  equal to the instance, and return good status.
		
        ( (CFImpExpPlugType *) myInstance )->_PPROCFPlugFormat->AddRef( myInstance );
        *ppv = myInstance;
        CFRelease( interfaceID );
        return S_OK;
    }
    else if( CFEqual( interfaceID, IUnknownUUID ) ) 
	{
		
        //  If the IUnknown interface was requested, same as above.
		
        ( (CFImpExpPlugType *) myInstance )->_PPROCFPlugFormat->AddRef( myInstance );
        *ppv = myInstance;
        CFRelease( interfaceID );
        return S_OK;
    }
    else 
	{
		
        //  Requested interface unknown, bail with error.
		
        *ppv = NULL;
        CFRelease( interfaceID );
        return E_NOINTERFACE;
    }
}

static ULONG CFImpExpPlugAddRef( void *myInstance )
{
    ( (CFImpExpPlugType *) myInstance )->_refCount += 1;
    return ( (CFImpExpPlugType *) myInstance )->_refCount;
}

// -------------------------------------------------------------------------------------------
//
//  When an interface is released, decrement the refCount.
//  If the refCount goes to zero, deallocate the instance.
//

static ULONG CFImpExpPlugRelease( void *myInstance )
{
    ( (CFImpExpPlugType *) myInstance )->_refCount -= 1;
    if ( ( (CFImpExpPlugType *) myInstance )->_refCount == 0 ) {
        _deallocCFImpExpPlugType( (CFImpExpPlugType *) myInstance );
        return 0;
    }
    else
        return ( (CFImpExpPlugType *) myInstance )->_refCount;
}

OSErr CFImpExpGetType(PPInfoRec *info, FSRefPtr AlienFile)
{
	
}

static MADFileFormatPlugin CFImpExpPlugFormat =
{
	NULL,
	CFImpExpPlugQueryInterface,
	CFImpExpPlugAddRef,
	CFImpExpPlugRelease,
	PLUGMAIN
};

static CFImpExpPlugType *_allocCFPlugType( CFUUIDRef factoryID )
{
    //  Allocate memory for the new instance.
	
    CFImpExpPlugType *newOne = (CFImpExpPlugType *)malloc( sizeof(CFImpExpPlugType) );
	
    //  Point to the function table
	
    newOne->_PPROCFPlugFormat = &CFImpExpPlugFormat;
	
    //  Retain and keep an open instance refcount for each factory.
	
    if (factoryID) {
        newOne->_factoryID = (CFUUIDRef)CFRetain( factoryID );
        CFPlugInAddInstanceForFactory( factoryID );
    }
	
    //  This function returns the IUnknown interface so set the refCount to one.
	
    newOne->_refCount = 1;
    return newOne;
}

// -------------------------------------------------------------------------------------------
//
//  Utility function that deallocates the instance when the refCount goes to zero.
//

static void _deallocCFImpExpPlugType( CFImpExpPlugType *myInstance )
{
    CFUUIDRef factoryID = myInstance->_factoryID;
    free( myInstance );
    if ( factoryID ) {
        CFPlugInRemoveInstanceForFactory( factoryID );
        CFRelease( factoryID );
    }
}

//Compatibility with newer plug-in architecture
EXP PPImpExpMain(OSType order, Ptr AlienFileName, MADMusic *madFile, PPInfoRec *info, MADDriverSettings *init)
{
	return PLUGMAIN(order, AlienFileName, madFile, info, init);
}

EXP void * PLUGINFACTORY( CFAllocatorRef allocator, CFUUIDRef typeID )
{
	
    //  If correct type is being requested, allocate an instance of TestType and return
    //  the IUnknown interface.
	
    if ( CFEqual( typeID, kPlayerPROModFormatTypeID ) ) {
        CFImpExpPlugType *result = _allocCFPlugType( PLUGUUID );
        return result;
    }
    else {
		
        // If the requested type is incorrect, return NULL.
		
        return NULL;
    }
}

