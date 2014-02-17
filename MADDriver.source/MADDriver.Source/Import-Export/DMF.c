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
//#include "DMF.h"
#include "IT.h"

#define LOW(para) ((para) & 15)
#define HI(para) ((para) >> 4)

//int Tdecode32(void*) UNAVAILABLE_ATTRIBUTE;
//short Tdecode16(void*) UNAVAILABLE_ATTRIBUTE;

Cmd* GetMADCommand(register short PosX, register short TrackIdX, register PatData* tempMusicPat)
{
	if (PosX < 0)
		PosX = 0;
	else if (PosX >= tempMusicPat->header.size)
		PosX = tempMusicPat->header.size - 1;
		
	return &(tempMusicPat->Cmds[(tempMusicPat->header.size * TrackIdX) + PosX]);
}

static inline void mystrcpy(Ptr a, BytePtr b)
{
	BlockMoveData(b + 1, a, b[0]);
}

#if 0
static void ConvertITEffect(Byte B0, Byte B1, Byte *Cmd, Byte *Arg)
{
	//Str255	tStr;
	Byte		LoB1 = LOW(B1);
	Byte		HiB1 = HI(B1);
	
	switch (B0 + 0x40)
	{
			// Speed
		case 'A':	*Cmd = speedE;		*Arg = B1;	break;
			// Tempo
		case 'T':	*Cmd = speedE;		*Arg = B1;	break;
			
		case 'B':	*Cmd = fastskipE;		*Arg = B1;	break;
			
		case 'C':	*Cmd = skipE;			*Arg = B1;	break;
			
		case 'D':
			if (LoB1 == 0 || HiB1 == 0)		// Slide volume
			{
				*Cmd = slidevolE;		*Arg = B1;
			}
			else if (HiB1 == 0x0F)		// Fine Slide volume DOWN
			{
				*Cmd = extendedE;
				*Arg = 11 << 4;
				*Arg += LoB1;
			}
			else if (LoB1 == 0x0F)		// Fine Slide volume UP
			{
				*Cmd = extendedE;
				*Arg = 10 << 4;
				*Arg += HiB1;
			}
			break;
			
		case 'E':
			if (HiB1 == 0x0F)		// FineSlide DOWN
			{
				*Cmd = extendedE;
				*Arg = 2 << 4;		//not supported
				*Arg += LoB1;
			}
			else if (HiB1 == 0x0E)	// ExtraFineSlide DOWN
			{
				*Cmd = 0;			*Arg = 0;		//not supported
			}
			else					// Slide DOWN
			{
				*Cmd = upslideE;		*Arg = B1;
			}
			break;
			
		case 'F':
			if (HiB1 == 0x0F)		// FineSlide UP
			{
				*Cmd = extendedE;
				*Arg = 1 << 4;		//not supported
				*Arg += LoB1;
			}
			else if (HiB1 == 0x0E)	// ExtraFineSlide UP
			{
				*Cmd = 0;			*Arg = 0;		//not supported
			}
			else					// Slide UP
			{
				*Cmd = downslideE;		*Arg = B1;
			}
			break;
			
		case 'G':	*Cmd = portamentoE;	*Arg = B1;	break;
		case 'H':	*Cmd = vibratoE;		*Arg = B1;	break;
			
		case 'J':	*Cmd = arpeggioE;		*Arg = B1;	break;
		case 'K':	*Cmd = vibratoslideE;	*Arg = B1;	break;
		case 'L':	*Cmd = portaslideE;		*Arg = B1;	break;
		case 'O':	*Cmd = offsetE;		*Arg = B1;	break;
			
		case 'S':		// Special Effects
			switch (HiB1)
		{
			case 2:	*Cmd = extendedE;	*Arg = 5 << 4;		*Arg += LoB1;		break;	// FineTune
			case 3:	*Cmd = extendedE;	*Arg = 4 << 4;		*Arg += LoB1;		break;	// Set Vibrato WaveForm
			case 4:	*Cmd = extendedE;	*Arg = 7 << 4;		*Arg += LoB1;		break;	// Set Tremolo WaveForm
			case 0xB:	*Cmd = extendedE;	*Arg = 6 << 4;		*Arg += LoB1;		break;	// Loop pattern
			case 0xC:	*Cmd = extendedE;	*Arg = 12 << 4;	*Arg += LoB1;		break;	// Cut sample
			case 0xD:	*Cmd = extendedE;	*Arg = 13 << 4;	*Arg += LoB1;		break;	// Delay sample
			case 0xE:	*Cmd = extendedE;	*Arg = 14 << 4;	*Arg += LoB1;		break;	// Delay pattern
			default:	*Cmd = 0;		*Arg = 0;							break;
		}
			break;
		default:	*Cmd = 0;			*Arg = 0;		break;
	}
}

static void ConvertMADEffect(Byte Cmd, Byte Arg, Byte *B0, Byte *B1)
{
	*B0 = 0;
	*B1 = 0;

	switch (Cmd)
	{
		case speedE:		*B0 = 'A' - 0x40;	*B1 = Arg;	break;
		case fastskipE:		*B0 = 'B' - 0x40;	*B1 = Arg;	break;
		case skipE:		*B0 = 'C' - 0x40;	*B1 = Arg;	break;
		case slidevolE:		*B0 = 'D' - 0x40;	*B1 = Arg;	break;
		case upslideE:		*B0 = 'E' - 0x40;	*B1 = Arg;	break;
		case downslideE:	*B0 = 'F' - 0x40;	*B1 = Arg;	break;
		case portamentoE:	*B0 = 'G' - 0x40;	*B1 = Arg;	break;
		case vibratoE:		*B0 = 'H' - 0x40;	*B1 = Arg;	break;
			
		case arpeggioE:		*B0 = 'J' - 0x40;	*B1 = Arg;	break;
		case vibratoslideE:	*B0 = 'K' - 0x40;	*B1 = Arg;	break;
		case portaslideE:	*B0 = 'L' - 0x40;	*B1 = Arg;	break;
		case offsetE:		*B0 = 'O' - 0x40;	*B1 = Arg;	break;
			
		case extendedE:
			switch ((Arg & 0xF0) >> 4)
		{
			case 1:	*B0 = 'F' - 0x40;	*B1 = 0xf0 + (Arg & 0x0F);		break;
			case 2:	*B0 = 'E' - 0x40;	*B1 = 0xf0 + (Arg & 0x0F);		break;
				
			case 5:	*B0 = 'S' - 0x40;	*B1 = 0x20 + (Arg & 0x0F);		break;
				
			case 4:	*B0 = 'S' - 0x40;	*B1 = 0x30 + (Arg & 0x0F);		break;
			case 7:	*B0 = 'S' - 0x40;	*B1 = 0x40 + (Arg & 0x0F);		break;
			case 6:	*B0 = 'S' - 0x40;	*B1 = 0xb0 + (Arg & 0x0F);		break;
			case 12:	*B0 = 'S' - 0x40;	*B1 = 0xc0 + (Arg & 0x0F);		break;
			case 13:	*B0 = 'S' - 0x40;	*B1 = 0xd0 + (Arg & 0x0F);		break;
			case 14:	*B0 = 'S' - 0x40;	*B1 = 0xe0 + (Arg & 0x0F);		break;
				
			case 11:	*B0 = 'D' - 0x40;	*B1 = 0xf0 + (Arg & 0x0F);		break;
			case 10:	*B0 = 'D' - 0x40;	*B1 = 0x0f + ((Arg & 0x0F)<<4);	break;
		}
			break;
	}
}
#endif

static OSErr ConvertIT2Mad(Ptr theIT, long MODSize, MADMusic *theMAD, MADDriverSettings *init)
{
	long 				i, x, z, channel, Row;
	//long 				sndSize, OffSetToSample, OldTicks, PatMax;
	long				starting;
	Ptr					MaxPtr;
	//OSErr				theErr;
	Ptr					theInstrument[64], destPtr;
	Byte				tempChar, *theITCopy;
	//short				Note, Octave, maxTrack;
	//short				ITperiod[12] = {1712,1616,1524,1440,1356,1280,1208,1140,1076,1016, 960, 907};
	//long				note_st3period;
	//long				note_amigaperiod;
	//Byte				*ChannelSettings;
	
	/**** Variables pour le MAD ****/
	Cmd				*aCmd;

	/**** Variables pour le IT ****/
	
	ITForm			ITinfo;
	/********************************/

	for (i = 0 ; i < 64; i ++)
	{
		theInstrument[i] = NULL;
	}

	/**** Header principal *****/
	theITCopy = (Byte*) theIT;
	
	BlockMoveData(theITCopy, &ITinfo, sizeof(ITinfo));
	theITCopy += 192;
	
	INT16(&ITinfo.orderNum);
	INT16(&ITinfo.insNum);
	INT16(&ITinfo.smpNum);
	INT16(&ITinfo.patNum);
	INT16(&ITinfo.flags);
	INT16(&ITinfo.cwtv);
	
	/**** Order Num *****/
	ITinfo.orders = (unsigned char *) MADPlugNewPtr(ITinfo.orderNum, init);
	if (ITinfo.orders == NULL) return MADNeedMemory;
	BlockMoveData(theITCopy, ITinfo.orders, ITinfo.orderNum);
	theITCopy += ITinfo.orderNum;
	
	/**** Ins Num *****/
	ITinfo.parapins = (SInt32 *) MADPlugNewPtr(ITinfo.insNum * 4L, init);
	if (ITinfo.parapins == NULL) {
		DisposePtr((Ptr)ITinfo.orders);
		
		return MADNeedMemory;
	}
	BlockMoveData(theITCopy, ITinfo.parapins, ITinfo.insNum * 4L);
	theITCopy += ITinfo.insNum * 4L;
	for (i = 0; i < ITinfo.insNum; i++) {
		INT32(&ITinfo.parapins[i]);
	}
	
	/**** Samp Num *****/
	ITinfo.parapsamp = (SInt32 *) MADPlugNewPtr(ITinfo.smpNum * 4L, init);
	if (ITinfo.parapsamp == NULL)
	{
		DisposePtr((Ptr)ITinfo.orders);
		DisposePtr((Ptr)ITinfo.parapins);
		
		return MADNeedMemory;
	}
	BlockMoveData(theITCopy, ITinfo.parapsamp, ITinfo.smpNum * 4L);
	theITCopy += ITinfo.smpNum * 4L;
	for (i = 0; i < ITinfo.smpNum; i++) {
		INT32(&ITinfo.parapsamp[i]);
	}
	
	/**** Pat Num *****/
	ITinfo.parappat = (long *) MADPlugNewPtr(ITinfo.patNum * 4L, init);
	if (ITinfo.parappat == NULL)
	{
		DisposePtr((Ptr)ITinfo.orders);
		DisposePtr((Ptr)ITinfo.parapins);
		DisposePtr((Ptr)ITinfo.parapsamp);
		
		return MADNeedMemory;
	}
	BlockMoveData(theITCopy, ITinfo.parappat, ITinfo.patNum * 4L);
	theITCopy += ITinfo.patNum * 4L;
	for (i = 0; i < ITinfo.patNum; i++) {
		INT32(&ITinfo.parappat[i]);
	}
	
	/**** Ins Data ****/
	if (ITinfo.insNum > 64) ITinfo.insNum = 64;
	ITinfo.insdata = (ITInsForm *) MADPlugNewPtr(sizeof(ITInsForm) * ITinfo.insNum, init);
	if (ITinfo.insdata == NULL) {
		DisposePtr((Ptr)ITinfo.orders);
		DisposePtr((Ptr)ITinfo.parapins);
		DisposePtr((Ptr)ITinfo.parapsamp);
		DisposePtr((Ptr)ITinfo.parappat);
		
		return MADNeedMemory;
	}
	
	for (i = 0; i < ITinfo.insNum; i++) {
		theITCopy = (Byte*) theIT;
		theITCopy += ITinfo.parapins[i];
		
		BlockMoveData(theITCopy, &ITinfo.insdata[i], sizeof(ITInsForm));
		
		MOT32(&ITinfo.insdata[i].ID);
		
		if (ITinfo.insdata[i].ID != 'IMPI') {
			DisposePtr((Ptr)ITinfo.orders);
			DisposePtr((Ptr)ITinfo.parapins);
			DisposePtr((Ptr)ITinfo.parapsamp);
			DisposePtr((Ptr)ITinfo.parappat);
			DisposePtr((Ptr)ITinfo.insdata);
			
			return MADFileNotSupportedByThisPlug;
		}
		
#if 0
		if (ITinfo.insdata[i].insflags&1)
		{
			ITinfo.insdata[i].insloopbeg		= Tdecode32(&ITinfo.insdata[i].insloopbeg);
			ITinfo.insdata[i].insloopend		= Tdecode32(&ITinfo.insdata[i].insloopend);
		}
		else
		{
			ITinfo.insdata[i].insloopbeg		= 0;
			ITinfo.insdata[i].insloopend		= 0;
		}
		
		ITinfo.insdata[i].c2spd						= Tdecode32(&ITinfo.insdata[i].c2spd);
		ITinfo.insdata[i].insgvspos				= Tdecode16(&ITinfo.insdata[i].insgvspos);
		ITinfo.insdata[i].insint512				= Tdecode16(&ITinfo.insdata[i].insint512);
		ITinfo.insdata[i].insintlastused	= Tdecode32(&ITinfo.insdata[i].insintlastused);
		
		if (ITinfo.insdata[i].instype == 1 && ITinfo.insdata[i].inspack == 0 && ITinfo.insdata[i].inssig == 'SCRS')
		{
			long tempL;
			
			theITCopy = (Byte*) theIT;
			
			tempL = (((long)ITinfo.insdata[i].memsegh)<<16|ITinfo.insdata[i].memsegl)<<4;
			
			theITCopy += tempL;
			
			theInstrument[i] = (Ptr) theITCopy;
		}
		else
		{
			theInstrument[i] = 0L;
		}
#endif
	}
	
	/**** Samp Data ****/
	//if (ITinfo.insNum > 64) ITinfo.insNum = 64;
	ITinfo.sampdata = (ITSampForm *)MADPlugNewPtr(sizeof(ITSampForm) * ITinfo.smpNum, init);
	if (ITinfo.sampdata == NULL) {
		DisposePtr((Ptr)ITinfo.orders);
		DisposePtr((Ptr)ITinfo.parapins);
		DisposePtr((Ptr)ITinfo.parapsamp);
		DisposePtr((Ptr)ITinfo.parappat);
		DisposePtr((Ptr)ITinfo.insdata);
		
		return MADNeedMemory;
	}
	
	for (i = 0; i < ITinfo.smpNum; i++) {
		theITCopy = (Byte*) theIT;
		theITCopy += ITinfo.parapsamp[i];
		
		BlockMoveData(theITCopy, &ITinfo.sampdata[i], sizeof(ITSampForm));
		MOT32(&ITinfo.sampdata[i].ID);
		
		if (ITinfo.sampdata[i].ID != 'IMPS') //Debugger();
		{
			DisposePtr((Ptr)ITinfo.orders);
			DisposePtr((Ptr)ITinfo.parapins);
			DisposePtr((Ptr)ITinfo.parapsamp);
			DisposePtr((Ptr)ITinfo.parappat);
			DisposePtr((Ptr)ITinfo.insdata);
			DisposePtr((Ptr)ITinfo.sampdata);
			
			return MADFileNotSupportedByThisPlug;
		}
		
		INT32(&ITinfo.sampdata[i].length);
		INT32(&ITinfo.sampdata[i].loopBegin);
		INT32(&ITinfo.sampdata[i].loopEnd);
		INT32(&ITinfo.sampdata[i].C5Speed);
		INT32(&ITinfo.sampdata[i].SusLoopBegin);
		INT32(&ITinfo.sampdata[i].SusLoopEnd);
		INT32(&ITinfo.sampdata[i].samplePtr);
		
		theInstrument[i] = (Ptr) theIT;
		theInstrument[i] += ITinfo.sampdata[i].samplePtr;
	}
	
	
	// ******** Le IT a été lu et analysé ***********
	// ******** Copie des informations dans le MAD ***
	
	theMAD->header = (MADSpec*) MADPlugNewPtrClear(sizeof(MADSpec), init);
	if (theMAD->header == NULL) {
		DisposePtr((Ptr)ITinfo.orders);
		DisposePtr((Ptr)ITinfo.parapins);
		DisposePtr((Ptr)ITinfo.parapsamp);
		DisposePtr((Ptr)ITinfo.parappat);
		DisposePtr((Ptr)ITinfo.insdata);
		DisposePtr((Ptr)ITinfo.sampdata);
		
		return MADNeedMemory;
	}
	
	theMAD->header->MAD = 'MADK';
	for(i=0; i<32; i++) theMAD->header->name[i] = 0;
	for(i=0; i<28; i++) theMAD->header->name[i] = ITinfo.name[i];
	
	mystrcpy(theMAD->header->infos, "\pConverted by PlayerPRO DMF Plug (\xA9\x41ntoine ROSSET <rossetantoine@bluewin.ch>)");
	
	theMAD->header->numPat		= ITinfo.patNum;
	theMAD->header->numPointers	= ITinfo.orderNum;
	theMAD->header->speed		= ITinfo.iSpeed;
	theMAD->header->tempo		= ITinfo.iTempo;
	
	for (i = 0; i < 128; i++) theMAD->header->oPointers[i] = 0;
	for (i = 0; i < ITinfo.orderNum; i++) {
		theMAD->header->oPointers[i] = ITinfo.orders[i];
		
		if (theMAD->header->oPointers[i] < 0 || theMAD->header->oPointers[i] >= ITinfo.patNum) theMAD->header->oPointers[i] = 0;
	}
	
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
	
	theMAD->sets = (FXSets*)NewPtrClear(MAXTRACK * sizeof(FXSets));
	if (!theMAD->sets) {
		DisposePtr((Ptr)ITinfo.orders);
		DisposePtr((Ptr)ITinfo.parapins);
		DisposePtr((Ptr)ITinfo.parapsamp);
		DisposePtr((Ptr)ITinfo.parappat);
		DisposePtr((Ptr)ITinfo.insdata);
		DisposePtr((Ptr)ITinfo.sampdata);
		DisposePtr((Ptr)theMAD->header);
		
		return MADNeedMemory;
	}
	for (i = 0; i < MAXTRACK; i++) theMAD->header->chanBus[i].copyId = i;

	// ********************
	// ***** INSTRUMENTS *****
	// ********************
	
	theMAD->fid = (InstrData*)MADPlugNewPtr(sizeof(InstrData) * MAXINSTRU, init);
	if (!theMAD->fid) {
		DisposePtr((Ptr)ITinfo.orders);
		DisposePtr((Ptr)ITinfo.parapins);
		DisposePtr((Ptr)ITinfo.parapsamp);
		DisposePtr((Ptr)ITinfo.parappat);
		DisposePtr((Ptr)ITinfo.insdata);
		DisposePtr((Ptr)ITinfo.sampdata);
		DisposePtr((Ptr)theMAD->header);
		DisposePtr((Ptr)theMAD->sets);
		
		return MADNeedMemory;
	}
	
	theMAD->sample = (sData**)MADPlugNewPtrClear(sizeof(sData*) * MAXINSTRU * MAXSAMPLE, init);
	if (!theMAD->sample) {
		DisposePtr((Ptr)ITinfo.orders);
		DisposePtr((Ptr)ITinfo.parapins);
		DisposePtr((Ptr)ITinfo.parapsamp);
		DisposePtr((Ptr)ITinfo.parappat);
		DisposePtr((Ptr)ITinfo.insdata);
		DisposePtr((Ptr)ITinfo.sampdata);
		DisposePtr((Ptr)theMAD->header);
		DisposePtr((Ptr)theMAD->sets);
		DisposePtr((Ptr)theMAD->fid);
		
		return MADNeedMemory;
	}
	
	for (i = 0; i < MAXINSTRU; i++) theMAD->fid[i].firstSample = i * MAXSAMPLE;

	for(i  = 0 ; i < MAXINSTRU; i++) {
		for (x = 0; x < MAXSAMPLE; x++) theMAD->sample[i*MAXSAMPLE + x] = NULL;
		
		theMAD->fid[i].numSamples	= 0;
	}
	
	if (ITinfo.flags & 2) {
		// USE INSTRUMENTS
	} else {
		// USE SAMPLES AS INSTRUMENTS
		for(i=0; i<ITinfo.smpNum; i++) {
			InstrData *curIns = &theMAD->fid[i];
			
			curIns->type = 0;
			
			if (theInstrument[i] != NULL)
			{
				sData	*curData;
				
				curIns->numSamples	= 1;
				curIns->volFade			= DEFAULT_VOLFADE;
				
				curData = theMAD->sample[i*MAXSAMPLE + 0] = (sData*) MADPlugNewPtrClear(sizeof(sData), 1);
				if (curData == NULL) {
					for (i = 0; i < MAXSAMPLE * MAXINSTRU; i++) {
						if (theMAD->sample[i]) {
							if (theMAD->sample[i]->data) {
								DisposePtr(theMAD->sample[i]->data);
							}
							DisposePtr((Ptr)theMAD->sample[i]);
						}
					}
					
					DisposePtr((Ptr)ITinfo.orders);
					DisposePtr((Ptr)ITinfo.parapins);
					DisposePtr((Ptr)ITinfo.parapsamp);
					DisposePtr((Ptr)ITinfo.parappat);
					DisposePtr((Ptr)ITinfo.insdata);
					DisposePtr((Ptr)ITinfo.sampdata);
					
					DisposePtr((Ptr)theMAD->header);
					DisposePtr((Ptr)theMAD->sets);
					DisposePtr((Ptr)theMAD->fid);
					
					return MADNeedMemory;
				}
				
				curData->size			= ITinfo.sampdata[i].length;
				curData->loopBeg 	= ITinfo.sampdata[i].loopBegin;
				curData->loopSize	= ITinfo.sampdata[i].loopEnd - ITinfo.sampdata[i].loopBegin;
				curData->vol			= ITinfo.sampdata[i].GvL;
				curData->c2spd		= ITinfo.sampdata[i].C5Speed;
				curData->loopType	= 0;
				curData->amp			= 8;		if (ITinfo.sampdata[i].Flag&1)	curData->amp		= 16;
				
				curData->relNote	= 0;
				for (x = 0; x < 28; x++) theMAD->fid[i].name[x] = ITinfo.sampdata[i].DOSName[x];
				
				curData->data 		= MADPlugNewPtr(curData->size, init);
				if (curData->data == NULL) {
					for (i = 0; i < MAXSAMPLE * MAXINSTRU; i++) {
						if (theMAD->sample[i]) {
							if (theMAD->sample[i]->data) {
								DisposePtr(theMAD->sample[i]->data);
							}
							DisposePtr((Ptr)theMAD->sample[i]);
						}
					}
					
					DisposePtr((Ptr)ITinfo.orders);
					DisposePtr((Ptr)ITinfo.parapins);
					DisposePtr((Ptr)ITinfo.parapsamp);
					DisposePtr((Ptr)ITinfo.parappat);
					DisposePtr((Ptr)ITinfo.insdata);
					DisposePtr((Ptr)ITinfo.sampdata);
					
					DisposePtr((Ptr)theMAD->header);
					DisposePtr((Ptr)theMAD->sets);
					DisposePtr((Ptr)theMAD->fid);
					
					return MADNeedMemory;
				}
				
				if (curData->data != NULL)
				{
					BlockMoveData(theInstrument[i], curData->data, curData->size);
					
					/*if (ITinfo.ffv != 1)
					{
						long temp;
						
						for (temp = 0; temp < curData->size; temp++) *(curData->data + temp) -= 0x80;
					}*/
				}
			}
			else curIns->numSamples = 0;
		}
	}
	//	*********************
	//	*           Check MaxTrack         *
	//	*********************
	/*
	maxTrack = 0;
	for(i=0;i<32;i++)
	{
		if(ITinfo.chanset[i]<16) maxTrack++;
	}
	maxTrack++;
	maxTrack /= 2;
	maxTrack *= 2;
	*/
	// ********************
	// ***** TEMPORAIRE ******
	// ********************
	
	theMAD->header->numChn = 32;
	
	starting = 0;

	for (i = 0; i < MAXPATTERN; i++) theMAD->partition[i] = NULL;
	for (i = 0; i < theMAD->header->numPat ; i++) {
		ITPatForm *curITPat;
		
		curITPat = (ITPatForm*) (theIT + ITinfo.parappat[i]);
		
		INT16(&curITPat->length);
		INT16(&curITPat->row);
		
		theMAD->partition[i] = (PatData*) MADNewPtrClear(sizeof(PatHeader) + theMAD->header->numChn * curITPat->row * sizeof(Cmd), init);
		if (theMAD->partition[i] == NULL) {
			for (i = 0; i < MAXPATTERN; i++) {
				if (theMAD->partition[i]) {
					DisposePtr((Ptr)theMAD->partition[i]);
				}
			}
			for (i = 0; i < MAXSAMPLE * MAXINSTRU; i++) {
				if (theMAD->sample[i]) {
					if (theMAD->sample[i]->data) {
						DisposePtr(theMAD->sample[i]->data);
					}
					DisposePtr((Ptr)theMAD->sample[i]);
				}
			}
			
			DisposePtr((Ptr)ITinfo.orders);
			DisposePtr((Ptr)ITinfo.parapins);
			DisposePtr((Ptr)ITinfo.parapsamp);
			DisposePtr((Ptr)ITinfo.parappat);
			DisposePtr((Ptr)ITinfo.insdata);
			DisposePtr((Ptr)ITinfo.sampdata);
			
			DisposePtr((Ptr)theMAD->header);
			DisposePtr((Ptr)theMAD->sets);
			DisposePtr((Ptr)theMAD->fid);
			
			return MADNeedMemory;
		}
		
		theMAD->partition[i]->header.size		= curITPat->row;
		theMAD->partition[i]->header.compMode	= 'NONE';
		
		for (x = 0; x < 20; x++) theMAD->partition[i]->header.name[x] = 0;
		
		MaxPtr = (Ptr) theMAD->partition[i];
		MaxPtr += sizeof(PatHeader) + theMAD->header->numChn * curITPat->row * sizeof(Cmd);
		
		for (Row = 0; Row < curITPat->row; Row++) {
			for (z = 0; z < theMAD->header->numChn; z++) {
				aCmd = GetMADCommand(Row, z, theMAD->partition[i]);
				
				aCmd->note	= 0xFF;
				aCmd->ins	= 0;
				aCmd->cmd	= 0;
				aCmd->arg	= 0;
				aCmd->vol	= 0xFF;
			}
		}
		
		if (ITinfo.parappat[i] > 0) {
			Ptr		curDataPat = curITPat->data;
			Byte	maskvariable = 0;
			Boolean	NeedChannelToRead = true;
			
			Row = 0;
			while (Row < curITPat->row) {
				tempChar = *curDataPat;
				curDataPat++;
				
				if (tempChar == 0)
					Row++;
				else {
					if (NeedChannelToRead) {
						// Channel
						channel = (tempChar-1) & 63;
						if (channel >= 0 && channel < theMAD->header->numChn) aCmd = GetMADCommand(Row, channel, theMAD->partition[i]);
						else aCmd = NULL;
					}
					
					if(tempChar & 128) {
						maskvariable = *curDataPat;
						curDataPat++;
					}
					
					// NOTE
					if (maskvariable & 1 || maskvariable & 16)
					{
						if (aCmd != NULL)
						{
							aCmd->note = *curDataPat;
							if (aCmd->note == 255) aCmd->note = 0xFE;
							else if (aCmd->note < 0 || aCmd->note >= NUMBER_NOTES) aCmd->note = 0xFF;
						}
						curDataPat ++;
					}
					
					// INSTRUMENT
					if (maskvariable & 2 || maskvariable & 32)
					{
						if (aCmd != NULL)
						{
							aCmd->ins = 1 + *curDataPat;
						}
						curDataPat++;
					}
					
					// VOLUME
					if (maskvariable & 4 || maskvariable & 64)
					{
						if (aCmd != NULL)
						{
							aCmd->vol = *curDataPat;
							if (aCmd->vol > 64) aCmd->vol = 64;
							aCmd->vol += 0x10;
						}
						curDataPat++;
					}
					else aCmd->vol = 255;
					
					// PARAMETER
					if (tempChar & 8 || maskvariable & 128)
					{
						if (aCmd != NULL)
						{
							/*	if (theITCopy[0] != 255)
							 {
							 ConvertITEffect(theITCopy[0], theITCopy[1], &aCmd->cmd, &aCmd->arg);
							 }	*/
						}
						curDataPat += 2;
					}
				}
			}
		}
	}
	
	DisposePtr((Ptr) ITinfo.orders);			DisposePtr((Ptr) ITinfo.parapins);
	DisposePtr((Ptr) ITinfo.parappat);		DisposePtr((Ptr) ITinfo.insdata);
	DisposePtr((Ptr) ITinfo.parapsamp);

	return noErr;
}

static OSErr ExtractITInfo(PPInfoRec *info, Ptr AlienFile)
{
	//short		i, maxInstru, tracksNo;
	ITForm		ITinfo;
	/********************************/

	/**** Header principal *****/
	BlockMoveData(AlienFile, &ITinfo, 96);
	
	/*** Signature ***/
	
	info->signature = 'DMF ';
	
	/*** Internal name ***/
	
	//ITinfo.name[25] = '\0';
	strlcpy(info->internalFileName, ITinfo.name, sizeof(ITinfo.name));
	
	/*** Total Patterns ***/
	INT16(&ITinfo.patNum);
	info->totalPatterns = ITinfo.patNum;
	
	/*** Partition Length ***/
	INT16(&ITinfo.orderNum);
	info->partitionLength = ITinfo.orderNum;
	
	/*** Total Instruments ***/
	INT16(&ITinfo.insNum);
	info->totalInstruments = ITinfo.insNum;
	
	/*** Tracks ***/
	
	//info->totalTracks	 = Tdecode16(&ITinfo.insNum);
	
	mystrcpy(info->formatDescription, "\pDMF Plug");

	return noErr;
}

static inline OSErr TestITFile(void *AlienFile)
{
	ITForm *myIT = (ITForm*) AlienFile;
	OSType myID = myIT->ID;
	MOT32(&myID);

	if (myID == 'DDMF')
		return noErr;
	else
		return MADFileNotSupportedByThisPlug;
}

static OSErr mainDMF(OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
{
	OSErr	myErr = noErr;
	Ptr		AlienFile;
	UNFILE	iFileRefI;
	long	sndSize;
	
	switch (order) {
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
					if (AlienFile == NULL) myErr = MADNeedMemory;
					else
					{
						myErr = iRead(sndSize, AlienFile, iFileRefI);
						if (myErr == noErr)
						{
							myErr = TestITFile(AlienFile);
							if (myErr == noErr)
							{
								myErr = ConvertIT2Mad(AlienFile,  GetPtrSize(AlienFile), MadFile, init);
							}
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
				sndSize = 1024L;
				
				AlienFile = MADPlugNewPtr(sndSize, init);
				if (AlienFile == NULL) myErr = MADNeedMemory;
				else
				{
					myErr = iRead(sndSize, AlienFile, iFileRefI);
					if(myErr == noErr) myErr = TestITFile(AlienFile);
					
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
					myErr = iRead(sndSize, AlienFile,iFileRefI);
					if (myErr == noErr)
					{
						myErr = ExtractITInfo(info, AlienFile);
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

#define PLUGUUID (CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x45, 0xAE, 0x25, 0xED, 0x47, 0xCE, 0x44, 0xD8, 0xA3, 0x51, 0xE3, 0xCB, 0x1C, 0x48, 0xED, 0xA8))
//45AE25ED-47CE-44D8-A351-E3CB1C48EDA8

#define PLUGMAIN mainDMF
#define PLUGINFACTORY DMFFactory
#include "CFPlugin-bridge.c"
