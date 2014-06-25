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

typedef short			MADErr;
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
