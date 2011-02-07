#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <PlayerPROCore/PlayerPROCore.h>

/* -----------------------------------------------------------------------------
   Step 1
   Set the UTI types the importer supports
  
   Modify the CFBundleDocumentTypes entry in Info.plist to contain
   an array of Uniform Type Identifiers (UTI) for the LSItemContentTypes 
   that your importer can handle
  
   ----------------------------------------------------------------------------- */

/* -----------------------------------------------------------------------------
   Step 2 
   Implement the GetMetadataForFile function
  
   Implement the GetMetadataForFile function below to scrape the relevant
   metadata from your document and return it as a CFDictionary using standard keys
   (defined in MDItem.h) whenever possible.
   ----------------------------------------------------------------------------- */

/* -----------------------------------------------------------------------------
   Step 3 (optional) 
   If you have defined new attributes, update the schema.xml file
  
   Edit the schema.xml file to include the metadata keys that your importer returns.
   Add them to the <allattrs> and <displayattrs> elements.
  
   Add any custom types that your importer requires to the <attributes> element
  
   <attribute name="com_mycompany_metadatakey" type="CFString" multivalued="true"/>
  
   ----------------------------------------------------------------------------- */



/* -----------------------------------------------------------------------------
    Get metadata attributes from file
   
   This function's job is to extract useful information your file format supports
   and return it as a dictionary
   ----------------------------------------------------------------------------- */

Boolean GetMetadataForFile(void* thisInterface, 
			   CFMutableDictionaryRef attributes, 
			   CFStringRef contentTypeUTI,
			   CFStringRef pathToFile)
{
    /* Pull any available metadata from the file at the specified path */
    /* Return the attribute keys and attribute values in the dict */
    /* Return TRUE if successful, FALSE if there was no data provided */
    
    //#warning To complete your importer please implement the function GetMetadataForFile in GetMetadataForFile.c
	MADDriverRec			*MADDriver;
	MADMusic				*MADMusic1;
	MADLibrary				*MADLib;
	MADDriverSettings		init;
	
	MADGetBestDriver(&init);
	init.driverMode = NoHardwareDriver;
	if(MADInitLibrary(NULL, FALSE, &MADLib) != noErr) return FALSE;
	if( MADCreateDriver( &init, MADLib, &MADDriver) != noErr) 
	{
		MADDisposeLibrary(MADLib);
		return FALSE;
	}
	{
		char		type[ 5];
		OSType		info;
		CFStringRef ostypes;
		
		ostypes = UTTypeCopyPreferredTagWithClass(contentTypeUTI, kUTTagClassOSType);
		
		info = UTGetOSTypeFromString(ostypes);
		if (!info) goto fail1;
		
		
		OSType2Ptr( info, type);
		
		if( MADPlugAvailable( MADLib, type))		// Is available a plug to open this file?
		{
			CFURLRef tempRef = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, pathToFile, kCFURLPOSIXPathStyle, FALSE);
			MADLoadMusicCFURLFile(MADLib, &MADMusic1, info, tempRef);
			CFRelease(tempRef);
			
		} else goto fail1;
	}
	
	MADStopDriver(MADDriver);				// Stop driver interrupt function
	MADDisposeDriver(MADDriver);			// Dispose music driver
	MADDisposeLibrary(MADLib);				// Close music library
	return TRUE;
	
fail1:
	MADStopDriver(MADDriver);				// Stop driver interrupt function
	MADDisposeDriver(MADDriver);			// Dispose music driver
	MADDisposeLibrary(MADLib);				// Close music library
	
    return FALSE;
}
