//
//  FileUtils.h
//  PPMacho
//
//  Created by C.W. Betts on 1/31/14.
//
//

#ifndef PPMacho_FileUtils_h
#define PPMacho_FileUtils_h

#include <CoreServices/CoreServices.h>

OSType GetOSTypeFromSpecUsingUTI(FSSpec theSpec);
OSType GetOSTypeFromRefUsingUTI(FSRef theSpec);

#endif
