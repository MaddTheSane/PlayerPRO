//
//  GetMetadataForFile.h
//  MDImporterPlugin
//
//  Created by C.W. Betts on 2/22/13.
//
//

#ifndef MDImporterPlugin_GetMetadataForFile_h
#define MDImporterPlugin_GetMetadataForFile_h

#include <CoreFoundation/CFDictionary.h>
#include <CoreFoundation/CFString.h>

// The import function to be implemented in GetMetadataForFile.c
__private_extern__ Boolean GetMetadataForURL(void* thisInterface,
											 CFMutableDictionaryRef attributes,
											 CFStringRef contentTypeUTI,
											 CFURLRef urlForFile);
#endif
