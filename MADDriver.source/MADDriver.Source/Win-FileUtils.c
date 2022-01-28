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

#include "RDriver.h"
#include "FileUtils.h"

void iFileCreate( Ptr AlienFileName, long)
{
}

FILE* iFileOpen( Ptr name)
{
	return fopen( name, "rb");
}

long iGetEOF( FILE* iFileRefI)
{
	long curEOF;
	
	fseek( iFileRefI, 0, SEEK_END);
	curEOF = ftell( iFileRefI);
	fseek( iFileRefI, 0, 0);
	
	return curEOF;
}

OSErr iRead( long size, Ptr dest, FILE* iFileRefI)
{
	fread( dest, size, 1, iFileRefI);
	
	return noErr;
}

OSErr iSeekCur( long size, FILE* iFileRefI)
{
	return fseek( iFileRefI , size, SEEK_CUR);
}

OSErr iWrite( long size, Ptr dest, FILE* iFileRefI)
{
	fwrite( dest, size, 1, iFileRefI);
	
	return noErr;
}

void iClose( FILE* iFileRefI)
{
	fclose( iFileRefI);
}

void MOT32( void *msg_buf)
{
  unsigned char 	*buf = (unsigned char*) msg_buf;
  unsigned long		out;
  
  out = ( (unsigned long) buf[0] << 24L) | ( (unsigned long) buf[1] << 16L) | ( (unsigned long) buf[ 2] << 8L) | ( (unsigned long) buf[3]);
	*((unsigned long*) msg_buf) = out;
}

void MOT16( void *msg_buf)
{
  unsigned char 	*buf = (unsigned char*) msg_buf;
  unsigned short	out;
  
  out = ((unsigned short) buf[0] << 8L) | ( (unsigned short) buf[1]);
	*((unsigned short*) msg_buf) = out;
}

void xINT32( void *msg_buf)
{
}

void INT16( void *msg_buf)
{
}

Ptr MADstrcpy( Ptr dst, const char* src)
{
	long i = 0;
	
	do
	{
		dst[ i] = src[ i];
	}while( src[ i++]);
	
	return dst;
}

int MADstrcmp( const char *dst, const char* src)
{
	long i = 0;
	
	do
	{
		if( dst[ i] != src[ i]) return -1;
	}while( src[ i++]);
	
	return 0;
}
