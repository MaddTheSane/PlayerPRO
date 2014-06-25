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
#include "AMF.h"

#if defined(NOEXPORTFUNCS) && NOEXPORTFUNCS
#include "embeddedPlugs.h"
#endif

//TODO: make this multiple-instance safe!
static char *theAMFRead;

#define READAMFFILE(dst, size)	{memcpy(dst, theAMFRead, size); theAMFRead += (long)size;}

#ifdef _MAC_H
#define Tdecode16(msg_buf) CFSwapInt16LittleToHost(*(short*)msg_buf)
#define Tdecode32(msg_buf) CFSwapInt32LittleToHost(*(int*)msg_buf)
#else
#ifdef __LITTLE_ENDIAN__
#define Tdecode16(msg_buf) *(short*)msg_buf
#define Tdecode32(msg_buf) *(int*)msg_buf
#else

static inline UInt16 Tdecode16(void *msg_buf)
{
	UInt16 toswap = *((UInt16*) msg_buf);
	PPLE16(&toswap);
	return toswap;
}

static inline UInt32 Tdecode32(void *msg_buf)
{
	UInt32 toswap = *((UInt32*) msg_buf);
	PPLE32(&toswap);
	return toswap;
}

#endif
#endif

static MADErr AMF2Mad(char *AMFCopyPtr, long size, MADMusic *theMAD, MADDriverSettings *init)
{
	MADByte			tempByte;
	short			i, x, noIns, tempShort, trackCount, trckPtr, t;
	//int			inOutCount, OffSetToSample = 0, z;
	//MADErr			theErr = MADNoErr;
	//char*				tempPtr;
	MADFourChar			AMFType;
	/*int			finetune[16] =
	 {
	 8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757,
	 7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280
	 };*/
	
	short			pan, uusize, oldIns = 1;
	
	theAMFRead = AMFCopyPtr;
	
	READAMFFILE(&AMFType, 4);		// AMF Type
	PPBE32(&AMFType);
	
	if (AMFType >= 0x414D460C)
		pan = 32;
	else
		pan = 16;
	
	if (AMFType == 0x414D4601 )
		uusize = 3;
	else if (AMFType >= 0x414D460A )
		oldIns = 0;
	else if (AMFType!= 0x414D4608 && AMFType != 0x414D4609)
		return MADFileNotSupportedByThisPlug;
	
	// Conversion
	theMAD->header = (MADSpec*) calloc(sizeof(MADSpec), 1);
	if (theMAD->header == NULL) return MADNeedMemory;
	
	strncpy(theMAD->header->infos, "Converted by PlayerPRO AMF Plug (\xA9\x41ntoine ROSSET <rossetantoine@bluewin.ch>)", sizeof(theMAD->header->infos));
	
	theMAD->header->MAD = 'MADK';
	
	READAMFFILE(theMAD->header->name, 32);
	READAMFFILE(&tempByte, 1);
	noIns = tempByte;
	
	READAMFFILE(&tempByte, 1);
	theMAD->header->numPat = tempByte;
	
	READAMFFILE(&tempShort, 2);
	trackCount = Tdecode16(&tempShort);
	
	if (AMFType >= 0x414D4609 ) {
		READAMFFILE(&tempByte, 1);
		theMAD->header->numChn = tempByte;
		
		READAMFFILE(&tempByte, pan);
		if (AMFType < 0x414D460B ) {
			//memcpy(&module->channelPanning,order16,16);
		}
	}
	else
		theMAD->header->numChn = 4;
	if (AMFType >= 0x414D460D) {
		READAMFFILE(&tempByte, 1);
		theMAD->header->tempo = tempByte;
		
		READAMFFILE(&tempByte, 1);
		theMAD->header->speed = tempByte;
	} else {
		theMAD->header->speed			= 6;
		theMAD->header->tempo			= 125;
	}
	
	//theMAD->header->numPointers		= oldMAD->numPointers;
	//BlockMoveData(oldMAD->oPointers, theMAD->header->oPointers, 128);
	
	theMAD->sets = (FXSets*) calloc(MAXTRACK * sizeof(FXSets), 1);
	for (i = 0; i < MAXTRACK; i++)
		theMAD->header->chanBus[i].copyId = i;
	/**** Patterns *******/
	
	for (i = 0; i < theMAD->header->numPat; i++ )
	{
		long patSize;
		
		if (AMFType >= 0x414D460E ) {
			READAMFFILE(&tempShort, 2);
			patSize = Tdecode16(&tempShort);
		} else
			patSize = 64;
		
		theMAD->partition[i] = (PatData*) calloc(sizeof(PatHeader) + theMAD->header->numChn * patSize * sizeof(Cmd), 1);
		if (theMAD->partition[i] == NULL)
			return MADNeedMemory;
		
		theMAD->partition[i]->header.size		= (int)patSize;
		theMAD->partition[i]->header.compMode	= 'NONE';
		
		for (x = 0; x < 20; x++) theMAD->partition[i]->header.name[x] = 0;
		
		theMAD->partition[i]->header.patBytes = 0;
		theMAD->partition[i]->header.unused2 = 0;
		
		for (x = 0; x < theMAD->header->numChn; x++ ) {
			READAMFFILE(&tempShort, 2);
		}
	}
	for (i = theMAD->header->numPat; i < MAXPATTERN ; i++) theMAD->partition[i] = NULL;
	
	for (i = 0; i < MAXTRACK; i++) {
		if (i % 2 == 0)
			theMAD->header->chanPan[i] = MAX_PANNING/4;
		else
			theMAD->header->chanPan[i] = MAX_PANNING - MAX_PANNING/4;
		
		theMAD->header->chanVol[i] = MAX_VOLUME;
	}
	
	theMAD->header->generalVol		= 64;
	theMAD->header->generalSpeed	= 80;
	theMAD->header->generalPitch	= 80;
	
	
	/**** Instruments header *****/
	
	theMAD->fid = (InstrData*)calloc(sizeof(InstrData), MAXINSTRU);
	if (!theMAD->fid)
		return MADNeedMemory;
	
	theMAD->sample = (sData**)calloc(sizeof(sData*), MAXINSTRU * MAXSAMPLE);
	if (!theMAD->sample)
		return MADNeedMemory;
	
	for (i = 0; i < MAXINSTRU; i++) theMAD->fid[i].firstSample = i * MAXSAMPLE;
	
	for (i = 0; i < noIns; i++) {
		InstrData		*curIns = &theMAD->fid[i];
		
		if (oldIns) {
			OLDINSTRUMENT oi;
			
			READAMFFILE(&oi, sizeof(OLDINSTRUMENT));
			
			memcpy(curIns->name, oi.name, 32);
			curIns->type = 0;
			
			if (oi.size > 0) {
				sData	*curData;
				ushort	oiloopstart = 0;
				ushort	oiloopend = 0;
				
				curIns->numSamples = 1;
				
				curData = theMAD->sample[i * MAXSAMPLE + 0] = (sData*)calloc(sizeof(sData), 1);
				
				curData->size		= Tdecode32(&oi.size);
				//FIXME: were loopstart and loopend supposed to be byteswapped on PowerPC?
				oiloopstart	= Tdecode16(&oi.loopstart);
				oiloopend	= Tdecode16(&oi.loopend);
				curData->loopBeg	= oiloopstart;
				curData->loopSize	= oiloopend - oiloopstart;
				if (oiloopend == 65535) {
					curData->loopSize = curData->loopBeg = 0;
				}
				curData->vol		= oi.volume;
				curData->c2spd		= Tdecode16(&oi.rate); //finetune[oldMAD->fid[i].fineTune];
				curData->loopType	= 0;
				curData->amp		= 8;
				
				curData->relNote	= 0;
				
				curData->data 		= (char*)malloc(curData->size);
				if (curData->data == NULL)
					return MADNeedMemory;
			} else
				curIns->numSamples = 0;
		} else {
			INSTRUMENT oi;
			
			READAMFFILE(&oi, sizeof(INSTRUMENT));
			theAMFRead--;
			
			memcpy(curIns->name, oi.name, 32);
			curIns->type = 0;
			
			if (oi.size > 0) {
				sData	*curData;
				
				curIns->numSamples = 1;
				
				curData = theMAD->sample[i * MAXSAMPLE + 0] = (sData*)calloc(sizeof(sData), 1);
				
				curData->size		= Tdecode32(&oi.size);
				curData->loopBeg 	= Tdecode32(&oi.loopstart);
				curData->loopSize 	= Tdecode32(&oi.loopend) - Tdecode32(&oi.loopstart);
				if (oi.loopend == 65535) {
					curData->loopSize = curData->loopBeg = 0;
				}
				curData->vol		= oi.volume;
				curData->c2spd		= NOFINETUNE;	//oi.rate;	//finetune[oldMAD->fid[i].fineTune];
				curData->loopType	= 0;
				curData->amp		= 8;
				
				curData->relNote	= 0;
				
				curData->data 		= malloc(curData->size);
				if (curData->data == NULL)
					return MADNeedMemory;
			}
			else
				curIns->numSamples = 0;
		}
	}
	
	trckPtr = 0;
	for (t = 0; t < trackCount; t++) {
		READAMFFILE(&tempShort, 2);
		tempShort = Tdecode16(&tempShort);
		if (tempShort > trckPtr)
			trckPtr = tempShort;
	}
	
	for (t = 0; t < trckPtr; t++) {
		READAMFFILE(&tempShort, 2);
		tempShort = Tdecode16(&tempShort);
		
		READAMFFILE(&tempByte, 1);
		
		if (tempShort == 0)
			t = t;
		else {
			char *tPtr = (char*)malloc(tempShort * 3 + size);
			READAMFFILE(tPtr, tempShort * 3 + size);
			free(tPtr);
		}
	}
	
	for (i = 0; i < noIns; i++) {
		InstrData *curIns = &theMAD->fid[i];
		
		if (curIns->numSamples > 0) {
			sData	*curData;
			curData = theMAD->sample[i * MAXSAMPLE + 0];
			READAMFFILE(curData->data, curData->size);
		}
	}
	
	return MADNoErr;
}

static MADErr TestAMFFile(void *AlienFile)
{
	MADFourChar	myMADSign = *((MADFourChar*) AlienFile);
	PPBE32(&myMADSign);
	
	if ((myMADSign & 0xFFFFFF00) == 0x414D4600)
		return MADNoErr;
	else
		return MADFileNotSupportedByThisPlug;
}

static MADErr ExtractAMFInfo(PPInfoRec *info, char *AlienFile)
{
	//TODO: implement
	//long		PatternSize;
	//short		i;
	//short		tracksNo;
	
	/*** Signature ***/
	
	info->signature = 'AMF ';
	
	/*** Internal name ***/
	
	info->internalFileName[31] = '\0';
	
	/*** Tracks ***/
	
	info->totalTracks = 0;
	
	/*** Total Patterns ***/
	
	info->totalPatterns = 0;
	
	/*** Partition Length ***/
	
	info->partitionLength = 0;
	
	/*** Total Instruments ***/
	info->totalInstruments = 0;
	
	strncpy(info->formatDescription, "AMF Plug", sizeof(info->formatDescription));
	
	return MADNoErr;
}

#ifndef _MAC_H

EXP MADErr FillPlug(PlugInfo *p);
EXP MADErr PPImpExpMain(MADFourChar order, char *AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init);

EXP MADErr FillPlug(PlugInfo *p)		// Function USED IN DLL - For PC & BeOS
{
	strncpy(p->type, 		"AMF ", sizeof(p->type));
	strncpy(p->MenuName, 	"AMF Files", sizeof(p->MenuName));
	p->mode	=		MADPlugImport;
	p->version =	2 << 16 | 0 << 8 | 0;
	
	return MADNoErr;
}
#endif


/*****************/
/* MAIN FUNCTION */
/*****************/
#if defined(NOEXPORTFUNCS) && NOEXPORTFUNCS
MADErr mainAMF(MADFourChar order, char *AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#else
extern MADErr PPImpExpMain(MADFourChar order, char *AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#endif
{
	MADErr	myErr;
	char	*AlienFile;
	UNFILE	iFileRefI;
	long	sndSize;
	
	switch (order) {
		case MADPlugImport:
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI) {
				sndSize = iGetEOF(iFileRefI);
				
				// ** MEMORY Test
				AlienFile = malloc(sndSize * 2L);
				if (AlienFile == NULL) {
					myErr = MADNeedMemory;
				} else {
					free(AlienFile);
					
					AlienFile = malloc(sndSize);
					myErr = iRead(sndSize, AlienFile,iFileRefI);
					if (myErr == MADNoErr) {
						myErr = TestAMFFile(AlienFile);
						if (myErr == MADNoErr) {
							myErr = AMF2Mad(AlienFile, sndSize, MadFile, init);
						}
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
				sndSize = 5000; // Read only 5000 first bytes for optimisation
				
				AlienFile = malloc(sndSize);
				if (AlienFile == NULL)
					myErr = MADNeedMemory;
				else {
					myErr = iRead(sndSize, AlienFile, iFileRefI);
					myErr = TestAMFFile(AlienFile);
					
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
				
				sndSize = 5000; // Read only 5000 first bytes for optimisation
				
				AlienFile = malloc(sndSize);
				if (AlienFile == NULL)
					myErr = MADNeedMemory;
				else {
					myErr = iRead(sndSize, AlienFile, iFileRefI);
					if (myErr == MADNoErr)
						myErr = ExtractAMFInfo(info, AlienFile);
					
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
