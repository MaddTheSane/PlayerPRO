/*	AIFF/AIFC		*/
/*  IMPORT/EXPORT	*/
/*	v 1.0			*/
/*	1996 by ANR		*/

#include <PlayerPROCore/PlayerPROCore.h>
#include <CoreFoundation/CoreFoundation.h>

static inline OSErr TestAIFF(ContainerChunk* CC)
{
	if (CC->formType == AIFCID)
		return noErr;
	else if (CC->formType == AIFFID)
		return noErr;
	else if (CFSwapInt32(CC->formType) == AIFCID)
		return noErr;
	else if (CFSwapInt32(CC->formType) == AIFFID)
		return noErr;
	else
		return MADFileNotSupportedByThisPlug;
}

static OSErr mainAIFF(void					*unused,
					  OSType				order,						// Order to execute
					  InstrData				*InsHeader,					// Ptr on instrument header
					  sData					**sample,					// Ptr on samples data
					  short					*sampleID,					// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
					  // If sampleID == -1 : add sample else replace selected sample.
					  CFURLRef				AlienFileURL,				// IN/OUT file
					  PPInfoPlug			*thePPInfoPlug)
{
	OSErr	myErr = noErr;
	char	*AlienFile;
	short	iFileRefI;
	long	inOutBytes;
		
	switch(order)
	{
		case 'IMPL':
		{
			char			*theSound;
			long			lS, lE;
			short			sS;
			unsigned long	rate;
			Boolean			stereo;
			FSSpec			newFile;
			
			myErr = ConvertDataToWAVE(*AlienFileFSSpec, &newFile, thePPInfoPlug);
			if (myErr == noErr) {
				theSound = ConvertWAV(&newFile, &lS, &lE, &sS, &rate, &stereo);
				
				if (theSound)
					myErr = inAddSoundToMAD(theSound, lS, lE, sS, 60, rate, stereo, AlienFileFSSpec->name, InsHeader, sample, sampleID);
				else
					myErr = MADNeedMemory;
				
				FSpDelete(&newFile);
			}
		}
		break;
		
		case 'TEST':
		{
			char *theSound;
			
			myErr = FSpOpenDF(AlienFileFSSpec, fsCurPerm, &iFileRefI);
			if (myErr == noErr) {
				inOutBytes = 50L;
				theSound = malloc(inOutBytes);
				if (theSound == NULL) myErr = MADNeedMemory;
				else {
					FSRead( iFileRefI, &inOutBytes, theSound);
					
					myErr = TestAIFF( (ContainerChunk*) theSound);
				}
				
				free(theSound);
				
				FSClose(iFileRefI);
			}
		}
		break;
		
		case 'EXPL':
			if (*sampleID >= 0)
			{
				OSType			compType = 'NONE';
				unsigned int	rate;
				sData 			*curData = sample[*sampleID];
				short			numChan;
				
				myErr = FSpCreate(AlienFileFSSpec, 'TVOD', 'AIFF', smCurrentScript);
				myErr = FSpOpenDF(AlienFileFSSpec, fsCurPerm, &iFileRefI);
				
				if (myErr == noErr) {
					inOutBytes 	= curData->size;
					rate		= curData->c2spd;
					
					if (curData->stereo) numChan = 2;
					else numChan = 1;
					
					myErr = SetupAIFFHeader(iFileRefI,
											numChan,
											rate << 16L,
											curData->amp,
											compType,
											inOutBytes,
											0);
					
					myErr = FSWrite(iFileRefI, &inOutBytes, curData->data);
					
					/*
					marker = (MarkerChunk*) CH;
					if (marker->numMarkers == 2)
					{
						*loopStart = marker->Markers[ 0].position;
						mm = (Marker*) marker->Markers;
						mm = (Marker*) ((Ptr) mm + 8L);
						mm = (Marker*) ((Ptr) mm + marker->Markers[ 0].markerName[0]);
						*loopEnd = mm->position;
						
						if (*sampleSize == 16)
						{
							*loopStart *= 2;
							*loopEnd *= 2;
						}
					}*/
					
					FSClose(iFileRefI);
				}
			}
		break;
		
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
		
	return myErr;
}

// C4B85FAC-BD58-4661-9004-CBBF84BA4EDD
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xC4, 0xB8, 0x5F, 0xAC, 0xBD, 0x58, 0x46, 0x61, 0x90, 0x04, 0xCB, 0xBF, 0x84, 0xBA, 0x4E, 0xDD)
#define PLUGINFACTORY AIFFFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainAIFF //The old main function, renamed please

#include "CFPlugin-InstrBridge.c"

