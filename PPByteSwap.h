/*
 *  PPByteSwap.h
 *  PlayerPROCore
 *
 *  Created by C.W. Betts on 10/19/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *  Routines to help with keeping data the proper endian.
 *
 */

#ifndef __PPBYTESWAP_H
#define __PPBYTESWAP_H

#ifndef NOINLINE
#ifdef _MAC_H
#include <CoreServices/CoreServices.h>
#endif

static inline void MADByteSwap32(void *theData)
{
	UInt32			temp = *((UInt32*) theData);
#ifdef _MAC_H
	*((UInt32*) theData) = Endian32_Swap(temp);
#else
	*((UInt32*) theData) = ((((temp & 0xff000000) >> 24) | (( temp & 0x00ff0000) >> 8) | (( temp & 0x0000ff00) << 8) | (temp & 0x000000ff) << 24));
#endif
}

static inline void MADByteSwap16(void *theData)
{
	UInt16			buf = *((UInt16*) theData);
#ifndef _MAC_H
	*((UInt16*) theData) = ((buf << 8) | (buf >> 8));
#else
	*((UInt16*) theData) = Endian16_Swap(buf);
#endif
}

static inline void MOT32(void *msg_buf)
{
#ifdef _MAC_H
	
	*((UInt32*) msg_buf) = EndianU32_BtoN(*((UInt32*) msg_buf));
	
#else
	
#ifdef __LITTLE_ENDIAN__
	MADByteSwap32(msg_buf);
#endif
#endif
}

static inline void MOT16(void *msg_buf)
{
#ifdef _MAC_H
	
	*((UInt16*) msg_buf) = EndianU16_BtoN(*((UInt16*) msg_buf));
	
#else
	
#ifdef __LITTLE_ENDIAN__
	MADByteSwap16(msg_buf);
#endif
#endif
}


/////////////////////////////////
static inline void INT32(void *msg_buf)
{
#ifdef _MAC_H
	
	*((UInt32*) msg_buf) = EndianU32_LtoN(*((UInt32*) msg_buf));
	
#else
#ifdef __BIG_ENDIAN__
	MADByteSwap32(msg_buf);
#endif
#endif
}

static inline void INT16(void *msg_buf)
{
#ifdef _MAC_H
	
	*((UInt16*)msg_buf) = EndianU16_LtoN(*((UInt16*)msg_buf));
	
#else
#ifdef __BIG_ENDIAN__
	MADByteSwap16(msg_buf);
#endif
#endif
}
#endif

#endif

