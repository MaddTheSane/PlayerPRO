/*	WAV		*/
/*  IMPORT	*/

#include <PlayerPROCore/MAD.h>
#include <PlayerPROCore/PPDefs.h>
#include <PlayerPROCore/FileUtils.h>
#include <PlayerPROCore/PPPlug.h>
#include <Sound.h>
#include "WAV.h"

/*______________________________________________________________________
 | _|_																	|
 |  À  WAVE2SND Project: Balthazar				Started:	12/14/92	|
 | 									 	  Last Modified:	12/21/92	|
 | 									 	 		Release:	12/27/92	|
 | 																		|
 | 	Craig Marciniak														|
 | 	151 83rd Ave, NE #305												|
 | 	Fridley, Mn 55432													|
 | 	612-780-5603														|
 | 						 												|
 | Copyright © 1992 by Craig A. Marciniak          All rights reserved. |
 _______________________________________________________________________*/

#define WAVE_FORMAT_PCM		1
#define kmaxVolume			7
 
#define ksysFailed 			"\pSorry, This software requires system 7.0.0 or greater."

#define kStereoStr 			"\pStereo"
#define kMonoStr 			"\pMonaural"

//typedef unsigned short	WORD;
//typedef unsigned long	DWORD;
//typedef long			FOURCC;

OSErr TestWAV( PCMWavePtr CC)
{
	if( CC->ckid =='RIFF') return noErr;
	else return MADFileNotSupportedByThisPlug;
}

/*___________________ long byte swap for Intel <-> Motorola Conversions*/
//Just going to byteswap on big-endian platforms
/*unsigned long longswap(unsigned long ul)
{
//	return (ul >> 24) | ((ul >> 8) & 0xff00) | ((ul << 8) & 0xff0000) | (ul << 24);
	return EndianU32_LtoN(ul);
}*/
#define longswap(ul) EndianU32_LtoN(ul)

/*___________________ word byte swap for Intel <-> Motorola Conversions*/

/*unsigned short shrtswap(unsigned int us)
{
//	return ((us >> 8) | (us << 8)) & 0xffff;
	return EndianU16_LtoN(us);
}*/
#define shrtswap(us) EndianU16_LtoN(us)

/*_______________________________________________________________________*/

Ptr ConvertWAV(FSSpec *fileSpec, long *loopStart, long *loopEnd, short	*sampleSize, unsigned long *rate, Boolean *stereo)
{
	PCMWavePtr			WAVERsrc;
	short				fRef,tempResRef,x;
	long				fSize;
	int					theHit;
	char				test;
 	short				gRefNum;
 	short				gVolSet;
	
	*stereo = false;
	
	if(!FSpOpenDF(fileSpec,fsRdWrPerm,&fRef))
	{
		GetEOF(fRef,&fSize);
		if(!(WAVERsrc = (PCMWavePtr) NewPtr(fSize))) 
		{
			FSCloseFork(fRef); return NULL;
		}
		
		if(FSRead(fRef,&fSize,&(*WAVERsrc)))
		{
			FSCloseFork(fRef); return NULL;
		}
		
		if((*WAVERsrc).ckid =='RIFF')
		{
			(*WAVERsrc).cksize = longswap((*WAVERsrc).cksize);
			
			if((*WAVERsrc).fccType =='WAVE')
			{
				(*WAVERsrc).dwDataOffset = longswap((*WAVERsrc).dwDataOffset);
				
				if((*WAVERsrc).fmtType,'fmt ')
				{
					(*WAVERsrc).wFormatTag      = shrtswap((*WAVERsrc).wFormatTag);
					(*WAVERsrc).nCannels        = shrtswap((*WAVERsrc).nCannels);
					(*WAVERsrc).nSamplesPerSec  = longswap((*WAVERsrc).nSamplesPerSec);
					(*WAVERsrc).nSamplesPerSec  = (*WAVERsrc).nSamplesPerSec << 16;
					(*WAVERsrc).nAvgBytesPerSec = longswap((*WAVERsrc).nAvgBytesPerSec);
					(*WAVERsrc).nBlockAlign     = shrtswap((*WAVERsrc).nBlockAlign);
					(*WAVERsrc).wBitsPerSample  = shrtswap((*WAVERsrc).wBitsPerSample);
					(*WAVERsrc).dataSize        = longswap((*WAVERsrc).dataSize);
					
					*loopStart	= 0;
					*loopEnd 	= 0;
					*sampleSize = (*WAVERsrc).wBitsPerSample;
					*rate		= (*WAVERsrc).nSamplesPerSec;
					
					if( (*WAVERsrc).nCannels == 2) *stereo = true;
					else *stereo = false;
					
					if((*WAVERsrc).wFormatTag != 1)
					{
						return NULL;
					}
				}
				else
				{
					DisposePtr( (Ptr) WAVERsrc);
					return NULL;
				}	
			}
			else
			{
				DisposePtr( (Ptr) WAVERsrc);
				return NULL;
			}
		}
		else
		{
			DisposePtr( (Ptr) WAVERsrc);
			return NULL;
		}
		FSClose(fRef);
	}
	
	{
		long sndSize = WAVERsrc->dataSize;
		
		BlockMoveData( (Ptr)(long) (WAVERsrc->theData), (Ptr) WAVERsrc , sndSize);
		
		SetPtrSize( (Ptr) WAVERsrc, sndSize);
		
		switch( *sampleSize)
		{
			case 8:
				ConvertInstrumentIn( (Byte*) WAVERsrc, sndSize);
			break;
			
			case 16:
				{
					long			i;
					unsigned short	*tt = (unsigned short*) WAVERsrc;
					
					for( i = 0; i < sndSize/2; i++) tt[ i] = shrtswap( tt[ i]);
				}
			break;
		}
	}
	return (Ptr) WAVERsrc;
}

EXP OSErr mainWave(OSType					order,						// Order to execute
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
	/*	case 'PLAY':
		{
			Ptr				theSound;
			long			lS, lE;
			short			sS;
			unsigned long	rate;
			Boolean			stereo;
		
			theSound = ConvertWAV( AlienFileFSSpec, &lS, &lE, &sS, &rate, &stereo);
			
			if( theSound != 0L)
			{
				myErr = CallRPlaySoundUPP( theSound, GetPtrSize( theSound), 0, 0xFF, sS, lS, lE, rate, stereo);
				
				DisposePtr( theSound);
				theSound = 0L;
			}
		}
		break;*/
	
		case 'IMPL':
		{
			Ptr				theSound;
			long			lS, lE;
			short			sS;
			unsigned long	rate;
			Boolean			stereo;
		
			theSound = ConvertWAV( AlienFileFSSpec, &lS, &lE, &sS, &rate, &stereo);
			
			if( theSound != NULL)
				inAddSoundToMAD( theSound, lS, lE, sS, 60, rate, stereo, AlienFileFSSpec->name, InsHeader, sample, sampleID);
		}
		break;
		
		case 'TEST':
		{
			Ptr	theSound;
			
			myErr = FSpOpenDF( AlienFileFSSpec, fsCurPerm, &iFileRefI);
			if( myErr == noErr)
			{
				inOutBytes = 100L;
				theSound = NewPtr( inOutBytes);
				if( theSound == NULL) myErr = MADNeedMemory;
				else
				{
					FSRead( iFileRefI, &inOutBytes, theSound);
					
					myErr = TestWAV( (PCMWavePtr) theSound);
				}
				
				DisposePtr( theSound);
				
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
