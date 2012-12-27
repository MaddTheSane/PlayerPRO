//
//  MainDriverCarbon.c
//  PPMacho
//
//  Created by C.W. Betts on 12/23/12.
//
//

#include <PlayerPROCore/RDriverCarbon.h>

#if defined(_MAC_H) && !defined(TARGET_OS_IPHONE)

OSErr MADLoadMusicFSpFile( MADLibrary *lib, MADMusic **music, char *plugType, FSSpecPtr theSpec)
{
#ifdef __LP64__
	return MADOrderNotImplemented;
#else
	if (theSpec == NULL) {
		return MADParametersErr;
	}
	
	FSRef tempRef;
	FSpMakeFSRef(theSpec, &tempRef);
	return MADLoadMusicFSRef(lib, music, plugType, &tempRef);
#endif
}

OSErr MADLoadMusicFSRef ( MADLibrary *lib, MADMusic **music, char *plugType, FSRefPtr theRef)
{
	if (theRef == NULL) {
		return MADParametersErr;
	}
	
	CFURLRef tempURL;
	OSErr returnErr = noErr;
	tempURL = CFURLCreateFromFSRef(kCFAllocatorDefault, theRef);
	returnErr = MADLoadMusicCFURLFile(lib, music, plugType, tempURL);
	CFRelease(tempURL);
	return returnErr;
}

OSErr MADMusicIdentifyFSRef( MADLibrary *lib, char *type, FSRefPtr theRef)
{
	if (theRef == NULL) {
		return MADParametersErr;
	}

	CFURLRef tempURL;
	OSErr returnErr = noErr;
	tempURL = CFURLCreateFromFSRef(kCFAllocatorDefault, theRef);
	returnErr = MADMusicIdentifyCFURL(lib, type, tempURL);
	CFRelease(tempURL);
	return returnErr;
}

OSErr MADMusicIdentifyFSpFile (MADLibrary *lib, char *type, FSSpecPtr theSpec)
{
#ifdef __LP64__
	return MADOrderNotImplemented;
#else
	if (theSpec == NULL) {
		return MADParametersErr;
	}
	
	FSRef tempRef;
	FSpMakeFSRef(theSpec, &tempRef);
	return MADMusicIdentifyFSRef(lib, type, &tempRef);
#endif
}

#endif
