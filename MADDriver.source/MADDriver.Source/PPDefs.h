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

#ifdef _MAC_H
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
#define inline __inline
#define true TRUE
#define false FALSE
#define __LITTLE_ENDIAN__
#endif

typedef unsigned char 	Byte;
typedef char 			*Ptr;
typedef Ptr				*Handle;
typedef unsigned char 	Boolean;
typedef unsigned char 	Str255[256], Str63[64];
typedef uint8_t			UInt8;
typedef int8_t			SInt8;
typedef uint16_t		UInt16;
typedef int16_t			SInt16;
typedef short			OSErr;
#ifndef __MACTYPES__
typedef uint32_t		UInt32;
typedef int32_t			SInt32;
#endif
typedef UInt32			FourCharCode;
typedef FourCharCode	OSType;
typedef SInt32			Fixed;
typedef UInt32			UnsignedFixed;

#ifndef WIN32
#ifndef PROJECT_IS_USING_OWN_WIN32_FUNC_NAMES
//Windows Defines
typedef SInt16			WORD;
typedef UInt16			UWORD;
#ifndef _MAC_H
typedef SInt32			HRESULT;
typedef UInt32			ULONG;
typedef void*			LPVOID;
#else
//Apple's CFPlugInCom file defines ULONG.  The Mac port uses CFPluginCom.
//Using this to avoid multiple definitions
#include <CoreFoundation/CFPlugInCOM.h>
#endif
typedef SInt32			LONG;
typedef UInt8			UBYTE;
typedef SInt8			BYTE;
typedef UInt16			UINT;
#if (!defined(BOOL) && !defined(__OBJC__))
//#define BOOL
typedef Boolean			BOOL;
#endif
typedef UInt32			DWORD;
typedef UInt16			USHORT;
typedef SInt16			SHORT;
typedef FourCharCode	FOURCC;

#endif
#endif

#endif
