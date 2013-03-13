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

#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////

#ifdef _MAC_H
short iFileOpen( Ptr name);
void iFileCreate( Ptr name, long);

long iGetEOF( short iFileRefI);
OSErr iRead( long size, Ptr dest, short iFileRefI);
OSErr iWrite( long size, Ptr dest, short iFileRefI);
OSErr iSeekCur( long size, short iFileRefI);

void iClose( short iFileRefI);

char* MADstrcpy( char*, const char*);
int MADstrcmp( const char *dst, const char* src);

#define UNFILE	short

unsigned char* MYC2PStr( Ptr cStr);
void MYP2CStr( unsigned char *cStr);
#endif


////////////////////////////////////////////////////////////

#ifdef _INTEL_H
FILE* iFileOpen( Ptr name);
void iFileCreate( Ptr name, long);

long iGetEOF( FILE* iFileRefI);
OSErr iRead( long size, Ptr dest, FILE* iFileRefI);
OSErr iWrite( long size, Ptr dest, FILE* iFileRefI);
OSErr iSeekCur( long size, FILE* iFileRefI);

void iClose( FILE* iFileRefI);

char* MADstrcpy( char*, const char*);
int MADstrcmp( const char *dst, const char* src);

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define UNFILE	FILE*

#endif

////////////////////////////////////////////////////////////

#ifdef _BE_H
FILE* iFileOpen( Ptr name);
void iFileCreate( Ptr name, long);

long iGetEOF( FILE* iFileRefI);
OSErr iRead( long size, Ptr dest, FILE* iFileRefI);
OSErr iWrite( long size, Ptr dest, FILE* iFileRefI);
OSErr iSeekCur( long size, FILE* iFileRefI);

void iClose( FILE* iFileRefI);

char* MADstrcpy( char*, const char*);
int MADstrcmp( const char *dst, const char* src);

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define UNFILE	FILE*

#endif

////////////////////////////////////////////////////////////


void xINT32( void *msg_buf);
void INT16( void *msg_buf);
void MOT32( void *msg_buf);
void MOT16( void *msg_buf);


#ifdef __cplusplus
}
#endif

#endif
