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
#include <CoreFoundation/CFAvailability.h>
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

#ifdef __APPLE__
#define MADENUM CF_ENUM
#define MADOPTIONS CF_OPTIONS
#else
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
#endif

#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#if defined(NOEXPORTFUNCS) && NOEXPORTFUNCS
#define PPEXPORT extern
#ifdef __cplusplus
#define EXP extern "C"
#else
#define EXP extern
#endif
#else
#define PPEXPORT extern __attribute__((visibility("default")))
#ifdef __cplusplus
#define EXP extern "C" __attribute__((visibility("default")))
#else
#define EXP PPEXPORT
#endif
#endif
#endif

#if !defined(__BIG_ENDIAN__) && defined(WORDS_BIGENDIAN)
#define __BIG_ENDIAN__ 1
#endif

//////////////////////////////////////////////////////////////////////
#if defined(__APPLE__)			// MACINTOSH
#define _MAC_H

//////////////////////////////////////////////////////////////////////
#else 			// WIN32 - 95/NT

#ifndef DEPRECATED_ATTRIBUTE
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#define DEPRECATED_ATTRIBUTE __attribute__((deprecated))
#else
#define DEPRECATED_ATTRIBUTE
#endif
#endif

#ifdef LINUX
#define __UNIX__ 1
#endif

#endif

#ifdef WIN32
#if defined(NOEXPORTFUNCS) && NOEXPORTFUNCS
#define PPEXPORT extern
#ifdef __cplusplus
#define EXP extern "C"
#else
#define EXP extern
#endif
#else
#ifdef __cplusplus
#define EXP extern "C" __declspec(dllexport)
#else
#define EXP extern __declspec(dllexport)
#endif
#ifdef BUILDINGPPRO
#define PPEXPORT extern __declspec(dllexport)
#else
#define PPEXPORT extern __declspec(dllimport)
#ifdef _MSC_VER
#pragma comment(lib, "PlayerPROCore.lib")
#endif
#endif
#endif
#endif

//Final checks
#ifndef EXP
#define PPEXPORT extern
#ifdef __cplusplus
#define EXP extern "C"
#else
#define EXP PPEXPORT
#endif
#endif

#if !defined(_MAC_H)

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif


/********************						***********************/
/*** 			  		  Error messages 						***/
/********************						***********************/

/*!
 *	@enum		MADErr
 *	@abstract	error values returned by most PlayerPRO functions
 *	@constant	MADNoErr
 *				No error was encountered
 *	@constant	MADNeedMemory
 *				There isn't enough memory to execute the command.
 *	@constant	MADReadingErr
 *				An error occured when trying to reading a file
 *	@constant	MADIncompatibleFile
 *				The file isn't compatible with PlayerPROCore
 *	@constant	MADLibraryNotInitialized
 *				A library function was called without an initialized library
 *	@constant	MADParametersErr
 *				Bad parapaters were sent to a function.
 *	@constant	MADUnknownErr
 *				An unknown error occured
 *	@constant	MADSoundManagerErr
 *				An error occured when trying to initialize the sound system
 *	@constant	MADOrderNotImplemented
 *				the plug-in doesn't implement the order specified.
 *	@constant	MADFileNotSupportedByThisPlug
 *				The file that a plug-in attempted to load was incompatible
 *				with said plug-in
 *	@constant	MADCannotFindPlug
 *				PlayerPRO couldn't find the plug-in specified.
 *	@constant	MADMusicHasNoDriver
 *				Attempted to use a music function that wasn't attached to a driver.
 *	@constant	MADDriverHasNoMusic
 *				Attempted to use a driver function that requires the use of a loaded music file
 *	@constant	MADSoundSystemUnavailable
 *				The sound system requested isn't available for the current architecture
 *	@constant	MADWritingErr
 *				An error occured when trying to write to a file
 *	@constant	MADUserCanceledErr
 *				The user cancelled an action. This shouldn't be seen by the user.
 */
typedef MADENUM(short, MADErr) {
	MADNoErr						= 0,
	MADNeedMemory 					= -1,
	MADReadingErr					= -2,
	MADIncompatibleFile				= -3,
	MADLibraryNotInitialized		= -4,
	MADParametersErr				= -5,
	MADUnknownErr					= -6,
	MADSoundManagerErr				= -7,
	MADOrderNotImplemented			= -8,
	MADFileNotSupportedByThisPlug	= -9,
	MADCannotFindPlug				= -10,
	MADMusicHasNoDriver				= -11,
	MADDriverHasNoMusic				= -12,
	MADSoundSystemUnavailable		= -13,
	MADWritingErr					= -14,
	MADUserCanceledErr				= -15
};

#ifndef __APPLE__
typedef uint8_t			MADByte;
typedef uint32_t		MADFourChar;
typedef unsigned char	MADPStr255[256];
#else
typedef Byte	MADByte;
typedef OSType	MADFourChar;
typedef Str255	MADPStr255;
#endif

#endif
