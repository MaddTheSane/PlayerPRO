/********************						***********************/
//
//	Player PRO 5.9 - DRIVER SOURCE CODE -
//
//	Library Version 5.9
//
//	To use with MAD Library for Mac: Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	20 Micheli-Du-Crest
//	1205 GENEVA
//	SWITZERLAND
//
//	COPYRIGHT ANTOINE ROSSET 1996, 1997, 1998, 1999, 2000, 2001, 2002
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:			(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 		RossetAntoine@bluewin.ch
//
/********************						***********************/

#include "FileUtils.h"
#include "Script.h"

void pStrcpy(register unsigned char *s1, register unsigned char *s2)
{
	register short len, i;
	
	if (*s2 <= 220) 
	{
		len = *s2;
		for ( i = 0; i <= len; i++)
		{
			s1[ i] = s2[ i];
		}
	}
}

long MYstrlen( Ptr cStr)
{
	long i = 0;
	
	while( cStr[ i] != 0)
	{
		i++;
		if( i > 300) return i = 0;
	}
	
	return i;
}

unsigned char* MYC2PStr( Ptr cStr)
{
	long size = MYstrlen( cStr);
	BlockMoveData( cStr, cStr + 1, MYstrlen( cStr));
	cStr[ 0] = size;

	return (unsigned char*) cStr;
}

void MYP2CStr( unsigned char *cStr)
{
	long size = cStr[ 0];
	BlockMoveData( cStr + 1, cStr, size);
	cStr[ size] = 0;
}

short iFileOpen( Ptr name)
{
	short 	temp;
	OSErr		iErr;
	FSSpec	spec;
	
	HGetVol( 0L, &spec.vRefNum, &spec.parID);
	
	MYC2PStr( name);
	
	pStrcpy( spec.name, (unsigned char*) name);
	
	iErr = FSpOpenDF( &spec, fsCurPerm, &temp);
	MYP2CStr( (unsigned char*) name);
	
	if( iErr) return 0;
	else return temp;
}

long iGetEOF( short iFileRefI)
{
	long curEOF;
	
	GetEOF( iFileRefI, &curEOF);
	
	return curEOF;
}

OSErr iRead( long size, Ptr dest, short iFileRefI)
{
	return FSRead( iFileRefI, &size, dest);
}

OSErr iSeekCur( long size, short iFileRefI)
{
	return SetFPos( iFileRefI, fsFromMark, size);
}

void iFileCreate( Ptr name, long type)
{
FSSpec	spec;
	
	HGetVol( 0L, &spec.vRefNum, &spec.parID);
	
	MYC2PStr( name);
	
	pStrcpy( spec.name, (unsigned char*) name);
	
	FSpDelete( &spec);
	FSpCreate( &spec, 'SNPL', type, smSystemScript);
	
	MYP2CStr( (unsigned char*) name);
}

OSErr iWrite( long size, Ptr dest, short iFileRefI)
{
	return FSWrite( iFileRefI, &size, dest);
}

void iClose( short iFileRefI)
{
	FSClose( iFileRefI);
}

/////////////////////////////////

void MOT32( void *msg_buf)
{
}

void MOT16( void *msg_buf)
{
}

/////////////////////////////////
void INT32( void *msg_buf)
{
  unsigned char 	*buf = (unsigned char*) msg_buf;
  unsigned long		out;
  
  out = ( (unsigned long) buf[3] << 24) | ( (unsigned long) buf[2] << 16) | ( (unsigned long) buf[ 1] << 8) | ( (unsigned long) buf[0]);
	*((unsigned long*) msg_buf) = out;
}

void INT16( void *msg_buf)
{
  unsigned char 	*buf = (unsigned char*) msg_buf;
  short						out;
  
  out =  ( (short) buf[1] << 8) | ( (short) buf[0]);
  *((short*) msg_buf) = out;
}
/////////////////////////////////

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
