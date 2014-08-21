//
//  RDriverCarbon.h
//  PPMacho
//
//  Created by C.W. Betts on 12/23/12.
//
//

#ifndef PPMacho_RDriverCarbon_h
#define PPMacho_RDriverCarbon_h
#include <PlayerPROCore/RDriver.h>

#if defined(_MAC_H) && !TARGET_OS_IPHONE

#include <CoreServices/CoreServices.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __LP64__
#define UNAVAILABLE_ON_64BIT UNAVAILABLE_ATTRIBUTE
#else
#define UNAVAILABLE_ON_64BIT
#endif

PPEXPORT MADErr MADLoadMusicFSRef(MADLibrary *lib, MADMusic **music, char *plugType, FSRefPtr theRef) DEPRECATED_ATTRIBUTE;
PPEXPORT MADErr MADLoadMusicFSpFile(MADLibrary *lib, MADMusic **music, char *plugType, FSSpecPtr theSpec) DEPRECATED_ATTRIBUTE UNAVAILABLE_ON_64BIT;

PPEXPORT MADErr MADMusicIdentifyFSRef(MADLibrary *lib, char *type, FSRefPtr theRef) DEPRECATED_ATTRIBUTE;
PPEXPORT MADErr MADMusicIdentifyFSpFile(MADLibrary *lib, char *type, FSSpecPtr theSpec) DEPRECATED_ATTRIBUTE UNAVAILABLE_ON_64BIT;

#undef UNAVAILABLE_ON_64BIT

#ifdef __cplusplus
}
#endif

#endif
#endif
