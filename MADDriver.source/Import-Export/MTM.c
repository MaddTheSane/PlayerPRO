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
#ifdef __BLOCKS__
#include <dispatch/dispatch.h>
#endif
#endif
#include "MTM.h"

#if defined(NOEXPORTFUNCS) && NOEXPORTFUNCS
#include "embeddedPlugs.h"
#endif

#ifdef WIN32
#define strlcpy(dst, src, size) strncpy_s(dst, size, src, _TRUNCATE)
#endif

static inline struct MTMTrack* GetMTMCommand( short position, short whichTracks, void *PatPtr)
{
	return (void*)((size_t)PatPtr + whichTracks * 192 + position * 3);
}

static OSErr ConvertMTM2Mad(MTMDef *MTMFile, size_t MTMSize, MADMusic *theMAD, MADDriverSettings *init)
{
	short 			i, x, z;
	SInt32 			sndSize, OffSetToSample, temp, inOutCount;
	Ptr				MaxPtr;
	Ptr				theInstrument[64], destPtr;
	SInt32 			finetune[16] = {
		8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757,
		7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280
	};
	
	/**** Variables pour le MAD ****/
	Cmd				*aCmd;
	
	/**** Variables pour le MTM ****/
	
	struct Instru		*instru[64];
	struct MTMTrack		*theCom;
	Ptr					samplePtr, patPtr, positionPtr;
	short				*patTracks;
	/********************************/
	
	/**** Calcul de divers offsets *****/
	
	PPLE16(&MTMFile->tracks);
	PPLE16(&MTMFile->comments);
	
	MaxPtr 		= (Ptr) ((size_t)MTMFile + MTMFile);
	positionPtr	= (Ptr) ((size_t)MTMFile + 66 + MTMFile->NOS * 37);
	patPtr 		= (Ptr) ((size_t)MTMFile + 194 + MTMFile->NOS * 37);
	destPtr		= (Ptr) ((size_t)MTMFile + 194 + MTMFile->NOS * 37 + MTMFile->tracks * 192);
	patTracks	= (short*)destPtr;
	samplePtr 	= (Ptr) ((size_t)MTMFile + 194 + MTMFile->NOS * 37 + MTMFile->tracks * 192 +
						 (MTMFile->patNo + 1) * 32 * 2 + MTMFile->comments);
	
	/**** Analyse des instruments ****/
	if (MTMFile->NOS > 64) return MADUnknowErr;
	
	for (i = 0, OffSetToSample = 0; i < MTMFile->NOS ; i++) {
		theInstrument[i] = samplePtr + OffSetToSample;
		
		instru[i] = (struct Instru*)((size_t)MTMFile + 66 + i * 37);
		
		PPLE32(&instru[i]->size);
		PPLE32(&instru[i]->loopBegin);
		PPLE32(&instru[i]->loopEnd);
		
		sndSize = instru[i]->size;
		if (theInstrument[i] + sndSize > MaxPtr)
			return MADUnknowErr;
		
		OffSetToSample += sndSize;
	}
	
	
	/***********************************************/
	/******** Le MTM a été lu et analysé ***********/
	/***** Copie des informations dans le MAD ******/
	/***********************************************/
	
	/**************************/
	/*** MAD Initialisation ***/
	/**************************/
	
	inOutCount = sizeof(MADSpec);
	theMAD->header = (MADSpec*)calloc(inOutCount, 1);
	if (theMAD->header == NULL)
		return MADNeedMemory;
	
	theMAD->header->MAD = 'MADK';
	
	/*****************************************/
	/*** Copie des informations MTM -> MAD ***/
	/*****************************************/
	
	for (i = 0; i < 22; i++) {
		theMAD->header->name[i] = MTMFile->songname[i];
	}
	
	strlcpy(theMAD->header->infos, "Converted by PlayerPRO MTM Plug (\xA9\x41ntoine ROSSET <rossetantoine@bluewin.ch>)", sizeof(theMAD->header->infos));
	
	theMAD->header->tempo		= 125;
	theMAD->header->speed		= 6;
	theMAD->header->numPat		= MTMFile->patNo + 1;
	theMAD->header->numPointers	= MTMFile->positionNo;
#ifdef __BLOCKS__
	dispatch_apply(128, dispatch_get_global_queue(0, 0), ^(size_t i) {
		theMAD->header->oPointers[i] = positionPtr[i];
	});
#else
	for (i = 0; i < 128; i++) {
		theMAD->header->oPointers[i] = positionPtr[i];
	}
#endif
	theMAD->header->numChn = MTMFile->trackback;
	
	theMAD->sets = (FXSets*)calloc(sizeof(FXSets), MAXTRACK);
	
#ifdef __BLOCKS__
	dispatch_apply(MAXTRACK, dispatch_get_global_queue(0, 0), ^(size_t i) {
		theMAD->header->chanBus[i].copyId = i;
		
		if (i % 2 == 0)
			theMAD->header->chanPan[i] = MAX_PANNING / 4;
		else
			theMAD->header->chanPan[i] = MAX_PANNING - MAX_PANNING / 4;
		
		theMAD->header->chanVol[i] = MAX_VOLUME;
	});
#else
	for (i = 0; i < MAXTRACK; i++) {
		theMAD->header->chanBus[i].copyId = i;
		
		if (i % 2 == 0) theMAD->header->chanPan[ i] = MAX_PANNING/4;
		else theMAD->header->chanPan[ i] = MAX_PANNING - MAX_PANNING/4;
		
		theMAD->header->chanVol[ i] = MAX_VOLUME;
	}
#endif
	
	theMAD->header->generalVol		= 64;
	theMAD->header->generalSpeed	= 80;
	theMAD->header->generalPitch	= 80;
	
	
	// INSTRUMENTS
	
	theMAD->fid = (InstrData*)calloc(sizeof(InstrData), MAXINSTRU);
	if (!theMAD->fid)
		return MADNeedMemory;
	
	theMAD->sample = (sData**)calloc(sizeof(sData*), MAXINSTRU * MAXSAMPLE);
	if (!theMAD->sample)
		return MADNeedMemory;
	
#ifdef __BLOCKS__
	dispatch_apply(MAXINSTRU, dispatch_get_global_queue(0, 0), ^(size_t i) {
		theMAD->fid[i].firstSample = i * MAXSAMPLE;
	});
#else
	for (i = 0; i < MAXINSTRU; i++) theMAD->fid[i].firstSample = i * MAXSAMPLE;
#endif
	
	for (i = 0; i < MTMFile->NOS; i++) {
		for (x = 0; x < 22; x++) theMAD->fid[i].name[x] = instru[i]->name[x];
		theMAD->fid[i].type = 0;
		
		if (instru[i]->size > 0) {
			sData *curData;
			
			theMAD->fid[i].numSamples = 1;
			theMAD->fid[i].volFade = DEFAULT_VOLFADE;
			
			curData = theMAD->sample[i * MAXSAMPLE] = (sData*)calloc(sizeof(sData), 1);
			
			curData->size		= instru[i]->size;
			curData->loopBeg 	= instru[i]->loopBegin;
			curData->loopSize 	= instru[i]->loopEnd - instru[i]->loopBegin;
			curData->vol		= instru[i]->volume;
			curData->c2spd		= finetune[instru[i]->fineTune];
			curData->loopType	= 0;
			curData->amp		= 8;
			
			curData->relNote	= 0;
			//	for (x = 0; x < 22; x++) curData->name[x] = instru[i]->name[x];
			
			curData->data = (Ptr)malloc(curData->size);
			if (curData->data == NULL)
				return MADNeedMemory;
			
			memcpy(curData->data, theInstrument[i], curData->size);
			
			destPtr = curData->data;
			for (temp = 0; temp < curData->size; temp++) *(destPtr + temp) -= 0x80;
		} else
			theMAD->fid[i].numSamples = 0;
	}
	
	for (i = 0; i < theMAD->header->numPat; i++) {
		
		theMAD->partition[i] = (PatData*)calloc(sizeof(PatHeader) + theMAD->header->numChn * 64 * sizeof(Cmd), 1);
		if (theMAD->partition[i] == NULL)
			return MADNeedMemory;
		
		theMAD->partition[i]->header.size = 64L;
		theMAD->partition[i]->header.compMode = 'NONE';
		
		for (x = 0; x < 20; x++) theMAD->partition[i]->header.name[x] = 0;
		
		theMAD->partition[ i]->header.patBytes = 0;
		theMAD->partition[ i]->header.unused2 = 0;
		
		MaxPtr = (Ptr) theMAD->partition[ i];
		MaxPtr += sizeof( PatHeader) + theMAD->header->numChn * 64L * sizeof( Cmd);
		
		for (z = 0; z < 32; z++) PPLE16(&patTracks[z]);
		
		for (x = 0; x < 64; x++) {
			for(z=0; z<theMAD->header->numChn; z++) {
				aCmd = GetMADCommand(x, z, theMAD->partition[i]);
				if ((Ptr)aCmd + sizeof(Cmd) > MaxPtr)
					return MADUnknowErr;
				
				if (patTracks[ z] == 0) {
					aCmd->ins	= 0;
					aCmd->note	= 0xFF;
					aCmd->cmd	= 0;
					aCmd->arg	= 0;
					aCmd->vol	= 0xFF;
				} else {
					theCom = GetMTMCommand(x, patTracks[z], patPtr);
					
					aCmd->ins 	= theCom->instru;
					
					if (theCom->pitch)
						aCmd->note 	= theCom->pitch + 22;
					else
						aCmd->note = 0xFF;
					
					aCmd->cmd 	= theCom->EffectCmd;
					aCmd->arg 	= theCom->EffectArg;
					aCmd->vol	= 0xFF;
				}
			}
		}
		
		/*** Avance dans les tracks-patterns suivants ***/
		patTracks += 32;
	}
	
	return noErr;
}

static OSErr ExtractInfo(PPInfoRec *info, MTMDef *myFile)
{
#ifndef __BLOCKS__
	short	i;
	
	for (i = 0; i < sizeof( myFile->songname); i++) {
		info->internalFileName[i] = myFile->songname[i];
	}
#else
	dispatch_apply(sizeof(myFile->songname), dispatch_get_global_queue(0, 0), ^(size_t i) {
		info->internalFileName[i] = myFile->songname[i];
	});
#endif
	info->internalFileName[21] = 0;
	
	strlcpy(info->formatDescription, "MTM Plug", sizeof(info->formatDescription));
	
	info->totalPatterns		= myFile->patNo;
	info->partitionLength	= myFile->positionNo;
	info->totalTracks		= myFile->tracks;
	info->totalInstruments	= myFile->NOS;
	info->signature			= 'MTM ';
	
	return noErr;
}

static inline OSErr TestFile(MTMDef *myFile)
{
	if (myFile->Id[0] == 'M' &&
		myFile->Id[1] == 'T' &&
		myFile->Id[2] == 'M')
		return noErr;
	else
		return MADFileNotSupportedByThisPlug;
}

#ifndef _MAC_H

EXP OSErr FillPlug( PlugInfo *p);
EXP OSErr PPImpExpMain( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init);

EXP OSErr FillPlug( PlugInfo *p)		// Function USED IN DLL - For PC & BeOS
{
	strlcpy( p->type, 		"MTM ", sizeof(p->type));
	strlcpy( p->MenuName, 	"MTM Files", sizeof(p->MenuName));
	p->mode	=	MADPlugImport;
	p->version = 2 << 16 | 0 << 8 | 0;
	
	return noErr;
}
#endif


/*****************/
/* MAIN FUNCTION */
/*****************/

#if defined(NOEXPORTFUNCS) && NOEXPORTFUNCS
OSErr mainMTM(OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#else
extern OSErr PPImpExpMain(OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
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
				
				// ** TEST MEMOIRE :  Environ 2 fois la taille du fichier**
				AlienFile = (Ptr)malloc(sndSize * 2);
				if (AlienFile == NULL) {
					myErr = MADNeedMemory;
				} else {
					free(AlienFile);
					
					AlienFile = (Ptr)malloc(sndSize);
					iRead(sndSize, AlienFile, iFileRefI);
					
					myErr = TestFile((MTMDef*)AlienFile);
					if (myErr == noErr) {
						myErr = ConvertMTM2Mad((MTMDef*)AlienFile, sndSize, MadFile, init);
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
				
				AlienFile = (Ptr)malloc( sndSize);
				if (AlienFile == NULL) {
					myErr = MADNeedMemory;
				} else {
					iRead(sndSize, AlienFile, iFileRefI);
					myErr = TestFile((MTMDef*)AlienFile);
					free(AlienFile);
					AlienFile = NULL;
				}
				iClose(iFileRefI);
			} else
				myErr = MADReadingErr;
			break;
			
		case 'INFO':
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI) {
				info->fileSize = iGetEOF(iFileRefI);
				
				sndSize = 5000; // Read only 5000 first bytes for optimisation
				
				AlienFile = (Ptr)malloc(sndSize);
				if (AlienFile == NULL) {
					myErr = MADNeedMemory;
				} else {
					iRead(sndSize, AlienFile, iFileRefI);
					myErr = ExtractInfo(info, (MTMDef*)AlienFile);
					free(AlienFile);
					AlienFile = NULL;
				}
				iClose( iFileRefI);
			} else
				myErr = MADReadingErr;
			break;
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
	return myErr;
}
