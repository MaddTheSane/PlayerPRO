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
#include "string.h"
#include "FileUtils.h"

#if defined(powerc) || defined(__powerc)
enum {
		PlayerPROPlug = kCStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof( OSType)))
		| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof( Ptr*)))
		| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof( MADMusic*)))
		| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof( PPInfoRec*)))
		| STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof( MADDriverSettings*)))
};

ProcInfoType __procinfo = PlayerPROPlug;
#else
#include <A4Stuff.h>
#include <SetUpA4.h>
#endif

/**************************************************************************
**************************************************************************/

unsigned char* MYC2PStr( Ptr cStr);

void MyDebugStr( short, Ptr a, Ptr b)
{
	MYC2PStr( b);
	DebugStr( (unsigned char*) b);
}

Ptr MADPlugNewPtr( long size, MADDriverSettings* init)
{
	return NewPtr( size);
}

Ptr MADPlugNewPtrClear( long size, MADDriverSettings* init)
{
	return NewPtrClear( size);
}

Boolean compMem( Ptr a, Ptr b, long s)
{
long 	i;

	for( i = 0; i < s; i++)
	{
		if( a[ i] != b[ i]) return false;
	}
	
	return true;
}

OSErr TestMIDIFile( Ptr AlienFile)
{
	if( compMem( AlienFile, "MThd", 4)) return noErr;
	else return MADFileNotSupportedByThisPlug;
}

OSErr ExtractMIDIInfo( PPInfoRec *info, Ptr theMIDI)
{
	long	PatternSize;
	short	i;
	short	maxInstru;
	short	tracksNo;
	long	inOutCount;
	
	info->signature = 'Midi';
	strcpy( info->internalFileName, "");
	info->totalPatterns = 0;
	info->partitionLength = 0;
	info->totalInstruments = 0;
	info->totalTracks = 0;
	strcpy( info->formatDescription, "Midi Plug");
	
	return noErr;
}

void CreateResult( Ptr aPtr)
{
	MYC2PStr( aPtr);
	DebugStr( ( unsigned char *) aPtr);
}

void  ConvertMidiFile( char	*src, MADMusic *theMAD, MADDriverSettings *init);

OSErr main( OSType order, char *AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
{
	OSErr	myErr;
	Ptr		AlienFile;
	short	vRefNum, iFileRefI;
	long	dirID, sndSize;
	
#ifndef powerc
	long	oldA4 = SetCurrentA4(); 			//this call is necessary for strings in 68k code resources
#endif

//	MYC2PStr( AlienFileName);
	
	myErr = noErr;

	switch( order)
	{
		case 'IMPL':
			iFileRefI = iFileOpen( AlienFileName);
			if( iFileRefI)
			{
				GetEOF( iFileRefI, &sndSize);
			
				// ** MEMORY Test Start
				AlienFile = MADPlugNewPtr( sndSize * 2L, init);
				if( AlienFile == 0L) myErr = MADNeedMemory;
				// ** MEMORY Test End
				
				else
				{
					DisposePtr( AlienFile);
					
					AlienFile = MADPlugNewPtr( sndSize, init);
					myErr = FSRead( iFileRefI, &sndSize, AlienFile);
					
					
					if( myErr == noErr)
					{
						myErr = TestMIDIFile( AlienFile);
						
						if( myErr == noErr) ConvertMidiFile( AlienFile, MadFile, init);
						
						DisposePtr( AlienFile);	AlienFile = 0L;
					}
					FSClose( iFileRefI);
				}
			}
			else myErr = MADReadingErr;
		break;
		
		case 'TEST':
			iFileRefI = iFileOpen( AlienFileName);
			if( iFileRefI)
			{
				sndSize = 1024L;
				
				AlienFile = MADPlugNewPtr( sndSize, init);
				if( AlienFile == 0L) myErr = MADNeedMemory;
				else
				{
					myErr = FSRead( iFileRefI, &sndSize, AlienFile);
					myErr = TestMIDIFile( AlienFile);
					
					DisposePtr( AlienFile);	AlienFile = 0L;
				}
				FSClose( iFileRefI);
			}
			else myErr = MADReadingErr;
		break;

		case 'INFO':
			iFileRefI = iFileOpen( AlienFileName);
			if( iFileRefI)
			{
				GetEOF( iFileRefI, &info->fileSize);
			
				sndSize = 5000L;	// Read only 5000 first bytes for optimisation
				
				AlienFile = MADPlugNewPtr( sndSize, init);
				if( AlienFile == 0L) myErr = MADNeedMemory;
				else
				{
					myErr = FSRead( iFileRefI, &sndSize, AlienFile);
					myErr = ExtractMIDIInfo( info, AlienFile);
					DisposePtr( AlienFile);	AlienFile = 0L;
				}
				FSClose( iFileRefI);
			}
			else myErr = MADReadingErr;
		break;
		
		default:
			myErr = MADOrderNotImplemented;
		break;
	}
	
//	MYP2CStr( (unsigned char*) AlienFileName);
	
	#ifndef powerc
		SetA4( oldA4);
	#endif
	return myErr;
}