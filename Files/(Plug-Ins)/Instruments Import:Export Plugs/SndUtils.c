/********************						***********************/
//
//	Player PRO 4.5x --	Usefull functions for sound file import/export
//
//	Version 3.0
//
//	To use with PlayerPRO & CodeWarrior ( current vers 8)
//
//	Antoine ROSSET
//	16 Tranchees
//	1206 GENEVA
//	SWITZERLAND
//	
//	FAX:			(+41 22) 346 11 97
//	Compuserve:		100277,164
//	Phone:			(+41 79) 203 74 62
//	Internet: 		rosset@dial.eunet.ch
/********************						***********************/

#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>

#if 0
void ConvertInstrumentIn( register Byte *tempPtr, register long sSize)
{
	register Byte val = 0x80;

	while( sSize > 0)
	{
		sSize--;
		*(tempPtr + sSize) -= val;
	}
}
#endif

sData* inMADCreateSample()
{
	sData	*curData;

	curData = (sData*) calloc( sizeof( sData), 1);
	
	if (curData == NULL) {
		return NULL;
	}
	
	curData->size		= 0;
	curData->loopBeg	= 0;
	curData->loopSize	= 0;
	curData->vol		= MAX_VOLUME;
	curData->c2spd		= NOFINETUNE;
	curData->loopType	= 0;
	curData->amp		= 8;
	curData->relNote	= 0;
	curData->data		= NULL;
	
	return curData;		
}

OSErr inAddSoundToMAD(Ptr			theSound,
					  size_t		sndLen,
					  long			lS,
					  long			lE,
					  short			sS,
					  short			bFreq,
					  unsigned int	rate,
					  Boolean		stereo,
					  Str255		name,
					  InstrData		*InsHeader,					// Ptr on instrument header
					  sData			**sample,					// Ptr on samples data
					  short			*sampleID)
{
	OSErr theErr = noErr;
	char *cName = malloc(name[0] + 1);
	memcpy(cName, &name[1], name[0]);
	cName[name[0]] = '\0';
	
	theErr = inAddSoundToMADCString(theSound, sndLen, lS, lE, sS, bFreq, rate, stereo, cName, InsHeader, sample, sampleID);
	
	free(cName);
	return theErr;
}


OSErr inAddSoundToMADCString(Ptr			theSound,
							 size_t			sndLen,
							 long			lS,
							 long			lE,
							 short			sS,
							 short			bFreq,
							 unsigned int	rate,
							 Boolean		stereo,
							 char			*name,
							 InstrData		*InsHeader,					// Ptr on instrument header
							 sData			**sample,					// Ptr on samples data
							 short			*sampleID)
{
	long 	inOutBytes;
	sData	*curData;

	if( theSound == NULL) return MADParametersErr;

	if (!sampleID || !InsHeader || !sample) {
		return MADParametersErr;
	}
	if( *sampleID > MAXSAMPLE) return MADParametersErr;

	inOutBytes = sndLen;
	
	///////
	
	if( *sampleID >= 0)		// replace this sample
	{
		curData = sample[ *sampleID];
	}
	else					// add a sample : why? because *sampleID == -1
	{
		*sampleID = InsHeader->numSamples;
		InsHeader->numSamples++;
		
		curData = sample[ *sampleID] = inMADCreateSample();
		
		if (!curData) {
			return MADNeedMemory;
		}
	}
	
	if( curData->data != NULL) free( curData->data);
	curData->data = theSound;
	
	curData->size		= inOutBytes;
	curData->loopBeg	= lS;
	curData->loopSize	= lE - lS;
	curData->vol		= 64;
	curData->amp		= sS;
	curData->c2spd		= rate;
	curData->relNote	= 60 - bFreq;
	curData->stereo		= stereo;
	
	strlcpy(curData->name, name, 32);
	
	return noErr;
}
