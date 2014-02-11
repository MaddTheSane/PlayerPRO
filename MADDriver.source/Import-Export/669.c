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

#ifdef __APPLE__
#include <PlayerPROCore/PlayerPROCore.h>
#else
#include "RDriver.h"
#include "FileUtils.h"
#endif
#include "669.h"

#if defined(NOEXPORTFUNCS) && NOEXPORTFUNCS
#include "embeddedPlugs.h"
#endif

#ifdef _MAC_H
#define Tdecode16(msg_buf) CFSwapInt16LittleToHost(*(short*)msg_buf)
#define Tdecode32(msg_buf) CFSwapInt32LittleToHost(*(int*)msg_buf)
#else
#ifdef __LITTLE_ENDIAN__
#define Tdecode16(msg_buf) *(short*)msg_buf
#define Tdecode32(msg_buf) *(int*)msg_buf
#else

static inline UInt16 Tdecode16( void *msg_buf)
{
	UInt16 toswap = *((UInt16*) msg_buf);
	PPLE16(&toswap);
	return toswap;
}

static inline UInt32 Tdecode32( void *msg_buf)
{
	UInt32 toswap = *((UInt32*) msg_buf);
	PPLE32(&toswap);
	return toswap;
}

#endif
#endif

#ifdef WIN32
#define strlcpy(dst, src, size) strncpy_s(dst, size, src, _TRUNCATE)
#endif

static OSErr Convert6692Mad(Ptr AlienFile, size_t MODSize, MADMusic *theMAD, MADDriverSettings *init)
{
	SixSixNine	*the669;
	short		i, x, z;
	SInt32		OffSetToSample;
	size_t		temp;
	Ptr			MaxPtr;
	Ptr			theInstrument[64], destPtr;
	short		Note, Octave;
	Byte		*thePasByte;
	
	/**** Variables pour le MAD ****/
	Cmd			*aCmd;
	
	/**** Variables pour le MOD ****/
	struct PatSix		*PatInt;
	struct PatCmd		*theCommand;
	struct SampleInfo	*SInfo;
	
	/********************************/
	
	the669 = (SixSixNine*)AlienFile;
	theMAD->header = (MADSpec*)calloc(sizeof(MADSpec), 1);
	
	if (!theMAD->header)
		return MADNeedMemory;
	
	MaxPtr = (Ptr)((size_t)the669 + MODSize);
	
	OffSetToSample = 0x1f1 + the669->NOS * 25 + the669->NOP * 0x600L;
	
	for (i = 0; i < the669->NOS ; i++) {
		temp = (size_t) the669;
		temp += 0x1f1L + i*25L + 13L;
		
		SInfo = (SampleInfo*)temp;
		
		PPLE16(&SInfo->length);
		PPLE16(&SInfo->loopStart);
		PPLE16(&SInfo->loopEnd);
		
		theInstrument[i] = (Ptr)((size_t)the669 + (size_t)OffSetToSample);
		OffSetToSample += SInfo->length;
	}
	
	/******** Le 669 a été lu et analysé ***********/
	/******** Copie des informations dans le MAD ***/
	
	theMAD->header->MAD = 'MADK';
	strlcpy(theMAD->header->name, the669->message, sizeof(theMAD->header->name));
	
	strlcpy(theMAD->header->infos, "Converted by PlayerPRO 669 Plug (\xA9\x41ntoine ROSSET <rossetantoine@bluewin.ch>)", sizeof(theMAD->header->infos));
	
	theMAD->header->numPointers = 128;	//the669->loopOrder;
	theMAD->header->tempo = 125;
	theMAD->header->speed = 6;
	theMAD->header->numPat = the669->NOP;
	for(i=0; i<128; i++) {
		theMAD->header->oPointers[i] = the669->orderList[i];
		if (theMAD->header->oPointers[i] >= theMAD->header->numPat)
			theMAD->header->oPointers[i] = theMAD->header->numPat - 1;
	}
	theMAD->header->numChn = 8;
	
	theMAD->sets = (FXSets*)calloc(sizeof(FXSets), MAXTRACK);
	if (!theMAD->sets) {
		free(theMAD->header);
		return MADNeedMemory;
	}
	//TODO: dispatch this
	for (i = 0; i < MAXTRACK; i++) theMAD->header->chanBus[i].copyId = i;
	
	//TODO: dispatch this
	for (i = 0; i < MAXTRACK; i++) {
		if (i % 2 == 0)
			theMAD->header->chanPan[i] = MAX_PANNING / 4;
		else
			theMAD->header->chanPan[i] = MAX_PANNING - MAX_PANNING / 4;
		
		theMAD->header->chanVol[i] = MAX_VOLUME;
	}
	
	theMAD->header->generalVol		= 64;
	theMAD->header->generalSpeed	= 80;
	theMAD->header->generalPitch	= 80;
	
	theMAD->fid = (InstrData*) calloc(sizeof(InstrData), MAXINSTRU);
	if (!theMAD->fid) {
		free(theMAD->sets);
		free(theMAD->header);
		return MADNeedMemory;
	}
	
	theMAD->sample = (sData**) calloc(sizeof(sData*), MAXINSTRU * MAXSAMPLE);
	if (!theMAD->sample) {
		free(theMAD->fid);
		free(theMAD->sets);
		free(theMAD->header);
		return MADNeedMemory;
	}
	
	//TODO: dispatch this
	for (i = 0; i < MAXINSTRU; i++) theMAD->fid[i].firstSample = i * MAXSAMPLE;
	
	for(i = 0; i < the669->NOS; i++) {
		temp = (size_t) the669;
		temp += 0x1f1 + i * 25 + 13;
		
		SInfo = (SampleInfo*)temp;
		
		if (SInfo->length > 0) {
			sData	*curData;
			
			theMAD->fid[i].numSamples = 1;
			theMAD->fid[i].volFade = DEFAULT_VOLFADE;
			
			curData = theMAD->sample[i * MAXSAMPLE + 0] = (sData*)calloc(sizeof(sData), 1);
			
			if (!curData) {
				for (i = 0; i < MAXINSTRU * MAXSAMPLE; i++) {
					if (theMAD->sample[i]) {
						if (theMAD->sample[i]->data) {
							free(theMAD->sample[i]->data);
						}
						free(theMAD->sample[i]);
					}
				}
				free(theMAD->sample);
				free(theMAD->fid);
				free(theMAD->sets);
				free(theMAD->header);
				return MADNeedMemory;
			}
			
			curData->size		= SInfo->length;
			curData->loopBeg 	= 0;
			curData->loopSize 	= 0;
			curData->vol		= 64;
			curData->c2spd		= NOFINETUNE;
			curData->loopType	= 0;
			curData->amp		= 8;
			
			curData->relNote	= 0;
			//strlcpy(curData->name, instru[i]->name, sizeof(curData->name));
			//for (x = 0; x < 22; x++) curData->name[x] = instru[i]->name[x];
			
			curData->data 		= malloc(curData->size);
			if (curData->data == NULL) {
				for (i = 0; i < MAXINSTRU * MAXSAMPLE; i++) {
					if (theMAD->sample[i]) {
						if (theMAD->sample[i]->data)
							free(theMAD->sample[i]->data);
						free(theMAD->sample[i]);
					}
				}
				free(theMAD->sample);
				free(theMAD->fid);
				free(theMAD->sets);
				free(theMAD->header);
				return MADNeedMemory;
			}
			
			memcpy(curData->data, theInstrument[i], curData->size);
			
			destPtr = curData->data;
			for (temp = 0; temp < curData->size; temp++)
				*(destPtr + temp) -= 0x80;
		} else
			theMAD->fid[i].numSamples = 0;
	}
	
	PatInt = (struct PatSix*)the669 + 0x1f1 + the669->NOS * 0x19;
	
	temp = (SInt32) the669;
	temp += 0x1f1L + (SInt32) the669->NOS * 0x19L;
	
	PatInt = (struct PatSix*)temp;
	
	/***** TEMPORAIRE ********/
	
	theMAD->header->numChn = 8;
	//	theMAD->header->PatMax = 1;
	
	for (i = 0; i < theMAD->header->numPat; i++) {
		theMAD->partition[i] = (PatData*)calloc(sizeof(PatHeader) + theMAD->header->numChn * 64 * sizeof(Cmd), 1);
		if (!theMAD->partition[i]) {
			for (i = 0; i < MAXPATTERN; i++) {
				if (theMAD->partition[i]) {
					free(theMAD->partition[i]);
				}
			}
			for (i = 0; i < MAXINSTRU * MAXSAMPLE; i++) {
				if (theMAD->sample[i]) {
					if (theMAD->sample[i]->data) {
						free(theMAD->sample[i]->data);
					}
					free(theMAD->sample[i]);
				}
			}
			free(theMAD->sample);
			free(theMAD->fid);
			free(theMAD->sets);
			free(theMAD->header);
			return MADNeedMemory;
		}
		theMAD->partition[i]->header.size = 64;
		theMAD->partition[i]->header.compMode = 'NONE';
		//TODO: dispatch this
		for (x = 0; x < 20; x++) theMAD->partition[ i]->header.name[ x] = 0;
		theMAD->partition[i]->header.patBytes = 0;
		theMAD->partition[i]->header.unused2 = 0;
		
		for (x = 0 ; x < 64; x++) {
			for (z = 0; z<theMAD->header->numChn; z++) {
				aCmd = GetMADCommand(x, z, theMAD->partition[i]);
				
				theCommand = &PatInt[i].Cmds[x][z];
				if ((Ptr)theCommand >= MaxPtr) {
					for (i = 0; i < MAXPATTERN; i++) {
						if (theMAD->partition[i]) {
							free(theMAD->partition[i]);
						}
					}
					for (i = 0; i < MAXINSTRU * MAXSAMPLE; i++) {
						if (theMAD->sample[i]) {
							if (theMAD->sample[i]->data) {
								free(theMAD->sample[i]->data);
							}
							free(theMAD->sample[i]);
						}
						free(theMAD->sample);
						free(theMAD->fid);
						free(theMAD->sets);
						free(theMAD->header);
					}
					return MADIncompatibleFile;
				}
				
				thePasByte = ( Byte*) theCommand;
				
				if (thePasByte[0] == 0xFF) {
					aCmd->cmd 	= 0;
					aCmd->arg 	= 0;
					aCmd->ins 	= 0;
					aCmd->note	= 0xFF;
					aCmd->vol	= 0xFF;
				} else if (thePasByte[0] == 0xFE) {
					aCmd->ins 	= 0;
					aCmd->note 	= 0xFF;
					aCmd->cmd 	= 0;
					aCmd->arg 	= 0;
					
					aCmd->vol	= theCommand->Volume;
					aCmd->vol	= (aCmd->vol * 64) / 15;
				} else {
					aCmd->ins = theCommand->Instru + 1;
					aCmd->note = theCommand->AmigaPeriod;
					
					Note = (aCmd->note & 0xF0) >> 4;
					Octave = (aCmd->note & 0x0F);
					
					aCmd->note = Note*12 + Octave;
					
					if (aCmd->note == 0)
						aCmd->note = 0xFF;
					else
						aCmd->note += 24;
					
					aCmd->vol	= theCommand->Volume;
					aCmd->vol	= (aCmd->vol * 64) / 15;
				}
				
				if (thePasByte[2] == 0xFF) {
					aCmd->cmd = 0;
					aCmd->arg = 0;
				} else {
					aCmd->cmd = 0;
					aCmd->arg = 0;
				}
			}
		}
	}
	
	return noErr;
}

static OSErr Extract669Info(PPInfoRec *info, void *AlienFile)
{
	SixSixNine	*the669 = (SixSixNine*)AlienFile;
	
	/*** Signature ***/
	
	info->signature = '669 ';
	
	/*** Internal name ***/
	
	//the669->message[ 30] = '\0';
	strlcpy(info->internalFileName, the669->message, sizeof(info->internalFileName));
	
	/*** Total Patterns ***/
	
	info->totalPatterns = 0;
	
	/*** Partition Length ***/
	
	info->partitionLength = 0;
	
	/*** Total Instruments ***/
	
	info->totalInstruments = 0;
	
	strlcpy(info->formatDescription, "669 Plug", sizeof(info->formatDescription));
	
	return noErr;
}

static OSErr Test669File(void *AlienFile)
{
	SixSixNine *the669 = (SixSixNine*)AlienFile;
	
	if (the669->marker == 0x6669 || the669->marker == 0x6966)
		return noErr;
	else
		return MADFileNotSupportedByThisPlug;
}

#ifndef _MAC_H

EXP OSErr FillPlug(PlugInfo *p);
EXP OSErr PPImpExpMain(OSType order, char *AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init);

EXP OSErr FillPlug( PlugInfo *p)		// Function USED IN DLL - For PC & BeOS
{
	strlcpy(p->type, "669 ", sizeof(p->type));
	strlcpy(p->MenuName, "669 Files", sizeof(p->MenuName));
	p->mode	= MADPlugImport;
	p->version = 2 << 16 | 0 << 8 | 0;
	
	return noErr;
}
#endif

#if defined(NOEXPORTFUNCS) && NOEXPORTFUNCS
OSErr main669( OSType order, char *AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#else
extern OSErr PPImpExpMain(OSType order, char *AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#endif
{
	OSErr	myErr = noErr;
	Ptr		AlienFile;
	UNFILE	iFileRefI;
	long	sndSize;
	
	switch (order) {
		case MADPlugImport:
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI) {
				sndSize = iGetEOF(iFileRefI);
				
				// ** MEMORY Test Start
				AlienFile = (Ptr)malloc(sndSize * 2L);
				if (AlienFile == NULL)
					myErr = MADNeedMemory;
				// ** MEMORY Test End
				
				else {
					free(AlienFile);
					
					AlienFile = (Ptr)malloc(sndSize);
					myErr = iRead(sndSize, AlienFile, iFileRefI);
					if (myErr == noErr) {
						myErr = Test669File(AlienFile);
						if (myErr == noErr)
							myErr = Convert6692Mad(AlienFile, sndSize, MadFile, init);
					}
					free(AlienFile);
					AlienFile = NULL;
				}
				iClose(iFileRefI);
			} else
				myErr = MADReadingErr;
			break;
			
		case MADPlugTest:
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI) {
				sndSize = 1024L;
				
				AlienFile = (Ptr)malloc(sndSize);
				if (AlienFile == NULL)
					myErr = MADNeedMemory;
				else {
					myErr = iRead(sndSize, AlienFile, iFileRefI);
					
					myErr = Test669File(AlienFile);
					
					free(AlienFile);
					AlienFile = NULL;
				}
				iClose(iFileRefI);
			} else
				myErr = MADReadingErr;
			break;
			
		case MADPlugInfo:
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI) {
				info->fileSize = iGetEOF(iFileRefI);
				
				sndSize = 5000L; // Read only 5000 first bytes for optimisation
				
				AlienFile = (Ptr)malloc(sndSize);
				if (AlienFile == NULL)
					myErr = MADNeedMemory;
				else {
					myErr = iRead(sndSize, AlienFile, iFileRefI );
					if (myErr == noErr)
						myErr = Extract669Info(info, AlienFile);

					free(AlienFile);
					AlienFile = NULL;
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
