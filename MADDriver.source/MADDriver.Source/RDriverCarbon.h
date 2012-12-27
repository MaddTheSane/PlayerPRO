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

OSErr MADLoadMusicFSpFile( MADLibrary *lib, MADMusic **music, char *plugType, FSSpecPtr theSpec) DEPRECATED_ATTRIBUTE;
OSErr MADLoadMusicFSRef ( MADLibrary *lib, MADMusic **music, char *plugType, FSRefPtr theRef) DEPRECATED_ATTRIBUTE;

OSErr MADMusicIdentifyFSRef( MADLibrary *lib, char *type, FSRefPtr theRef) DEPRECATED_ATTRIBUTE;
OSErr MADMusicIdentifyFSpFile (MADLibrary *lib, char *type, FSSpecPtr theSpec) DEPRECATED_ATTRIBUTE;

#endif
#endif
