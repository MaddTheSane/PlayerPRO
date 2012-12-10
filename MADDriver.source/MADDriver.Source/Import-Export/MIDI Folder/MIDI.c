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

#include <PlayerPROCore/PlayerPROCore.h>

#ifdef __LP64__
#error this code will only work on 32-bit mode
#error this code needs to be rewritten. 
#endif

#include <string.h>

/**************************************************************************
**************************************************************************/

static unsigned char* MYC2PStr( Ptr cStr)
{
	long length = strlen(cStr);
	memmove(cStr + 1, cStr, length);
	cStr[0] = length;
	return (unsigned char*)cStr;
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
	/*long	PatternSize;
	short	i;
	short	maxInstru;
	short	tracksNo;
	long	inOutCount;*/
	
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

extern OSErr PPImpExpMain( OSType order, char *AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
{
	OSErr	myErr = noErr;
	Ptr		AlienFile;
	UNFILE	iFileRefI;
	long	sndSize;
	
	switch( order)
	{
		case MADPlugImport:
			iFileRefI = iFileOpenRead( AlienFileName);
			if( iFileRefI)
			{
				sndSize =iGetEOF( iFileRefI);
			
				// ** MEMORY Test Start
				AlienFile = malloc( sndSize * 2L);
				if( AlienFile == NULL) myErr = MADNeedMemory;
				// ** MEMORY Test End
				
				else
				{
					free( AlienFile);
					
					AlienFile = malloc( sndSize);
					myErr = iRead( sndSize, AlienFile, iFileRefI);
					
					
					if( myErr == noErr)
					{
						myErr = TestMIDIFile( AlienFile);
						
						if( myErr == noErr) ConvertMidiFile( AlienFile, MadFile, init);
						
						free( AlienFile);	AlienFile = NULL;
					}
					iClose( iFileRefI);
				}
			}
			else myErr = MADReadingErr;
		break;
		
		case MADPlugTest:
			iFileRefI = iFileOpenRead( AlienFileName);
			if( iFileRefI)
			{
				sndSize = 1024L;
				
				AlienFile = malloc( sndSize);
				if( AlienFile == NULL) myErr = MADNeedMemory;
				else
				{
					myErr = iRead( sndSize, AlienFile, iFileRefI);
					if(myErr == noErr) myErr = TestMIDIFile( AlienFile);
					
					free( AlienFile);	AlienFile = NULL;
				}
				iClose( iFileRefI);
			}
			else myErr = MADReadingErr;
		break;

		case 'INFO':
			iFileRefI = iFileOpenRead( AlienFileName);
			if( iFileRefI)
			{
				info->fileSize = iGetEOF( iFileRefI);
			
				sndSize = 5000L;	// Read only 5000 first bytes for optimisation
				
				AlienFile = malloc( sndSize);
				if( AlienFile == NULL) myErr = MADNeedMemory;
				else
				{
					myErr = iRead( sndSize, AlienFile, iFileRefI);
					if(myErr == noErr) myErr = ExtractMIDIInfo( info, AlienFile);
					free( AlienFile);	AlienFile = NULL;
				}
				iClose( iFileRefI);
			}
			else myErr = MADReadingErr;
		break;
		
		default:
			myErr = MADOrderNotImplemented;
		break;
	}
		
	return myErr;
}
