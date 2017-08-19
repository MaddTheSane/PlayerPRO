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
#ifndef TARGET_OS_TV
#define TARGET_OS_TV 0
#endif
#include <MacTypes.h>
#include <CoreFoundation/CFAvailability.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#ifdef WIN32
#include <Windows.h>
#ifndef __cplusplus
#define inline __inline
#endif
#define __LITTLE_ENDIAN__ 1
#endif

#if __STDC_VERSION__ < 199901L && __cplusplus < 199711L
#warning C99 minimum is required!
#endif

#ifdef __APPLE__
#define MADENUM CF_ENUM
#define MADOPTIONS CF_OPTIONS
#else
#ifndef __has_feature
#define __has_feature(x) 0
#endif
#ifndef __has_extension
#define __has_extension(x) 0
#endif
#define __MADENUM_GET_MACRO(_1, _2, NAME, ...) NAME
#if (__cplusplus && __cplusplus >= 201103L && (__has_extension(cxx_strong_enums) || __has_feature(objc_fixed_enum))) || (!__cplusplus && __has_feature(objc_fixed_enum))
#define __MADNAMEDENUM(_type, _name)     enum _name : _type _name; enum _name : _type
#define __MADANONENUM(_type)             enum : _type
#if (__cplusplus)
#define MADOPTIONS(_type, _name) _type _name; enum : _type
#else
#define MADOPTIONS(_type, _name) enum _name : _type _name; enum _name : _type
#endif
#else
#define __MADNAMEDENUM(_type, _name) _type _name; enum
#define __MADANONENUM(_type) enum
#define MADOPTIONS(_type, _name) _type _name; enum
#endif
#define MADENUM(...) __MADENUM_GET_MACRO(__VA_ARGS__, __MADNAMEDENUM, __MADANONENUM)(__VA_ARGS__)
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

#if defined(__GNUC__) && (__GNUC__ == 4) && !defined(DEBUG)
#define PPINLINE static __inline__ __attribute__((always_inline))
#elif defined(__GNUC__)
#define PPINLINE static __inline__
#elif defined(__cplusplus)
#define PPINLINE static inline
#elif defined(_MSC_VER)
#define PPINLINE static __inline
#elif TARGET_OS_WIN32
#define PPINLINE static __inline__
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
 *	@abstract	Error values returned by most PlayerPRO functions
 */
typedef MADENUM(short, MADErr) {
	/// No error was encountered
	MADNoErr						= 0,
	/// There isn't enough memory to execute the command.
	MADNeedMemory 					= -1,
	/// An error occured when trying to reading a file.
	MADReadingErr					= -2,
	/// The file isn't compatible with PlayerPROCore.
	MADIncompatibleFile				= -3,
	/// A library function was called without an initialized library.
	MADLibraryNotInitialized		= -4,
	/// Bad paramaters were sent to a function.
	MADParametersErr				= -5,
	/// An unknown error occured.
	MADUnknownErr					= -6,
	/// An error occured when trying to initialize the sound system.
	MADSoundManagerErr				= -7,
	/// The plug-in doesn't implement the order specified.
	MADOrderNotImplemented			= -8,
	/// The file that a plug-in attempted to load was incompatible
	/// with said plug-in
	MADFileNotSupportedByThisPlug	= -9,
	/// PlayerPRO couldn't find the plug-in specified.
	MADCannotFindPlug				= -10,
	/// Attempted to use a music function that wasn't attached to a driver.
	MADMusicHasNoDriver				= -11,
	/// Attempted to use a driver function that requires the use of a loaded music file.
	MADDriverHasNoMusic				= -12,
	/// The sound system requested isn't available for the current architecture.
	MADSoundSystemUnavailable		= -13,
	/// An error occured when trying to write to a file
	MADWritingErr					= -14,
	/// The user cancelled an action. This shouldn't be seen by the user.
	MADUserCanceledErr				= -15,
	
	
	
	/// Use \c MADUnknownErr instead.
	MADUnknowErr					= MADUnknownErr
};

/// A byte, represented as an unsigned number.
typedef uint8_t			MADByte;

///Pascal Strings. Kept for legacy reasons.
typedef unsigned char	MADPStr255[256], MADPStr63[64];
#ifndef __APPLE__
/// A four-character string, in Mac OS Roman encoding.
typedef uint32_t		MADFourChar;
#else
/// A four-character string, in Mac OS Roman encoding.
typedef OSType	MADFourChar;
#endif

/// We're using our own boolean type for fields in files because
/// some archs (specifically ppc-32) have the size of the \c bool being
/// greater than 1.
typedef MADENUM(unsigned char, MADBool) {
	MADFalse = 0,
	MADTrue = 1,
};

#endif
