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

#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef FILE*		UNFILE;
	
PPEXPORT UNFILE iFileOpen( Ptr name) DEPRECATED_ATTRIBUTE;
PPEXPORT UNFILE iFileOpenRead( Ptr name);
PPEXPORT UNFILE iFileOpenWrite( Ptr name);
PPEXPORT void iFileCreate(Ptr path, OSType type);

PPEXPORT long iGetEOF( UNFILE iFileRefI);
PPEXPORT OSErr iRead( long size, Ptr dest, UNFILE iFileRefI);
PPEXPORT OSErr iWrite( long size, Ptr dest, UNFILE iFileRefI);
PPEXPORT OSErr iSeekCur( long size, UNFILE iFileRefI);

PPEXPORT void iClose( UNFILE iFileRefI);
	
PPEXPORT OSType Ptr2OSType( Ptr str);
PPEXPORT void OSType2Ptr( OSType type, Ptr str);


////////////////////////////////////////////////////////////

#ifdef NOINLINE
PPEXPORT void PPLE32( void *msg_buf);
PPEXPORT void PPLE16( void *msg_buf);
PPEXPORT void PPBE32( void *msg_buf);
PPEXPORT void PPBE16( void *msg_buf);
#else

static inline void MADByteSwap32(void *msg_buf)
{
	UInt32			temp = *((UInt32*) msg_buf);
#ifdef _MAC_H
	*((UInt32*) msg_buf) = CFSwapInt32(temp);
#else
	*((UInt32*) msg_buf) = ((((temp & 0xff000000) >> 24) | \
	(( temp & 0x00ff0000) >> 8) | (( temp & 0x0000ff00) << 8) | \
	(temp & 0x000000ff) << 24));
#endif
}

static inline void MADByteSwap16(void *msg_buf)
{
	UInt16			buf = *((UInt16*) msg_buf);
#ifdef _MAC_H
	*((UInt16*) msg_buf) = CFSwapInt16(buf);
#else
	*((UInt16*) msg_buf) = (((((UInt16)buf)<<8) & 0xFF00) | ((((UInt16)buf)>>8) & 0x00FF));
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
#endif

#ifdef __cplusplus
}
#endif

#endif
