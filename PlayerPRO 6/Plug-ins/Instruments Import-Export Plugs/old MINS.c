/*	MINS			*/
/*  IMPORT/EXPORT	*/
/*	v 1.0			*/
/*	1996 by ANR		*/

#include "PPPlug.h"
#include <Sound.h>

struct oldInstrData				// INSTRUMENT
{
	char 	name[ 32];			// instrument name
	Byte 	type;				// Instrument type = 0
	Byte	no;					// Instrument number
	
	short	firstSample;		// First sample ID in sample list
	short	numSamples;			// Number of samples in instrument
	
//	short	MIDI;				// MIDI Channel Value

//	short	MIDIType;			// 0 Sound output only, 1 MIDI output only, 2 both
	
	/**/
	
	Byte	what[ 96];			// Sample number for all notes
	EnvRec 	volEnv[ 12];		// Points for volume envelope
	EnvRec	pannEnv[ 12];		// Points for panning envelope
//	EnvRec	pitchEnv[ 12];		// Points for panning envelope
// ENVELOPPES PLUS LONGUES !!!!!!!
	
	Byte	volSize;			// Number of volume points
	Byte	pannSize;			// Number of panning points
//	Byte	pitchSize;			// Number of panning points
	
	Byte	volSus;				// Volume sustain point
	Byte	volBeg;				// Volume loop start point
	Byte	volEnd;				// Volume loop end point
	
	Byte	pannSus;			// Panning sustain point
	Byte	pannBeg;			// Panning loop start point
	Byte	pannEnd;			// Panning loop end point
	
//	Byte	pitchSus;			// Pitch sustain point
//	Byte	pitchBeg;			// Pitch loop start point
//	Byte	pitchEnd;			// Pitch loop end point
	
	Byte	volType;			// Volume type: bit 0: On; 1: Sustain; 2: Loop
	Byte	pannType;			// Panning type: bit 0: On; 1: Sustain; 2: Loop
	
	unsigned short	volFade;	// Volume fadeout
	
	Byte	vibDepth;
	Byte	vibRate;
};
typedef struct oldInstrData oldInstrData;


OSErr TestMINS( oldInstrData *CC)
{
	if (CC->type == 0 && CC->numSamples >= 0 && CC->numSamples < MAXSAMPLE) return noErr;
	else return MADFileNotSupportedByThisPlug;
}

OSErr MAD2KillInstrument( InstrData *curIns, sData **sample)
{
	short		i;
	Boolean		IsReading;

	for (i = 0; i < curIns->numSamples; i++)
	{
		if (sample[ i] != 0L)
		{
			if (sample[ i]->data != 0L)
			{
				DisposePtr( (Ptr) sample[ i]->data);
				sample[ i]->data = 0L;
			}
			DisposePtr( (Ptr) sample[ i]);
			sample[ i] = 0L;
		}
	}
	
	
	for (i = 0; i < 32; i++) curIns->name[ i]	= 0;
	curIns->type		= 0;
	curIns->numSamples	= 0;
	
	/**/
	
	for (i = 0; i < 96; i++) curIns->what[ i]		= 0;
	for (i = 0; i < 12; i++)
	{
		curIns->volEnv[ i].pos		= 0;
		curIns->volEnv[ i].val		= 0;
	}
	for (i = 0; i < 12; i++)
	{
		curIns->pannEnv[ i].pos	= 0;
		curIns->pannEnv[ i].val	= 0;
	}
	for (i = 0; i < 12; i++)
	{
		curIns->pitchEnv[ i].pos	= 0;
		curIns->pitchEnv[ i].val	= 0;
	}
	curIns->volSize		= 0;
	curIns->pannSize	= 0;
	
	curIns->volSus		= 0;
	curIns->volBeg		= 0;
	curIns->volEnd		= 0;
	
	curIns->pannSus		= 0;
	curIns->pannBeg		= 0;
	curIns->pannEnd		= 0;

	curIns->volType		= 0;
	curIns->pannType	= 0;
	
	curIns->volFade		= 0;
	curIns->vibDepth	= 0;
	curIns->vibRate		= 0;
	
	return noErr;
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
	short	iFileRefI, x;
	long	inOutCount;
	
	switch( order)
	{
		case 'IMPL':
		{
			Ptr				theSound;
			
			myErr = FSpOpenDF( AlienFileFSSpec, fsCurPerm, &iFileRefI);
			if (myErr == noErr)
			{
				GetEOF( iFileRefI, &inOutCount);
				
				theSound = NewPtr( inOutCount);
				if (theSound == 0L) myErr = MADNeedMemory;
				else
				{
					DisposePtr( theSound);
					
					MAD2KillInstrument( InsHeader, sample);
					
					// READ instrument header
					
					inOutCount = sizeof( oldInstrData);
					
					{
						oldInstrData		oldData;
						short				i;
						
						
						myErr = FSRead( iFileRefI, &inOutCount, &oldData);
						
						BlockMoveData( oldData.name, InsHeader->name, 32);
						InsHeader->type = oldData.type;
						InsHeader->no = oldData.no;
	
						InsHeader->firstSample = oldData.firstSample;
						InsHeader->numSamples = oldData.numSamples;
						BlockMoveData( oldData.what, InsHeader->what, 96);
						
						for (i = 0; i < 12; i++)
						{
							InsHeader->volEnv[ i] = oldData.volEnv[ i];
							InsHeader->pannEnv[ i] = oldData.pannEnv[ i];
						}
						
						InsHeader->volSize = oldData.volSize;
						InsHeader->pannSize = oldData.pannSize;
						InsHeader->volSus = oldData.volSus;
						InsHeader->volBeg = oldData.volBeg;
						InsHeader->volEnd = oldData.volEnd;
	
						InsHeader->pannSus = oldData.pannSus;
						InsHeader->pannBeg = oldData.pannBeg;
						InsHeader->pannEnd = oldData.pannEnd;
	
						InsHeader->volType = oldData.volType;
						InsHeader->pannType = oldData.pannType;
	
						InsHeader->volFade = oldData.volFade;
	
						InsHeader->vibDepth = oldData.vibDepth;
						InsHeader->vibRate = oldData.vibRate;

					}
					
					// READ samples headers & data
					
					for (x = 0; x < InsHeader->numSamples; x++)
					{
						sData *curData = sample[ x] = inMADCreateSample();
						
						inOutCount = sizeof( sData);
						
						myErr = FSRead( iFileRefI, &inOutCount, curData);
						
						curData->data = NewPtr( curData->size);
						if (curData->data != 0L)
						{
							inOutCount = curData->size;
							myErr = FSRead( iFileRefI, &inOutCount, curData->data);
						}
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
			if (myErr == noErr)
			{
				inOutCount = 50L;
				theSound = NewPtr( inOutCount);
				if (theSound == 0L) myErr = MADNeedMemory;
				else
				{
					FSRead( iFileRefI, &inOutCount, theSound);
					
					myErr = TestMINS( (oldInstrData*) theSound);
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
