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

#define _CRT_SECURE_NO_WARNINGS 1

#include "RDriver.h"
#include "FileUtils.h"

void iFileCreate(Ptr path, OSType type)
{
}

FILE* iFileOpen( Ptr name)
{
	return iFileOpenRead( name);
}

FILE* iFileOpenRead( Ptr name)
{
	return fopen( name, "rb");
}

FILE* iFileOpenWrite( Ptr name)
{
	return fopen( name, "wb");
}

long iGetEOF( FILE* iFileRefI)
{
	long curEOF, oldPos;
	
	oldPos = ftell(iFileRefI);
	fseek( iFileRefI, 0, SEEK_END);
	curEOF = ftell( iFileRefI);
	fseek( iFileRefI, oldPos, SEEK_SET);
	
	return curEOF;
}

OSErr iRead( long size, Ptr dest, FILE* iFileRefI)
{
	fread( dest, size, 1, iFileRefI);
	
	return noErr;
}

OSErr iSeekCur( long size, FILE* iFileRefI)
{
	return fseek( iFileRefI, size, SEEK_CUR);
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
	char safetyNet[5];
	strcpy(safetyNet, "    ");
	
	i = strlen( str);
	memcpy(safetyNet, str, i);
	type = '    ';
	memcpy( &type, safetyNet, 4);
	PPBE32(&type);
	
	return type;
}


