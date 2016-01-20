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
#include <string.h>

/**************************************************************************
**************************************************************************/

unsigned char* MYC2PStr(Ptr cStr);

static Boolean compMem(const void *a, const void *b, size_t s)
{
	if (memcmp(a, b, s) == 0) {
		return true;
	} else {
		return false;
	}
}

static OSErr TestMIDIFile(const void *AlienFile)
{
	if(compMem(AlienFile, "MThd", 4))
		return noErr;
	else
		return MADFileNotSupportedByThisPlug;
}

static OSErr ExtractMIDIInfo(PPInfoRec *info, Ptr theMIDI)
{
	/*long	PatternSize;
	short	i;
	short	maxInstru;
	short	tracksNo;
	long	inOutCount;*/
	
	info->signature = 'Midi';
	strcpy(info->internalFileName, "");
	info->totalPatterns = 0;
	info->partitionLength = 0;
	info->totalInstruments = 0;
	info->totalTracks = 0;
	strcpy(info->formatDescription, "Midi Plug");
	
	return noErr;
}

void CreateResult(char *aPtr)
{
	MYC2PStr(aPtr);
	DebugStr((unsigned char *)aPtr);
}

void ConvertMidiFile(char *src, MADMusic *theMAD, MADDriverSettings *init);

static OSErr mainMIDI(OSType order, char *AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
{
	OSErr	myErr = noErr;
	Ptr		AlienFile;
	UNFILE	iFileRefI;
	long	sndSize;
	
	switch (order)
	{
		case 'IMPL':
			iFileRefI = iFileOpen(AlienFileName);
			if (iFileRefI) {
				sndSize = iGetEOF(iFileRefI);
				
				// ** MEMORY Test Start
				AlienFile = MADPlugNewPtr(sndSize * 2L, init);
				if(AlienFile == NULL)
					myErr = MADNeedMemory;
				// ** MEMORY Test End
				
				else {
					DisposePtr(AlienFile);
					
					AlienFile = MADPlugNewPtr(sndSize, init);
					myErr = iRead(sndSize, AlienFile, iFileRefI);
					
					if (myErr == noErr) {
						myErr = TestMIDIFile(AlienFile);
						
						if(myErr == noErr)
							ConvertMidiFile(AlienFile, MadFile, init);
						
						DisposePtr(AlienFile);	AlienFile = NULL;
					}
					iClose(iFileRefI);
				}
			} else
				myErr = MADReadingErr;
			break;
			
		case 'TEST':
			iFileRefI = iFileOpen(AlienFileName);
			if (iFileRefI) {
				sndSize = 1024;
				
				AlienFile = MADPlugNewPtr(sndSize, init);
				if(AlienFile == NULL)
					myErr = MADNeedMemory;
				else {
					myErr = iRead(sndSize, AlienFile, iFileRefI);
					if(myErr == noErr)
						myErr = TestMIDIFile(AlienFile);
					
					DisposePtr(AlienFile);	AlienFile = NULL;
				}
				iClose(iFileRefI);
			} else
				myErr = MADReadingErr;
			break;
			
		case 'INFO':
			iFileRefI = iFileOpen(AlienFileName);
			if (iFileRefI) {
				info->fileSize = iGetEOF(iFileRefI);
				
				sndSize = 5000L;	// Read only 5000 first bytes for optimisation
				
				AlienFile = MADPlugNewPtr(sndSize, init);
				if(AlienFile == NULL)
					myErr = MADNeedMemory;
				else {
					myErr = iRead(sndSize, AlienFile, iFileRefI);
					if(myErr == noErr)
						myErr = ExtractMIDIInfo(info, AlienFile);
					DisposePtr(AlienFile);	AlienFile = NULL;
				}
				iClose(iFileRefI);
			} else
				myErr = MADReadingErr;
			break;
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
	return myErr;
}

#define PLUGUUID (CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x87, 0x3A, 0xA7, 0x91, 0xE9, 0xE5, 0x42, 0xEB, 0x8F, 0xE0, 0x35, 0x1A, 0x99, 0xCF, 0x9A, 0x3A))
//873AA791-E9E5-42EB-8FE0-351A99CF9A3A

#define PLUGMAIN mainMIDI
#define PLUGINFACTORY PPMIDIFactory
#include "../CFPlugin-bridge.c"
