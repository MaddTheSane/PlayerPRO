/*	SND				*/
/*  IMPORT/EXPORT	*/
/*	v 1.0			*/
/*	1996 by ANR		*/

//	Usage:

#include "PPPlug.h"
#include "sound.h"
#include "soundinput.h"

#if defined(powerc) || defined(__powerc)
enum {
		PlayerPROPlug = kCStackBased
		| RESULT_SIZE(SIZE_CODE( sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof( OSType)))
		| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof( InstrData*)))
		| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof( sData**)))
		| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof( short*)))
		| STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof( FSSpec*)))
		| STACK_ROUTINE_PARAMETER(6, SIZE_CODE(sizeof( PPInfoPlug*)))
};

ProcInfoType __procinfo = PlayerPROPlug;
#else
#include <A4Stuff.h>
#endif

void AddLoopToSndHandle( Handle sound, long Start, long End)
{
	Ptr 			soundPtr;
	short 			soundFormat;
	short 			numSynths, numCmds;
	long 			offset;
	SoundHeaderPtr 	header;
	CmpSoundHeader	*CmpHeader;
	ExtSoundHeader	*ExtHeader;
	OSErr 			result;
		
	/* make the sound safe to use at interrupt time. */
	HLock( sound);
	soundPtr = *sound;
	
	/* determine what format sound we have. */
	soundFormat = *(short*)soundPtr;
	
	switch(soundFormat)
	{
		case 1:						/* format 1 sound. */
			/* look inside the format 1 resource and deduce offsets. */
			numSynths = ((short*)soundPtr)[1];					/* get # synths. */
			numCmds = *(short*)(soundPtr+4+numSynths*6);		/* get # commands. */
		break;
		
		case 2:						/* format 2 sound. */
			numSynths = 0;			/* format 2 sounds have no synth's. */
			numCmds = ((short*)soundPtr)[2];
		break;
		
		default:					/* jack says, what about 12? or 6? */
			DebugStr("\p NSndToHandle... Burkk");
	} 

	/* compute address of sound header. */
	offset = 6 + 6*numSynths + 8*numCmds;
	header = (SoundHeaderPtr) (StripAddress(*sound) + offset);
		
	switch( header->encode)
	{
		case cmpSH:
			CmpHeader = (CmpSoundHeader*) header;
			if( CmpHeader->sampleSize == 16) { Start /= 2; End /= 2; }
			
			CmpHeader->loopStart	= Start;
			CmpHeader->loopEnd		= End;
		break;

		case extSH:
			ExtHeader = (ExtSoundHeader*) header;
			if( ExtHeader->sampleSize == 16) { Start /= 2; End /= 2; }
			
			ExtHeader->loopStart	= Start;
			ExtHeader->loopEnd		= End;
		break;
		
		case stdSH:
			header->loopStart	= Start;
			header->loopEnd		= End;
		break;
	}
	
	HUnlock( sound);
}

Ptr NSndToPtr( Ptr soundPtr, long *loopStart, long *loopEnd, short *sampleSize, unsigned long *sampleRate, long *baseFreq, Boolean *stereo)
{
	short 			soundFormat, numChannels;
	short 			numSynths, numCmds, CompressID;
	long 			offset, MusSize;
	SoundHeaderPtr 	header;
	CmpSoundHeader	*CmpHeader;
	ExtSoundHeader	*ExtHeader;
	SndCommand 		cmd;
	OSErr 			result;
	long			i,x, numFrames;
	Boolean			change = false;
	Str255			aStr;
	
	*loopStart = 0;
	*loopEnd = 0;
	*sampleSize = 8;
	*stereo = false;
	
	// determine what format sound we have.
	soundFormat = *(short*) soundPtr;
	
	switch( soundFormat)
	{
		case 1:						// format 1 sound.
			// look inside the format 1 resource and deduce offsets.
			numSynths = ((short*)soundPtr)[1];					// get # synths. 
			numCmds = *(short*)(soundPtr+4+numSynths*6);		// get # commands.
		break;
		
		case 2:						// format 2 sound. 
			numSynths = 0;			// format 2 sounds have no synth's. 
			numCmds = ((short*)soundPtr)[2];
		break;
		
		default:					// jack says, what about 12? or 6?
			DebugStr("\p NSndToHandle... Burkk");
		break;
	} 

	// compute address of sound header.
	offset = 6 + 6*numSynths + 8*numCmds;
	header = (SoundHeaderPtr) ( ((Ptr) soundPtr) + offset);
	
	switch( header->encode)
	{
		case cmpSH:
		{
			CompressionInfo 		cp;
			OSType					format;
			
			CmpHeader = (CmpSoundHeader*) header;
			CompressID = CmpHeader->compressionID;
			numChannels = CmpHeader->numChannels;			
			format = CmpHeader->format;
			
			*loopStart = CmpHeader->loopStart;
			*loopEnd = CmpHeader->loopEnd;
			*sampleSize = CmpHeader->sampleSize;
			if( numChannels == 2) *stereo = true;
			else *stereo = false;
			
			if( sampleRate != 0L) 	*sampleRate	= CmpHeader->sampleRate;
			if( baseFreq != 0L) 	*baseFreq 	= CmpHeader->baseFrequency;
			
			MusSize = (*CmpHeader).numFrames;
			
			result = GetCompressionInfo( (*CmpHeader).compressionID, (*CmpHeader).format, numChannels, *sampleSize, &cp);
			if (result != noErr)
			DebugStr("\pGetCompressionInfo");
			
			BlockMoveData( (*CmpHeader).sampleArea, soundPtr, (*CmpHeader).numFrames * cp.bytesPerFrame);
			
			{
			SoundConverter			sc;
			SoundComponentData		inputFormat, outputFormat;
			unsigned long			inputFrames, outputFrames;
			unsigned long			inputBytes, outputBytes;
			Ptr						inputPtr, outputPtr;
			OSErr					err;
			Ptr						dstPtr;
			
			inputFormat.flags = 0;
			inputFormat.format = format;
			inputFormat.numChannels = numChannels;
			inputFormat.sampleSize = *sampleSize;
			inputFormat.sampleRate = *sampleRate;
			inputFormat.sampleCount = 0;
			inputFormat.buffer = nil;
			inputFormat.reserved = 0;
			
			outputFormat = inputFormat;
			if( *sampleSize == 8) outputFormat.format = kOffsetBinary;
			else outputFormat.format = k16BitBigEndianFormat;
			
			err = SoundConverterOpen(&inputFormat, &outputFormat, &sc);
			if (err != noErr)
			DebugStr("\pOpen failed");
			
			err = SoundConverterBeginConversion(sc);
			if (err != noErr)
			DebugStr("\pBegin Conversion failed");
			
			inputFrames = MusSize;
			
			dstPtr = NewPtr( inputFrames * numChannels * (*sampleSize/8) * cp.samplesPerPacket);
			if( dstPtr == 0L)
			{
				DisposePtr( soundPtr);
				return 0L;
			}
			
			err = SoundConverterConvertBuffer(sc, soundPtr, inputFrames, dstPtr, &outputFrames, &outputBytes);
			if (err != noErr)
			DebugStr("\pConversion failed");
			
			MusSize = outputBytes;
			
			err = SoundConverterEndConversion(sc, dstPtr, &outputFrames, &outputBytes);
			if (err != noErr)
			DebugStr("\pEnd Conversion failed");
			
			if( outputBytes != 0) Debugger();
			
			err = SoundConverterClose(sc);
			if (err != noErr)
			DebugStr("\pClose failed");
			
			DisposePtr( soundPtr);
			soundPtr = dstPtr;
			}
		}
		break;
		
		case extSH:
			ExtHeader = (ExtSoundHeader*) header;
			
			MusSize = ExtHeader->numFrames;
			numChannels = ExtHeader->numChannels;
			
			*loopStart = ExtHeader->loopStart;
			*loopEnd = ExtHeader->loopEnd;
			*sampleSize = ExtHeader->sampleSize;
			
			if( sampleRate != 0L) 	*sampleRate	= ExtHeader->sampleRate;
			if( baseFreq != 0L) 	*baseFreq 	= ExtHeader->baseFrequency;
			
			if( numChannels == 2) *stereo = true;
			else *stereo = false;
			
			if( *stereo) MusSize *= 2;
			
			if( *sampleSize == 16)
			{
				MusSize *= 2;
				*loopStart *= 2;
				*loopEnd *= 2;
			}
			
			if( numChannels == 1) BlockMoveData( ExtHeader->sampleArea, soundPtr, MusSize);
			else if( numChannels == 2) BlockMoveData( ExtHeader->sampleArea, soundPtr, MusSize);
			else
			{
				if( *sampleSize == 8)
				{
					for( i = 0; i < MusSize; i ++)
					{
						soundPtr[ i] = ExtHeader->sampleArea[ i * numChannels];
					}
				}
				else
				{
					MusSize /= 2;
					for( i = 0; i < MusSize; i ++)
					{
						((short*) soundPtr)[ i] = ((short*) ExtHeader->sampleArea)[ i * numChannels];
					}
					MusSize *= 2;
				}
			}
			
		break;
		
		default:
		
		case stdSH:
			*loopStart = header->loopStart;
			*loopEnd = header->loopEnd;
			
			if( sampleRate != 0L) 	*sampleRate	= header->sampleRate;
			if( baseFreq != 0L) 	*baseFreq 	= header->baseFrequency;
			
			MusSize = header->length;
			BlockMoveData( (*header).sampleArea, soundPtr, MusSize);
		break;
	}
	
	if( *sampleSize == 8)
	{
		ConvertInstrumentIn( (Byte*) soundPtr, MusSize);
	}
	
	SetPtrSize( soundPtr, MusSize);
	
	if( *loopEnd - *loopStart < 4) { *loopEnd = 0;	*loopStart = 0;}
	
	return soundPtr;
}

OSErr TestSND( short *soundPtr)
{
	if( *soundPtr == 1 || *soundPtr == 2) return noErr;
	else return MADFileNotSupportedByThisPlug;
}

OSErr main(		OSType					order,						// Order to execute
				InstrData				*InsHeader,					// Ptr on instrument header
				sData					**sample,					// Ptr on samples data
				short					*sampleID,					// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
																	// If sampleID == -1 : add sample else replace selected sample.
				FSSpec					*AlienFileFSSpec,			// IN/OUT file
				PPInfoPlug				*thePPInfoPlug)
{
	OSErr	myErr;
	Ptr		AlienFile;
	short	iFileRefI;
	long	inOutBytes;
	
	#ifndef powerc
		long	oldA4 = SetCurrentA4(); 			//this call is necessary for strings in 68k code resources
	#endif
	
	switch( order)
	{
		case 'IMPL':
		{
			Ptr				theSound;
			long			lS, lE, bFreq;
			short			sS;
			unsigned long	rate;
			Boolean			stereo;
			
			myErr = FSpOpenDF( AlienFileFSSpec, fsCurPerm, &iFileRefI);
			if( myErr == noErr)
			{
				GetEOF( iFileRefI, &inOutBytes);
				
				theSound = NewPtr( inOutBytes);
				if( theSound == 0L) myErr = MADNeedMemory;
				else
				{
					FSRead( iFileRefI, &inOutBytes, theSound);
					
					theSound = NSndToPtr( theSound, &lS, &lE, &sS, &rate, &bFreq, &stereo);
					
					if( theSound) inAddSoundToMAD( theSound, lS, lE, sS, bFreq, rate, stereo, AlienFileFSSpec->name, InsHeader, sample, sampleID);
					else
					{
						myErr = MADNeedMemory;
					}
				}
				
				FSClose( iFileRefI);
			}
		}
		break;
		
		case 'TEST':
		{
			Ptr	theSound;
			
			myErr = FSpOpenDF( AlienFileFSSpec, fsCurPerm, &iFileRefI);
			if( myErr == noErr)
			{
				inOutBytes = 50L;
				theSound = NewPtr( inOutBytes);
				if( theSound == 0L) myErr = MADNeedMemory;
				else
				{
					FSRead( iFileRefI, &inOutBytes, theSound);
					
					myErr = TestSND( (short*) theSound);
				}
				
				DisposePtr( theSound);
				
				FSClose( iFileRefI);
			}
		}
		break;
		
/*		case 'EXPL':
			if( *sampleID >= 0)
			{
				sData 	*curData = sample[ *sampleID];
				short	temp, fRefNum;
				Handle	tempHandle;
				
				tempHandle = NewHandle( 2048L);
				
				inOutBytes = curData->size;
				
				SetupSndHeader(		(SndListHandle) tempHandle,
									1,
									((unsigned long) curData->c2spd)<<16L,
									curData->amp,
									'NONE',
									60 - curData->relNote,
									inOutBytes,
									&temp);
				
				SetHandleSize( tempHandle, inOutBytes + temp);
				
				HLock( tempHandle);
				BlockMoveData( curData->data, *tempHandle + temp, inOutBytes);
				HUnlock( tempHandle);
				
				AddLoopToSndHandle(	tempHandle,
									curData->loopBeg,
									curData->loopBeg + curData->loopSize);
				
				FSpCreateResFile( AlienFileFSSpec, 'movr', 'snd ', smCurrentScript);
				fRefNum = FSpOpenResFile( AlienFileFSSpec, fsCurPerm);
				UseResFile( fRefNum);
				AddResource( tempHandle, 'snd ', 7438, AlienFileFSSpec->name);
				WriteResource( tempHandle);
				DetachResource( tempHandle);
				
				DisposeHandle( tempHandle);
				
				CloseResFile( fRefNum);
			}
		break;*/
		
		default:
			myErr = MADOrderNotImplemented;
		break;
	}
	
	#ifndef powerc
		SetA4( oldA4);
	#endif
	
	return myErr;
}