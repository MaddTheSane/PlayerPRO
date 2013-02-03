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
#if TARGET_OS_IPHONE
#include <MacTypes.h>
#else
//For MacTypes.h on older OSes
#include <CoreServices/CoreServices.h>
#endif
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

#ifndef __MACTYPES__
#define __MACTYPES__
typedef unsigned char 	Byte;
typedef char 			*Ptr, **Handle;
typedef unsigned char 	Boolean;
typedef unsigned char 	Str255[256], Str63[64];
typedef uint8_t			UInt8;
typedef int8_t			SInt8;
typedef uint16_t		UInt16;
typedef int16_t			SInt16;
typedef short			OSErr;
typedef uint32_t		UInt32;
typedef int32_t			SInt32;
typedef UInt32			FourCharCode;
typedef FourCharCode	OSType;
typedef SInt32			Fixed;
typedef UInt32			UnsignedFixed;
#endif

#endif
