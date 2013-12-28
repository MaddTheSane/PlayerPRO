/********************						***********************/
//
//	Player PRO 5.0 - DRIVER SOURCE CODE -
//
//	Library Version 5.0
//
//	To use with MAD Library for Mac: Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	16 Tranchees
//	1206 GENEVA
//	SWITZERLAND
//
//	COPYRIGHT ANTOINE ROSSET 1996, 1997, 1998
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:				(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 	RossetAntoine@bluewin.ch
//
/********************						***********************/

#ifndef __PPC_FILEUTILS_H__
#define __PPC_FILEUTILS_H__

#ifndef __MADI__
#include "MAD.h"
#endif

#ifdef _MAC_H
#include <CoreFoundation/CFByteOrder.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef FILE* UNFILE;

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////
	
PPEXPORT UNFILE	iFileOpen(const char *name) DEPRECATED_ATTRIBUTE;
PPEXPORT UNFILE	iFileOpenRead(const char *name) DEPRECATED_ATTRIBUTE;
PPEXPORT UNFILE	iFileOpenWrite(const char *name) DEPRECATED_ATTRIBUTE;
PPEXPORT void	iFileCreate(const char *path, OSType type) DEPRECATED_ATTRIBUTE;

PPEXPORT long	iGetEOF(UNFILE iFileRefI) DEPRECATED_ATTRIBUTE;
PPEXPORT OSErr	iRead(long size, void *dest, UNFILE iFileRefI) DEPRECATED_ATTRIBUTE;
PPEXPORT OSErr	iWrite(long size, const void *src, UNFILE iFileRefI) DEPRECATED_ATTRIBUTE;
PPEXPORT OSErr	iSeekCur(long size, UNFILE iFileRefI) DEPRECATED_ATTRIBUTE;

PPEXPORT void	iClose(UNFILE iFileRefI) DEPRECATED_ATTRIBUTE;
	
////////////////////////////////////////////////////////////

//TODO: use system-based functions, such as the ones used on OS X/iOS
static inline void MADByteSwap32(void *msg_buf)
{
	uint32_t temp = *((uint32_t*)msg_buf);
#ifdef _MAC_H
	*((uint32_t*)msg_buf) = CFSwapInt32(temp);
#else
	*((uint32_t*)msg_buf) = ((((temp & 0xff000000) >> 24) | \
	((temp & 0x00ff0000) >> 8) | ((temp & 0x0000ff00) << 8) | \
	(temp & 0x000000ff) << 24));
#endif
}

//TODO: use system-based functions, such as the ones used on OS X/iOS
static inline void MADByteSwap16(void *msg_buf)
{
	uint16_t buf = *((uint16_t*)msg_buf);
#ifdef _MAC_H
	*((uint16_t*)msg_buf) = CFSwapInt16(buf);
#else
	*((uint16_t*)msg_buf) = ((((buf) << 8) & 0xFF00) | (((buf) >> 8) & 0x00FF));
#endif
}

/////////////////////////////////

#ifdef __LITTLE_ENDIAN__
static inline void PPBE32(void *msg_buf)
{
	MADByteSwap32(msg_buf);
}

static inline void PPBE16(void *msg_buf)
{
	MADByteSwap16(msg_buf);
}
#else
#define PPBE32(msg_buf)
#define PPBE16(msg_buf)
#endif

/////////////////////////////////

#ifdef __BIG_ENDIAN__
static inline void PPLE32(void *msg_buf)
{
	MADByteSwap32(msg_buf);
}

static inline void PPLE16(void *msg_buf)
{
	MADByteSwap16(msg_buf);
}
#else
#define PPLE32(msg_buf)
#define PPLE16(msg_buf)
#endif

/////////////////////////////////

static inline void OSType2Ptr(OSType type, char *str)
{
	PPBE32(&type);
	memcpy(str, &type, 4);
	str[ 4] = 0;
}

static inline OSType Ptr2OSType(const char *str)
{
	short	i;
	OSType	type = '    ';
	
	i = strlen(str);
	if (i > 4)
		i = 4;
	memcpy(&type, str, i);
	PPBE32(&type);
	
	return type;
}

#ifdef __cplusplus
}
#endif

#endif
