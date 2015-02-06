/********************						***********************/
//
//	Player PRO 4.5x --	Usefull functions for sound file import/export
//
//	Version 3.0
//
//	To use with PlayerPRO & CodeWarrior (current vers 8)
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

#include "RDriver.h"
#include "MADPlug.h"
#include "RDriverInt.h"
#include "MADPrivate.h"
#include <alloca.h>

sData* inMADCreateSample()
{
	sData *curData;
	
	curData = (sData*)calloc(sizeof(sData), 1);
	
	if (curData == NULL)
		return NULL;
	
	curData->vol		= MAX_VOLUME;
	curData->c2spd		= NOFINETUNE;
	curData->amp		= 8;
	curData->data		= NULL;
	
	return curData;
}

sData *MADCreateSampleRaw()
{
	return inMADCreateSample();
}

Cmd* MADGetCmd(short row, short track, Pcmd* myPcmd)
{
	if (row < 0) {
		row = 0;
	} else if (row >= myPcmd->length) {
		row = myPcmd->length - 1;
	}
	
	if (track < 0) {
		track = 0;
	} else if (track >= myPcmd->tracks) {
		track = myPcmd->tracks - 1;
	}
	
	return &(myPcmd->myCmd[(myPcmd->length * track) + row]);
}

MADErr inAddSoundToMAD(void			*theSound,
					   size_t		sndLen,
					   long			lS,
					   long			lE,
					   short		sS,
					   short		bFreq,
					   unsigned int	rate,
					   bool			stereo,
					   Str255		name,
					   InstrData	*InsHeader,					// Ptr on instrument header
					   sData		**sample,					// Ptr on samples data
					   short		*sampleID)
{
	MADErr theErr = MADNoErr;
	char *cName = alloca(name[0] + 1);
	if (!cName)
		return MADNeedMemory;
	memset(cName, 0, name[0] + 1);
	memcpy(cName, &name[1], name[0]);
	
	theErr = MADAddSoundToMAD(theSound, sndLen, (int)lS, (int)lE, sS, bFreq, rate, stereo, cName, InsHeader, sample, sampleID);
	
	return theErr;
}

IntPcmd *MADPcmdToInt(Pcmd *inVal)
{
	IntPcmd *toRet = malloc(sizeof(IntPcmd));
	toRet->tracks = inVal->tracks;
	toRet->length = inVal->length;
	toRet->trackStart = inVal->trackStart;
	toRet->posStart = inVal->posStart;
	toRet->cmdCount = (inVal->structSize - sizeof(Pcmd)) / sizeof(Cmd);
	size_t ourSize = inVal->structSize - sizeof(Pcmd);
	toRet->myCmd = malloc(ourSize);
	memcpy(toRet->myCmd, inVal->myCmd, ourSize);
	
	return toRet;
}

Pcmd *MADIntPcmdToPcmd(IntPcmd *inVal, bool freeIntPcmd)
{
	size_t stuctSize = inVal->cmdCount * sizeof(Cmd) + sizeof(Pcmd);
	Pcmd *toRet = malloc(stuctSize);
	toRet->tracks = inVal->tracks;
	toRet->length = inVal->length;
	toRet->trackStart = inVal->trackStart;
	toRet->posStart = inVal->posStart;
	toRet->structSize = (int)stuctSize;
	memcpy(toRet->myCmd, inVal->myCmd, inVal->cmdCount * sizeof(Cmd));
	if (freeIntPcmd) {
		MADFreeIntPcmd(inVal);
	}
	
	return toRet;
}

MADErr MADCopyPcmdToPcmd(Pcmd* toCopy, Pcmd** outCopy)
{
	if (!toCopy || !outCopy || !*outCopy) {
		return MADParametersErr;
	}
	
	Pcmd *toRet = realloc(*outCopy, toCopy->structSize);
	if (!toRet) {
		toRet = malloc(toCopy->structSize);
		if (!toRet) {
			return MADNeedMemory;
		}
		free(*outCopy);
	}
	
	memcpy(toRet, toCopy, toCopy->structSize);
	*outCopy = toRet;
	
	return MADNoErr;
}

void MADFreeIntPcmd(IntPcmd *toDelete)
{
	if (toDelete->myCmd) {
		free(toDelete->myCmd);
	}
	
	free(toDelete);
}

MADErr MADAddSoundToMAD(void			*theSound,
						size_t			sndLen,
						int				loopStart,
						int				loopEnd,
						short			sS,
						short			bFreq,
						unsigned int	rate,
						bool			stereo,
						char			*name,
						InstrData		*InsHeader,			// Ptr on instrument header
						sData			**sample,			// Ptr on samples data
						short			*sampleID)
{
	long 	inOutBytes;
	sData	*curData;
	
	if (theSound == NULL || !sampleID || !InsHeader ||
		!sample || *sampleID > MAXSAMPLE)
		return MADParametersErr;
	
	inOutBytes = sndLen;
	
	///////
	
	if (*sampleID >= 0) {		// replace this sample
		curData = sample[*sampleID];
	} else {					// add a sample : why? because *sampleID == -1
		*sampleID = InsHeader->numSamples;
		InsHeader->numSamples++;
		
		curData = sample[*sampleID] = inMADCreateSample();
		
		if (!curData)
			return MADNeedMemory;
	}
	
	if (curData->data != NULL)
		free(curData->data);
	curData->data = theSound;
	
	curData->size		= (int)inOutBytes;
	curData->loopBeg	= loopStart;
	curData->loopSize	= loopEnd - loopStart;
	curData->vol		= 64;
	curData->amp		= sS;
	curData->c2spd		= rate;
	curData->relNote	= 60 - bFreq;
	curData->stereo		= stereo;
	
	strlcpy(curData->name, name, sizeof(curData->name));
	
	return MADNoErr;
}

void MADConvertInstrument(Byte *tempPtr, size_t sSize)
{
	ConvertInstrumentIn(tempPtr, sSize);
}

void MADConvertInstrument16(short *tempPtr, size_t sSize)
{
	ConvertInstrumentIn16(tempPtr, sSize);
}
