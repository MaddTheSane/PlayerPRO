//
//  SDL-Audio.c
//  PPMacho
//
//  Created by C.W. Betts on 8/11/14.
//
//

#include <stdio.h>
#include <stdbool.h>
#include <dlfcn.h>
#ifdef _MAC_H
#include <CoreFoundation/CoreFoundation.h>
#endif

#include "PPPrivate.h"

#ifndef _SDL
#error This file requires SDL to be enabled via preprocessor macros in order to build!
#elif !defined(SDL_IS_LINKED)
#error SDL_IS_LINKED needs to be defined, even if you are not linking to it.
#warning You can set the value to 0 if you are not linking, and SDL will be loaded at run-time.
#endif


#if SDL_IS_LINKED
#define SDL_OpenAudiop(desired, obtained, userdata) SDL_OpenAudio(desired, obtained)
#define SDL_CloseAudiop(userdata) SDL_CloseAudio()
#define SDL_Initp(flags, userdata) SDL_Init(flags)
#define SDL_InitSubSystemp(flags, userdata) SDL_InitSubSystem(flags)
#define SDL_WasInitp(flags, userdata) SDL_WasInit(flags)
#define SDL_QuitSubSystemp(flags, userdata) SDL_QuitSubSystem(flags)
#define SDL_Quitp(userdata) SDL_Quit()

#else

#define SDL_OpenAudiop(desired, obtained, userdata) ((MADDriverRec*)userdata)->SDLOpenAudio(desired, obtained)
#define SDL_CloseAudiop(userdata) ((MADDriverRec*)userdata)->SDLCloseAudio()
#define SDL_Initp(flags, userdata) ((MADDriverRec*)userdata)->SDLInit(flags)
#define SDL_InitSubSystemp(flags, userdata) ((MADDriverRec*)userdata)->SDLInitSub(flags)
#define SDL_WasInitp(flags, userdata) ((MADDriverRec*)userdata)->SDLWasInit(flags)
#define SDL_QuitSubSystemp(flags, userdata) ((MADDriverRec*)userdata)->SDLQuitSubSystem(flags)
#define SDL_Quitp(userdata) ((MADDriverRec*)userdata)->SDLQuit()

static char *locateSDL2Library(MADDriverRec* MADDriver)
{
	return NULL;
}

static char *locateSDL1Library(MADDriverRec* MADDriver)
{
	return NULL;
}

#endif

static void MADAudioCallback(void *userdata, Uint8 * stream,
							 int len)
{
	SDL_Initp(0, userdata);
}

MADErr initSDL(MADDriverRec *inMADDriver)
{
	return MADSoundManagerErr;
}

MADErr closeSDL(MADDriverRec *inMADDriver)
{
	return MADSoundManagerErr;
}

bool TestSDLAvailability()
{
#if SDL_IS_LINKED
	return true
#else
	char *aPath = locateSDL2Library(NULL);
	if (aPath) {
		free(aPath);
		return true;
	}
	aPath = locateSDL1Library(NULL);
	if (aPath) {
		free(aPath);
		return true;
	}
	
	return false;
#endif
}
