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

//NOTE: this file can be used on Linux as well.

#include "RDriver.h"
#include "FileUtils.h"

void iFileCreate( UNFILEName folder, OSType)
{
}

void iFileCreateName( UNFILEName folder, Ptr name, OSType)
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
