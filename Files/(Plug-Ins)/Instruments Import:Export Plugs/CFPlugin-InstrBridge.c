/*
 *  CFPlugin-InstrBridge.c
 *  PPMacho
 *
 *  Created by C.W. Betts on 12/19/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

//This file is to be included at the end of a digital plugin.
//This is a bridge for old-style PlayerPRO Digital plugins.
//the following needs to be defined before this file is included:
/*
 #define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorDefault, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46) //The UUID of the plugin.  MUST match the one in the Info.plist file
 #define PLUGINFACTORY SixSixNineFactory //The factory name as defined in the Info.plist file
 #define PLUGMAIN main669 //The old main function, renamed please
 */

typedef struct _CFInstrPlugType {
	PPInstrumentPlugin	*_PPROCFPlugFormat;
	CFUUIDRef			_factoryID;
	UInt32				_refCount;
} CFInstrPlugType;

static void _deallocCFInstrPlugType( CFInstrPlugType *myInstance );


static HRESULT CFInstrPlugQueryInterface( void *myInstance, REFIID iid, LPVOID *ppv )
{
    //  Create a CoreFoundation UUIDRef for the requested interface.
	
    CFUUIDRef interfaceID = CFUUIDCreateFromUUIDBytes( kCFAllocatorDefault, iid );
	
    // Test the requested ID against the valid interfaces.
	
    if( CFEqual( interfaceID, kPlayerPROInstrumentPlugInterfaceID ) ) 
	{
		
        //  If the TestInterface was requested, bump the ref count, set the ppv parameter
        //  equal to the instance, and return good status.
		
        ( (CFInstrPlugType *) myInstance )->_PPROCFPlugFormat->AddRef( myInstance );
        *ppv = myInstance;
        CFRelease( interfaceID );
        return S_OK;
    }
    else if( CFEqual( interfaceID, IUnknownUUID ) ) 
	{
		
        //  If the IUnknown interface was requested, same as above.
		
        ( (CFInstrPlugType *) myInstance )->_PPROCFPlugFormat->AddRef( myInstance );
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

static ULONG CFInstrPlugAddRef( void *myInstance )
{
    ( (CFInstrPlugType *) myInstance )->_refCount += 1;
    return ( (CFInstrPlugType *) myInstance )->_refCount;
}

// -------------------------------------------------------------------------------------------
//
//  When an interface is released, decrement the refCount.
//  If the refCount goes to zero, deallocate the instance.
//

static ULONG CFInstrPlugRelease( void *myInstance )
{
    ( (CFInstrPlugType *) myInstance )->_refCount -= 1;
    if ( ( (CFInstrPlugType *) myInstance )->_refCount == 0 ) {
        _deallocCFInstrPlugType( (CFInstrPlugType *) myInstance );
        return 0;
    }
    else
        return ( (CFInstrPlugType *) myInstance )->_refCount;
}

static PPInstrumentPlugin CFInstrPlugFormat =
{
	NULL,
	CFInstrPlugQueryInterface,
	CFInstrPlugAddRef,
	CFInstrPlugRelease,
	PLUGMAIN
};

static CFInstrPlugType *_allocCFInstrPlugType( CFUUIDRef factoryID )
{
    //  Allocate memory for the new instance.
	
    CFInstrPlugType *newOne = (CFInstrPlugType *)malloc( sizeof(CFInstrPlugType) );
	
    //  Point to the function table
	
    newOne->_PPROCFPlugFormat = &CFInstrPlugFormat;
	
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

static void _deallocCFInstrPlugType( CFInstrPlugType *myInstance )
{
    CFUUIDRef factoryID = myInstance->_factoryID;
    free( myInstance );
    if ( factoryID ) {
        CFPlugInRemoveInstanceForFactory( factoryID );
        CFRelease( factoryID );
    }
}

EXP void * PLUGINFACTORY( CFAllocatorRef allocator, CFUUIDRef typeID )
{
	
    //  If correct type is being requested, allocate an instance of TestType and return
    //  the IUnknown interface.
	
    if ( CFEqual( typeID, kPlayerPROInstrumentPlugTypeID ) ) {
        CFInstrPlugType *result = _allocCFInstrPlugType( PLUGUUID );
        return result;
    }
    else {
		
        // If the requested type is incorrect, return NULL.
		
        return NULL;
    }
}

