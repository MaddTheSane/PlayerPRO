/*
 *  PPDefs.h
 *  PlayerPROCore
 *
 *  Created by C.W. Betts on 9/7/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *  These are cross-platform defines.
 *
 */

#ifndef __PPDEFS_H
#define __PPDEFS_H

#ifdef __APPLE__
#include <TargetConditionals.h>
#ifndef TARGET_OS_IPHONE
#define TARGET_OS_IPHONE 0
#endif
#include <MacTypes.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#ifndef WIN32
#include <stdbool.h>
#else
#include <Windows.h>
#ifndef __cplusplus
#define inline __inline
#define true TRUE
#define false FALSE
#endif
#define __LITTLE_ENDIAN__ 1
#endif

#if (__cplusplus && __cplusplus >= 201103L && (__has_extension(cxx_strong_enums) || __has_feature(objc_fixed_enum))) || (!__cplusplus && __has_feature(objc_fixed_enum))
#define MADENUM(_type, _name) enum _name : _type _name; enum _name : _type
#if (__cplusplus)
#define MADOPTIONS(_type, _name) _type _name; enum : _type
#else
#define MADOPTIONS(_type, _name) enum _name : _type _name; enum _name : _type
#endif
#else
#define MADENUM(_type, _name) _type _name; enum
#define MADOPTIONS(_type, _name) _type _name; enum
#endif

/********************						***********************/
/*** 			  		  Error messages 						***/
/********************						***********************/
//Thsese had to be moved here to make Swift happy

/*!
 *	@enum MADErr
 *	@abstract error values returned by most PlayerPRO functions
 */
typedef MADENUM(short, MADErr) {
	MADIsRunningSheet				= 10,
	MADNoErr						= 0,
	MADNeedMemory 					= -1,
	MADReadingErr					= -2,
	MADIncompatibleFile				= -3,
	MADLibraryNotInitialized		= -4,
	MADParametersErr				= -5,
	MADUnknowErr					= -6,
	MADSoundManagerErr				= -7,
	MADOrderNotImplemented			= -8,
	MADFileNotSupportedByThisPlug	= -9,
	MADCannotFindPlug				= -10,
	MADMusicHasNoDriver				= -11,
	MADDriverHasNoMusic				= -12,
	MADSoundSystemUnavailable		= -13,
	MADWritingErr					= -14,
	MADUnknownErr					= MADUnknowErr
};

#ifndef __APPLE__
typedef uint8_t			MADByte;
typedef unsigned char	MADBool;
typedef uint32_t		MADFourChar;
typedef unsigned char	MADPStr255[256];
#else
typedef Byte	MADByte;
typedef Boolean	MADBool;
typedef OSType	MADFourChar;
typedef Str255	MADPStr255;
#endif

#endif
