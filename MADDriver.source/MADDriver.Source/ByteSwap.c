/*
 *  ByteSwap.c
 *  PPMacho
 *
 *  Created by C.W. Betts on 11/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "MAD.h"

static __inline__ void MADByteSwap32(void *theData)
{
	UInt32			temp = *((UInt32*) theData);
#ifdef _MAC_H
	*((UInt32*) theData) = Endian32_Swap(temp);
#else
	*((UInt32*) theData) = ((((temp & 0xff000000) >> 24) | (( temp & 0x00ff0000) >> 8) | (( temp & 0x0000ff00) << 8) | (temp & 0x000000ff) << 24));
#endif
}

static __inline__ void MADByteSwap16(void *theData)
{
	UInt16			buf = *((UInt16*) theData);
#ifdef _MAC_H
	*((UInt16*) theData) = Endian16_Swap(buf);
#else
	*((UInt16*) msg_buf) = (((((UInt16)buf)<<8) & 0xFF00) | ((((UInt16)buf)>>8) & 0x00FF));
#endif
}

extern void MOT32(void *msg_buf)
{
#ifdef __LITTLE_ENDIAN__
	MADByteSwap32(msg_buf);
#endif
}

extern void MOT16(void *msg_buf)
{
#ifdef __LITTLE_ENDIAN__
	MADByteSwap16(msg_buf);
#endif
}


/////////////////////////////////
extern void INT32(void *msg_buf)
{
#ifdef __BIG_ENDIAN__
	MADByteSwap32(msg_buf);
#endif
}

extern void INT16(void *msg_buf)
{
#ifdef __BIG_ENDIAN__
	MADByteSwap16(msg_buf);
#endif
}

