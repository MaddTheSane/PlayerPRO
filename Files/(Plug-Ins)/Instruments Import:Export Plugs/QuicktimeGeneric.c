/*	v 1.0			*/
/*	1999 by ANR		*/

#include <PlayerPROCore/PlayerPROCore.h>
#include <AIFF.h>
#include <SoundInput.h>
#include <QuicktimeComponents.h>
#include <Sound.h>
#include <Movies.h>

OSErr mainQTInst(OSType					order,						// Order to execute
				 InstrData				*InsHeader,					// Ptr on instrument header
				 sData					**sample,					// Ptr on samples data
				 short					*sampleID,					// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
																	// If sampleID == -1 : add sample else replace selected sample.
				 FSSpec					*AlienFileFSSpec,			// IN/OUT file
				 PPInfoPlug				*thePPInfoPlug)
{
	OSErr	myErr = noErr;
	Ptr		AlienFile;
	short	iFileRefI;
	long	inOutBytes;
		
	switch( order)
	{
		case 'PLAY':
		break;
		
		case 'IMPL':
		{
			Ptr				theSound;
			long			lS, lE;
			short			sS;
			unsigned long	rate;
			Boolean			stereo;
			FSSpec			newFile;
			
			myErr = ConvertDataToWAVE( *AlienFileFSSpec, &newFile, thePPInfoPlug);
			if( myErr == noErr)
			{
				theSound = ConvertWAV( &newFile, &lS, &lE, &sS, &rate, &stereo);
				
				if( theSound) inAddSoundToMAD( theSound, lS, lE, sS, 60, rate, stereo, newFile.name, InsHeader, sample, sampleID);
				else
				{
					myErr = MADNeedMemory;
				}
				
				FSpDelete( &newFile);
			}
		}
		break;
		
		case 'TEST':
		{
			FInfo fInfo;
			
			FSpGetFInfo( AlienFileFSSpec, &fInfo);
			
			if( fInfo.fdType == thePPInfoPlug->fileType) myErr = noErr;
			else myErr = MADFileNotSupportedByThisPlug;
		}
		break;
		
		case 'EXPL':
			if( *sampleID >= 0)
			{
				OSType				compType = 'NONE';
				unsigned long		rate;
				sData 				*curData = sample[ *sampleID];
				short				numChan;
				
				myErr = FSpCreate( AlienFileFSSpec, 'TVOD', 'AIFF', smCurrentScript);
				if(myErr == noErr) myErr = FSpOpenDF( AlienFileFSSpec, fsCurPerm, &iFileRefI);
				
				if( myErr == noErr)
				{
					inOutBytes 	= curData->size;
					rate		= curData->c2spd;
					
					if( curData->stereo) numChan = 2;
					else numChan = 1;
					
					myErr = SetupAIFFHeader(	iFileRefI,
												numChan,
												rate << 16L,
												curData->amp,
												compType,
												inOutBytes,
												0);
					
					if(myErr == noErr) myErr = FSWrite( iFileRefI, &inOutBytes, curData->data);
					FSClose( iFileRefI);
				}
			}
		break;
		
		default:
			myErr = MADOrderNotImplemented;
		break;
	}
	
	return myErr;
}

// 26E9A321-0E15-48E3-8A94-062C46FDB875
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x26, 0xE9, 0xA3, 0x21, 0x0E, 0x15, 0x48, 0xE3, 0x8A, 0x94, 0x06, 0x2C, 0x46, 0xFD, 0xB8, 0x75)
#define PLUGINFACTORY QTInstFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainQTInst //The old main function, renamed please

#include "CFPlugin-InstrBridge.c"
