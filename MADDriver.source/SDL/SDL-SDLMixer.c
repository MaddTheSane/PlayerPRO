//
//  SDL-SDLMixer.c
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

#ifndef _SDL_MIXER
#error This file requires SDL Mixer to be enabled via preprocessor macros in order to build!
#elif !defined(SDL_MIXER_IS_LINKED)
#error SDL_IS_LINKED needs to be defined, even if you are not linking to it.
#warning You can set the value to 0 if you are not linking, and SDL will be loaded at run-time.
#endif

#if SDL_MIXER_IS_LINKED

#else

static char *locateSDL2Library(MADDriverRec* MADDriver)
{
	return NULL;
}

static char *locateSDL1Library(MADDriverRec* MADDriver)
{
	return NULL;
}

#endif

MADErr initSDLMixer(MADDriverRec *inMADDriver)
{
	return MADSoundManagerErr;
}

MADErr closeSDLMixer(MADDriverRec *inMADDriver)
{
	return MADSoundManagerErr;
}


bool TestSDLMixerAvailability()
{
#if SDL_MIXER_IS_LINKED
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
