//
//  MainDriverCarbon.c
//  PPMacho
//
//  Created by C.W. Betts on 12/23/12.
//
//

#include <PlayerPROCore/RDriverCarbon.h>


OSErr MADLoadMusicFSpFile( MADLibrary *lib, MADMusic **music, char *plugType, FSSpecPtr theSpec)
{
#ifdef __LP64__
	return MADOrderNotImplemented;
#else
	FSRef tempRef;
	FSpMakeFSRef(theSpec, &tempRef);
	return MADLoadMusicFSRef(lib, music, plugType, &tempRef);
#endif
}

OSErr MADLoadMusicFSRef ( MADLibrary *lib, MADMusic **music, char *plugType, FSRefPtr theRef)
{
	CFURLRef tempURL;
	OSErr returnErr = noErr;
	tempURL = CFURLCreateFromFSRef(kCFAllocatorDefault, theRef);
	returnErr = MADLoadMusicCFURLFile(lib, music, plugType, tempURL);
	CFRelease(tempURL);
	return returnErr;
}

OSErr MADMusicIdentifyFSRef( MADLibrary *lib, char *type, FSRefPtr theRef)
{
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
	FSRef tempRef;
	FSpMakeFSRef(theSpec, &tempRef);
	return MADMusicIdentifyFSRef(lib, type, &tempRef);
#endif
}
