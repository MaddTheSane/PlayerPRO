/*
 *  ByteSwap.c
 *  PPMacho
 *
 *  Created by C.W. Betts on 11/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 * 
 *  This is in case the developer(s) decided that they didn't want the inline byteswapping functions
 *
 */

#define NOINLINE 1
#include "MAD.h"
#include "FileUtils.h"
#ifdef _MAC_H
#include <CoreFoundation/CFByteOrder.h>
#endif

#ifdef WIN32
#define __inline__ __inline
#endif

static __inline__ void MADByteSwap32(void *theData)
{
	UInt32			temp = *((UInt32*) theData);
#ifdef _MAC_H
	*((UInt32*) theData) = CFSwapInt32(temp);
#else
	*((UInt32*) theData) = ((((temp & 0xff000000) >> 24) | (( temp & 0x00ff0000) >> 8) | (( temp & 0x0000ff00) << 8) | (temp & 0x000000ff) << 24));
#endif
}

static __inline__ void MADByteSwap16(void *theData)
{
	UInt16			buf = *((UInt16*) theData);
#ifdef _MAC_H
	*((UInt16*) theData) = CFSwapInt16(buf);
#else
	*((UInt16*) theData) = (((((UInt16)buf)<<8) & 0xFF00) | ((((UInt16)buf)>>8) & 0x00FF));
#endif
}

void PPBE32(void *msg_buf)
{
#ifdef __LITTLE_ENDIAN__
	MADByteSwap32(msg_buf);
#endif
}

void PPBE16(void *msg_buf)
{
#ifdef __LITTLE_ENDIAN__
	MADByteSwap16(msg_buf);
#endif
}


/////////////////////////////////
void PPLE32(void *msg_buf)
{
#ifdef __BIG_ENDIAN__
	MADByteSwap32(msg_buf);
#endif
}

void PPLE16(void *msg_buf)
{
#ifdef __BIG_ENDIAN__
	MADByteSwap16(msg_buf);
#endif
}

void OSType2Ptr( OSType type, Ptr str)
{
	PPBE32(&type);
	
	memcpy( str, &type, 4);
	str[ 4] = 0;
}

OSType Ptr2OSType( char* str)
{
	short   i;
	OSType  type;
	
	i = strlen( str);
	if( i > 4) i = 4;
	type = '    ';
	memcpy( &type, str, i);
	PPBE32(&type);
	
	return type;
}


