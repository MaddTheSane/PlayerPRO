/********************						***********************/
//
//	Player PRO 5.9 - DRIVER SOURCE CODE -
//
//	Library Version 5.9
//
//	To use with MAD Library for Mac: Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	20 Micheli-Du-Crest
//	1205 GENEVA
//	SWITZERLAND
//
//	COPYRIGHT ANTOINE ROSSET 1996, 1997, 1998, 1999, 2000, 2001, 2002
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:			(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 		RossetAntoine@bluewin.ch
//
/********************						***********************/

#include "RDriver.h"
#include "RDriverInt.h"
#include "MADFileUtils.h"
#include <string.h>
#include "MADPrivate.h"
#include "VSTFunctions.h"
#ifdef _MAC_H
#include <CoreFoundation/CoreFoundation.h>
#else
#ifdef __BLOCKS__
#include <dispatch/dispatch.h>
#endif
typedef void *CFReadStreamRef;
#endif

///////////////////////////////

static inline void ByteSwapsData(sData *toSwap);
static inline void SwapFXSets(FXSets *set);
static inline void ByteSwapInstrData(InstrData *toSwap);
static inline void ByteSwapMADSpec(MADSpec *toSwap);
static inline void ByteSwapPatHeader(PatHeader *toSwap);

typedef enum InputType {
	MADFileType = 1,
#ifdef _MAC_H
	MADCFReadStreamType = 2,
#endif
	MADPtrType = 3
} MADInputType;

static MADErr MADReadMAD(MADMusic **music, UNFILE srcFile, MADInputType InPutType, CFReadStreamRef MADRsrc, char *MADPtr);

static inline unsigned char* MYC2PStr(char *cStr)
{
	long size = strlen(cStr);
	memmove(cStr + 1, cStr, size);
	cStr[0] = size;
	
	return (unsigned char*) cStr;
}

static inline char *MYP2CStr(unsigned char *pStr)
{
	long size = pStr[0];
	memmove(pStr, pStr + 1, size);
	pStr[size] = 0;
	
	return (char*)pStr;
}

MADMusic* CreateFreeMADK()
{
	MADMusic	*music = (MADMusic*)calloc(sizeof(MADMusic), 1);
	MADSpec		*MADI;
	short		i, x, z;
	Cmd			*aCmd;
	if (music == NULL)
		return NULL;
	// ******* HEADER *********
	
	MADI = (MADSpec*)calloc(sizeof(MADSpec), 1);
	if (MADI == NULL)
	{
		free(music);
		return NULL;
	}
	
	MADI->MAD					= MADID;
	MADI->numPat				= 1;
	MADI->numPointers			= 1;
	MADI->numInstru				= 0;
	MADI->numChn				= 4;
	MADI->speed					= 6;
	MADI->tempo					= 125;
	
	MADI->EPitch 				= 8000;
	MADI->ESpeed 				= 8000;
	MADI->generalVol			= 64;
	MADI->generalPitch			= 252;
	MADI->generalSpeed			= 252;
	MADI->generalPan			= 64;
	MADI->XMLinear = false;
	
	music->header	= MADI;
	
	for (i = 0; i < MAXTRACK; i++) {
		if (i % 2 == 0) MADI->chanPan[i] = MAX_PANNING/4;
		else MADI->chanPan[i] = MAX_PANNING - MAX_PANNING/4;
		
		MADI->chanVol[i] = MAX_VOLUME;
	}
	
	for (i = 0; i < MAXTRACK; i++) {
		MADI->chanBus[i].copyId = i;
	}
	
	music->fid = (InstrData*)calloc(sizeof(InstrData), MAXINSTRU);
	if (music->fid == NULL) {
		free(MADI);
		free(music);
		return NULL;
	}
	
	music->sample = (sData**) calloc(sizeof(sData*), MAXINSTRU * MAXSAMPLE);
	if (music->sample == NULL) {
		free(music->fid);
		free(MADI);
		free(music);
		return NULL;
	}
	// ******* PARTITION *********
	
	for (i = 0; i < MADI->numPat; i++) {
		music->partition[i] = (PatData*)calloc(sizeof(PatHeader) + MADI->numChn * 64 * sizeof(Cmd), 1);
		if (music->partition[i] == NULL) {
			for(i = 0; i < MADI->numPat; i++) {
				if (music->partition[i] != NULL)
					free(music->partition[i]);
			}
			free(music->fid);
			free(music->sample);
			free(MADI);
			free(music);
			return NULL;
		}
		
		
		music->partition[i]->header.size 		= 64;
		music->partition[i]->header.compMode 	= 'NONE';
		music->partition[i]->header.patBytes 	= 0;
		music->partition[i]->header.unused2 	= 0;
		
		for (x = 0; x < 64; x++) {
			for (z = 0; z < music->header->numChn; z++) {
				aCmd = GetMADCommand(x,  z, music->partition[i]);
				
				MADKillCmd(aCmd);
			}
		}
	}
	for (i = music->header->numPat; i < MAXPATTERN; i++)
		music->partition[i] = NULL;
	
	music->sets = (FXSets*)calloc(sizeof(FXSets), MAXTRACK);
	
	return music;
}

size_t MADGetMusicSize(const MADMusic *music)
{
	int i, x;
	size_t fileSize;
	
	if (music->header == NULL)
		return 0;
	
	fileSize = sizeof(MADSpec);
	fileSize += music->header->numInstru * sizeof(InstrData);
	for (i = 0; i < music->header->numPat; i++)
		fileSize += sizeof(PatHeader) + music->header->numChn * music->partition[i]->header.size * sizeof(Cmd);
	for (i = 0; i < music->header->numInstru ; i++) {
		for (x = 0; x < music->fid[i].numSamples ; x++) {
			sData	*curData = music->sample[i * MAXSAMPLE + x];
			
			fileSize += sizeof(sData32);
			fileSize += curData->size;
		}
	}
	//TODO: FXSets
	
	return fileSize;
}

void ConvertTo64Rows(MADMusic *music)
{
	int		i, x, z;
	bool	IsReading;
	int		patID, found;
	
	if (music->header == NULL)
		return;
	
	IsReading = music->musicUnderModification;
	music->musicUnderModification = true;
	
	for (i = music->header->numPat-1; i >= 0 ; i--) {
		// Resize pattern to 64 rows and put a pattern break
		
		int		newSize;
		PatData	*newPat = NULL;
		
		patID = i;
		
		newSize = sizeof(PatHeader) + music->header->numChn * 64L * sizeof(Cmd);
		
		if (music->partition[i]->header.size < 64) {
			Cmd *srccmd = NULL, *dstcmd = NULL;
			int patsize;
			
			newPat = (PatData*)calloc(newSize, 1);
			
			newPat->header.size		= 64;
			newPat->header.compMode	= 'NONE';
			newPat->header.patBytes	= 0;
			newPat->header.unused2	= 0;
			
			memcpy(newPat->header.name, music->partition[i]->header.name, 32);
			
			// Upgrade length to 64
			
			for (x = 0, patsize = 0; x < 64; x++, patsize++) {
				for (z = 0; z < music->header->numChn; z++) {
					if (x < music->partition[i]->header.size) {
						// src
						srccmd = GetMADCommand(patsize, z, music->partition[i]);
						
						// dst
						dstcmd = GetMADCommand(x, z, newPat);
						if (dstcmd && srccmd) {
							*dstcmd = *srccmd;
						}
					}
					else
					{
						dstcmd = GetMADCommand(x, z, newPat);
						
						MADKillCmd(dstcmd);
					}
				}
			}
			
			// dst
			dstcmd = GetMADCommand(music->partition[i]->header.size - 1, 0, newPat);		// Pose le pattern break
			
			MADKillCmd(dstcmd);
			
			dstcmd->cmd = MADEffectSkip;
			dstcmd->arg = 0;
			
			// Replace old pattern
			
			free(music->partition[i]);
			music->partition[i] = newPat;
		} else if (music->partition[i]->header.size > 64) {
			int			patsize = 0;
			PatData*	srcPat = music->partition[i];
			
			while (patsize < srcPat->header.size) {
				newPat = (PatData*)calloc(newSize, 1);
				
				newPat->header.size		= 64;
				newPat->header.compMode	= 'NONE';
				newPat->header.patBytes	= 0;
				newPat->header.unused2	= 0;
				
				strncpy(newPat->header.name, srcPat->header.name, 32);
				
				for (x = 0; x < 64; x++, patsize++) {
					for (z = 0; z < music->header->numChn; z++) {
						Cmd		*srccmd, *dstcmd;
						
						if (patsize < srcPat->header.size) {
							// src
							srccmd = GetMADCommand(patsize, z, srcPat);
							
							// dst
							dstcmd = GetMADCommand(x, z, newPat);
							
							*dstcmd = *srccmd;
						} else {
							dstcmd = GetMADCommand(x, z, newPat);
							MADKillCmd(dstcmd);
						}
					}
				}
				
				// Update patterns list & partition
				
				if (patID != i) {
					for (x = music->header->numPat; x > patID ; x--)
						music->partition[x] = music->partition[x - 1];
					music->header->numPat++;
				}
				
				for (x = music->header->numPat; x < MAXPATTERN; x++)
					music->partition[x] = NULL;
				
				music->partition[patID] = newPat;
				
				patID++;
			}
			patID--;
			
			
			// Do we need a pattern break ?
			
			if ((srcPat->header.size / 64) * 64 != srcPat->header.size) {
				short	breakpos;
				Cmd		*dstcmd;
				
				breakpos = srcPat->header.size - (srcPat->header.size / 64) * 64;
				
				dstcmd = GetMADCommand(breakpos-1, 0, newPat);
				MADKillCmd(dstcmd);
				if (dstcmd) {
					dstcmd->cmd = MADEffectSkip;
					dstcmd->arg = 0;
				}
			}
			
			// Update la partition list
			
			for (x = 0; x < music->header->numPointers ; x++) {
				if (music->header->oPointers[x] > i)
					music->header->oPointers[x] += patID - i;
			}
			
			found = 0;
			
			for (x = music->header->numPointers-1; x >= 0 ; x--) {
				if (music->header->oPointers[x] == i) {
					found++;
					
					// Avance le reste de la partition
					
					for (z = 256-1; z >= x ; z--) {
						if (z + patID - i < 256) {
							music->header->oPointers[z + patID - i] = music->header->oPointers[z];
						}
					}
					
					for (z = 0; z <= patID - i; z++)
						music->header->oPointers[x + z] = i + z;
				}
			}
			
			music->header->numPointers += found * (patID - i);
			
			free(srcPat);
		}
	}
	
	music->musicUnderModification = IsReading;
}

size_t MADMinimize(MADMusic *music)
{
	short 		i, x, z;
	bool		remove, IsReading;
	bool		inst[MAXINSTRU];
	size_t		before, after;
	
	if (music->header == NULL) return 0;
	
	IsReading = music->musicUnderModification;
	music->musicUnderModification = true;
	
	// Check size before
	
	before = MADGetMusicSize(music);
	
	// Check for unused patterns
	
	if (music->header->numPat > 1) {
		for (i = 0; i < music->header->numPat; i++) {
			remove = true;
			for (x = 0; x < music->header->numPointers; x++) {
				if (music->header->oPointers[x] == i) {
					remove = false;
					break;
				}
			}
			
			if (remove) {
				music->header->numPat--;
				
				free(music->partition[i]);
				music->partition[i] = NULL;
				
				for (x = i; x < music->header->numPat; x++) {
					music->partition[x] = music->partition[x + 1];
				}
				for (x = music->header->numPat; x < MAXPATTERN; x++)
					music->partition[x] = NULL;
				
				/***** Update la partition list ******/
				
				for (x = 0; x < 128; x++) {
					if (music->header->oPointers[x] > i)
						music->header->oPointers[x]--;
					else if (music->header->oPointers[x] == i)
						music->header->oPointers[x] = 0;
				}
				
				if (i > 0)
					i--;
			}
		}
	}
	
	// Check for unused instruments
	for (i = 0; i < 32 ; i++)
		inst[i] = false;
	
	for (i = 0; i < music->header->numPat; i++) {
		for (x = 0; x < music->partition[i]->header.size; x++) {
			for (z = 0; z < music->header->numChn; z++) {
				Cmd		*cmd;
				
				cmd = GetMADCommand(x, z, music->partition[i]);
				
				if (cmd->ins > 0)
					inst[cmd->ins - 1] = true;
			}
		}
	}
	
	for (i = 0; i < MAXINSTRU; i++) {
		if (inst[i] == false)
			MADKillInstrument(music, i);
	}
	
	// Check for after loop data
	for (i = 0; i < MAXINSTRU; i++) {
		for (x = 0; x < music->fid[i].numSamples; x++) {
			sData	*curData = music->sample[i * MAXSAMPLE + x];
			
			if (curData->loopSize > 2) {
				if (curData->size > curData->loopBeg + curData->loopSize) {
					curData->data = realloc(curData->data, curData->loopBeg + curData->loopSize);
					curData->size = curData->loopBeg + curData->loopSize;
				}
			}
		}
	}
	
	music->musicUnderModification = IsReading;
	after = MADGetMusicSize(music);
	return before - after;
}

#pragma mark - Driver list generation and checking
static MADSoundOutputBit driverList = 0;
static bool driverlistInited = false;

static void BuildAvailableDriverList()
{
	if (driverlistInited == false) {
		driverList = MIDISoundDriverBit |
#ifdef _BE_H
		BeOSSoundDriverBit |
#endif
#ifdef WIN32
		DirectSound95NTBit | Wave95NTBit |
#endif
#ifdef _MAC_H
		CoreAudioDriverBit |
#endif
#ifdef _ESOUND
		ESDDriverBit |
#endif
		0;
		
		driverlistInited = true;
	}
}

bool MADSoundDriverIsAvalable(MADSoundOutput theDriver)
{
	if (theDriver == NoHardwareDriver) {
		return true;
	} else if (theDriver > 31) {
		return false;
	}
	BuildAvailableDriverList();
	if (driverList & (1 << theDriver)) {
		return true;
	} else {
		return false;
	}
}

MADSoundOutputBit MADSoundDriverList()
{
	BuildAvailableDriverList();
	return driverList;
}
#pragma mark -

void MADGetBestDriver(MADDriverSettings *Init)
{
	memset(Init->additionalSettings.padding, 0, sizeof(Init->additionalSettings));
	Init->outPutBits		= 16;
	Init->outPutMode		= DeluxeStereoOutPut;
	Init->outPutRate		= 44100;
	Init->numChn			= 4;
	Init->surround			= false;
	Init->repeatMusic		= true;
	Init->MicroDelaySize	= 25;
	Init->Reverb			= false;
	Init->ReverbSize		= 100;
	Init->ReverbStrength	= 20;
	Init->oversampling		= 1;
	Init->TickRemover		= false;
#ifdef _MAC_H
	//Just going to use CoreAudio
	Init->driverMode		= CoreAudioDriver;
#elif defined(WIN32)
	Init->driverMode		= Wave95NT;
#else
#warning no default audio device set!
	//Fall-through: If we reach this, it's probably becasue we don't have an audio device for this platform.
	Init->driverMode		= NoHardwareDriver;
#endif
}

MADErr MADCreateMicroDelay(MADDriverRec *intDriver)
{
	intDriver->MDelay = (intDriver->DriverSettings.MicroDelaySize * (intDriver->DriverSettings.outPutRate)* intDriver->DriverSettings.oversampling) / 1000;
	
	return MADNoErr;
}

void MADDisposeReverb(MADDriverRec *intDriver)
{
	if (intDriver->DriverSettings.Reverb)
		free(intDriver->ReverbPtr);
	intDriver->ReverbPtr = NULL;
}

MADErr MADCreateReverb(MADDriverRec *intDriver)
{
	if (intDriver->DriverSettings.Reverb) {
		intDriver->RDelay = (intDriver->DriverSettings.ReverbSize * (intDriver->DriverSettings.outPutRate)) / 1000;
		
		switch (intDriver->DriverSettings.outPutBits) {
			case 8:
				intDriver->ReverbPtr = (char*)malloc(intDriver->RDelay * 2L);
				if (intDriver->ReverbPtr == NULL)
					return MADNeedMemory;
				memset(intDriver->ReverbPtr, 0x80, intDriver->RDelay * 2);
				break;
				
			case 16:
				intDriver->ReverbPtr = (char*)calloc(intDriver->RDelay, 4);
				if (intDriver->ReverbPtr == NULL)
					return MADNeedMemory;
				break;
		}
	}
	
	return MADNoErr;
}

void MADDisposeDriverBuffer(MADDriverRec *intDriver)
{
	short i;
	
	for (i = 0; i < MAXDRAW; i++) {
		if (intDriver->OsciDrawPtr[i] != NULL) {
			free(intDriver->OsciDrawPtr[i]);
			intDriver->OsciDrawPtr[i] = NULL;
		}
	}
	
	if (intDriver->IntDataPtr != NULL) {
		free(intDriver->IntDataPtr);
		intDriver->IntDataPtr = NULL;
	}
}

MADErr MADCreateDriverBuffer(MADDriverRec *intDriver)
{
	short	i, x;
	int		BufSize = 0;
	
	if (intDriver == NULL) {
		return MADParametersErr;
	}
	
	BufSize = intDriver->ASCBUFFER;
	
	switch(intDriver->DriverSettings.outPutMode) {
		case MonoOutPut:
			BufSize = BufSize;
			break;
		case StereoOutPut:
		case DeluxeStereoOutPut:
			BufSize *= 2;
			break;
		case PolyPhonic:
			BufSize *= 4;
			break;
	}
	
	switch (intDriver->DriverSettings.outPutBits) {
		case 16:
			BufSize *= 2;
			break;
		case 20: //This is actually 2.5 more than 8, but I don't want to round
		case 24:
			BufSize *= 3;
			break;
	}
	
	intDriver->curDrawPtr = 0;
	for (i = 0; i < MAXDRAW; i++)
		intDriver->OsciDrawPtr[i] = NULL;
	
	for (i = 0; i < MAXDRAW; i++) {
		intDriver->newData[i] = false;
		intDriver->OsciDrawPtr[i] = (char*)calloc(BufSize / intDriver->DriverSettings.oversampling, 1);
		if (intDriver->OsciDrawPtr[i] == NULL) {
			for (x = 0; x < MAXDRAW; x++) {
				if (intDriver->OsciDrawPtr[i] != NULL) {
					free(intDriver->OsciDrawPtr[i]);
				}
			}
			return MADNeedMemory;
		}
	}
	
	intDriver->IntDataPtr = (char*)malloc(BufSize);
	if (intDriver->IntDataPtr == NULL) {
		for (x = 0; x < MAXDRAW; x++) {
			if (intDriver->OsciDrawPtr[i] != NULL) {
				free(intDriver->OsciDrawPtr[i]);
			}
		}
		return MADNeedMemory;
	}
	
	intDriver->BufSize				= BufSize;
	intDriver->base.OscilloWavePtr	= intDriver->IntDataPtr;
	intDriver->base.OscilloWaveSize	= BufSize / intDriver->DriverSettings.oversampling;
	
	return MADNoErr;
}

MADErr MADCreateTiming(MADDriverRec *intDriver)
{
	/*************************/
	/**     Rate table      **/
	/*************************/
	
	if (intDriver == NULL)
		return MADParametersErr;
	
	intDriver->VSYNC	= ((intDriver->DriverSettings.outPutRate) * 125L * intDriver->DriverSettings.oversampling) / (50);
	intDriver->trackDiv = intDriver->DriverSettings.numChn;
	//	if (intDriver->DriverSettings.outPutMode == StereoOutPut) intDriver->trackDiv /= 2;
	
	return MADNoErr;
}

MADDriverSettings MADGetDriverSettings(const MADDriverRecPtr theDriver)
{
	return theDriver->DriverSettings;
}

MADErr MADChangeDriverSettings(MADDriverSettings *DriverInitParam, MADDriverRec** returnDriver)
{
	MADMusic	*music;
	bool		playing;
	MADErr		err;
	long		fullTime, curTime;
	MADLibrary	*lib;
	if (DriverInitParam == NULL || returnDriver == NULL) {
		return MADParametersErr;
	}
	music = (*returnDriver)->base.curMusic;
	playing = (*returnDriver)->base.Reading;
	lib = (*returnDriver)->base.lib;
	
	MADGetMusicStatus(*returnDriver, &fullTime, &curTime);
	
	if ((err = MADStopDriver(*returnDriver)) != MADNoErr)
		return err;
	if ((err = MADDisposeDriver(*returnDriver)) != MADNoErr)
		return err;
	
	if ((err = MADCreateDriver(DriverInitParam, lib, returnDriver)) != MADNoErr)
		return err;
	
	if ((err = MADStartDriver(*returnDriver)) != MADNoErr)
		return err;
	
	if (music) {
		MADAttachDriverToMusic(*returnDriver, music, NULL);
		MADSetMusicStatus(*returnDriver, 0, fullTime, curTime);
		
		if (playing)
			MADPlayMusic(*returnDriver);
	}
	
	return MADNoErr;
}

MADErr MADCreateDriver(MADDriverSettings *DriverInitParam, MADLibrary *lib, MADDriverRec** returnDriver)
{
	MADErr			theErr = MADNoErr;
	int				i;
	MADDriverRec*	MDriver;
	
	if (DriverInitParam == NULL || lib == NULL || returnDriver == NULL)
		return MADParametersErr;
	*returnDriver = NULL;
	
	/*************************/
	/** Paramaters checking **/
	/*************************/
	
	if (DriverInitParam->numChn % 2 != 0) {
		DriverInitParam->numChn /= 2;
		DriverInitParam->numChn *= 2;
		//	DriverInitParam->numChn++;
	}
	if (DriverInitParam->numChn < 2)
		theErr = MADParametersErr;
	if (DriverInitParam->numChn > MAXTRACK)
		theErr = MADParametersErr;
	
	if (DriverInitParam->outPutBits != 8 && DriverInitParam->outPutBits != 16)
		theErr = MADParametersErr;
	
	if (DriverInitParam->outPutRate < 5000)
		theErr = MADParametersErr;
	if (DriverInitParam->outPutRate > 48000)
		theErr = MADParametersErr;
	
	if (DriverInitParam->outPutMode != DeluxeStereoOutPut &&
	   DriverInitParam->outPutMode != PolyPhonic)
		theErr = MADParametersErr;
	
	if (!MADSoundDriverIsAvalable(DriverInitParam->driverMode)) {
		if (theErr == MADNoErr) {
			theErr = MADSoundSystemUnavailable;
		}
	}
	
	if (DriverInitParam->MicroDelaySize < 0)
		theErr = MADParametersErr;
	if (DriverInitParam->MicroDelaySize > 1000)
		theErr = MADParametersErr;
	
	if (DriverInitParam->Reverb) {
		if (DriverInitParam->ReverbSize < 25)
			theErr = MADParametersErr;
		if (DriverInitParam->ReverbSize > 1000)
			theErr = MADParametersErr;
		
		if (DriverInitParam->ReverbStrength < 0)
			theErr = MADParametersErr;
		if (DriverInitParam->ReverbStrength > 70)
			theErr = MADParametersErr;
	}
	
	if (DriverInitParam->oversampling < 1)
		theErr = MADParametersErr;
	if (DriverInitParam->oversampling > 30)
		theErr = MADParametersErr;
	
	if (theErr != MADNoErr)
		return theErr;
	
	/*************************/
#if !defined(HAS_LONG_LONG) || !defined(HAS_LONG_DOUBLE)
	DriverInitParam->oversampling = 1;// We do NOT support oversampling on compilers that don't have long longs and long doubles
#endif
	
	MDriver = (MADDriverRec*)calloc(sizeof(MADDriverRec), 1);
	
	MDriver->base.lib = lib;
	MDriver->base.curMusic = NULL;
	
	MDriver->base.Reading = false;
	MDriver->wasReading = false;
	
	theErr = MADStopDriver(MDriver);
	if (theErr != MADNoErr) {
		free(MDriver);
		return theErr;
	}
	
	//theErr = MADDisposeDriver(MDriver);
	//if (theErr != MADNoErr) return theErr;
	
	theErr = MADCreateVibrato(MDriver);
	if (theErr != MADNoErr) {
		free(MDriver);
		return theErr;
	}
	
	for (i = 0; i < MAXTRACK; i++)
		MDriver->base.Active[i] = true;
	
	MDriver->DriverSettings		= *DriverInitParam;
	MDriver->base.musicEnd			= false;
	MDriver->base.Reading			= false;
	MDriver->JumpToNextPattern	= true;
	MDriver->smallcounter		= 128;			// Start immediately
	MDriver->BufCounter			= 0;
	MDriver->BytesToGenerate	= 0;
	MDriver->base.speed				= 6;
	MDriver->base.finespeed			= 125;
	MDriver->base.VExt				= 8000;
	MDriver->base.FreqExt			= 8000;
	MDriver->base.VolGlobal			= 64;
	MDriver->SendMIDIClockData	= false;
	MDriver->MODMode			= false;
	MDriver->XMLinear			= false;
	MDriver->base.Equalizer			= false;
	MDriver->MultiChanNo		= 48;
	MDriver->globPan			= 64;
	MDriver->currentlyExporting	= false;
	MDriver->thisExport			= false;
	MDriver->OneMoreBeforeEnd	= false;
	MDriver->useOsciBuffers		= false;
	MDriver->MIN_PITCH			= GetOldPeriod(NUMBER_NOTES-1, NOFINETUNE, MDriver);
	MDriver->MAX_PITCH			= GetOldPeriod(0, NOFINETUNE, MDriver);
	
	MDriver->MOD_MIN_PITCH		= GetOldPeriod(24 + 4*12 -1, NOFINETUNE, MDriver);
	MDriver->MOD_MAX_PITCH		= GetOldPeriod(24, NOFINETUNE, MDriver);
	
	MDriver->clipL	= false;
	MDriver->clipR	= false;
	
	MDriver->levelL = 0;
	MDriver->levelR = 0;
	
	
	MADCleanDriver(MDriver);
	
	/*************************/
	/** 	Driver MODE	    **/
	/*************************/
	
	switch(MDriver->DriverSettings.driverMode) {
			
#ifdef _MAC_H
		case CoreAudioDriver:
			MDriver->ASCBUFFER = 1024L * MDriver->DriverSettings.oversampling;
			break;
#endif
			
#ifdef _ESOUND
		case ESDDriver:
			MDriver->ASCBUFFER = 1024L * MDriver->DriverSettings.oversampling;
			break;
#endif
			
#ifdef HAVE_PULSEAUDIO
		case PulseAudioDriver:
			MDriver->ASCBUFFER = 1024L * MDriver->DriverSettings.oversampling;
			break;
#endif
			
#ifdef HAVE_PORTAUDIO
		case PortAudioDriver:
			MDriver->ASCBUFFER = 1024L * MDriver->DriverSettings.oversampling;
			break;
#endif
			
		case BeOSSoundDriver:
			MDriver->ASCBUFFER = 1024L;
			if (MDriver->DriverSettings.outPutBits == 8) MDriver->ASCBUFFER *= 2L;
			break;
			
		case NoHardwareDriver:
			MDriver->ASCBUFFER = 1024L * MDriver->DriverSettings.oversampling;
			break;
			
#ifdef WIN32
		case DirectSound95NT:
		case Wave95NT:
			MDriver->ASCBUFFER = 7500 * MDriver->DriverSettings.oversampling;
			break;
#endif
			
		default:
			MDriver->DriverSettings.driverMode = NoHardwareDriver;
			MDriver->ASCBUFFER = 1024L * MDriver->DriverSettings.oversampling;
			//return MADParametersErr;
			break;
	}
	
	MDriver->ASCBUFFERReal = MDriver->ASCBUFFER / MDriver->DriverSettings.oversampling;
	
	theErr = MADCreateDriverBuffer(MDriver);
	if (theErr != MADNoErr) {
		free(MDriver);
		return theErr;
	}
	theErr = MADCreateTiming(MDriver);
	if (theErr != MADNoErr) {
		MADDisposeDriverBuffer(MDriver);
		free(MDriver);
		return theErr;
	}
	theErr = MADCreateReverb(MDriver);
	if (theErr != MADNoErr) {
		MADDisposeDriverBuffer(MDriver);
		free(MDriver);
		return theErr;
	}
	theErr = MADCreateVolumeTable(MDriver);
	if (theErr != MADNoErr) {
		MADDisposeReverb(MDriver);
		MADDisposeDriverBuffer(MDriver);
		free(MDriver);
		return theErr;
	}
	theErr = MADInitEqualizer(MDriver);
	if (theErr != MADNoErr) {
		MADDisposeReverb(MDriver);
		MADDisposeDriverBuffer(MDriver);
		free(MDriver);
		return theErr;
	}
	
	/**********************************************/
	/**    Interruption - Hardware Support       **/
	/**********************************************/
	
	switch(MDriver->DriverSettings.driverMode)
	{
			//#ifdef _MIDIHARDWARE_
		case MIDISoundDriver:
			
			OpenMIDIHardware(MDriver);
			
			/*	MDriver->gOutNodeRefNum = -1;
			 OMSAddPort('SNPL', 'out ', omsPortTypeOutput, NULL, NULL, &MDriver->MIDIPortRefNum);*/
			
#if 0
			theErr = InitDBSoundManager(MDriver, initStereo);
			if (theErr != MADNoErr) {
				MADCloseEqualizer(MDriver);
				MADDisposeReverb(MDriver);
				MADDisposeDriverBuffer(MDriver);
				free(MDriver);
				return theErr;
			}
#endif
			break;
			
#ifdef _MAC_H
		case CoreAudioDriver:
			theErr = initCoreAudio(MDriver);
			if (theErr != MADNoErr) {
				MADCloseEqualizer(MDriver);
				MADDisposeReverb(MDriver);
				MADDisposeDriverBuffer(MDriver);
				free(MDriver);
				return theErr;
			}
			break;
#endif
			
#ifdef WIN32
		case DirectSound95NT:
			if (!DirectSoundInit(MDriver))
				theErr = MADUnknownErr;
			if (theErr != MADNoErr) {
				MADCloseEqualizer(MDriver);
				MADDisposeReverb(MDriver);
				MADDisposeDriverBuffer(MDriver);
				free(MDriver);
				return theErr;
			}
			break;
			
		case Wave95NT:
			if (!W95_Init(MDriver))
				theErr = MADUnknownErr;
			if (theErr != MADNoErr) {
				MADCloseEqualizer(MDriver);
				MADDisposeReverb(MDriver);
				MADDisposeDriverBuffer(MDriver);
				free(MDriver);
				return theErr;
			}
			break;
#endif
			
#ifdef _ESOUND
		case ESDDriver:
			theErr = initESD(MDriver);
			if (theErr != MADNoErr) {
				MADCloseEqualizer(MDriver);
				MADDisposeReverb(MDriver);
				MADDisposeDriverBuffer(MDriver);
				free(MDriver);
				return theErr;
			}
			break;
#endif
			
#ifdef HAVE_PULSEAUDIO
		case PulseAudioDriver:
			theErr = initPulseAudio(MDriver);
			if (theErr != MADNoErr) {
				MADCloseEqualizer(MDriver);
				MADDisposeReverb(MDriver);
				MADDisposeDriverBuffer(MDriver);
				free(MDriver);
				return theErr;
			}
			break;
#endif
			
#ifdef HAVE_PORTAUDIO
		case PortAudioDriver:
			theErr = initPortAudio(MDriver);
			if (theErr != MADNoErr) {
				MADCloseEqualizer(MDriver);
				MADDisposeReverb(MDriver);
				MADDisposeDriverBuffer(MDriver);
				free(MDriver);
				return theErr;
			}
			break;
#endif
			
		case NoHardwareDriver:
		case BeOSSoundDriver:
			break;
			
		default:
			break;
	}
	
	*returnDriver = MDriver;
	
	return MADNoErr;
}

MADErr MADDisposeDriver(MADDriverRec* MDriver)
{
	if (MDriver->IntDataPtr == NULL)
		return MADNoErr;
	
	MDriver->base.Reading = false;
	
	MADCleanDriver(MDriver);
	
	switch (MDriver->DriverSettings.driverMode) {
		case MIDISoundDriver:
			CloseMIDIHarware();
			break;
			
#ifdef _MAC_H
		case CoreAudioDriver:
			closeCoreAudio(MDriver);
			break;
#endif
			
#ifdef WIN32
		case DirectSound95NT:
			DirectSoundClose(MDriver);
			break;
			
		case Wave95NT:
			W95_Exit(MDriver);
			break;
#endif
			
#ifdef _ESOUND
		case ESDDriver:
			closeESD(MDriver);
			break;
#endif
			
#ifdef HAVE_PULSEAUDIO
		case PulseAudioDriver:
			closePulseAudio(MDriver);
			break;
#endif
			
#ifdef HAVE_PORTAUDIO
		case PortAudioDriver:
			closeOSS(MDriver);
			break;
#endif
			
		default:
			break;
	}
	
	MADDisposeDriverBuffer(MDriver);
	MADDisposeVolumeTable(MDriver);
	MADDisposeReverb(MDriver);
	
	free(MDriver);
	
	return MADNoErr;
}

MADErr MADInitLibrary(const char *PlugsFolderName, MADLibrary **lib)
{
	static const int mytab[12] = {
		1712*16, 1616*16, 1524*16, 1440*16, 1356*16, 1280*16,
		1208*16, 1140*16, 1076*16, 1016*16, 960*16, 907*16
	};
	if (lib == NULL)
		return MADParametersErr;
	
	*lib = (MADLibrary*)calloc(sizeof(MADLibrary), 1);
	
	if (*lib == NULL)
		return MADNeedMemory;
	
	(*lib)->IDType = 'MADD';
	
	memcpy((*lib)->mytab, mytab, sizeof(mytab));
	
	MInitImportPlug(*lib, PlugsFolderName);
	return MADNoErr;
}

MADErr MADDisposeLibrary(MADLibrary *MLibrary)
{
	if (MLibrary != NULL)
	{
		CloseImportPlug(MLibrary);
		MLibrary->IDType = 'XXXX';
		
		free(MLibrary);
		MLibrary = NULL;
	}
	return MADNoErr;
}

MADErr MADAttachDriverToMusic(MADDriverRec *driver, MADMusic *music, char *MissingPlugs)
{
	short	alpha, x, i, index;
	bool	needToReset;
	
	if (!driver)
		return MADParametersErr;
	if (!music)
		return MADParametersErr;
	
	driver->base.musicEnd = false;
	
	if (music != driver->base.curMusic)
		needToReset = true;
	else
		needToReset = false;
	
	// Save old preferences
	
	if (driver->base.curMusic) {
		if (driver->base.curMusic->header) {
			driver->base.curMusic->header->generalVol = driver->base.VolGlobal;
			driver->base.curMusic->header->ESpeed = driver->base.VExt;
			driver->base.curMusic->header->EPitch = driver->base.FreqExt;
			
			driver->base.curMusic->header->generalSpeed = driver->base.VExt/100;
			driver->base.curMusic->header->generalPitch = driver->base.FreqExt/100;
			driver->base.curMusic->header->generalPan = driver->globPan;
		}
	}
	
	/////////////////////////
	
	driver->base.curMusic = music;
	driver->base.VolGlobal = music->header->generalVol;
	if (driver->base.VolGlobal <= 0)
		driver->base.VolGlobal = 64;
	
	if (music->header->generalSpeed == 252) {
		driver->base.VExt	= music->header->ESpeed;
		driver->base.FreqExt	= music->header->EPitch;
	} else {
		driver->base.VExt = music->header->generalSpeed;
		if (driver->base.VExt <= 160)
			driver->base.VExt *= 100;
		if (driver->base.VExt <= 160)
			driver->base.VExt = 8000;
		
		
		driver->base.FreqExt = music->header->generalPitch;
		if (driver->base.FreqExt <= 160)
			driver->base.FreqExt *= 100;
		if (driver->base.FreqExt <= 160)
			driver->base.FreqExt = 8000;
	}
	
	if (music->header->generalPan == 0)
		music->header->generalPan = 64;
	driver->globPan = music->header->generalPan;
	
	if (music->header->MultiChanNo < 4)
		music->header->MultiChanNo = 48;
	if (music->header->MultiChanNo > 99)
		music->header->MultiChanNo = 99;
	
	if (music->header->MultiChanNo <= 0)
		driver->MultiChanNo = 48;
	else
		driver->MultiChanNo = music->header->MultiChanNo;
	
	if (music->header->MODMode)
		driver->MODMode = true;
	else
		driver->MODMode = false;
	
	if (music->header->XMLinear)
		driver->XMLinear = true;
	else
		driver->XMLinear = false;
	
	//INSTALL ALL VST EFFECTS !!!!!!
	//TODO: VST Effects
	// Purge previous Effects !
	for (i = 0; i < 10 ; i++) {
		if (driver->masterVST[i]) {
			DisposeVSTEffect(driver->masterVST[i]);
			driver->masterVST[i] = NULL;
		}
	}
	
	for (i = 0; i < MAXTRACK ; i++) { // Channel Effects
		for (x = 0; x < 4; x++) {
			if (driver->chanVST[i][x]) {
				DisposeVSTEffect(driver->chanVST[i][x]);
				driver->chanVST[i][x] = NULL;
			}
		}
	}
	
	// Install new effects
	alpha = 0;
	
	for (i = 0; i < 10 ; i++) { // Global Effects
		if (music->header->globalEffect[i]) {
			index = ConvertUniqueIDToIndex(music->sets[alpha].FXID);
			
			if (index >= 0) {
				driver->masterVST[i] = CreateVSTEffect(index);
				ApplyVSTSets(driver->masterVST[i], &music->sets[alpha]);
			}
			else if (MissingPlugs)
			{
				if (strlen((char*)MissingPlugs) > 0)
					strcat((char*)MissingPlugs, ", ");
				MYP2CStr(music->sets[alpha].name);
				strcpy((char*)MissingPlugs,(char*)&music->sets[alpha].name);
				MYC2PStr((char*)music->sets[alpha].name);
			}
			alpha++;
		}
	}
	
	for (i = 0; i < MAXTRACK ; i++) {	// Channel Effects
		for (x = 0; x < 4; x++) {
			if (music->header->chanEffect[i][x]) {
				index = ConvertUniqueIDToIndex(music->sets[alpha].FXID);
				
				if (index >= 0) {
					driver->chanVST[i][x] = CreateVSTEffect(index);
					ApplyVSTSets(driver->chanVST[i][x], &music->sets[alpha]);
				} else if (MissingPlugs) {
					strcat((char*)MissingPlugs, ", ");
					MYP2CStr(music->sets[alpha].name);
					strcpy((char*)MissingPlugs, (char*)&music->sets[alpha].name);
					MYC2PStr((char*)music->sets[alpha].name);
				}
				alpha++;
			}
		}
	}
	
	if (needToReset) {
		MADReset(driver);
	}
	MADUpdateTracksNumber(driver);
	
	return MADNoErr;
}

MADErr MADLoadMusicPtr(MADMusic **music, char* myPtr)
{
	MADErr theErr;
	
	//MADDisposeMusic(music);
	
	theErr = MADReadMAD(music, 0, MADPtrType, NULL, myPtr);
	if (theErr != MADNoErr)
		return theErr;
	
	return MADNoErr;
}

MADErr MADLoadMADFileCString(MADMusic **music, const char *fName)
{
	MADErr	theErr;
	UNFILE	srcFile;
	
	if (fName == NULL || music == NULL) {
		return MADParametersErr;
	}
	
	//MADDisposeMusic(music);
	
#ifdef _MAC_H
	CFURLRef theRef = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault, (const UInt8*)fName, strlen(fName), false);
	if (theRef) {
		CFReadStreamRef tmpDatRef = CFReadStreamCreateWithFile(kCFAllocatorDefault, theRef);
		CFRelease(theRef);
		
		theErr = MADReadMAD(music, NULL, MADCFReadStreamType, tmpDatRef, NULL);
		
		CFRelease(tmpDatRef);
		return theErr;
	} else {
#endif
	
	srcFile = iFileOpenRead(fName);
	if (srcFile == 0) return MADReadingErr;
	
	theErr = MADReadMAD(music, srcFile, MADFileType, NULL, NULL);
	if (theErr != MADNoErr) {
		iClose(srcFile);
		//MADDisposeMusic(music, MADDriver);
		return theErr;
	}
	
	iClose(srcFile);
#ifdef _MAC_H
	}
#endif
	return MADNoErr;
}

#ifdef _MAC_H
//hack around the fact that there isn't an equivalent of CFStringGetMaximumSizeOfFileSystemRepresentation for CFURLs
static inline CFIndex getCFURLFilePathRepresentationLength(CFURLRef theRef, Boolean resolveAgainstBase)
{
	CFStringRef fileString;
	CFIndex strLength;
	fileString = CFURLCopyFileSystemPath(theRef, kCFURLPOSIXPathStyle);
	strLength = CFStringGetMaximumSizeOfFileSystemRepresentation(fileString);
	CFRelease(fileString);
	
	return strLength;
}

static MADErr getCStringFromCFURL(CFURLRef theRef, char **cStrOut)
{
	char *URLcString = NULL, *trimURLcString = NULL;
	CFIndex pathLen;
	bool pathOK;
	if (cStrOut == NULL) {
		return MADParametersErr;
	}
	if (theRef == NULL) {
		*cStrOut = NULL;
		return MADParametersErr;
	}
	pathLen = getCFURLFilePathRepresentationLength(theRef, true);
	URLcString = malloc(pathLen);
	if (URLcString == NULL) {
		*cStrOut = NULL;
		return MADNeedMemory;
	}
	pathOK = CFURLGetFileSystemRepresentation(theRef, true, (unsigned char*)URLcString, pathLen);
	if (pathOK == false) {
		free(URLcString);
		*cStrOut = NULL;
		return MADReadingErr;
	}
	trimURLcString = realloc(URLcString, strlen(URLcString) + 1);
	//So realloc failed... No biggie. Just use the bigger string.
	if (!trimURLcString) {
		*cStrOut = URLcString;
		return MADNoErr;
	}
	
	*cStrOut = trimURLcString;
	return MADNoErr;
}

MADErr MADLoadMusicCFURLFile(MADLibrary *lib, MADMusic **music, char *type, CFURLRef theRef)
{
	char *URLcString = NULL;
	MADErr theErr = MADNoErr;
	
	if (!strcmp("MADK", type)) {
		CFReadStreamRef tmpDatRef = CFReadStreamCreateWithFile(kCFAllocatorDefault, theRef);
		
		theErr = MADReadMAD(music, NULL, MADCFReadStreamType, tmpDatRef, NULL);
		
		CFRelease(tmpDatRef);
		return theErr;
	}
	
	theErr = getCStringFromCFURL(theRef, &URLcString);
	
	if (theErr != MADNoErr)
		return theErr;
	
	theErr = MADLoadMusicFileCString(lib, music, type, URLcString);
	free(URLcString);
	return theErr;
}

#if 0
MADErr MADCopyCurrentPartition(MADMusic *aPartition)
{
	long	i, x;
	
	if (MDriver->header == NULL) return -1;
	if (aPartition == NULL) return -1;
	
	aPartition->header = (MADSpec*) MADNewPtr(sizeof(MADSpec), &MDriver->DriverSettings);
	if (aPartition->header == NULL) return -1;
	
	BlockMoveData(MDriver->header, aPartition->header, sizeof(MADSpec));
	
	for (i = 0; i < MDriver->header->numPat ; i++)
	{
		aPartition->partition[i] = (PatData*) MADNewPtr(GetPtrSize((char*) MDriver->partition[i]), &MDriver->DriverSettings);
		if (aPartition->partition[i] == NULL) return -1;
		
		BlockMoveData(MDriver->partition[i], aPartition->partition[i], GetPtrSize((char*) MDriver->partition[i]));
	}
	for (i = MDriver->header->numPat; i < MAXPATTERN ; i++) aPartition->partition[i] = NULL;
	
	for (i = 0; i < MAXINSTRU ; i++)
	{
		for (x = 0; x < MDriver->header->fid[i].numSamples; x++)
		{
			sData	*dstData, *srcData = MDriver->sample[i][x];
			
			aPartition->sample[i][x] = dstData = (sData*) MADNewPtr(sizeof(sData), &MDriver->DriverSettings);
			if (dstData == NULL) return -1;
			
			BlockMoveData(srcData, dstData, sizeof(sData));
			
			dstData->data = MADNewPtr(srcData->size, &MDriver->DriverSettings);
			if (dstData->data == NULL) return -1;
			
			BlockMoveData(srcData->data, dstData->data, srcData->size);
		}
	}
	
	return MADNoErr;
}
#endif


MADErr MADMusicIdentifyCFURL(MADLibrary *lib, char *type, CFURLRef URLRef)
{
	char *URLcString = NULL;
	MADErr theErr = getCStringFromCFURL(URLRef, &URLcString);
	
	if (theErr != MADNoErr)
		return theErr;
	
	theErr = MADMusicIdentifyCString(lib, type, URLcString);
	free(URLcString);
	return theErr;
}

MADErr MADMusicInfoCFURL(MADLibrary *lib, char *type, CFURLRef theRef, MADInfoRec *InfoRec)
{
	char *URLcString = NULL;
	MADErr theErr = getCStringFromCFURL(theRef, &URLcString);
	
	if (theErr != MADNoErr)
		return theErr;
	
	theErr = MADMusicInfoCString(lib, type, URLcString, InfoRec);
	free(URLcString);
	return theErr;
}

MADErr MADMusicExportCFURL(MADLibrary *lib, MADMusic *music, char *type, CFURLRef fileURL)
{
	char *URLcString = NULL;
	MADErr theErr = getCStringFromCFURL(fileURL, &URLcString);
	
	if (theErr)
		return theErr;
	
	theErr = MADMusicExportCString(lib, music, type, URLcString);
	free(URLcString);
	return theErr;
}

MADErr MADMusicSaveCFURL(MADMusic *music, CFURLRef urlRef, bool compressMAD)
{
	int		alpha = 0;
	int		i, x;
	size_t	inOutCount;
	CFWriteStreamRef	curFile = NULL;
	MADErr	theErr = MADNoErr;
	MADSpec aHeader;
	
	if (music->musicUnderModification)
		return MADWritingErr;
	
	curFile = CFWriteStreamCreateWithFile(kCFAllocatorDefault, urlRef);
	if (!curFile) {
		return MADWritingErr;
	}
	CFWriteStreamOpen(curFile);
	CFStreamStatus theStat = CFWriteStreamGetStatus(curFile);
	while (theStat != kCFStreamStatusOpen && theStat != kCFStreamStatusError) {
		theStat = CFWriteStreamGetStatus(curFile);
	}
	if (theStat == kCFStreamStatusError) {
		CFWriteStreamClose(curFile);
		CFRelease(curFile);
		return MADWritingErr;
	}
	
	
	//TODO: error-checking
	
	music->musicUnderModification = true;
	
	for (i = 0, x = 0; i < MAXINSTRU; i++) {
		music->fid[i].no = i;
		
		// Is there something in this instrument?
		if (music->fid[i].numSamples > 0 || music->fid[i].name[0] != 0) {
			x++;
		}
	}
	
	aHeader = *music->header;
	aHeader.numInstru = x;
	ByteSwapMADSpec(&aHeader);
	CFWriteStreamWrite(curFile, (const UInt8*)&aHeader, sizeof(MADSpec));
	
	if (compressMAD) {
		for (i = 0; i < music->header->numPat ; i++) {
			if (music->partition[i]) {
				PatData *tmpPat = CompressPartitionMAD1(music, music->partition[i]);
				inOutCount = tmpPat->header.patBytes + sizeof(PatHeader);
				tmpPat->header.compMode = 'MAD1';
				ByteSwapPatHeader(&tmpPat->header);
				CFWriteStreamWrite(curFile, (const UInt8*)tmpPat, inOutCount);
				free(tmpPat);
			}
		}
	} else {
		for (i = 0; i < music->header->numPat; i++) {
			if (music->partition[i]) {
				PatData *tmpPat;
				inOutCount = sizeof(PatHeader);
				inOutCount += music->header->numChn * music->partition[i]->header.size * sizeof(Cmd);
				tmpPat = calloc(inOutCount, 1);
				memcpy(tmpPat, music->partition[i], inOutCount);
				tmpPat->header.compMode = 'NONE';
				ByteSwapPatHeader(&tmpPat->header);
				CFWriteStreamWrite(curFile, (const UInt8*)tmpPat, inOutCount);
				free(tmpPat);
			}
		}
	}
	
	for (i = 0; i < MAXINSTRU; i++) {
		if (music->fid[i].numSamples > 0 || music->fid[i].name[0] != 0) {	// Is there something in this instrument?
			InstrData instData;
			music->fid[i].no = i;
			instData = music->fid[i];
			ByteSwapInstrData(&instData);
			CFWriteStreamWrite(curFile, (const UInt8*)&instData, sizeof(InstrData));
		}
	}
	
	for (i = 0; i < MAXINSTRU; i++) {
		for (x = 0; x < music->fid[i].numSamples; x++) {
			sData	curData;
			sData32	copyData;
			void	*dataCopy = NULL;
			curData = *music->sample[music->fid[i].firstSample + x];
			
			inOutCount = sizeof(sData32);
			ByteSwapsData(&curData);
			memcpy(&copyData, &curData, inOutCount);
			copyData.data = 0;
			CFWriteStreamWrite(curFile, (const UInt8*)&copyData, inOutCount);
			
			inOutCount = music->sample[music->fid[i].firstSample + x]->size;
			dataCopy = malloc(inOutCount);
			memcpy(dataCopy, curData.data, inOutCount);
			if (curData.amp == 16) {
				short *shortPtr = (short*)dataCopy;
				dispatch_apply(inOutCount / 2, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(size_t ll) {
					MADBE16(&shortPtr[ll]);
				});
			}
			CFWriteStreamWrite(curFile, (const UInt8*)dataCopy, inOutCount);
			free(dataCopy);
		}
	}
	
	// EFFECTS *** *** *** *** *** *** *** *** *** *** *** ***
	
	for (i = 0; i < 10 ; i++) {	// Global Effects
		if (music->header->globalEffect[i]) {
			FXSets aSet;
			inOutCount = sizeof(FXSets);
			aSet = music->sets[alpha];
			SwapFXSets(&aSet);
			CFWriteStreamWrite(curFile, (const UInt8*)&aSet, inOutCount);
			alpha++;
		}
	}
	
	for (i = 0; i < music->header->numChn; i++) {	// Channel Effects
		for (x = 0; x < 4; x++) {
			if (music->header->chanEffect[i][x]) {
				FXSets aSet;
				inOutCount = sizeof(FXSets);
				aSet = music->sets[alpha];
				SwapFXSets(&aSet);
				CFWriteStreamWrite(curFile, (const UInt8*)&aSet, inOutCount);
				alpha++;
			}
		}
	}
	
	music->musicUnderModification = false;
	
	CFWriteStreamClose(curFile);
	CFRelease(curFile);
	
	return theErr;
}

MADErr MADMusicTestCFURL(MADLibrary *lib, char *type, CFURLRef theRef)
{
	char *URLcString = NULL;
	MADErr theErr = getCStringFromCFURL(theRef, &URLcString);
	
	if (theErr != MADNoErr)
		return theErr;
	
	theErr = MADMusicTestCString(lib, type, URLcString);
	free(URLcString);
	return theErr;
}
#endif

MADErr MADMusicTestCString(MADLibrary *inMADDriver, char *kindFile, const char *AlienFile)
{
	if (!inMADDriver || !kindFile || !AlienFile) {
		return MADParametersErr;
	}
	if (strcmp("MADK", kindFile) == 0) {
		return CheckMADFile(AlienFile);
	}
	return PPTestFile(inMADDriver, kindFile, (char*)AlienFile);
}

MADErr MADMusicExportCString(MADLibrary *lib, MADMusic *music, char *type, char* cName)
{
	if (!lib || !music || !type || !cName) {
		return MADParametersErr;
	}
	return PPExportFile(lib, type, cName, music);
}

MADErr MADMusicInfoCString(MADLibrary *lib, char *type, char* cName, MADInfoRec *InfoRec)
{
	if (lib == NULL || cName == NULL || InfoRec == NULL || type == NULL) {
		return MADParametersErr;
	}
	memset(InfoRec, 0, sizeof(MADInfoRec));
	return PPInfoFile(lib, type, cName, InfoRec);
}

MADErr MADMusicIdentifyCString(MADLibrary *lib, char *type, char *fName)
{
	if (lib == NULL || type == NULL || fName == NULL) {
		return MADParametersErr;
	}
	return PPIdentifyFile(lib, type, fName);
}

MADErr MADLoadMusicFileCString(MADLibrary *lib, MADMusic **music, char *plugType, char *fName)
{
	MADErr iErr = MADNoErr;
	
	if (music == NULL || plugType == NULL || fName == NULL) {
		return MADParametersErr;
	}
	
	if (!strcmp("MADK", plugType)) {
		iErr = MADLoadMADFileCString(music, fName);
	} else if (lib == NULL) {
		iErr = MADParametersErr;
	} else {
		iErr = PPImportFile(lib, plugType, fName, music);
	}
	
	return iErr;
}

MADErr MADSetMusicStatus(MADDriverRec *MDriver, long minV, long maxV, long curV)
{
	short	i, x, y;
	Cmd		*aCmd = NULL;
	
	float	timeResult;
	long	time;
	long	speed, finespeed, fullTime, curTime;
	long	dstTime;
	
	if (MDriver == NULL)
		return MADParametersErr;
	
	if (MDriver->base.curMusic == NULL || MDriver->base.curMusic->header == NULL)
		return MADDriverHasNoMusic;
	
	if (maxV > curV)
		MDriver->base.musicEnd = false;
	
	MADGetMusicStatus(MDriver, &fullTime, &curTime);
	
	curV -= minV;
	maxV -= minV;
	
	if (maxV)
		dstTime = (curV * fullTime)/maxV;
	else
		dstTime = 0;
	timeResult	= 0;
	time		= 0;
	speed		= MDriver->base.curMusic->header->speed;
	finespeed	= MDriver->base.curMusic->header->tempo;
	
	for (i = 0; i < MDriver->base.curMusic->header->numPointers; i++) {
		for (x = 0; x < MDriver->base.curMusic->partition[MDriver->base.curMusic->header->oPointers[i]]->header.size; x++) {
			time ++;
			
			if (dstTime <= timeResult + ((float) (time * 125L * speed * 60)) / ((float) (50 * finespeed))) {
				MDriver->base.PL = i;
				MDriver->base.PartitionReader = x;
				MDriver->base.Pat = MDriver->base.curMusic->header->oPointers[i];
				
				MADCheckSpeed(MDriver->base.curMusic, MDriver);
				
				MADPurgeTrack(MDriver);
				
				return MADNoErr;
			}
			
			for (y = 0; y <  MDriver->base.curMusic->header->numChn; y++) {
				aCmd = GetMADCommand(x, y, MDriver->base.curMusic->partition[MDriver->base.curMusic->header->oPointers[i]]);
				
				/** MADEffectSpeed **/
				if (aCmd) {
					if (aCmd->cmd == MADEffectSpeed) {
						/** Compute time for this interval **/
						
						timeResult += ((float) (time * 125L * speed * 60)) / ((float) (50 * finespeed));
						time = 0;
						
						/************************************/
						
						if (aCmd->arg < 32) {
							if (aCmd->arg != 0)
								speed = aCmd->arg;
						} else {
							if (aCmd->arg != 0)
								finespeed = aCmd->arg;
						}
					}
					
					/** MADEffectSkip **/
					
					if (aCmd->cmd == MADEffectSkip) {
						for (; x < MDriver->base.curMusic->partition[MDriver->base.curMusic->header->oPointers[i]]->header.size; x++) {
							
						}
					}
				}
			}
		}
	}
	
	return MADUnknownErr;
}

MADErr MADGetMusicStatus(MADDriverRec *MDriver, long *fullTime, long *curTime)
{
	short	i, x, y;
	Cmd		*aCmd;
	float	timeResult;
	long	time;
	long	speed, finespeed;
	
	if (MDriver == NULL) {
		*curTime = 0;
		*fullTime = 1;
		
		return MADParametersErr;
	}
	
	if (MDriver->base.curMusic == NULL || MDriver->base.curMusic->header == NULL) {
		*curTime = 0;
		*fullTime = 1;
		
		return MADDriverHasNoMusic;
	}
	
	timeResult	= 0;
	time		= 0;
	speed		= MDriver->base.curMusic->header->speed;
	finespeed	= MDriver->base.curMusic->header->tempo;
	
	*curTime	= -1;
	
	for (i = 0; i < MDriver->base.curMusic->header->numPointers; i++) {
		for (x = 0; x < MDriver->base.curMusic->partition[MDriver->base.curMusic->header->oPointers[i]]->header.size; x++) {
			time ++;
			
			if (i == MDriver->base.PL && x == MDriver->base.PartitionReader) {
				*curTime = timeResult + ((float) (time * 125L * speed * 60)) / ((float) (50 * finespeed));
			}
			
			for (y = 0; y <  MDriver->base.curMusic->header->numChn; y++) {
				aCmd = GetMADCommand(x, y, MDriver->base.curMusic->partition[MDriver->base.curMusic->header->oPointers[i]]);
				
				if (aCmd) {
					/** MADEffectSpeed **/
					if (aCmd->cmd == MADEffectSpeed) {
						/** Compute time for this interval **/
						
						timeResult += ((float) (time * 125L * speed * 60)) / ((float) (50 * finespeed));
						time = 0;
						
						/************************************/
						
						if (aCmd->arg < 32) {
							if (aCmd->arg != 0)
								speed = aCmd->arg;
						} else {
							if (aCmd->arg != 0)
								finespeed = aCmd->arg;
						}
					} else if (aCmd->cmd == MADEffectSkip) {
						/** MADEffectSkip **/
							for (; x < MDriver->base.curMusic->partition[MDriver->base.curMusic->header->oPointers[i]]->header.size; x++) {
								if (i == MDriver->base.PL && x == MDriver->base.PartitionReader) {
									*curTime = timeResult + ((float) (time * 125L * speed * 60)) / ((float) (50 * finespeed));
								}
							}
						}
				}
			}
		}
	}
	
	timeResult += ((float) (time * 125L * speed * 60)) / ((float) (50 * finespeed));
	
	*fullTime = timeResult;
	
	if (*curTime == -1)
		*curTime = *fullTime;
	
	return MADNoErr;
}

static inline void ByteSwapsData(sData *toSwap)
{
	MADBE32(&toSwap->size);
	MADBE32(&toSwap->loopBeg);
	MADBE32(&toSwap->loopSize);
	MADBE16(&toSwap->c2spd);
}

static inline void SwapFXSets(FXSets *set)
{
#ifndef __BLOCKS__
	int y;
#endif
	MADBE16(&set->id);
	MADBE16(&set->noArg);
	MADBE16(&set->track);
	MADBE32(&set->FXID);
#ifdef __BLOCKS__
	dispatch_apply(100, dispatch_get_global_queue(0, 0), ^(size_t y) {
		MADBE32(&set->values[y]);
	});
#else
	for (y = 0; y < 100; y++) {
		MADBE32(&set->values[y]);
	}
#endif
}

static inline void ByteSwapInstrData(InstrData *toSwap)
{
#ifndef __BLOCKS__
	int x;
#endif
	MADBE16(&toSwap->numSamples);
	MADBE16(&toSwap->firstSample);
	MADBE16(&toSwap->volFade);
	
	MADBE16(&toSwap->MIDI);
	MADBE16(&toSwap->MIDIType);
#ifdef __BLOCKS__
	dispatch_apply(12, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT , 0), ^(size_t x) {
		MADBE16(&toSwap->volEnv[x].pos);
		MADBE16(&toSwap->volEnv[x].val);
		MADBE16(&toSwap->pannEnv[x].pos);
		MADBE16(&toSwap->pannEnv[x].val);
		MADBE16(&toSwap->pitchEnv[x].pos);
		MADBE16(&toSwap->pitchEnv[x].val);
	});
#else
	for (x = 0; x < 12; x++) {
		MADBE16(&toSwap->volEnv[x].pos);
		MADBE16(&toSwap->volEnv[x].val);
		MADBE16(&toSwap->pannEnv[x].pos);
		MADBE16(&toSwap->pannEnv[x].val);
		MADBE16(&toSwap->pitchEnv[x].pos);
		MADBE16(&toSwap->pitchEnv[x].val);
	}
#endif
}

static inline void ByteSwapMADSpec(MADSpec *toSwap)
{
#ifndef __BLOCKS__
	int i;
#endif
	MADBE32(&toSwap->MAD);
	MADBE16(&toSwap->speed);
	MADBE16(&toSwap->tempo);
	MADBE32(&toSwap->EPitch);
	MADBE32(&toSwap->ESpeed);
	
#ifdef __BLOCKS__
	dispatch_apply(10, dispatch_get_global_queue(0, 0), ^(size_t i) {
		MADBE32(&toSwap->globalEffect[i]);
	});
	
	dispatch_apply(MAXTRACK * 4, dispatch_get_global_queue(0, 0), ^(size_t i) {
		MADBE32(&toSwap->chanEffect[i / 4][i % 4]);
	});
	
	dispatch_apply(MAXTRACK, dispatch_get_global_queue(0, 0), ^(size_t i) {
		MADBE16(&toSwap->chanBus[i].copyId);
	});	
#else
	for (i = 0; i < 10; i++) {
		MADBE32(&toSwap->globalEffect[i]);
	}
	
	for (i = 0; i < MAXTRACK * 4; i++) {
		MADBE32(&toSwap->chanEffect[i / 4][i % 4]);
	}
	
	for (i = 0; i < MAXTRACK; i++) {
		MADBE16(&toSwap->chanBus[i].copyId);
	}
#endif
}

static inline void ByteSwapPatHeader(PatHeader *toSwap)
{
	MADBE32(&toSwap->size);
	MADBE32(&toSwap->compMode);
	MADBE32(&toSwap->patBytes);
	MADBE32(&toSwap->unused2);
}

MADErr MADReadMAD(MADMusic **music, UNFILE srcFile, MADInputType InPutType, CFReadStreamRef MADReadStream, char* MADPtr)
{
	short		i, x;
	size_t		inOutCount, OffSetToSample = 0;
	MADErr		theErr = MADNoErr;
	PatHeader	tempPatHeader;
	PatData*	tempPat;
	MADMusic	*MDriver;
	
#ifdef _MAC_H
	CFIndex		bytesRead = 0;
	
	if (InPutType == MADCFReadStreamType) {
		CFStreamStatus theStat;
		CFReadStreamOpen(MADReadStream);
		theStat = CFReadStreamGetStatus(MADReadStream);
		while (theStat != kCFStreamStatusOpen && theStat != kCFStreamStatusError) {
			theStat = CFReadStreamGetStatus(MADReadStream);
		}
		if (theStat == kCFStreamStatusError) {
			CFReadStreamClose(MADReadStream);
			return MADReadingErr;
		}
	}
#endif
	
	*music = NULL;
	
	MDriver = (MADMusic*)calloc(sizeof(MADMusic), 1);
	if (!MDriver)
		return MADNeedMemory;
	
	MDriver->musicUnderModification = false;
	
	/**** HEADER ****/
	inOutCount = sizeof(MADSpec);
	
	MDriver->header = (MADSpec*)calloc(inOutCount, 1);
	if (MDriver->header == NULL) {
		free(MDriver);
		return MADNeedMemory;
	}
	
	switch (InPutType) {
		case MADFileType:
			if (iRead(inOutCount, MDriver->header, srcFile))
				theErr = MADIncompatibleFile;
			break;
			
#ifdef _MAC_H
		case MADCFReadStreamType:
			bytesRead = CFReadStreamRead(MADReadStream, (UInt8*)MDriver->header, inOutCount);
			if (bytesRead == -1)
				theErr = MADReadingErr;
			else if (bytesRead != inOutCount)
				theErr = MADIncompatibleFile;
			break;
#endif
			
		case MADPtrType:
			memcpy(MDriver->header, MADPtr, inOutCount);
			OffSetToSample += inOutCount;
			break;
			
			
		default:
			free(MDriver->header);
			free(MDriver);
			return MADParametersErr;
			break;
	}
	
	if (theErr != MADNoErr) {
		free(MDriver->header);
		free(MDriver);
		
		return theErr;
	}
	
	ByteSwapMADSpec(MDriver->header);
	
	if (MDriver->header->MAD != 'MADK' || MDriver->header->numInstru > MAXINSTRU) {
		free(MDriver->header);
		free(MDriver);
		return MADIncompatibleFile;
	}
	
	if (MDriver->header->MultiChanNo == 0)
		MDriver->header->MultiChanNo = 48;
	
	
	/**** PARTITION ****/
	for (i = 0; i < MDriver->header->numPat; i++) {
		/** Lecture du header de la partition **/
		inOutCount = sizeof(PatHeader);
		
		switch (InPutType) {
			case MADFileType:
				if (iRead(inOutCount, &tempPatHeader, srcFile))
					theErr = MADIncompatibleFile;
				iSeekCur(-(long)sizeof(PatHeader), srcFile);
				break;
				
#ifdef _MAC_H
			case MADCFReadStreamType:
				bytesRead = CFReadStreamRead(MADReadStream, (UInt8*)&tempPatHeader, inOutCount);
				if (bytesRead == -1)
					theErr = MADReadingErr;
				else if (bytesRead != inOutCount)
					theErr = MADIncompatibleFile;
				break;
#endif
				
			case MADPtrType:
				memcpy(&tempPatHeader, MADPtr + OffSetToSample, inOutCount);
				break;
		}
		
		if (theErr != MADNoErr) {
			for (x = 0; x < i; x++) {
				if (MDriver->partition[x] != NULL)
					free(MDriver->partition[x]);
			}
			free(MDriver->header);
			free(MDriver);
			
			return theErr;
		}
		
		ByteSwapPatHeader(&tempPatHeader);
		
		/*************************************************/
		/** Lecture du header + contenu de la partition **/
		/*************************************************/
		
		switch (tempPatHeader.compMode) {
			case 'MAD1':
				inOutCount = sizeof(PatHeader) + tempPatHeader.patBytes;
				break;
				
			default:
			case 'NONE':
				inOutCount = sizeof(PatHeader) + MDriver->header->numChn * tempPatHeader.size * sizeof(Cmd);
				break;
		}
		
		MDriver->partition[i] = (PatData*)malloc(inOutCount);
		if (MDriver->partition[i] == NULL) {
			for (x = 0; x < i; x++) {
				if (MDriver->partition[x] != NULL)
					free(MDriver->partition[x]);
			}
			free(MDriver->header);
			free(MDriver);
			
			return MADNeedMemory;
		}
		
		switch(InPutType) {
			case MADFileType:
				if (iRead(inOutCount, MDriver->partition[i], srcFile))
					theErr = MADIncompatibleFile;
				break;
				
#ifdef _MAC_H
			case MADCFReadStreamType:
				inOutCount -= sizeof(PatHeader);
				MDriver->partition[i]->header = tempPatHeader;
				bytesRead = CFReadStreamRead(MADReadStream, (UInt8*)&(MDriver->partition[i]->Cmds), inOutCount);
				if (bytesRead == -1)
					theErr = MADReadingErr;
				else if (bytesRead != inOutCount)
					theErr = MADIncompatibleFile;
				break;
#endif
				
			case MADPtrType:
				memcpy(MDriver->partition[i], MADPtr + OffSetToSample, inOutCount);
				OffSetToSample += inOutCount;
				break;
		}
		
		if (theErr != MADNoErr) {
			for (x = 0; x < i; x++) {
				if (MDriver->partition[x] != NULL)
					free(MDriver->partition[x]);
			}
			free(MDriver->header);
			free(MDriver);
			
			return theErr;
		}
		
		if (InPutType != MADCFReadStreamType)
			ByteSwapPatHeader(&MDriver->partition[i]->header);
		
		if (MDriver->partition[i]->header.compMode == 'MAD1') {
			tempPat = DecompressPartitionMAD1(MDriver, MDriver->partition[i]);
			
			if (tempPat == NULL) {
				for (x = 0; x < i; x++) {
					if (MDriver->partition[x] != NULL)
						free(MDriver->partition[x]);
				}
				free(MDriver->header);
				free(MDriver);
				
				return MADNeedMemory;
			}
			
			free(MDriver->partition[i]);
			MDriver->partition[i] = tempPat;
		}
	}
	
	/**** INSTRUMENTS ****/
	MDriver->fid = (InstrData*)calloc(sizeof(InstrData), MAXINSTRU);
	if (!MDriver->fid) {
		for (x = 0; x < MDriver->header->numPat; x++) {
			if (MDriver->partition[x] != NULL)
				free(MDriver->partition[x]);
		}
		free(MDriver->header);
		free(MDriver);
		
		return MADNeedMemory;
	}
	
	inOutCount = sizeof(InstrData) * MDriver->header->numInstru;
	switch (InPutType) {
		case MADFileType:
			if (iRead(inOutCount, MDriver->fid, srcFile))
				theErr = MADIncompatibleFile;
			break;
			
#ifdef _MAC_H
		case MADCFReadStreamType:
			bytesRead = CFReadStreamRead(MADReadStream, (UInt8*)MDriver->fid, inOutCount);
			if (bytesRead == -1)
				theErr = MADReadingErr;
			else if (bytesRead != inOutCount)
				theErr = MADIncompatibleFile;
			break;
#endif
			
		case MADPtrType:
			memcpy(MDriver->fid, MADPtr + OffSetToSample, inOutCount);
			OffSetToSample += inOutCount;
			break;
	}
	
	if (theErr != MADNoErr) {
		for (x = 0; x < MAXINSTRU ; x++)
			MADKillInstrument(MDriver, x);
		
		for (x = 0; x < i; x++) {
			if (MDriver->partition[x] != NULL)
				free(MDriver->partition[x]);
		}
		free(MDriver->header);
		free(MDriver);
		
		return theErr;
	}
	
	for (i = MDriver->header->numInstru - 1; i >= 0 ; i--) {
		InstrData *curIns = &MDriver->fid[i];
		
		ByteSwapInstrData(curIns);
		
		if (i != curIns->no) {
			MDriver->fid[curIns->no] = *curIns;
			MADResetInstrument(curIns);
		}
	}
	MDriver->header->numInstru = MAXINSTRU;
	
	/**** SAMPLES ****/
	MDriver->sample = (sData**) calloc(sizeof(sData*), MAXINSTRU * MAXSAMPLE);
	if (!MDriver->sample) {
		for (x = 0; x < MAXINSTRU ; x++)
			MADKillInstrument(MDriver, x);
		
		for (x = 0; x < MDriver->header->numPat; x++) {
			if (MDriver->partition[x] != NULL)
				free(MDriver->partition[x]);
		}
		free(MDriver->header);
		free(MDriver);
		
		return MADNeedMemory;
	}
	
	for (i = 0; i < MAXINSTRU ; i++) {
		for (x = 0; x < MDriver->fid[i].numSamples ; x++) {
			sData	 *curData;
			
			// ** Read Sample header **
			
			curData = MDriver->sample[i * MAXSAMPLE + x] = (sData*)malloc(sizeof(sData));
			if (curData == NULL) {
				for (x = 0; x < MAXINSTRU ; x++)
					MADKillInstrument(MDriver, x);
				
				for (x = 0; x < MDriver->header->numPat; x++) {
					if (MDriver->partition[x] != NULL)
						free(MDriver->partition[x]);
				}
				free(MDriver->header);
				free(MDriver);
				
				return MADNeedMemory;
			}
			
			inOutCount = sizeof(sData32);
			
			switch (InPutType) {
				case MADFileType:
					if (iRead(inOutCount, curData, srcFile))
						theErr = MADIncompatibleFile;
					break;
					
#ifdef _MAC_H
				case MADCFReadStreamType:
					bytesRead = CFReadStreamRead(MADReadStream, (UInt8*)curData, inOutCount);
					if (bytesRead == -1)
						theErr = MADReadingErr;
					else if (bytesRead != inOutCount)
						theErr = MADIncompatibleFile;
					break;
#endif
					
				case MADPtrType:
					memcpy(curData, MADPtr + OffSetToSample, inOutCount);
					OffSetToSample += inOutCount;
					break;
			}
			
			if (theErr != MADNoErr) {
				for (x = 0; x < MAXINSTRU ; x++)
					MADKillInstrument(MDriver, x);
				
				for (x = 0; x < i; x++) {
					if (MDriver->partition[x] != NULL)
						free(MDriver->partition[x]);
				}
				free(MDriver->header);
				free(MDriver);
				
				return theErr;
			}
			
			ByteSwapsData(curData);
			
			// ** Read Sample DATA
			
			curData->data = (char*)malloc(curData->size);
			if (curData->data == NULL) {
				for (x = 0; x < MAXINSTRU ; x++)
					MADKillInstrument(MDriver, x);
				
				for (x = 0; x < MDriver->header->numPat; x++) {
					if (MDriver->partition[x] != NULL)
						free(MDriver->partition[x]);
				}
				free(MDriver->header);
				free(MDriver);
				
				return MADNeedMemory;
			}
			
			inOutCount = curData->size;
			switch (InPutType) {
				case MADFileType:
					if (iRead(inOutCount, curData->data, srcFile))
						theErr = MADIncompatibleFile;
					break;
					
#ifdef _MAC_H
				case MADCFReadStreamType:
					bytesRead = CFReadStreamRead(MADReadStream, (UInt8*)curData->data, inOutCount);
					if (bytesRead == -1)
						theErr = MADReadingErr;
					else if (bytesRead != inOutCount)
						theErr = MADIncompatibleFile;
					break;
#endif
					
				case MADPtrType:
					memcpy(curData->data, MADPtr + OffSetToSample, inOutCount);
					OffSetToSample += inOutCount;
					break;
			}
			
			if (curData->amp == 16) {
				short	*shortPtr = (short*)curData->data;
#ifdef __BLOCKS__
				dispatch_apply(curData->size / 2, dispatch_get_global_queue(0, 0), ^(size_t ll) {
					MADBE16(&shortPtr[ll]);
				});
#else
				size_t 	ll;
				for (ll = 0; ll < curData->size / 2; ll++)
					MADBE16(&shortPtr[ll]);
#endif
			}
		}
	}
#ifdef __BLOCKS__
	dispatch_apply(MAXINSTRU, dispatch_get_global_queue(0, 0), ^(size_t ixi) {
		MDriver->fid[ixi].firstSample = ixi * MAXSAMPLE;
	});
#else
	for (i = 0; i < MAXINSTRU; i++)
		MDriver->fid[i].firstSample = i * MAXSAMPLE;
#endif
	
	// EFFECTS *** *** *** *** *** *** *** *** *** *** *** ***
	
	{
		short	alpha;
		
		MDriver->sets = (FXSets*)calloc(sizeof(FXSets), MAXTRACK);
		
		alpha = 0;
		
		for (i = 0; i < 10 ; i++) {	// Global Effects
			if (MDriver->header->globalEffect[i]) {
				inOutCount = sizeof(FXSets);
				switch (InPutType) {
					case MADFileType:
						if (iRead(inOutCount, &MDriver->sets[alpha], srcFile))
							theErr = MADIncompatibleFile;
						break;
						
#ifdef _MAC_H
					case MADCFReadStreamType:
						bytesRead = CFReadStreamRead(MADReadStream, (UInt8*)&MDriver->sets[alpha], inOutCount);
						if (bytesRead == -1)
							theErr = MADReadingErr;
						else if (bytesRead != inOutCount)
							theErr = MADIncompatibleFile;
						break;
#endif
						
					case MADPtrType:
						memcpy(&MDriver->sets[alpha], MADPtr + OffSetToSample, inOutCount);
						OffSetToSample += inOutCount;
						break;
				}
				
				if (theErr != MADNoErr) {
					for (x = 0; x < MAXINSTRU ; x++)
						MADKillInstrument(MDriver, x);
					
					for (x = 0; x < i; x++) {
						if (MDriver->partition[x] != NULL)
							free(MDriver->partition[x]);
					}
					free(MDriver->header);
					free(MDriver);
					
					return theErr;
				}

				SwapFXSets(&MDriver->sets[alpha]);
				alpha++;
			}
		}
		
		for (i = 0; i < MDriver->header->numChn ; i++) {	// Channel Effects
			for (x = 0; x < 4; x++) {
				if (MDriver->header->chanEffect[i][x]) {
					inOutCount = sizeof(FXSets);
					switch(InPutType) {
						case MADFileType:
							if (iRead(inOutCount, &MDriver->sets[alpha], srcFile))
								theErr = MADIncompatibleFile;
							break;
							
#ifdef _MAC_H
						case MADCFReadStreamType:
							bytesRead = CFReadStreamRead(MADReadStream, (UInt8*)&MDriver->sets[alpha], inOutCount);
							if (bytesRead == -1)
								theErr = MADReadingErr;
							else if (bytesRead != inOutCount)
								theErr = MADIncompatibleFile;
							break;
#endif
							
						case MADPtrType:
							memcpy(&MDriver->sets[alpha], MADPtr + OffSetToSample, inOutCount);
							OffSetToSample += inOutCount;
							break;
					}
					if (theErr != MADNoErr) {
						for (x = 0; x < MAXINSTRU ; x++)
							MADKillInstrument(MDriver, x);
						
						for (x = 0; x < i; x++) {
							if (MDriver->partition[x] != NULL)
								free(MDriver->partition[x]);
						}
						free(MDriver->header);
						free(MDriver);
						
						return theErr;
					}
					
					SwapFXSets(&MDriver->sets[alpha]);
					alpha++;
				}
			}
		}
		
	}
	// *** *** *** *** *** *** *** *** *** *** *** *** *** ***
	
	
	MDriver->header->MAD = 'MADK';
	
	//MADCleanDriver(MDriver);
	
	*music = MDriver;
	
#ifdef _MAC_H
	if (InPutType == MADCFReadStreamType)
		CFReadStreamClose(MADReadStream);
#endif
	
	return MADNoErr;
}

MADErr MADMusicSaveCString(MADMusic *music, const char *cName, bool compressMAD)
{
	int		alpha = 0;
	int		i, x;
	size_t	inOutCount;
	UNFILE	curFile = NULL;
	MADErr	theErr = MADNoErr;
	MADSpec aHeader;

	if (music->musicUnderModification)
		return MADWritingErr;
	
	iFileCreate(cName, 'MADK');
	curFile = iFileOpenWrite(cName);
	if (!curFile) {
		return MADWritingErr;
	}
	
	//TODO: error-checking
	
	music->musicUnderModification = true;
	
	for (i = 0, x = 0; i < MAXINSTRU; i++) {
		music->fid[i].no = i;
		
		// Is there something in this instrument?
		if (music->fid[i].numSamples > 0 || music->fid[i].name[0] != 0) {
			x++;
		}
	}
	
	aHeader = *music->header;
	aHeader.numInstru = x;
	ByteSwapMADSpec(&aHeader);
	iWrite(sizeof(MADSpec), &aHeader, curFile);
	
	if (compressMAD) {
		for (i = 0; i < music->header->numPat ; i++) {
			if (music->partition[i]) {
				PatData *tmpPat = CompressPartitionMAD1(music, music->partition[i]);
				inOutCount = tmpPat->header.patBytes + sizeof(PatHeader);
				tmpPat->header.compMode = 'MAD1';
				ByteSwapPatHeader(&tmpPat->header);
				iWrite(inOutCount, tmpPat, curFile);
				free(tmpPat);
			}
		}
	} else {
		for (i = 0; i < music->header->numPat; i++) {
			if (music->partition[i]) {
				PatData *tmpPat;
				inOutCount = sizeof(PatHeader);
				inOutCount += music->header->numChn * music->partition[i]->header.size * sizeof(Cmd);
				tmpPat = calloc(inOutCount, 1);
				memcpy(tmpPat, music->partition[i], inOutCount);
				tmpPat->header.compMode = 'NONE';
				ByteSwapPatHeader(&tmpPat->header);
				iWrite(inOutCount, tmpPat, curFile);
				free(tmpPat);
			}
		}
	}
	
	for (i = 0; i < MAXINSTRU; i++) {
		if (music->fid[i].numSamples > 0 || music->fid[i].name[0] != 0) {	// Is there something in this instrument?
			InstrData instData;
			music->fid[i].no = i;
			instData = music->fid[i];
			ByteSwapInstrData(&instData);
			iWrite(sizeof(InstrData), &instData, curFile);
		}
	}
	
	for (i = 0; i < MAXINSTRU; i++) {
		for (x = 0; x < music->fid[i].numSamples; x++) {
			sData	curData;
			sData32	copyData;
			void	*dataCopy = NULL;
			curData = *music->sample[music->fid[i].firstSample + x];
			
			inOutCount = sizeof(sData32);
			ByteSwapsData(&curData);
			memcpy(&copyData, &curData, inOutCount);
			copyData.data = 0;
			iWrite(inOutCount, &copyData, curFile);
			
			inOutCount = music->sample[music->fid[i].firstSample + x]->size;
			dataCopy = malloc(inOutCount);
			memcpy(dataCopy, curData.data, inOutCount);
			if (curData.amp == 16) {
				short *shortPtr = (short*)dataCopy;
#ifdef __BLOCKS__
				dispatch_apply(inOutCount / 2, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(size_t ll) {
					MADBE16(&shortPtr[ll]);
				});
#else
				size_t ll;
				for (ll = 0; ll < inOutCount / 2 ; ll++) {
					MADBE16(&shortPtr[ll]);
				}
#endif
			}
			iWrite(inOutCount, dataCopy, curFile);
			free(dataCopy);
		}
	}
	
	// EFFECTS *** *** *** *** *** *** *** *** *** *** *** ***
	
	for (i = 0; i < 10 ; i++) {	// Global Effects
		if (music->header->globalEffect[i]) {
			FXSets aSet;
			inOutCount = sizeof(FXSets);
			aSet = music->sets[alpha];
			SwapFXSets(&aSet);
			iWrite(inOutCount, &aSet, curFile);
			alpha++;
		}
	}
	
	for (i = 0; i < music->header->numChn; i++) {	// Channel Effects
		for (x = 0; x < 4; x++) {
			if (music->header->chanEffect[i][x]) {
				FXSets aSet;
				inOutCount = sizeof(FXSets);
				aSet = music->sets[alpha];
				SwapFXSets(&aSet);
				iWrite(inOutCount, &aSet, curFile);
				alpha++;
			}
		}
	}
	
	iClose(curFile);

	music->musicUnderModification = false;
	
	return theErr;
}

MADErr MADMusicSavePointer(MADMusic *music, void **outPtr, size_t *outPtrSize, bool compressMAD)
{
	return MADOrderNotImplemented;
}

MADErr MADKillSample(MADMusic *MDriver, short ins, short sample)
{
	short		i;
	bool		IsReading;
	
	if (MDriver->header == NULL)
		return MADDriverHasNoMusic;
	
	IsReading = MDriver->musicUnderModification;
	MDriver->musicUnderModification = true;
	
	if (MDriver->sample[ins * MAXSAMPLE + sample] == NULL)
		return MADParametersErr;
	if (MDriver->fid[ins].numSamples <= sample)
		return MADParametersErr;
	
	if (MDriver->sample[ins * MAXSAMPLE + sample]->data)
		free(MDriver->sample[ins * MAXSAMPLE + sample]->data);
	free(MDriver->sample[ins * MAXSAMPLE + sample]);
	MDriver->sample[ins * MAXSAMPLE + sample] = NULL;
	
	MDriver->fid[ins].numSamples--;
	
	for (i = sample ; i < MDriver->fid[ins].numSamples; i++) {
		MDriver->sample[ins * MAXSAMPLE + i] = MDriver->sample[ins * MAXSAMPLE + i + 1];
	}
	
	for (i = 0; i < NUMBER_NOTES; i++) {
		if (MDriver->fid[ins].what[i] >= sample) {
			if (MDriver->fid[ins].what[i] > 0)
				MDriver->fid[ins].what[i]--;
		}
	}
	
	MDriver->musicUnderModification = IsReading;
	
	return MADNoErr;
}

sData *MADCreateSample(MADMusic *MDriver, short ins, short sample)
{
	sData	*curData;
	
	curData = (sData*)calloc(sizeof(sData), 1);
	if (curData) {
		//curData->size		= 0;
		//curData->loopBeg	= 0;
		//curData->loopSize	= 0;
		curData->vol		= MAX_VOLUME;
		curData->c2spd		= NOFINETUNE;
		curData->loopType	= MADLoopTypeClassic;
		curData->amp		= 8;
		//curData->relNote	= 0;
		//curData->name
		//curData->data		= NULL;
		
		// Install it
		
		if (sample < MDriver->fid[ins].numSamples) MADDebugStr(__LINE__, __FILE__, "MADCreateSample");
		
		MDriver->sample[ins * MAXSAMPLE + sample] = curData;
		MDriver->fid[ins].numSamples++;
	}
	
	return curData;
}

void MADPurgeTrack(MADDriverRec *intDriver)
{
	size_t i;
	
	for (i = 0; i<intDriver->DriverSettings.numChn; i++) {
		intDriver->base.chan[i].prevPtr			= 0;
		
		intDriver->base.chan[i].curPtr			= intDriver->base.chan[i].maxPtr;
		intDriver->base.chan[i].samplePtr		= NULL;
		intDriver->base.chan[i].lAC				= 0;
		intDriver->base.chan[i].loopBeg			= 0;
		intDriver->base.chan[i].loopSize			= 0;
		intDriver->base.chan[i].RemoverWorking	= false;
		intDriver->base.chan[i].TICKREMOVESIZE	= 1;
		
		intDriver->base.chan[i].lastWordR		= 0;
		intDriver->base.chan[i].lastWordL		= 0;
		intDriver->base.chan[i].curLevelL		= 0;
		intDriver->base.chan[i].curLevelR		= 0;
		intDriver->base.chan[i].prevPtr			= 0;
		intDriver->base.chan[i].prevVol0			= 1;
		intDriver->base.chan[i].prevVol1			= 1;
		intDriver->base.chan[i].preOff			= 0xFFFFFFFF;
		intDriver->base.chan[i].preVal2			= 0;
		intDriver->base.chan[i].preVal2R			= 0;
		
		intDriver->base.chan[i].spreVal2			= 0;
		intDriver->base.chan[i].spreVal2R		= 0;
		
		intDriver->base.chan[i].preVal			= 0;
		intDriver->base.chan[i].spreVal			= 0;
		intDriver->base.chan[i].LevelDirectionR	= true;
		intDriver->base.chan[i].LevelDirectionL	= true;
	}
}

void MADPurgeTrackIfInstru(MADDriverRec *intDriver, short instru)
{
	size_t i;
	
	for (i = 0; i < intDriver->DriverSettings.numChn; i++) {
		if (intDriver->base.chan[i].ins == instru && intDriver->base.chan[i].curPtr != intDriver->base.chan[i].maxPtr) {
			//intDriver->base.chan[i].lastWord		= 0;
			//intDriver->base.chan[i].curLevel		= 0;
			intDriver->base.chan[i].prevPtr			= 0;
			
			intDriver->base.chan[i].curPtr			= intDriver->base.chan[i].maxPtr;
			intDriver->base.chan[i].samplePtr		= NULL;
			intDriver->base.chan[i].lAC				= 0;
			intDriver->base.chan[i].loopBeg			= 0;
			intDriver->base.chan[i].loopSize			= 0;
			intDriver->base.chan[i].RemoverWorking	= false;
			intDriver->base.chan[i].TICKREMOVESIZE	= 1;
			
			intDriver->base.chan[i].lastWordR		= 0;
			intDriver->base.chan[i].lastWordL		= 0;
			intDriver->base.chan[i].curLevelL		= 0;
			intDriver->base.chan[i].curLevelR		= 0;
			intDriver->base.chan[i].prevPtr			= 0;
			intDriver->base.chan[i].prevVol0			= 1;
			intDriver->base.chan[i].prevVol1			= 1;
			intDriver->base.chan[i].preOff			= 0xFFFFFFFF;
			intDriver->base.chan[i].preVal2			= 0;
			intDriver->base.chan[i].preVal2R			= 0;
			
			intDriver->base.chan[i].spreVal2			= 0;
			intDriver->base.chan[i].spreVal2R		= 0;
			
			intDriver->base.chan[i].preVal			= 0;
			intDriver->base.chan[i].spreVal			= 0;
			intDriver->base.chan[i].LevelDirectionR	= true;
			intDriver->base.chan[i].LevelDirectionL	= true;
		}
	}
}

MADErr MADCleanCurrentMusic(MADMusic *MDriver, MADDriverRec *intDriver)
{
	if (MDriver->header != NULL) {
		short i, x;
		
		if (intDriver) {
			if (intDriver->base.Pat > MDriver->header->numPat)
				MADReset(intDriver);
			if (intDriver->base.PartitionReader > MDriver->partition[intDriver->base.Pat]->header.size)
				MADReset(intDriver);
		}
		
		for (i = 0; i < MAXINSTRU; i++) {
			for (x = 0; x < 96; x++) {
				if (MDriver->fid[i].what[x]) {
					if (MDriver->fid[i].what[x] >= MDriver->fid[i].numSamples)
						MDriver->fid[i].what[x] = 0;
				}
			}
			
			for (x = 0; x < MDriver->fid[i].numSamples; x++) {
				sData	*curData = MDriver->sample[i * MAXSAMPLE + x];
				
				if (curData == NULL)
					curData = MDriver->sample[i * MAXSAMPLE + x] = MADCreateSample(MDriver, i, x);
				
				if (curData->data == NULL) {
					curData->data = NULL;
					curData->size = 0;
				}
				
				if (curData->size <= 0) {
					curData->loopBeg = 0;
					curData->loopSize = 0;
				}
				
				if (curData->loopSize < 0)
					curData->loopSize = 0;
				
				if (curData->loopBeg > curData->size) {
					curData->loopBeg = 0;
					curData->loopSize = 0;
				}
				
				if (curData->loopBeg + curData->loopSize > curData->size) {
					curData->loopBeg = 0;
					curData->loopSize = 0;
				}
			}
		}
	}
	
	return MADNoErr;
}

MADErr MADResetInstrument(InstrData *curIns)
{
	short firstSample, num;
	if (curIns == NULL)
		return MADParametersErr;
	
	firstSample = curIns->firstSample;
	num = curIns->no;
	
	memset(curIns, 0, sizeof(InstrData));
	curIns->firstSample = firstSample;
	curIns->volFade = DEFAULT_VOLFADE;
	curIns->no = num;
	return MADNoErr;
}

MADErr MADKillInstrument(MADMusic *music, short ins)
{
	short		i;
	InstrData	*curIns;
	bool		IsReading;
	
	if (music == NULL)
		return MADParametersErr;
	
	curIns = &music->fid[ins];
	
	IsReading = music->musicUnderModification;
	music->musicUnderModification = true;
	
	for (i = 0; i < curIns->numSamples; i++) {
		if (music->sample[ins * MAXSAMPLE + i] != NULL) {
			if (music->sample[ins * MAXSAMPLE + i]->data != NULL) {
				free(music->sample[ins * MAXSAMPLE + i]->data);
				music->sample[ins * MAXSAMPLE + i]->data = NULL;
			}
			free(music->sample[ins * MAXSAMPLE + i]);
			music->sample[ins * MAXSAMPLE + i] = NULL;
		}
	}
	
	MADResetInstrument(curIns);
	music->musicUnderModification = IsReading;
	
	return MADNoErr;
}

MADErr MADKillCmd(Cmd *aCmd)
{
	if (aCmd == NULL)
		return MADParametersErr;
	
	*aCmd = (Cmd){0, 0xff, 0, 0, 0xff, 0};
	
	return MADNoErr;
}

void MADCheckSpeed(MADMusic *MDriver, MADDriverRec *intDriver)
{
	short		i, x, y;
	short		curPartitionReader;
	Cmd			*aCmd;
	bool		CmdSpeed = false, FineFound = false, ArgFound[MAXTRACK], NoteFound[MAXTRACK], InsFound[MAXTRACK];
	
	if (!MDriver || MDriver->header == NULL || !intDriver)
		return;
	
	for (i = 0; i < MAXTRACK; i++) {
		ArgFound[i] = false;
		NoteFound[i] = false;
		InsFound[i] = false;
	}
	
	for (i = intDriver->base.PL; i >= 0; i--) {
		if (i == intDriver->base.PL) {
			curPartitionReader = intDriver->base.PartitionReader;
			if (curPartitionReader >= MDriver->partition[MDriver->header->oPointers[i]]->header.size)
				curPartitionReader--;
		} else {
			curPartitionReader = MDriver->partition[MDriver->header->oPointers[i]]->header.size-1;
		}
		
		for (x = curPartitionReader; x >= 0; x--) {
			for (y = MDriver->header->numChn-1; y >= 0 ; y--) {
				MADChannel	*ch = &intDriver->base.chan[y];
				
				aCmd = GetMADCommand(x, y, MDriver->partition[MDriver->header->oPointers[i]]);
				
				if (aCmd->cmd == MADEffectSpeed) {
					if (aCmd->arg < 32) {
						if (aCmd->arg != 0) {
							if (CmdSpeed == false)
								intDriver->base.speed = aCmd->arg;
							CmdSpeed = true;
						}
					} else {
						if (aCmd->arg != 0) {
							if (FineFound == false)
								intDriver->base.finespeed = aCmd->arg;
							FineFound = true;
						}
					}
				}
				
				if (aCmd->arg != 0) {
					if (!ArgFound[y])
						ch->oldArg[aCmd->cmd] = aCmd->arg;
					ArgFound[y] = true;
				}
				
				if (aCmd->ins != 0) {
					if (!InsFound[y])
						ch->insOld = aCmd->ins;
					InsFound[y] = true;
				}
				
				if (aCmd->note != 0xFF) {
					if (!NoteFound[y])
						ch->noteOld = aCmd->note;
					NoteFound[y] = true;
				}
			}
		}
	}
	
	if (!CmdSpeed)
		intDriver->base.speed = MDriver->header->speed;
	if (!FineFound)
		intDriver->base.finespeed = MDriver->header->tempo;
}

void MADCheckSpeedPattern(MADMusic *MDriver, MADDriverRec *intDriver)
{
	short		x, y;
	short		curPartitionReader;
	Cmd			*aCmd;
	bool		CmdSpeed = false, FineFound = false;
	
	if (!MDriver || MDriver->header == NULL || !intDriver)
		return;
	
	curPartitionReader = intDriver->base.PartitionReader;
	if (curPartitionReader >= MDriver->partition[intDriver->base.Pat]->header.size)
		curPartitionReader--;
	
	for (x = curPartitionReader; x >= 0; x--) {
		for (y = MDriver->header->numChn-1; y >= 0 ; y--) {
			//MADChannel	*ch = &intDriver->base.chan[y];
			
			aCmd = GetMADCommand(x, y, MDriver->partition[intDriver->base.Pat]);
			
			if (aCmd) {
				if (aCmd->cmd == MADEffectSpeed) {
					if (aCmd->arg < 32) {
						if (aCmd->arg != 0) {
							if (CmdSpeed == false)
								intDriver->base.speed = aCmd->arg;
							CmdSpeed = true;
						}
					} else {
						if (aCmd->arg != 0) {
							if (FineFound == false)
								intDriver->base.finespeed = aCmd->arg;
							FineFound = true;
						}
					}
				}
			}
		}
	}
	
	if (!CmdSpeed)
		intDriver->base.speed 		= MDriver->header->speed;
	if (!FineFound)
		intDriver->base.finespeed 	= MDriver->header->tempo;
}

MADErr MADPlayMusic(MADDriverRec *MDriver)
{
	if (!MDriver)
		return MADParametersErr;
	if (!MDriver->base.curMusic)
		return MADDriverHasNoMusic;
	
	MDriver->base.Reading = true;
	
	return MADNoErr;
}

bool MADIsPlayingMusic(MADDriverRec *driver)
{
	if (driver == NULL)
		return false;
	
	return driver->base.Reading;
}

MADErr MADStopMusic(MADDriverRec *MDriver)
{
	if (!MDriver)
		return MADParametersErr;
	if (!MDriver->base.curMusic)
		return MADDriverHasNoMusic;
	
	MDriver->base.Reading = false;
	
	if (MDriver->SendMIDIClockData)
		SendMIDIClock(MDriver, 0xFC);
	
	return MADNoErr;
}

MADErr MADStartDriver(MADDriverRec *MDriver)
{
	if (MDriver == NULL)
		return MADParametersErr;
	
	MDriver->MADPlay = true;
	MDriver->base.Reading = false;
	MDriver->base.musicEnd = false;
	
	MADCleanDriver(MDriver);
	
	MADCheckSpeed(MDriver->base.curMusic, MDriver);
	
	switch(MDriver->DriverSettings.driverMode) {
		case MIDISoundDriver:
			//PlayChannel(MDriver);
			break;
			
#ifdef _MAC_H
		case CoreAudioDriver:
			break;
#endif
			
#ifdef _ESOUND
		case ESDDriver:
			PlayChannelESD(MDriver);
			break;
#endif
			
#ifdef HAS_PULSEAUDIO
		case PulseAudioDriver:
			PlayChannellPulseAudio(MDriver);
			break;
#endif
			
#ifdef HAS_PORTAUDIO
		case PortAudioDriver:
			PlayChannelPortAudio(MDriver);
			break;
#endif
			
		default:
			break;
	}
	
	return MADNoErr;
}

MADErr MADStopDriver(MADDriverRec *MDriver)
{
	if (!MDriver)
		return MADParametersErr;
	
	MDriver->MADPlay = false;
	
	MDriver->clipL	= false;
	MDriver->clipR	= false;
	
	MDriver->levelL = 0;
	MDriver->levelR = 0;
	
	switch (MDriver->DriverSettings.driverMode) {
		case MIDISoundDriver:
			//AllNoteOff(MDriver);
			//StopChannel(MDriver);
			break;
			
#ifdef _MAC_H
		case CoreAudioDriver:
			break;
#endif
			
#ifdef _ESOUND
		case ESDDriver:
			StopChannelESD(MDriver);
			break;
#endif
			
#ifdef HAS_PULSEAUDIO
		case PulseAudioDriver:
			StopChannelPulseAudio(MDriver);
			break;
#endif
			
#ifdef HAS_PORTAUDIO
		case PortAudioDriver:
			StopChannelPortAudio(MDriver);
			break;
#endif
		default:
			break;
	}
	
	MADCleanDriver(MDriver);
	
	if (MDriver->SendMIDIClockData)
		SendMIDIClock(MDriver, 0xFC);
	
	return MADNoErr;
}

MADErr MADReset(MADDriverRec *MDriver)
{
	MADCleanDriver(MDriver);
	
	MDriver->BufCounter = 0;
	MDriver->BytesToGenerate = 0;
	MDriver->smallcounter = 128;
	
	MDriver->base.PL = 0;
	MDriver->base.PartitionReader = 0;
	
	if (MDriver->base.curMusic != NULL) {
		MDriver->base.Pat = MDriver->base.curMusic->header->oPointers[MDriver->base.PL];
		MDriver->base.speed = MDriver->base.curMusic->header->speed;
		MDriver->base.finespeed = MDriver->base.curMusic->header->tempo;
	}
	
	return MADNoErr;
}

short MADGetNextReader(MADMusic *music, MADDriverRec *intDriver, short cur, short *pat)
{
	if (music == NULL)
		return 0;
	if (intDriver == NULL)
		return 0;
	if (music->header == NULL)
		return 0;
	
	cur++;
	if (cur >= music->partition[intDriver->base.Pat]->header.size) {
		cur = 0;
		
		if (intDriver->JumpToNextPattern) {
			*pat = music->header->oPointers[intDriver->base.PL + 1];
			
			if (intDriver->base.speed == 1 && intDriver->base.PL + 1 >= music->header->numPointers)
				*pat = music->header->oPointers[0];
		}
	}
	
	return cur;
}

MADErr MADDisposeMusic(MADMusic **music, MADDriverRec *MDriver)
{
	short x;
	
	if (!music)
		return MADNoErr;
	if (!*music)
		return MADNoErr;
	
	(*music)->musicUnderModification = true;
	
	if (MDriver) {
		if (MDriver->base.curMusic == *music)
			MDriver->base.curMusic = NULL;
	}
	
	if ((*music)->header != NULL) {
		for (x = 0; x < (*music)->header->numPat ; x++) {
			if ((*music)->partition[x] != NULL)
				free((*music)->partition[x]);
			(*music)->partition[x] = NULL;
		}
		
		for (x = (*music)->header->numPat; x < MAXPATTERN ; x++) {
			if ((*music)->partition[x] != NULL)
				return MADUnknownErr;
		}
		
		for (x = 0; x < MAXINSTRU ; x++)
			MADKillInstrument(*music, x);
		
		free((*music)->header);
		
		if ((*music)->fid == NULL)
			return MADUnknownErr;
		free((*music)->fid);
		
		if ((*music)->sample == NULL)
			return MADUnknownErr;
		free((*music)->sample);
		
		if ((*music)->sets == NULL)
			return MADUnknownErr;
		free((*music)->sets);
	}
	
	free(*music);
	*music = NULL;
	
	return MADNoErr;
}

#pragma pack(push, 2)

bool MADIsPressed(unsigned char* km2, unsigned short k)
{
	return (bool)((km2[k >> 3] >> (k & 7)) & 1);
}

#if 0
MADErr MADPlaySoundDataSYNC(MADDriverRec *MDriver, char *soundPtr, long size, long channel, long note, long amplitude, long loopBeg, long loopSize, double rate, bool stereo)
{
	MADErr		iErr;
	bool		continueLoop;
	KeyMap		km;
	
	iErr = MADPlaySoundData(MDriver, soundPtr, size, channel, note, amplitude, loopBeg, loopSize, rate, stereo);
	if (iErr == MADNoErr)
	{
		continueLoop = true;
		while(continueLoop)
		{
			GetKeys(km);
			
			if (MDriver->base.chan[channel].samplePtr == NULL) continueLoop = false;
			if (MADIsPressed((unsigned char*) km, 0x37) && MADIsPressed((unsigned char*) km, 0x2F)) continueLoop = false;
			if (Button()) continueLoop = false;
			
#if MAINPLAYERPRO
			DoGlobalNull();
			WaitNextEvent(everyEvent, &theEvent, 1, NULL);
#endif
		}
		
		if (MDriver->base.chan[channel].samplePtr != NULL)
		{
			MDriver->base.chan[channel].curPtr 		= MDriver->base.chan[channel].maxPtr;
			MDriver->base.chan[channel].samplePtr	= NULL;
			MDriver->base.chan[channel].lAC			= 0;
			MDriver->base.chan[channel].loopBeg 	= 0;
			MDriver->base.chan[channel].loopSize 	= 0;
		}
	}
	
	return iErr;
}
#endif

MADErr MADPlaySoundData(MADDriverRec *MDriver, const char *soundPtr, size_t size, int channel, MADByte note, int amplitude, size_t loopBeg, size_t loopSize, unsigned int rate, bool stereo)
{
	MADChannel *curVoice;
	
	if (channel < 0 || channel >= MDriver->DriverSettings.numChn)
		return MADParametersErr;
	
	if (MDriver->base.curMusic != NULL)
		MDriver->base.curMusic->musicUnderModification = true;
	
	curVoice = &MDriver->base.chan[channel];
	
	curVoice->prevPtr		= NULL;
	curVoice->samplePtr		= (char*)soundPtr;
	curVoice->stereo		= stereo;
	
	curVoice->maxPtr = curVoice->curPtr = curVoice->begPtr = (char*)soundPtr;
	curVoice->maxPtr		+= size;
	curVoice->sizePtr		= size;
	curVoice->lAC			= 0;
	curVoice->pingpong		= false;
	curVoice->PanningE8		= false;
	curVoice->trig			= 0;
	curVoice->preOff		= 0xFFFFFFFF;
	curVoice->preVal		= 0;
	curVoice->spreVal		= 0;
	curVoice->preVal2		= *curVoice->curPtr;
	if (curVoice->amp == 8)
		curVoice->preVal2R	= *(curVoice->curPtr+1);
	else
		curVoice->preVal2R	= *(curVoice->curPtr+2);
	curVoice->spreVal2	= *(short*)curVoice->curPtr;
	curVoice->spreVal2R	= *(short*)(curVoice->curPtr+2);
	
	if (note == 0xFF)
		note = 48;
	curVoice->note		= note;
	curVoice->ins		= 0;
	curVoice->viboffset	= 0;
	curVoice->amp		= amplitude;
	curVoice->fineTune	= rate;
	
	curVoice->period	= GetOldPeriod(curVoice->note, curVoice->fineTune, MDriver);
	
	if (loopBeg > size) {
		loopBeg = 0;
		loopSize = 0;
	}
	
	if (loopBeg + loopSize > size) {
		loopBeg = 0;
		loopSize = 0;
	}
	
	curVoice->loopBeg 		= loopBeg;
	curVoice->loopSize	 	= loopSize;
	
	if (loopSize > 0)
		curVoice->maxPtr 	= (char*)((size_t)curVoice->begPtr + loopBeg + loopSize);
	
	curVoice->pann			= 32;
	
	curVoice->vol 			= 64;
	curVoice->volFade		= 32767;
	curVoice->nextvolFade	= 32767;
	curVoice->volEnv		= 64;
	curVoice->KeyOn			= true;
	
	if (MDriver->base.curMusic != NULL)
		MDriver->base.curMusic->musicUnderModification = false;
	
	return MADNoErr;
}

void MADKeyOFF(MADDriverRec *MDriver, short track)
{
	if (track == -1) {
		short i;
		
		for (i = 0; i < MAXTRACK; i++)
			MDriver->base.chan[i].KeyOn = false;
	} else
		MDriver->base.chan[track].KeyOn = false;
}

#pragma pack(pop)

Cmd* GetMADCommand(short PosX, short TrackIdX, PatData* tempMusicPat)
{
	if (tempMusicPat == NULL)
		return NULL;
	
	if (PosX < 0)
		PosX = 0;
	else if (PosX >= tempMusicPat->header.size)
		PosX = tempMusicPat->header.size -1;
	
	return &(tempMusicPat->Cmds[(tempMusicPat->header.size * TrackIdX) + PosX]);
}

void MADDisposeVolumeTable(MADDriverRec *intDriver)
{
	if (intDriver == NULL) {
		return;
	}
	
	if (intDriver->DriverSettings.outPutMode == DeluxeStereoOutPut)
		MADKillOverShoot(intDriver);
}

MADErr MADCreateVolumeTable(MADDriverRec *intDriver)
{
	int		Tracks = 0;
	MADErr	theErr;
	if (intDriver == NULL)
		return MADParametersErr;
	
	Tracks = intDriver->DriverSettings.numChn;
	theErr = MADCreateMicroDelay(intDriver);
	if (theErr != MADNoErr)
		return theErr;
	
	switch(intDriver->DriverSettings.outPutMode) {
		case DeluxeStereoOutPut:
			Tracks	= 1;
			MADCreateOverShoot(intDriver);
			break;
			
		case PolyPhonic:
			Tracks 	= 1;
			break;
			
		default:
			Tracks = 1;
			break;
	}
	
	return MADNoErr;
}

void MADChangeTracks(MADDriverRec *MDriver, short newVal)
{
	bool	play = MDriver->MADPlay, reading = MDriver->base.Reading;
	
	MADStopDriver(MDriver);
	MDriver->DriverSettings.numChn = newVal;
	MADDisposeVolumeTable(MDriver);
	MADCreateVolumeTable(MDriver);
	
	MDriver->trackDiv = MDriver->DriverSettings.numChn;
	
	if (play)
		MADStartDriver(MDriver);
	if (reading)
		MADPlayMusic(MDriver);
}

void MADUpdateTracksNumber(MADDriverRec *MDriver)
{
	if (MDriver->base.curMusic != NULL) {
		if (MDriver->base.curMusic->header != NULL) {
			if (MDriver->base.curMusic->header->numChn != MDriver->DriverSettings.numChn)
				MADChangeTracks(MDriver, MDriver->base.curMusic->header->numChn);
		}
	}
}

MADErr MADCreateVibrato(MADDriverRec *MDriver)
{
	static const short vibrato_table[64] = {
		0,24,49,74,97,120,141,161,
		180,197,212,224,235,244,250,253,
		255,253,250,244,235,224,212,197,
		180,161,141,120,97,74,49,24
	};
	
	/*	{
	 0,50,100,149,196,241,284,325,362,396,426,452,473,490,502,510,512,
	 510,502,490,473,452,426,396,362,325,284,241,196,149,100,50,0,-49,
	 -99,-148,-195,-240,-283,-324,-361,-395,-425,-451,-472,-489,-501,
	 -509,-511,-509,-501,-489,-472,-451,-425,-395,-361,-324,-283,-240,
	 -195,-148,-99,-49
	 };*/
	
	
	memcpy(MDriver->vibrato_table, vibrato_table, sizeof(vibrato_table));
	
	return MADNoErr;
}

typedef MADOPTIONS(MADByte, MADCompBit) {
	ins 	= 1 << 0,
	note	= 1 << 1,
	cmd		= 1 << 2,
	argu	= 1 << 3,
	vol		= 1 << 4
};

PatData* DecompressPartitionMAD1(MADMusic *MDriver, PatData* myPat)
{
	PatData*	finalPtr;
	MADByte		*srcPtr;
	Cmd			*myCmd;
	short		maxCmd;
	MADCompBit	set;
	
	finalPtr = (PatData*)malloc(sizeof(PatHeader) + myPat->header.size * MDriver->header->numChn * sizeof(Cmd));
	if (finalPtr == NULL)
		return NULL;
	
	memcpy(finalPtr, myPat, sizeof(PatHeader));
	
	srcPtr = (MADByte*)myPat->Cmds;
	myCmd = (Cmd*)finalPtr->Cmds;
	maxCmd = finalPtr->header.size * MDriver->header->numChn;
	
	/*** Decompression Routine ***/
	
	while (maxCmd != 0) {
		maxCmd--;
		
		MADKillCmd(myCmd);
		
		set = *srcPtr++;
		
		if (set & ins)
			myCmd->ins = *srcPtr++;
		
		if (set & note)
			myCmd->note = *srcPtr++;
		
		if (set & cmd)
			myCmd->cmd = *srcPtr++;
		
		if (set & argu)
			myCmd->arg = *srcPtr++;
		
		if (set & vol)
			myCmd->vol = *srcPtr++;
		
		myCmd++;
	}
	
	return finalPtr;
}

PatData* CompressPartitionMAD1(MADMusic *MDriver, PatData* myPat)
{
	PatData		*finalPtr;
	uint8_t		*dstPtr;
	MADCompBit	*setByte;
	Cmd		*myCmd;
	int		maxCmd;
	size_t	NewPtrSize = 0;
	
	finalPtr = (PatData*)malloc(sizeof(PatHeader) + myPat->header.size * MDriver->header->numChn * 6L);
	if (finalPtr == NULL)
		return NULL;
	
	memcpy(finalPtr, myPat, sizeof(PatHeader));
	
	dstPtr = (uint8_t*)finalPtr->Cmds;
	myCmd = (Cmd*)myPat->Cmds;
	maxCmd = myPat->header.size * MDriver->header->numChn;
	
	/*** Compression Routine ***/
		
	while (maxCmd != 0) {
		maxCmd--;
		
		// Set byte
		setByte 	= dstPtr;
		*dstPtr++ 	= 0x00;
		NewPtrSize++;
		////////
		
		if (myCmd->ins > 0) {
			(*setByte) |= ins;
			*dstPtr++ = myCmd->ins;
			NewPtrSize++;
		}
		
		if (myCmd->note != 0xFF) {
			(*setByte) |= note;
			*dstPtr++ = myCmd->note;
			NewPtrSize++;
		}
		
		if (myCmd->cmd > 0) {
			(*setByte) |= cmd;
			*dstPtr++ = myCmd->cmd;
			NewPtrSize++;
		}
		
		if (myCmd->arg > 0) {
			(*setByte) |= argu;
			*dstPtr++ = myCmd->arg;
			NewPtrSize++;
		}
		
		if (myCmd->vol != 0xFF) {
			(*setByte) |= vol;
			*dstPtr++ = myCmd->vol;
			NewPtrSize++;
		}
		
		myCmd++;
	}
	
	finalPtr->header.patBytes = (int)NewPtrSize;
	
	finalPtr = realloc(finalPtr, NewPtrSize + sizeof(PatHeader));
	
	return finalPtr;
}

bool MADWasReading(MADDriverRec *driver)
{
	return MADIsPlayingMusic(driver);
}

void MADSetReading(MADDriverRec *driver, bool toSet)
{
	if (toSet == true) {
		MADPlayMusic(driver);
	} else {
		MADStopMusic(driver);
	}
}

bool MADIsDonePlaying(MADDriverRec *MDriver)
{
	if (!MDriver) {
		return false;
	}
	return MDriver->base.musicEnd;
}

void MADBeginExport(MADDriverRec *driver)
{
	if (driver) {
		driver->currentlyExporting = true;
	}
}

void MADEndExport(MADDriverRec *driver)
{
	if (driver) {
		driver->currentlyExporting = false;
	}
}

bool MADIsExporting(MADDriverRec *driver)
{
	if (!driver) {
		return false;
	} else {
		return driver->currentlyExporting;
	}
}

int MADAudioLength(MADDriverRec *theRec)
{
	if (!theRec)
		return 0;
	return (theRec->ASCBUFFERReal - theRec->BytesToRemoveAtEnd);
}
