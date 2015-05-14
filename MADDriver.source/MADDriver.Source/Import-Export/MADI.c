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

#include <PlayerPROCore/PlayerPROCore.h>
#include "MOD.h"
#include "MADI.h"

enum
{
	ins 	= 1 << 0,
	note	= 1 << 1,
	cmd		= 1 << 2,
	argu	= 1 << 3,
	vol		= 1 << 4
};


static oldPatData* oldDecompressPartitionMAD1(oldMADSpec *header, oldPatData* myPat)
{
	oldPatData*				finalPtr;
	Byte 					*srcPtr;
	oldCmd					*myCmd;
	short					maxCmd;
	Byte					set;
	
	finalPtr = (oldPatData*) NewPtrClear(sizeof(oldPatHeader) + myPat->header.size * header->numChn * sizeof(oldCmd));
	if (finalPtr == NULL)
	{
		DebugStr("\pMemory3 Bug");
		return NULL;
	}
	
	BlockMoveData(myPat, finalPtr, sizeof(oldPatHeader));

	srcPtr = (Byte*) myPat->Cmds;
	myCmd = (oldCmd*) finalPtr->Cmds;
	maxCmd = finalPtr->header.size * header->numChn;
	
	/*** Decompression Routine ***/
	
	while (maxCmd != 0)
	{
		maxCmd--;
		
		myCmd->cmd 	= 0;
		myCmd->note = 0xFF;
		myCmd->arg 	= 0;
		myCmd->ins 	= 0;
		myCmd->vol 	= 0xFF;
		
		set = *srcPtr++;
		
		if (set & ins)	myCmd->ins = *srcPtr++;
		if (set & note)	myCmd->note = *srcPtr++;
		if (set & cmd)	myCmd->cmd = *srcPtr++;
		if (set & argu)	myCmd->arg = *srcPtr++;
		if (set & vol)	myCmd->vol = *srcPtr++;
		
		myCmd++;
	}
	
	return finalPtr;
}

static oldCmd* GetOldCommand(short PosX, short	TrackIdX, oldPatData*	tempMusicPat)
{
	if (PosX < 0) PosX = 0;
	else if (PosX >= tempMusicPat->header.size) PosX = tempMusicPat->header.size -1;
	
	return(&(tempMusicPat->Cmds[(tempMusicPat->header.size * TrackIdX) + PosX]));
}

static void MOToldsData(struct oldsData * s)
{
	MOT32(&s->size);
	MOT32(&s->loopBeg);
	MOT32(&s->loopSize);
	MOT16(&s->c2spd);
}

static void MOToldPatHeader(struct oldPatHeader * p)
{
	MOT32(&p->size);
	MOT32(&p->compMode);
	MOT32(&p->patBytes);
	MOT32(&p->unused2); // this is probably superfluous, but who knows
}

static void MOToldEnvRec(struct oldEnvRec * e)
{
	MOT16(&e->pos);
	MOT16(&e->val);
}

static void MOToldInstrData(struct oldInstrData * i)
{
	int j;
	MOT16(&i->firstSample);
	MOT16(&i->numSamples);
	MOT16(&i->volFade);
	for(j = 0; j < 12; j++){
		MOToldEnvRec(&i->volEnv[j]);
		MOToldEnvRec(&i->pannEnv[j]);
	}
}

static void MOToldMADSpec(oldMADSpec * m)
{
	MOT32(&m->MAD);
	MOT16(&m->speed);
	MOT16(&m->tempo);
	MOT32(&m->EPitch);
	MOT32(&m->ESpeed);
}

static OSErr MADI2Mad(Ptr MADPtr, long size, MADMusic *theMAD, MADDriverSettings *init)
{
	short		i, x;
	long		inOutCount, OffSetToSample = 0, z;
#if 0
	OSErr		theErr = noErr;
	Ptr			tempPtr = NULL;
	long		finetune[16] = 
	{
		8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757,
		7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280
	};
#endif
	
	
	/**** Old MADH variables ****/
	
	oldMADSpec				*oldMAD;
	
	oldMAD = (oldMADSpec*) MADPtr;
	
	
	/**** HEADER ****/
	OSType MADType = oldMAD->MAD;
	MOT32(&MADType);
	if (MADType != 'MADI') return MADFileNotSupportedByThisPlug;
	OffSetToSample += sizeof(oldMADSpec);
	MOT32(&MADType);
	MOToldMADSpec(oldMAD);
	
	
	// Conversion
	inOutCount = sizeof(MADSpec);
	theMAD->header = (MADSpec*) MADPlugNewPtrClear(inOutCount, init);
	if (theMAD->header == NULL) return MADNeedMemory;
	
	theMAD->header->MAD = 'MADK';
	
	BlockMoveData(oldMAD->name, theMAD->header->name, 32);
	BlockMoveData(oldMAD->infos, theMAD->header->infos, INFOSSIZE);
	theMAD->header->generalPan		=	oldMAD->generalPan;
	theMAD->header->MultiChanNo		=	oldMAD->MultiChanNo;
	theMAD->header->EPitch			=	oldMAD->EPitch;
	theMAD->header->ESpeed			=	oldMAD->ESpeed;
	theMAD->header->XMLinear		=	oldMAD->XMLinear;
	theMAD->header->MODMode			=	oldMAD->MODMode;
	theMAD->header->showCopyright	=	oldMAD->showCopyright;
	theMAD->header->generalPitch	=	oldMAD->generalPitch;
	theMAD->header->generalSpeed	=	oldMAD->generalSpeed;
	theMAD->header->generalVol		=	oldMAD->generalVol;
	theMAD->header->numPat			= 	oldMAD->numPat;
	theMAD->header->numChn			= 	oldMAD->numChn;
	theMAD->header->numPointers		= 	oldMAD->numPointers;
	theMAD->header->numInstru		=	oldMAD->numInstru;
	theMAD->header->numSamples		=	oldMAD->numSamples;
	BlockMoveData(oldMAD->oPointers, theMAD->header->oPointers, 256);
	theMAD->header->speed			= 	oldMAD->speed;
	theMAD->header->tempo			= 	oldMAD->tempo;
	
	for (i = 0; i < MAXTRACK; i++) theMAD->header->chanPan[i] = oldMAD->chanPan[i];
	for (i = 0; i < MAXTRACK; i++) theMAD->header->chanVol[i] = oldMAD->chanVol[i];
	
	
	theMAD->sets = (FXSets*) NewPtrClear(MAXTRACK * sizeof(FXSets));
	
	for (i = 0; i < MAXTRACK; i++) theMAD->header->chanBus[i].copyId = i;
	/**** Patterns *******/
	
	for (i = 0; i < oldMAD->numPat; i++)
	{
		struct oldPatData		*tempPat, *tempPat2;
		struct oldPatHeader		tempPatHeader;
		
		/** Lecture du header de la partition **/
		inOutCount = sizeof(struct oldPatHeader);
		BlockMoveData(MADPtr + OffSetToSample, &tempPatHeader, inOutCount);
		
		/*************************************************/
		/** Lecture du header + contenu de la partition **/
		/*************************************************/
		
		MOToldPatHeader(&tempPatHeader);
		
		if (tempPatHeader.compMode == 'MAD1')
		{
			inOutCount = sizeof(oldPatHeader) + tempPatHeader.patBytes;
		}
		else
		{
			inOutCount = sizeof(oldPatHeader) + oldMAD->numChn * tempPatHeader.size * sizeof(oldCmd);
		}
		
		tempPat = (struct oldPatData*) MADPlugNewPtr(inOutCount, init);
		if (tempPat == NULL) DebugStr("\pMemory Prob1");
		
		BlockMoveData(MADPtr + OffSetToSample, tempPat, inOutCount);
		OffSetToSample += inOutCount;
		MOToldPatHeader(&tempPat->header);
		
		if (tempPat->header.compMode == 'MAD1')
		{
			tempPat2 = oldDecompressPartitionMAD1(oldMAD, tempPat);
			
			DisposePtr((Ptr) tempPat);
			
			tempPat = tempPat2;
		}
		
		/**************/
		/* CONVERSION */
		/**************/
		
		theMAD->partition[i] = (PatData*) MADPlugNewPtrClear(sizeof(PatHeader) + theMAD->header->numChn * tempPat->header.size * sizeof(Cmd), init);
		if (theMAD->partition[i] == NULL) return MADNeedMemory;
		
		theMAD->partition[i]->header.size 		= tempPat->header.size;
		theMAD->partition[i]->header.compMode 	= 'NONE';
		
		BlockMoveData(tempPat->header.name, theMAD->partition[i]->header.name, 20);
		
		theMAD->partition[i]->header.patBytes = 0;		theMAD->partition[i]->header.unused2 = 0;
		
		for (x = 0; x < theMAD->partition[i]->header.size; x++)
		{
			for (z = 0; z < theMAD->header->numChn; z++)
			{
				struct oldCmd *oldCmd;
				Cmd	*aCmd;
				
				aCmd = GetMADCommand(x,  z, theMAD->partition[i]);
				
				oldCmd 	= GetOldCommand(	x,
										z,
										tempPat);
				
				aCmd->ins 		= oldCmd->ins;
				aCmd->note 		= oldCmd->note;
				aCmd->cmd 		= oldCmd->cmd;
				aCmd->arg 		= oldCmd->arg;
				aCmd->vol		= oldCmd->vol;
				aCmd->unused 	= oldCmd->unused;
			}
		}
		
		DisposePtr((Ptr) tempPat);
		tempPat = NULL;
	}
	for (i = theMAD->header->numPat; i < MAXPATTERN ; i++) theMAD->partition[i] = NULL;
	
	/**** Instruments & Samples header *****/
	
	theMAD->fid = (InstrData*) MADPlugNewPtrClear(sizeof(InstrData) * (long) MAXINSTRU, init);
	if (!theMAD->fid) return MADNeedMemory;
	
	theMAD->sample = (sData**) MADPlugNewPtrClear(sizeof(sData*) * (long) MAXINSTRU * (long) MAXSAMPLE, init);
	if (!theMAD->sample) return MADNeedMemory;
	
	for (i = 0; i < oldMAD->numInstru; i++)
	{
		struct oldInstrData		oldIns;
		//InstrData	*curIns = &theMAD->fid[i];
		short		d;
		
		/** Lecture des instruments **/
		inOutCount = sizeof(struct oldInstrData);
		BlockMoveData(MADPtr + OffSetToSample, &oldIns, inOutCount);
		MOToldInstrData(&oldIns);
		OffSetToSample += inOutCount;
		
		d = oldIns.no;
		
		BlockMoveData(oldIns.name, theMAD->fid[d].name, 32);
		
		theMAD->fid[d].type = oldIns.type;
		theMAD->fid[d].numSamples = oldIns.numSamples;
		BlockMoveData(oldIns.what, theMAD->fid[d].what, 96);
		BlockMoveData(oldIns.volEnv, theMAD->fid[d].volEnv, 12 * sizeof(EnvRec));
		BlockMoveData(oldIns.pannEnv, theMAD->fid[d].pannEnv, 12 * sizeof(EnvRec));
		theMAD->fid[d].volSize = oldIns.volSize;
		theMAD->fid[d].pannSize = oldIns.pannSize;
		theMAD->fid[d].volSus = oldIns.volSus;
		theMAD->fid[d].volBeg = oldIns.volBeg;
		theMAD->fid[d].volEnd = oldIns.volEnd;
		theMAD->fid[d].pannSus = oldIns.pannSus;
		theMAD->fid[d].pannBeg = oldIns.pannBeg;
		theMAD->fid[d].pannEnd = oldIns.pannEnd;
		theMAD->fid[d].volType = oldIns.volType;
		theMAD->fid[d].pannType = oldIns.pannType;
		theMAD->fid[d].volFade = oldIns.volFade;
		theMAD->fid[d].vibDepth = oldIns.vibDepth;
		theMAD->fid[d].vibRate = oldIns.vibRate;
	}
	
	for (i = 0; i < MAXINSTRU; i++) theMAD->fid[i].firstSample = i * MAXSAMPLE;
	
	// Read Samples
	
	for (i = 0; i < MAXINSTRU ; i++)
	{
		for (x = 0; x < theMAD->fid[i].numSamples; x++)
		{
			oldsData	 *oldcurData;
			sData		 *curData;
			
			oldcurData = (oldsData*) (MADPtr + OffSetToSample);
			MOToldsData(oldcurData);
			OffSetToSample += sizeof(oldsData);
			
			curData = theMAD->sample[i*MAXSAMPLE + x] = (sData*) MADPlugNewPtrClear(sizeof(sData), init);
			
			curData->size		= oldcurData->size;
			curData->loopBeg 	= oldcurData->loopBeg;
			curData->loopSize 	= oldcurData->loopSize;
			curData->vol		= oldcurData->vol;
			curData->c2spd		= oldcurData->c2spd;
			curData->loopType	= oldcurData->loopType;
			curData->amp		= oldcurData->amp;
			//	curData->panning	= oldcurData->panning;
			curData->relNote	= oldcurData->relNote;
			
			BlockMoveData(oldcurData->name, curData->name, 32);
			curData->stereo		= oldcurData->stereo;
			
			curData->data 		= MADPlugNewPtr(curData->size, init);
			if (curData->data == NULL) return MADNeedMemory;
			
			BlockMoveData(MADPtr + OffSetToSample, curData->data, curData->size);
			OffSetToSample += curData->size;
#ifdef __LITTLE_ENDIAN__
			if (curData->amp == 16)
			{
				long 	ll;
				short	*shortPtr = (short*) curData->data;
				
				for (ll = 0; ll < curData->size/2; ll++) MOT16(&shortPtr[ll]);
			}
#endif
		}
	}
	
	return noErr;
}

static OSErr TestoldMADFile(Ptr AlienFile)
{
	OSType myMADSign = *((OSType*) AlienFile);
	MOT32(&myMADSign);
	
	if(	myMADSign == 'MADI') return   noErr;
	else return  MADFileNotSupportedByThisPlug;
}

static OSErr ExtractoldMADInfo(PPInfoRec *info, Ptr AlienFile)
{
	oldMADSpec	*myMOD = (oldMADSpec*) AlienFile;
	//long		PatternSize;
	short		i;
	//short		tracksNo;
	
	/*** Signature ***/
	
	info->signature = myMOD->MAD;
	MOT32(&info->signature);
	
	/*** Internal name ***/
	
	//myMOD->name[31] = '\0';
	strlcpy(info->internalFileName, myMOD->name, sizeof(myMOD->name));
	
	/*** Tracks ***/
	
	info->totalTracks = myMOD->numChn;
	
	/*** Total Patterns ***/
	
	info->totalPatterns = 0;
	for (i = 0; i < 128; i++)
	{
		if (myMOD->oPointers[i] >= info->totalPatterns)	info->totalPatterns = myMOD->oPointers[i];
	}
	info->totalPatterns++;
	
	/*** Partition Length ***/
	
	info->partitionLength = myMOD->numPointers;
	
	/*** Total Instruments ***/
	
	info->totalInstruments = myMOD->numInstru;
	
	strcpy(info->formatDescription, "MADI Plug");

	return noErr;
}

/*****************/
/* MAIN FUNCTION */
/*****************/

static OSErr mainMADI(OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
{
	OSErr	myErr = noErr;
	Ptr		AlienFile;
	UNFILE	iFileRefI;
	long	sndSize;
	
	switch (order)
	{
		case 'IMPL':
			iFileRefI = iFileOpen(AlienFileName);
			if (iFileRefI)
			{
				sndSize = iGetEOF(iFileRefI);
				
				// ** MEMORY Test Start
				AlienFile = MADPlugNewPtr(sndSize * 2L, init);
				if (AlienFile == NULL) myErr = MADNeedMemory;
				// ** MEMORY Test End
				
				else
				{
					DisposePtr(AlienFile);
					
					AlienFile = MADPlugNewPtr(sndSize, init);
					myErr = iRead(sndSize , AlienFile, iFileRefI);
					if (myErr == noErr)
					{
						myErr = TestoldMADFile(AlienFile);
						if (myErr == noErr)
						{
							myErr = MADI2Mad(AlienFile, GetPtrSize(AlienFile), MadFile, init);
						}
					}
					DisposePtr(AlienFile);	AlienFile = NULL;
				}
				iClose(iFileRefI);
			}
			else myErr = MADReadingErr;
			break;
			
		case 'TEST':
			iFileRefI = iFileOpen(AlienFileName);
			if (iFileRefI)
			{
				sndSize = 5000L;	// Read only 5000 first bytes for optimisation
				
				AlienFile = MADPlugNewPtr(sndSize, init);
				if (AlienFile == NULL) myErr = MADNeedMemory;
				else
				{
					myErr = iRead(sndSize , AlienFile, iFileRefI);
					if(myErr == noErr) myErr = TestoldMADFile(AlienFile);
					
					DisposePtr(AlienFile);	AlienFile = NULL;
				}
				iClose(iFileRefI);
			}
			else myErr = MADReadingErr;
			break;
			
		case 'INFO':
			iFileRefI = iFileOpen(AlienFileName);
			if (iFileRefI)
			{
				info->fileSize = iGetEOF(iFileRefI);
				
				sndSize = 5000L;	// Read only 5000 first bytes for optimisation
				
				AlienFile = MADPlugNewPtr(sndSize, init);
				if (AlienFile == NULL) myErr = MADNeedMemory;
				else
				{
					myErr = iRead(sndSize , AlienFile, iFileRefI);
					if (myErr == noErr)
					{
						myErr = ExtractoldMADInfo(info, AlienFile);
					}
					DisposePtr(AlienFile);	AlienFile = NULL;
				}
				iClose(iFileRefI);
			}
			else myErr = MADReadingErr;
			break;
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
	return myErr;
}

#define PLUGUUID (CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x9D, 0xC5, 0xE4, 0xB1, 0xF2, 0xE1, 0x4D, 0x6E, 0x96, 0x3F, 0x70, 0x0B, 0x9C, 0xC2, 0x45, 0xFB))
//9DC5E4B1-F2E1-4D6E-963F-700B9CC245FB

#define PLUGMAIN mainMADI
#define PLUGINFACTORY MADIFactory
#include "CFPlugin-bridge.c"
