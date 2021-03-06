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

//NOTE: your plug-in should include PlayerPROCore/PlayerPROCore.h
#include <PlayerPROCore/PlayerPROCore.h>
#include "Example.h"

static short FoundNote(short Period)
{
	short 			note;
	short			MODTuning[70] =
	{
		
		// -> Tuning 0
		
		1712,1616,1524,1440,1356,1280,1208,1140,1076,1016,960,906,
		856,808,762,720,678,640,604,570,538,508,480,453,
		428,404,381,360,339,320,302,285,269,254,240,226,
		214,202,190,180,170,160,151,143,135,127,120,113,
		107,101,95,90,85,80,75,71,67,63,60,56
	};
	note = 0xFF;
	
	
	if(Period != 0)
	{
		for(note = 0;note < 60;note++)
		{
			if(Period >= MODTuning[note]) break;
		}
		if(note >= NUMBER_NOTES) note = 0xFF;
	}
	
	if (note != 0xFF) note += 24;
	
	return note;
}

static void Convert16to8(Ptr srcPtr, Ptr destPtr, size_t size)
{
	long 	i;
	
	size /= 2;
	
	for (i = 0; i < size; i ++)
	{
		destPtr[i] = srcPtr[i*2];
	}
}

static void AnalyseSignatureMOD(long EOFo, OSType temp, short *maxInstru, int *PatternSize, short *tracksNo, MODDef* aMOD)
{
	int			test, i;
	Boolean		result;
	
	*maxInstru = 31;
	
	MADBE32(&temp);
	
	switch(temp)
	{
		case 'FLT4':	*PatternSize = sizeof(struct MODCom) * 64L * 4L;	*tracksNo = 4;	break;
		case 'FLT8':	*PatternSize = sizeof(struct MODCom) * 64L * 4L;	*tracksNo = 4;	break;
		case 'M.K.':	*PatternSize = sizeof(struct MODCom) * 64L * 4L;	*tracksNo = 4;	break;
		case 'M!K!':	*PatternSize = sizeof(struct MODCom) * 64L * 4L;	*tracksNo = 4;	break;
		case '2CHN':	*PatternSize = sizeof(struct MODCom) * 64L * 2L;	*tracksNo = 2;	break;
		case '3CHN':	*PatternSize = sizeof(struct MODCom) * 64L * 3L;	*tracksNo = 3;	break;
		case '4CHN':	*PatternSize = sizeof(struct MODCom) * 64L * 4L;	*tracksNo = 4;	break;
		case '5CHN':	*PatternSize = sizeof(struct MODCom) * 64L * 5L;	*tracksNo = 5;	break;
		case '6CHN':	*PatternSize = sizeof(struct MODCom) * 64L * 6L;	*tracksNo = 6;	break;
		case '7CHN':	*PatternSize = sizeof(struct MODCom) * 64L * 7L;	*tracksNo = 7;	break;
		case '8CHN':	*PatternSize = sizeof(struct MODCom) * 64L * 8L;	*tracksNo = 8;	break;
		case '9CHN':	*PatternSize = sizeof(struct MODCom) * 64L * 9L;	*tracksNo = 9;	break;
		case '10CH':	*PatternSize = sizeof(struct MODCom) * 64L * 10L;	*tracksNo = 10;	break;
		case '11CH':	*PatternSize = sizeof(struct MODCom) * 64L * 11L;	*tracksNo = 11;	break;
		case '12CH':	*PatternSize = sizeof(struct MODCom) * 64L * 12L;	*tracksNo = 12;	break;
		case '13CH':	*PatternSize = sizeof(struct MODCom) * 64L * 13L;	*tracksNo = 13;	break;
		case '14CH':	*PatternSize = sizeof(struct MODCom) * 64L * 14L;	*tracksNo = 14;	break;
		case '15CH':	*PatternSize = sizeof(struct MODCom) * 64L * 15L;	*tracksNo = 15;	break;
		case '16CH':	*PatternSize = sizeof(struct MODCom) * 64L * 16L;	*tracksNo = 16;	break;
		case '17CH':	*PatternSize = sizeof(struct MODCom) * 64L * 17L;	*tracksNo = 17;	break;
		case '18CH':	*PatternSize = sizeof(struct MODCom) * 64L * 18L;	*tracksNo = 18;	break;
		case '19CH':	*PatternSize = sizeof(struct MODCom) * 64L * 19L;	*tracksNo = 19;	break;
		case '20CH':	*PatternSize = sizeof(struct MODCom) * 64L * 20L;	*tracksNo = 20;	break;
		case '21CH':	*PatternSize = sizeof(struct MODCom) * 64L * 21L;	*tracksNo = 21;	break;
		case '22CH':	*PatternSize = sizeof(struct MODCom) * 64L * 22L;	*tracksNo = 22;	break;
		case '23CH':	*PatternSize = sizeof(struct MODCom) * 64L * 23L;	*tracksNo = 23;	break;
		case '24CH':	*PatternSize = sizeof(struct MODCom) * 64L * 24L;	*tracksNo = 24;	break;
		case '25CH':	*PatternSize = sizeof(struct MODCom) * 64L * 25L;	*tracksNo = 25;	break;
		case '26CH':	*PatternSize = sizeof(struct MODCom) * 64L * 26L;	*tracksNo = 26;	break;
		case '27CH':	*PatternSize = sizeof(struct MODCom) * 64L * 27L;	*tracksNo = 27;	break;
		case '28CH':	*PatternSize = sizeof(struct MODCom) * 64L * 28L;	*tracksNo = 28;	break;
		case '29CH':	*PatternSize = sizeof(struct MODCom) * 64L * 29L;	*tracksNo = 29;	break;
		case '30CH':	*PatternSize = sizeof(struct MODCom) * 64L * 30L;	*tracksNo = 30;	break;
		case '31CH':	*PatternSize = sizeof(struct MODCom) * 64L * 31L;	*tracksNo = 31;	break;
		case '32CH':	*PatternSize = sizeof(struct MODCom) * 64L * 32L;	*tracksNo = 32;	break;
			
		default:
			*PatternSize 	= sizeof(struct MODCom) * 64L * 4L;			*tracksNo = 4;
			
			result = true;
			test = 0;
			for (i = 0; i < 15; i++)
			{
				test += aMOD->fid[i].numWords;
				if (aMOD->fid[i].fineTune > 0x0F) result = false;
				
				if (aMOD->fid[i].numWords > 0)
				{
					if (aMOD->fid[i].loopWords > aMOD->fid[i].numWords) result = false;
				}
			}
			
			if (EOFo != -1)
			{
				int		PatMax = 0;
				MODDef	*MODInt;
				
				MODInt = (MODDef*) ((Ptr) aMOD - (Ptr) 0x1E0);
				
				for(i=0; i<128; i++)
				{
					if (MODInt->oPointers[i] < 0) MODInt->oPointers[i] = 0;
					if (MODInt->oPointers[i] > 128) MODInt->oPointers[i] = 0;
					
					if (MODInt->oPointers[i] >= PatMax)	PatMax = MODInt->oPointers[i];
				}
				PatMax++;
				
				if (PatMax * *PatternSize + test > EOFo) result = false;
			}
			
			if (test == 0) result = false;
			
			if (result) *maxInstru 		= 15;
			else *maxInstru				= 0;
			break;
	}
}

static struct MODCom* GetMODCommand(short position, short whichTracks, short whichPattern, short maxTracks, Ptr PatPtr)
{
	struct MODCom*	myMODCom;
	
	myMODCom =	(struct MODCom*) (PatPtr +
								  whichPattern * 64L * sizeof(struct MODCom) * maxTracks +
								  position * sizeof(struct MODCom) * maxTracks +
								  whichTracks * sizeof(struct MODCom));
	
	return myMODCom;
}

static OSErr PPConvertMod2Mad(Ptr aMOD,long MODSize, MADMusic	*theMAD, MADDriverSettings* init)
{
	short	i, PatMax, x, tracksNo, z, maxInstru;
	OSType	temp;
	int		sndSize, OffSetToSample, MPatSize, inOutCount;
	Ptr		theInstrument[64], MaxPtr;
	int		lastIns[32], lastNote[32];
	int		finetune[16] =
	{
		8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757,
		7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280
	};
	/**** Variables for MAD File *****/
	Cmd					*aCmd;
	
	/**** Variables for MOD File *****/
	MODDef 				*theMOD;
	struct MODPat		*PatInt;
	MODDef				*MODInt;
	struct MODCom		*n;
	/********************************/
	
	theMOD = ((MODDef*) aMOD);
	MaxPtr = (Ptr) theMOD;
	MaxPtr += MODSize;
	
	temp = *((OSType*)(aMOD + 0x438));		// Signature...
	
	AnalyseSignatureMOD(-1, temp, &maxInstru, &MPatSize, &tracksNo, theMOD);
	
	if (maxInstru == 0)
	{
		return MADFileNotSupportedByThisPlug;	// This file is NOT a Mod file !!!!!! This should NEVER happen !
	}
	else if (maxInstru == 15)				// Old Mods format with 15 instruments
	{
		MODInt = (MODDef*) ((Ptr) theMOD - (Ptr) 0x1E0);
		PatInt = (struct MODPat*) ((Ptr) MODInt->patterns - (Ptr) 0x4);
		
		PatMax = 0;
		for(i=0; i<128; i++)
		{
			if (MODInt->oPointers[i] < 0) MODInt->oPointers[i] = 0;
			if (MODInt->oPointers[i] > 128) MODInt->oPointers[i] = 0;
			
			if (MODInt->oPointers[i] >= PatMax)	PatMax = MODInt->oPointers[i];
		}
		PatMax++;
		
		//	if (MODInt->numPointers > 64) MODInt->numPointers = 64;
		//	for(i=64; i<128; i++) MODInt->oPointers[i] = 0;
		
		OffSetToSample = (int) 0x258 + PatMax * MPatSize;
	}
	else									// Mods format with 32 instruments
	{
		MODInt	= theMOD;
		PatInt	= MODInt->patterns;
		
		PatMax = 0;
		for(i=0; i<128; i++)
		{
			if (MODInt->oPointers[i] < 0) MODInt->oPointers[i] = 0;
			if (MODInt->oPointers[i] > 128) MODInt->oPointers[i] = 0;
			
			if (MODInt->oPointers[i] >= PatMax) PatMax = MODInt->oPointers[i];
		}
		PatMax++;
		
		OffSetToSample = 0x43c + PatMax * MPatSize;
	}
	
	for (i = 0; i < maxInstru ; i++)
	{
		theInstrument[i] = (Ptr) ((uintptr_t) theMOD + OffSetToSample);
		
		MADBE16(&theMOD->fid[i].numWords);
		MADBE16(&theMOD->fid[i].loopWord);
		MADBE16(&theMOD->fid[i].loopWords);
		
		sndSize = (theMOD->fid[i].numWords) * 2;
		
		if (theInstrument[i] + sndSize > MaxPtr)
		{
			theMOD->fid[i].numWords = MaxPtr - theInstrument[i];
			theMOD->fid[i].numWords /= 2L;
			
			if (theMOD->fid[i].numWords < 0) theMOD->fid[i].numWords = 0;
			
			sndSize = theMOD->fid[i].numWords * 2L;
		}
		
		OffSetToSample += sndSize;
		
		if (theMOD->fid[i].loopWords > 2 && sndSize > 0)
		{
			if ((int) theMOD->fid[i].loopWord +
			   (int) theMOD->fid[i].loopWords >
			   (int) theMOD->fid[i].numWords)
			{
				theMOD->fid[i].loopWords =	(int) theMOD->fid[i].numWords -
				(int) theMOD->fid[i].loopWord;
				
				if ((int) theMOD->fid[i].loopWord +
				   (int) theMOD->fid[i].loopWords >
				   (int) theMOD->fid[i].numWords)
				{
					theMOD->fid[i].loopWord = 0;
					theMOD->fid[i].loopWords = 0;
				}
			}
		}
		else
		{
			theMOD->fid[i].loopWord = 0;
			theMOD->fid[i].loopWords = 0;
		}
	}
	
	/***************************************************************/
	/******** MOD is ready to be converted into MAD File ***********/
	/***************************************************************/
	
	inOutCount = sizeof(MADSpec);
	theMAD->header = (MADSpec*) calloc(inOutCount, 1);
	if (theMAD->header == NULL) return MADNeedMemory;
	
	//Some info about the tracker being imported.
	//If your tracker doesn't have an info field, just do
	strncpy(theMAD->header->infos, "Converted by PlayerPRO MOD Plug (\251Antoine ROSSET <rossetantoine@bluewin.ch>)", sizeof(theMAD->header->infos));
	
	theMAD->header->MAD = 'MADK';
	theMAD->header->MODMode = true;
	
	for(i=0; i<22; i++) theMAD->header->name[i] = theMOD->NameSignature[i];
	
	theMAD->header->tempo = 125;
	theMAD->header->speed = 6;
	theMAD->header->numPat = PatMax;
	theMAD->header->numPointers = MODInt->numPointers;
	
	for(i=0; i<128; i++) theMAD->header->oPointers[i] = MODInt->oPointers[i];
	
	theMAD->header->numChn = tracksNo;
	
	x = 1;
	for (i = 0; i < MAXTRACK; i++)
	{
		if (x > 0) theMAD->header->chanPan[i] = MAX_PANNING/4;
		else theMAD->header->chanPan[i] = MAX_PANNING - MAX_PANNING/4;
		x--;
		
		if (x == -2) x = 2;
		
		theMAD->header->chanVol[i] = MAX_VOLUME;
	}
	
	theMAD->header->generalVol		= 64;
	theMAD->header->generalSpeed	= 80;
	theMAD->header->generalPitch	= 80;
	
	theMAD->sets = (FXSets*) calloc(MAXTRACK * sizeof(FXSets), 1);
	for (i = 0; i < MAXTRACK; i++) theMAD->header->chanBus[i].copyId = i;
	
	/////////////////////////////////
	// Instruments &  samples
	/////////////////////////////////
	
	theMAD->fid = (InstrData*) calloc(sizeof(InstrData) * (long) MAXINSTRU, 1);
	if (!theMAD->fid) return MADNeedMemory;
	
	theMAD->sample = (sData**) calloc(sizeof(sData*) * (long) MAXINSTRU * (long) MAXSAMPLE, 1);
	if (!theMAD->sample) return MADNeedMemory;
	
	for(i = 0; i < maxInstru; i++)
	{
		for (x = 0; x < 22; x++) theMAD->fid[i].name[x] = theMOD->fid[i].Filename[x];
		theMAD->fid[i].type = 0;
		theMAD->fid[i].volFade = DEFAULT_VOLFADE;
		
		if (theMOD->fid[i].numWords > 0)
		{
			sData	*curData;
			
			theMAD->fid[i].numSamples = 1;
			
			curData = theMAD->sample[i*MAXSAMPLE + 0] = (sData*) calloc(sizeof(sData), 1);
			
			curData->size		= theMOD->fid[i].numWords * 2L;
			curData->loopBeg 	= theMOD->fid[i].loopWord*2L;
			curData->loopSize 	= theMOD->fid[i].loopWords*2L;
			curData->vol		= theMOD->fid[i].volume;
			curData->c2spd		= finetune[theMOD->fid[i].fineTune&0xf];
			curData->loopType	= 0;
			curData->amp		= 8;
			curData->realNote	= 0;
			//	for (x = 0; x < 22; x++) curData->name[x] = theMOD->fid[i].Filename[x];
			
			
			curData->data 		= (Ptr)malloc(curData->size);
			if (curData->data == NULL) return MADNeedMemory;
			
			memcpy(curData->data, theInstrument[i], curData->size);
			
			if (theInstrument[i] + curData->size > MaxPtr) //DebugStr("\pMax Ptr Instru");
				return MADIncompatibleFile;
		}
		else theMAD->fid[i].numSamples = 0;
	}
	
	for (i = 0; i < MAXINSTRU; i++) theMAD->fid[i].firstSample = i * MAXSAMPLE;
	
	for (i=0; i < 32; i++)
	{
		lastIns[i] = 0;
		lastNote[i] = 0;
	}
	
	for(i=0; i<theMAD->header->numPat; i++)
	{
		theMAD->partition[i] = (PatData*) calloc(sizeof(PatHeader) + theMAD->header->numChn * 64L * sizeof(Cmd), 1);
		if (theMAD->partition[i] == NULL) return MADNeedMemory;
		
		theMAD->partition[i]->header.size 		= 64L;
		theMAD->partition[i]->header.compMode 	= 'NONE';
		
		for (x = 0; x < 20; x++) theMAD->partition[i]->header.name[x] = 0;
		
		theMAD->partition[i]->header.patBytes = 0;		theMAD->partition[i]->header.unused2 = 0;
		
		for(x=0; x<64; x++)
		{
			for(z=0; z<theMAD->header->numChn; z++)
			{
				aCmd = GetMADCommand( x,  z, theMAD->partition[i]);
				
				/////////////
				
				n 	= 	GetMODCommand(	x,
									  z,
									  i,
									  theMAD->header->numChn,
									  (Ptr) PatInt);
				
				if ((Ptr) n > MaxPtr) //DebugStr("\pMax Ptr GetMODCommand");
					return MADIncompatibleFile;
				
				aCmd->ins = (n->a&0x10)|(n->c>>4);
				
				aCmd->note = FoundNote((((unsigned short)n->a&0xf)<<8)+n->b);
				if (aCmd->note == 0xFF && aCmd->ins != 0)
				{
					if (aCmd->ins != lastIns[z])
					{
						aCmd->note = lastNote[z];
					}
				}
				
				if (aCmd->note != 0xFF) lastNote[z] = aCmd->note;
				if (aCmd->ins) lastIns[z] = aCmd->ins;
				
				aCmd->cmd = n->c & 0x0F;
				
				if (aCmd->cmd == 0x0A)
				{
					if (n->d == 0)
					{
						aCmd->cmd = 0;
					}
				}
				
				if (aCmd->cmd == 0x0C)
				{
					aCmd->vol	= 0x10 + (n->d);
					if (aCmd->arg > 0x50) aCmd->vol = 0x50;
					aCmd->cmd 	= 0;
					aCmd->arg 	= 0;
				}
				else
				{
					aCmd->arg 	= n->d;
					aCmd->vol	= 0xFF;
				}
			}
		}
	}
	for (i = theMAD->header->numPat; i < MAXPATTERN ; i++) theMAD->partition[i] = NULL;
	
	return MADNoErr;
}

static int ConvertSampleC4SPD(Ptr src, size_t srcSize, short amp, int srcC4SPD, Ptr dst, int dstC4SPD)
{
	short	*src16 = (short*) src, *dst16 = (short*) dst;
	Ptr		src8 = src, dst8 = dst;
	long	x;
	size_t	dstSize;
	
	if (dstC4SPD > srcC4SPD)
	{
		dstSize = (srcSize * dstC4SPD) / srcC4SPD;
		
		if (amp == 8)
		{
			for (x = 0; x < dstSize; x++)
			{
				dst8[x] = src8[(x * srcC4SPD) / dstC4SPD];
			}
		}
		else
		{
			for (x = 0; x < dstSize/2; x++)
			{
				dst16[x] = src16[(x * srcC4SPD) / dstC4SPD];
			}
		}
	}
	else
	{
		if (amp == 8)
		{
			for (x = 0; x < srcSize; x++)
			{
				dst8[(x * dstC4SPD) / srcC4SPD] = src8[x];
			}
		}
		else
		{
			for (x = 0; x < srcSize/2; x++)
			{
				dst16[(x * dstC4SPD) / srcC4SPD] = src16[x];
			}
		}
	}
	
	return (int)((srcSize * dstC4SPD) / srcC4SPD);
}

static Ptr PPConvertMad2Mod(MADMusic *theMAD, MADDriverSettings *init, long *PtrSize)
{
	int 				i, x, z, maxInstru;
	int 				OffSetToSample, InstruSize, *alpha;
	Ptr					theInstrument[64], destPtr;
	Boolean				CheckGoodMod;
	char				redut[4];
	short				MODTuning[75] =
	
	{
		
		// -> Tuning 0
		
		1712,1616,1524,1440,1356,1280,1208,1140,1076,1016,960,906,
		856,808,762,720,678,640,604,570,538,508,480,453,
		428,404,381,360,339,320,302,285,269,254,240,226,
		214,202,190,180,170,160,151,143,135,127,120,113,
		107,101,95,90,85,80,75,71,67,63,60,56
	};
	
	/**** Variables for MAD file ****/
	
	Cmd					*aCmd;
	
	/**** Variables for MOD file ****/
	
	MODDef 				*theMOD;
	Ptr					maxMOD;
	struct MODCom		*n;
	/********************************/
	
	maxInstru = 31;
	
	InstruSize = 0;
	for (i = 0; i < maxInstru ; i++)
	{
		if (theMAD->fid[i].numSamples > 0)
		{
			long dstSize;
			
			theInstrument[i] = theMAD->sample[i*MAXSAMPLE + 0]->data;
			
			dstSize = theMAD->sample[i*MAXSAMPLE + 0]->size;
			
			if (theMAD->sample[i*MAXSAMPLE + 0]->c2spd < 7895) dstSize = (dstSize * 8363L) / theMAD->sample[i*MAXSAMPLE + 0]->c2spd;
			
			InstruSize += dstSize;
		}
	}
	
	/******** MAD is ready to be converted **********/
	/******** Copy information in the MOD file    ***/
	
	*PtrSize = 0x43c + InstruSize + theMAD->header->numChn * 64L * theMAD->header->numPat * sizeof(struct MODCom);
	
	theMOD = (MODDef*) malloc(*PtrSize + 25000L);
	if (theMOD == NULL) return NULL;
	
	maxMOD = ((Ptr) theMOD + *PtrSize);
	
	theMOD->longFmtSignature = 'M.K.';
	if (theMAD->header->numChn > 4)
	{
		if (theMAD->header->numChn >= 10)
		{
			redut[0] = 0x30 + (theMAD->header->numChn / 10);
			redut[1] = 0x30 + (theMAD->header->numChn - ((theMAD->header->numChn / 10) * 10));
			redut[2] = 'C';
			redut[3] = 'H';
		}
		else
		{
			redut[0] = 0x30 + theMAD->header->numChn;
			redut[1] = 'C';
			redut[2] = 'H';
			redut[3] = 'N';
		}
		
		alpha = (int*) redut;
		
		theMOD->longFmtSignature = *alpha;
	}
	
	for(i=0; i<20; i++) theMOD->NameSignature[i] = theMAD->header->name[i];
	
	CheckGoodMod = false;
	theMOD->numPointers = theMAD->header->numPointers;
	for(i=0; i<128; i++)
	{
		theMOD->oPointers[i] = theMAD->header->oPointers[i];
		if (theMOD->oPointers[i] == theMAD->header->numPat - 1) CheckGoodMod = true;
	}
	if (!CheckGoodMod) theMOD->oPointers[theMOD->numPointers + 1] = theMAD->header->numPat - 1;
	
	for(i=0; i<maxInstru; i++)
	{
		if (theMAD->fid[i].numSamples > 0)
		{
			sData	*curData = theMAD->sample[i*MAXSAMPLE + 0];
			short	temp;
			
			for (x = 0; x < 22; x++) theMOD->fid[i].Filename[x] = theMAD->fid[i].name[x];
			
			if (curData->size/2L > 0xFFFFUL) theMOD->fid[i].numWords = 0xFFFFUL;
			else theMOD->fid[i].numWords = (short) (curData->size / 2L);
			
			temp = (curData->c2spd - NOFINETUNE) / 50;
			if (temp < 0) temp += 16;
			
			theMOD->fid[i].fineTune		= temp;
			
			theMOD->fid[i].volume 		= curData->vol;
			theMOD->fid[i].loopWord 	= curData->loopBeg / 2;
			theMOD->fid[i].loopWords 	= curData->loopSize / 2;
			
			if (curData->c2spd > 8757 || curData->c2spd < 7895)
			{
				theMOD->fid[i].fineTune = 0;
				theMOD->fid[i].loopWord = ((curData->loopBeg / 2) * 8363) / curData->c2spd;
				theMOD->fid[i].loopWords = ((curData->loopSize / 2) * 8363) / curData->c2spd;
				theMOD->fid[i].numWords = (theMOD->fid[i].numWords * 8363) / curData->c2spd;
			}
		}
		else
		{
			for (x = 0; x < 22; x++) theMOD->fid[i].Filename[x] = 0;
			
			theMOD->fid[i].numWords 	= 0;
			theMOD->fid[i].fineTune 	= 0;
			theMOD->fid[i].volume 		= 64;
			theMOD->fid[i].loopWord 	= 0;
			theMOD->fid[i].loopWords 	= 0;
		}
	}
	
	InstruSize = 0;
	
	OffSetToSample = (long) 0x43c + theMAD->header->numPat * sizeof(struct MODCom) * 64L * theMAD->header->numChn;
	
	for(i=0; i<maxInstru; i++)
	{
		sData	*curData = theMAD->sample[i*MAXSAMPLE + 0];
		
		if (theMAD->fid[i].numSamples > 0)
		{
			destPtr = (Ptr) ((long) theMOD + (long) OffSetToSample + (long) InstruSize);
			
			if (curData->c2spd > 8757 || curData->c2spd < 7895)
			{
				ConvertSampleC4SPD(theInstrument[i], curData->size, curData->amp, curData->c2spd, destPtr, 8363);
			}
			else memcpy(destPtr, theInstrument[i], (long) (theMOD->fid[i].numWords) * 2L);
			if (destPtr + (theMOD->fid[i].numWords) * 2L > maxMOD) //DebugStr("\pOut");
				return NULL;
			
			if (theMOD->fid[i].numWords > 0)
			{
				if (curData->amp == 16)
				{
					Convert16to8(	destPtr,
								 destPtr,
								 (long) (theMOD->fid[i].numWords) * 2L);
					
					theMOD->fid[i].loopWord /=2;
					theMOD->fid[i].loopWords /=2;
					theMOD->fid[i].numWords /= 2;
				}
				
				if (curData->stereo)
				{
					for (x = 0 ; x < (theMOD->fid[i].numWords) * 2L; x+=2)
					{
						destPtr[x / 2] = ((long) destPtr[x] + (long) destPtr[x + 1]) / 2L;
					}
					
					theMOD->fid[i].loopWord /=2;
					theMOD->fid[i].loopWords /=2;
					theMOD->fid[i].numWords /= 2;
				}
			}
			
			InstruSize += (long) (theMOD->fid[i].numWords) * 2L;
		}
	}
	
	for(i=0; i<theMAD->header->numPat; i++)
	{
		Cmd			nullCmd;
		
		nullCmd.ins		= 0;
		nullCmd.note	= 0xFF;
		nullCmd.cmd		= 0;
		nullCmd.arg		= 0;
		nullCmd.vol		= 0xFF;
		nullCmd.unused	= 0;
		
		for(x=0; x < 64; x++)
		{
			for(z=0; z < theMAD->header->numChn; z++)
			{
				//				short		note;
				
				if (x < theMAD->partition[i]->header.size)
				{
					aCmd 	= GetMADCommand(x,
											z,
											theMAD->partition[i]);
				}
				else aCmd = &nullCmd;
				
				n 			= GetMODCommand(x,
											z,
											i,
											theMAD->header->numChn,
											(Ptr) theMOD->patterns);
				
				//if ((Ptr) n > maxMOD) DebugStr("\pOut");
				//if ((Ptr) n < (Ptr) theMOD) DebugStr("\pOut");
				if ((Ptr) n > maxMOD) return NULL;
				if ((Ptr) n < (Ptr) theMOD) return NULL;
				
				
				n->a = aCmd->ins & 0xF0;
				n->c = (aCmd->ins & 0x0F)<<4;
				
				if (aCmd->note != 0xFF && aCmd->note != 0xFE)
				{
					short curNote;
					
					if (aCmd->ins != 0)
					{
						if (theMAD->fid[aCmd->ins-1].numSamples > 0)
							curNote = aCmd->note + theMAD->sample[(aCmd->ins-1)*MAXSAMPLE + 0]->realNote;
						else 	curNote = aCmd->note;
					}
					else curNote = aCmd->note;
					
					curNote -= 24;
					
					if (curNote > 0 && curNote < 65) curNote = MODTuning[curNote];
					else curNote = 0;
					
					n->b = curNote & 0xFF;
					n->a = n->a + (curNote>>8);
				}
				else
				{
					n->b = 0;
					n->a = n->a + 0x0;
				}
				
				if (aCmd->vol != 0xFF && aCmd->cmd == 0 && aCmd->arg == 0)
				{
					if (aCmd->vol >= 0x10 && aCmd->vol <= 0x50)
					{
						n->c = n->c + 0xC;
						n->d = aCmd->vol - 0x10;
					}
				}
				else
				{
					n->c = n->c + (aCmd->cmd & 0x0F);
					n->d = aCmd->arg;
				}
			}
		}
	}
	
	// Default speed & tempo
	
	if (theMAD->header->tempo != 125 || theMAD->header->speed != 6)
	{
		n 	= GetMODCommand(0,
							theMAD->header->numChn-1,
							theMAD->header->oPointers[0],
							theMAD->header->numChn,
							(Ptr) theMOD->patterns);
		
		//if ((Ptr) n > maxMOD) DebugStr("\pOut");
		//if ((Ptr) n < (Ptr) theMOD) DebugStr("\pOut");
		
		n->a = 0;
		n->b = 0;
		n->c = 0;
		n->c = n->c + 0x0F;			// speed
		n->d = theMAD->header->speed;
		
		n 	= GetMODCommand(0,
							theMAD->header->numChn-2,
							theMAD->header->oPointers[0],
							theMAD->header->numChn,
							(Ptr) theMOD->patterns);
		
		//if ((Ptr) n > maxMOD) DebugStr("\pOut");
		//if ((Ptr) n < (Ptr) theMOD) DebugStr("\pOut");
		
		n->a = 0;
		n->b = 0;
		n->c = 0;
		n->c = n->c + 0x0F;			// tempo
		n->d = theMAD->header->tempo;
	}
	
	return((Ptr) theMOD);
}

static OSErr ExtractMODInfo(MADInfoRec *info, Ptr AlienFile)
{
	MODDef	*myMOD = (MODDef*) AlienFile;
	int	PatternSize;
	short	i;
	short	maxInstru;
	
	/*** Signature ***/
	
	info->signature = myMOD->longFmtSignature;
	
	/*** Internal name ***/
	
	myMOD->NameSignature[19] = '\0';
	strncpy(info->internalFileName, myMOD->NameSignature, sizeof(myMOD->NameSignature));
	
	/*** Check MOD Type ***/
	
	AnalyseSignatureMOD(-1, info->signature, &maxInstru, &PatternSize, &info->totalTracks, myMOD);
	//The signature of the file.
	MADBE32(&info->signature);
	if (maxInstru == 0)
	{
		return MADFileNotSupportedByThisPlug;
	}
	else if (maxInstru == 15)	// Old mod format
	{
		info->signature = '----';
		myMOD = (MODDef*) ((Ptr) myMOD - (Ptr) 0x1E0);
	}
	
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
	
	for (i = 0, info->totalInstruments = 0; i < maxInstru ; i++)
	{
		if (myMOD->fid[i].numWords > 5) info->totalInstruments++;
	}
	
	//The plug-in type used to get the tracker
	strncpy(info->formatDescription, "MOD Plug", sizeof(info->formatDescription));
	
	return MADNoErr;
}

static OSErr TestMODFile(Ptr AlienFile, long EOFo)
{
	short		maxInstru;
	int		PatternSize;
	short		tracksNo;
	
	AnalyseSignatureMOD(EOFo, *((int*)(AlienFile + 0x438)), &maxInstru, &PatternSize, &tracksNo, (MODDef*) AlienFile);
	
	if (maxInstru == 0) return MADFileNotSupportedByThisPlug;
	else return MADNoErr;
}

//These must be exported so that C can see them.
//If your plug-in uses C++, export them using extern "C"
// The EXP preprocessor define is defined as extern "C", so you can use that

EXP OSErr PPImpExpMain(OSType order, Ptr AlienFileName, MADMusic *MadFile, MADInfoRec *info, MADDriverSettings *init);

#ifndef _MAC_H
EXP OSErr FillPlug(PlugInfo *p);
EXP OSErr FillPlug(PlugInfo *p)		// Function USED IN DLL - For PC, BeOS, and UNIX
{
	//If your architecture supports it, you can get this metadata from the plug-in file itself
	//You can also localize it if you feel so inclined
	strncpy(p->type, 		"MOD ", sizeof(p->type));// NEVER MORE THAN 4 CHARS!
	strncpy(p->MenuName, 	"MOD Files", sizeof(p->MenuName));
	p->mode	=	MADPlugImportExport;
	//Version 2.0.0
	//Increment the version when you make big changes so the newer one will be used
	p->version = 2 << 16 | 0 << 8 | 0;
	
	return MADNoErr;
}
#endif

/*****************/
/* MAIN FUNCTION */
/*****************/
//Every PlayerPRO import/export plug-in must have this function!
extern OSErr PPImpExpMain(OSType order, Ptr AlienFileName, MADMusic *MadFile, MADInfoRec *info, MADDriverSettings *init)
{
	OSErr	myErr;
	Ptr		AlienFile;
	long	sndSize;
	UNFILE	iFileRefI;
	
	myErr = MADNoErr;
	
	switch(order)
	{
		case MADPlugImport:
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI)
			{
				sndSize = iGetEOF(iFileRefI);
				
				// ** MEMORY Test Start
				AlienFile = (Ptr)malloc(sndSize * 2L);
				if (AlienFile == NULL) myErr = MADNeedMemory;
				// ** MEMORY Test End
				
				else
				{
					free(AlienFile);
					
					AlienFile = (Ptr)malloc(sndSize);
					iRead(sndSize, AlienFile, iFileRefI);
					
					myErr = TestMODFile(AlienFile, sndSize);
					if (myErr == MADNoErr)
					{
						myErr = PPConvertMod2Mad(AlienFile, sndSize, MadFile, init);
					}
					
					free(AlienFile);	AlienFile = NULL;
				}
				iClose(iFileRefI);
			}
			else myErr = MADReadingErr;
			break;
			
		case MADPlugTest:
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI)
			{
				sndSize = 5000;	// Read only 5000 first bytes for optimisation
				
				AlienFile = (Ptr)malloc(sndSize);
				if (AlienFile == NULL) myErr = MADNeedMemory;
				else
				{
					iRead(sndSize, AlienFile, iFileRefI);
					
					sndSize = iGetEOF(iFileRefI);
					
					myErr = TestMODFile(AlienFile, sndSize);
					
					free(AlienFile);	AlienFile = NULL;
				}
				iClose(iFileRefI);
			}
			else myErr = MADReadingErr;
			break;
			
		case MADPlugExport:
			AlienFile = PPConvertMad2Mod(MadFile, init, &sndSize);
			
			if (AlienFile != NULL)
			{
				iFileCreate(AlienFileName, 'STrk');
				iFileRefI = iFileOpenWrite(AlienFileName);
				if (iFileRefI)
				{
					iWrite(sndSize, AlienFile, iFileRefI);
					iClose(iFileRefI);
				}
				else
				{
					myErr = MADWritingErr;
				}
				free(AlienFile);	AlienFile = NULL;
			}
			else myErr = MADReadingErr;
			break;
			
		case 'INFO':
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI)
			{
				info->fileSize = iGetEOF(iFileRefI);
				
				sndSize = 5000;	// Read only 5000 first bytes for optimisation
				
				AlienFile = (Ptr)malloc(sndSize);
				if (AlienFile == NULL) myErr = MADNeedMemory;
				else
				{
					iRead(sndSize, AlienFile, iFileRefI);
					
					myErr = TestMODFile(AlienFile, info->fileSize);
					
					if (!myErr) myErr = ExtractMODInfo(info, AlienFile);
					
					free(AlienFile);	AlienFile = NULL;
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
