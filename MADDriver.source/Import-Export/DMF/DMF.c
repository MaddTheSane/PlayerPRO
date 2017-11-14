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

#if defined(__APPLE__) && !(defined(EMBEDPLUGS) && EMBEDPLUGS)
#include <PlayerPROCore/PlayerPROCore.h>
#else
#include "RDriver.h"
#include "MADFileUtils.h"
#endif
#include "DMF.h"

#if defined(EMBEDPLUGS) && EMBEDPLUGS
#include "embeddedPlugs.h"
#endif

#define LOW(para) ((para) & 15)
#define HI(para) ((para) >> 4)

#if 0
static void ConvertITEffect(MADByte B0, MADByte B1, MADByte *Cmd, MADByte *Arg)
{
	MADByte LoB1 = LOW(B1);
	MADByte HiB1 = HI(B1);
	
	switch (B0 + 0x40) {
			// Speed
		case 'A':
			*Cmd = MADEffectSpeed;
			*Arg = B1;
			break;
			// Tempo
		case 'T':
			*Cmd = MADEffectSpeed;
			*Arg = B1;
			break;
			
		case 'B':
			*Cmd = MADEffectFastSkip;
			*Arg = B1;
			break;
			
		case 'C':
			*Cmd = MADEffectSkip;
			*Arg = B1;
			break;
			
		case 'D':
			if (LoB1 == 0 || HiB1 == 0)		// Slide volume
			{
				*Cmd = MADEffectSlideVolume;		*Arg = B1;
			}
			else if (HiB1 == 0x0F)		// Fine Slide volume DOWN
			{
				*Cmd = MADEffectExtended;
				*Arg = 11 << 4;
				*Arg += LoB1;
			}
			else if (LoB1 == 0x0F)		// Fine Slide volume UP
			{
				*Cmd = MADEffectExtended;
				*Arg = 10 << 4;
				*Arg += HiB1;
			}
			break;
			
		case 'E':
			if (HiB1 == 0x0F) {
				// FineSlide DOWN
				*Cmd = MADEffectExtended;
				*Arg = 2 << 4; //not supported
				*Arg += LoB1;
			} else if (HiB1 == 0x0E) {
				// ExtraFineSlide DOWN
				//not supported
				*Cmd = 0;
				*Arg = 0;
			} else {
				// Slide DOWN
				*Cmd = MADEffectUpslide;
				*Arg = B1;
			}
			break;
			
		case 'F':
			if (HiB1 == 0x0F) {
				// FineSlide UP
				*Cmd = MADEffectExtended;
				*Arg = 1 << 4; //not supported
				*Arg += LoB1;
			} else if (HiB1 == 0x0E) {
				// ExtraFineSlide UP
				//not supported
				*Cmd = 0;
				*Arg = 0;
			} else {
				// Slide UP
				*Cmd = MADEffectDownslide;
				*Arg = B1;
			}
			break;
			
		case 'G':
			*Cmd = MADEffectPortamento;
			*Arg = B1;
			break;
			
		case 'H':
			*Cmd = MADEffectVibrato;
			*Arg = B1;
			break;
			
		case 'J':
			*Cmd = MADEffectArpeggio;
			*Arg = B1;
			break;
			
		case 'K':
			*Cmd = MADEffectVibratoSlide;
			*Arg = B1;
			break;
			
		case 'L':
			*Cmd = MADEffectPortaSlide;
			*Arg = B1;
			break;
			
		case 'O':
			*Cmd = MADEffectOffset;
			*Arg = B1;
			break;
			
		case 'S':		// Special Effects
			switch (HiB1) {
				case 2:
					// FineTune
					*Cmd = MADEffectExtended;
					*Arg = 5 << 4;
					*Arg += LoB1;
					break;
					
				case 3:
					// Set Vibrato WaveForm
					*Cmd = MADEffectExtended;
					*Arg = 4 << 4;
					*Arg += LoB1;
					break;
					
				case 4:
					// Set Tremolo WaveForm
					*Cmd = MADEffectExtended;
					*Arg = 7 << 4;
					*Arg += LoB1;
					break;
					
				case 0xB:
					// Loop pattern
					*Cmd = MADEffectExtended;
					*Arg = 6 << 4;
					*Arg += LoB1;
					break;
					
				case 0xC:
					// Cut sample
					*Cmd = MADEffectExtended;
					*Arg = 12 << 4;
					*Arg += LoB1;
					break;
					
				case 0xD:
					// Delay sample
					*Cmd = MADEffectExtended;
					*Arg = 13 << 4;
					*Arg += LoB1;
					break;
					
				case 0xE:
					// Delay pattern
					*Cmd = MADEffectExtended;
					*Arg = 14 << 4;
					*Arg += LoB1;
					break;
					
				default:
					*Cmd = 0;
					*Arg = 0;
					break;
			}
			break;
		default:
			*Cmd = 0;
			*Arg = 0;
			break;
	}
}

static void ConvertMADEffect(MADByte Cmd, MADByte Arg, MADByte *B0, MADByte *B1)
{
	*B0 = 0;
	*B1 = 0;
	
	switch (Cmd) {
		case MADEffectSpeed:
			*B0 = 'A' - 0x40;
			*B1 = Arg;
			break;
			
		case MADEffectFastSkip:
			*B0 = 'B' - 0x40;
			*B1 = Arg;
			break;
			
		case MADEffectSkip:
			*B0 = 'C' - 0x40;
			*B1 = Arg;
			break;
			
		case MADEffectSlideVolume:
			*B0 = 'D' - 0x40;
			*B1 = Arg;
			break;
			
		case MADEffectUpslide:
			*B0 = 'E' - 0x40;
			*B1 = Arg;
			break;
			
		case MADEffectDownslide:
			*B0 = 'F' - 0x40;
			*B1 = Arg;
			break;
			
		case MADEffectPortamento:
			*B0 = 'G' - 0x40;
			*B1 = Arg;
			break;
			
		case MADEffectVibrato:
			*B0 = 'H' - 0x40;
			*B1 = Arg;
			break;
			
		case MADEffectArpeggio:
			*B0 = 'J' - 0x40;
			*B1 = Arg;
			break;
			
		case MADEffectVibratoSlide:
			*B0 = 'K' - 0x40;
			*B1 = Arg;
			break;
			
		case MADEffectPortaSlide:
			*B0 = 'L' - 0x40;
			*B1 = Arg;
			break;
			
		case MADEffectOffset:
			*B0 = 'O' - 0x40;
			*B1 = Arg;
			break;
			
		case MADEffectExtended:
			switch((Arg & 0xF0) >> 4) {
				case 1:
					*B0 = 'F' - 0x40;
					*B1 = 0xf0 + (Arg & 0x0F);
					break;
				case 2:
					*B0 = 'E' - 0x40;
					*B1 = 0xf0 + (Arg & 0x0F);
					break;
					
				case 5:
					*B0 = 'S' - 0x40;
					*B1 = 0x20 + (Arg & 0x0F);
					break;
					
				case 4:
					*B0 = 'S' - 0x40;
					*B1 = 0x30 + (Arg & 0x0F);
					break;
					
				case 7:
					*B0 = 'S' - 0x40;
					*B1 = 0x40 + (Arg & 0x0F);
					break;
					
				case 6:
					*B0 = 'S' - 0x40;
					*B1 = 0xb0 + (Arg & 0x0F);
					break;
					
				case 12:
					*B0 = 'S' - 0x40;
					*B1 = 0xc0 + (Arg & 0x0F);
					break;
					
				case 13:
					*B0 = 'S' - 0x40;
					*B1 = 0xd0 + (Arg & 0x0F);
					break;
					
				case 14:
					*B0 = 'S' - 0x40;
					*B1 = 0xe0 + (Arg & 0x0F);
					break;
					
				case 11:
					*B0 = 'D' - 0x40;
					*B1 = 0xf0 + (Arg & 0x0F);
					break;
					
				case 10:
					*B0 = 'D' - 0x40;
					*B1 = 0x0f + ((Arg & 0x0F) << 4);
					break;
			}
			break;
	}
}
#endif

static MADErr ConvertIT2Mad(char* theIT, size_t MODSize, MADMusic *theMAD, MADDriverSettings *init)
{
	int		i, x, z, channel, Row;
	int		starting;
	char	*MaxPtr;
	char	*theInstrument[64];
	MADByte	tempChar, *theITCopy;
#if 0
	short	Note, Octave, maxTrack;
	short	ITperiod[12] = {1712,1616,1524,1440,1356,1280,1208,1140,1076,1016, 960, 907};
	int		note_st3period;
	int		note_amigaperiod;
	MADByte	*ChannelSettings;
#endif
	
	/**** Variables pour le MAD ****/
	Cmd				*aCmd = NULL;
	
	/**** Variables pour le IT ****/
	
	ITForm			ITinfo;
	/********************************/
	
	for (i = 0 ; i < 64; i ++) {
		theInstrument[i] = NULL;
	}
	
	/**** Header principal *****/
	theITCopy = (MADByte*) theIT;
	
	memcpy(&ITinfo, theITCopy, sizeof(ITinfo));
	theITCopy += 192;
	
	MADLE16(&ITinfo.orderNum);
	MADLE16(&ITinfo.insNum);
	MADLE16(&ITinfo.smpNum);
	MADLE16(&ITinfo.patNum);
	MADLE16(&ITinfo.flags);
	MADLE16(&ITinfo.cwtv);
	
	/**** Order Num *****/
	ITinfo.orders = (unsigned char *) malloc(ITinfo.orderNum);
	if (ITinfo.orders == NULL) return MADNeedMemory;
	memcpy(ITinfo.orders, theITCopy, ITinfo.orderNum);
	theITCopy += ITinfo.orderNum;
	
	/**** Ins Num *****/
	ITinfo.parapins = (int *)malloc(ITinfo.insNum * sizeof(int));
	if (ITinfo.parapins == NULL) {
		free(ITinfo.orders);
		
		return MADNeedMemory;
	}
	memcpy(ITinfo.parapins, theITCopy, ITinfo.insNum * sizeof(int));
	theITCopy += ITinfo.insNum * 4L;
	for (i = 0; i < ITinfo.insNum; i++) {
		MADLE32(&ITinfo.parapins[i]);
	}
	
	/**** Samp Num *****/
	ITinfo.parapsamp = (int*)malloc(ITinfo.smpNum * sizeof(int));
	if (ITinfo.parapsamp == NULL) {
		free(ITinfo.orders);
		free(ITinfo.parapins);
		
		return MADNeedMemory;
	}
	memcpy(ITinfo.parapsamp, theITCopy, ITinfo.smpNum * sizeof(int));
	theITCopy += ITinfo.smpNum * 4;
	for (i = 0; i < ITinfo.smpNum; i++) {
		MADLE32(&ITinfo.parapsamp[i]);
	}
	
	/**** Pat Num *****/
	ITinfo.parappat = (int *)malloc(ITinfo.patNum * sizeof(int));
	if (ITinfo.parappat == NULL) {
		free(ITinfo.orders);
		free(ITinfo.parapins);
		free(ITinfo.parapsamp);
		
		return MADNeedMemory;
	}
	memcpy(ITinfo.parappat, theITCopy, ITinfo.patNum * sizeof(int));
	theITCopy += ITinfo.patNum * 4L;
	for (i = 0; i < ITinfo.patNum; i++) {
		MADLE32(&ITinfo.parappat[i]);
	}
	
	/**** Ins Data ****/
	if (ITinfo.insNum > 64)
		ITinfo.insNum = 64;
	ITinfo.insdata = (ITInsForm *)malloc(sizeof(ITInsForm) * ITinfo.insNum);
	if (ITinfo.insdata == NULL) {
		free(ITinfo.orders);
		free(ITinfo.parapins);
		free(ITinfo.parapsamp);
		free(ITinfo.parappat);
		
		return MADNeedMemory;
	}
	for (i = 0; i < ITinfo.insNum; i++)
	{
		theITCopy = (MADByte*) theIT;
		theITCopy += ITinfo.parapins[i];
		
		memcpy(&ITinfo.insdata[i], theITCopy, sizeof(ITInsForm));
		MADBE32(&ITinfo.insdata[i].ID);
		if (ITinfo.insdata[i].ID != 'IMPI') {
			free(ITinfo.orders);
			free(ITinfo.parapins);
			free(ITinfo.parapsamp);
			free(ITinfo.parappat);
			free(ITinfo.insdata);
			
			return MADFileNotSupportedByThisPlug;
		}
		
#if 0
		if (ITinfo.insdata[i].insflags & 1) {
			MADLE32(&ITinfo.insdata[i].insloopbeg);
			MADLE32(&ITinfo.insdata[i].insloopend);
		} else {
			ITinfo.insdata[i].insloopbeg		= 0;
			ITinfo.insdata[i].insloopend		= 0;
		}
		
		MADLE32(&ITinfo.insdata[i].c2spd);
		MADLE16(&ITinfo.insdata[i].insgvspos);
		MADLE16(&ITinfo.insdata[i].insint512);
		MADLE32(&ITinfo.insdata[i].insintlastused);
		
		if (ITinfo.insdata[i].instype == 1 && ITinfo.insdata[i].inspack == 0 && ITinfo.insdata[i].inssig == 'SCRS')
		{
			long tempL;
			
			theITCopy = (MADByte*) theIT;
			
			tempL = (((long)ITinfo.insdata[i].memsegh)<<16|ITinfo.insdata[i].memsegl)<<4;
			
			theITCopy += tempL;
			
			theInstrument[i] = (char*) theITCopy;
		}
		else
		{
			theInstrument[i] = 0L;
		}
#endif
	}
	
	/**** Samp Data ****/
	//if (ITinfo.insNum > 64) ITinfo.insNum = 64;
	ITinfo.sampdata = (ITSampForm *)malloc(sizeof(ITSampForm) * ITinfo.smpNum);
	if (ITinfo.sampdata == NULL) {
		free(ITinfo.orders);
		free(ITinfo.parapins);
		free(ITinfo.parapsamp);
		free(ITinfo.parappat);
		free(ITinfo.insdata);
		
		return MADNeedMemory;
	}
	
	for (i = 0; i < ITinfo.smpNum; i++) {
		theITCopy = (MADByte*)theIT;
		theITCopy += ITinfo.parapsamp[i];
		
		memcpy(&ITinfo.sampdata[i], theITCopy, sizeof(ITSampForm));
		MADBE32(&ITinfo.sampdata[i].ID);
		if (ITinfo.sampdata[i].ID != 'IMPS')  {
			free(ITinfo.orders);
			free(ITinfo.parapins);
			free(ITinfo.parapsamp);
			free(ITinfo.parappat);
			free(ITinfo.insdata);
			free(ITinfo.sampdata);
			
			return MADFileNotSupportedByThisPlug;
		}
		
		MADLE32(&ITinfo.sampdata[i].length);
		MADLE32(&ITinfo.sampdata[i].loopBegin);
		MADLE32(&ITinfo.sampdata[i].loopEnd);
		MADLE32(&ITinfo.sampdata[i].C5Speed);
		MADLE32(&ITinfo.sampdata[i].SusLoopBegin);
		MADLE32(&ITinfo.sampdata[i].SusLoopEnd);
		MADLE32(&ITinfo.sampdata[i].samplePtr);
		
		theInstrument[i] = (char*)theIT;
		theInstrument[i] += ITinfo.sampdata[i].samplePtr;
	}
	
	
	// ******** Le IT a été lu et analysé ***********
	// ******** Copie des informations dans le MAD ***
	
	theMAD->header = (MADSpec*)calloc(sizeof(MADSpec), 1);
	if (theMAD->header == NULL) {
		free(ITinfo.orders);
		free(ITinfo.parapins);
		free(ITinfo.parapsamp);
		free(ITinfo.parappat);
		free(ITinfo.insdata);
		free(ITinfo.sampdata);
		
		return MADNeedMemory;
	}
	
	theMAD->header->MAD = 'MADK';
	for(i=0; i<32; i++) theMAD->header->name[i] = 0;
	for(i=0; i<28; i++) theMAD->header->name[i] = ITinfo.name[i];
	
	strcpy(theMAD->header->infos, "Converted by PlayerPRO DMF Plug (\251Antoine ROSSET <rossetantoine@bluewin.ch>)");
	
	theMAD->header->numPat		= ITinfo.patNum;
	theMAD->header->numPointers	= ITinfo.orderNum;
	theMAD->header->speed		= ITinfo.iSpeed;
	theMAD->header->tempo		= ITinfo.iTempo;
	
	for(i=0; i<128; i++) theMAD->header->oPointers[i] = 0;
	for(i=0; i<ITinfo.orderNum; i++)
	{
		theMAD->header->oPointers[i] = ITinfo.orders[i];
		
		if (theMAD->header->oPointers[i] < 0 || theMAD->header->oPointers[i] >= ITinfo.patNum)
			theMAD->header->oPointers[i] = 0;
	}
	
	for (i = 0; i < MAXTRACK; i++)
	{
		if (i % 2 == 0)
			theMAD->header->chanPan[i] = MAX_PANNING / 4;
		else
			theMAD->header->chanPan[i] = MAX_PANNING - MAX_PANNING / 4;
		
		theMAD->header->chanVol[i] = MAX_VOLUME;
	}
	
	theMAD->header->generalVol		= 64;
	theMAD->header->generalSpeed	= 80;
	theMAD->header->generalPitch	= 80;
	
	theMAD->sets = (FXSets*)calloc(sizeof(FXSets), MAXTRACK);
	if (!theMAD->sets) {
		free(ITinfo.orders);
		free(ITinfo.parapins);
		free(ITinfo.parapsamp);
		free(ITinfo.parappat);
		free(ITinfo.insdata);
		free(ITinfo.sampdata);
		free(theMAD->header);
		
		return MADNeedMemory;
	}
	for (i = 0; i < MAXTRACK; i++) theMAD->header->chanBus[i].copyId = i;
	
	// ********************
	// ***** INSTRUMENTS *****
	// ********************
	
	theMAD->fid = (InstrData*)calloc(sizeof(InstrData), MAXINSTRU);
	if (!theMAD->fid) {
		free(ITinfo.orders);
		free(ITinfo.parapins);
		free(ITinfo.parapsamp);
		free(ITinfo.parappat);
		free(ITinfo.insdata);
		free(ITinfo.sampdata);
		free(theMAD->header);
		free(theMAD->sets);
		
		return MADNeedMemory;
	}
	
	theMAD->sample = (sData**)calloc(sizeof(sData*), MAXINSTRU * MAXSAMPLE);
	if (!theMAD->sample) {
		free(ITinfo.orders);
		free(ITinfo.parapins);
		free(ITinfo.parapsamp);
		free(ITinfo.parappat);
		free(ITinfo.insdata);
		free(ITinfo.sampdata);
		free(theMAD->header);
		free(theMAD->sets);
		free(theMAD->fid);
		
		return MADNeedMemory;
	}
	
	
	for (i = 0 ; i < MAXINSTRU; i++) {
		theMAD->fid[i].firstSample = i * MAXSAMPLE;
		for (x = 0; x < MAXSAMPLE; x++)
			theMAD->sample[i * MAXSAMPLE + x] = NULL;
		
		theMAD->fid[i].numSamples	= 0;
	}
	
	
	
	if (ITinfo.flags & 2) {
		// USE INSTRUMENTS
	} else {
		// USE SAMPLES AS INSTRUMENTS
		for (i = 0; i < ITinfo.smpNum; i++) {
			InstrData *curIns = &theMAD->fid[i];
			
			curIns->type = 0;
			
			if (theInstrument[i] != NULL) {
				sData *curData;
				
				curIns->numSamples	= 1;
				curIns->volFade		= DEFAULT_VOLFADE;
				
				curData = theMAD->sample[i*MAXSAMPLE + 0] = (sData*) calloc(sizeof(sData), 1);
				if (curData == NULL) {
					for (i = 0; i < MAXSAMPLE * MAXINSTRU; i++) {
						if (theMAD->sample[i]) {
							if (theMAD->sample[i]->data) {
								free(theMAD->sample[i]->data);
							}
							free(theMAD->sample[i]);
						}
					}
					
					free(ITinfo.orders);
					free(ITinfo.parapins);
					free(ITinfo.parapsamp);
					free(ITinfo.parappat);
					free(ITinfo.insdata);
					free(ITinfo.sampdata);
					
					free(theMAD->header);
					free(theMAD->sets);
					free(theMAD->fid);
					
					return MADNeedMemory;
				}
				
				curData->size		= ITinfo.sampdata[i].length;
				curData->loopBeg	= ITinfo.sampdata[i].loopBegin;
				curData->loopSize	= ITinfo.sampdata[i].loopEnd - ITinfo.sampdata[i].loopBegin;
				curData->vol		= ITinfo.sampdata[i].GvL;
				curData->c2spd		= ITinfo.sampdata[i].C5Speed;
				curData->loopType	= 0;
				curData->amp		= 8;
				if (ITinfo.sampdata[i].Flag & 1)
					curData->amp	= 16;
				
				curData->realNote	= 0;
				for (x = 0; x < 28; x++) theMAD->fid[i].name[x] = ITinfo.sampdata[i].DOSName[x];
				
				curData->data 		= malloc(curData->size);
				if (curData->data == NULL) {
					for (i = 0; i < MAXSAMPLE * MAXINSTRU; i++) {
						if (theMAD->sample[i]) {
							if (theMAD->sample[i]->data) {
								free(theMAD->sample[i]->data);
							}
							free(theMAD->sample[i]);
						}
					}
					
					free(ITinfo.orders);
					free(ITinfo.parapins);
					free(ITinfo.parapsamp);
					free(ITinfo.parappat);
					free(ITinfo.insdata);
					free(ITinfo.sampdata);
					
					free(theMAD->header);
					free(theMAD->sets);
					free(theMAD->fid);
					
					return MADNeedMemory;
				}
				
				if (curData->data != NULL)
				{
					memcpy(curData->data, theInstrument[i], curData->size);
					
					/*if (ITinfo.ffv != 1)
					 {
					 long temp;
					 
					 for (temp = 0; temp < curData->size; temp++) *(curData->data + temp) -= 0x80;
					 }*/
				}
			} else
				curIns->numSamples = 0;
		}
	}
	//	*********************
	//	*           Check MaxTrack         *
	//	*********************
#if 0
	maxTrack = 0;
	for (i = 0; i < 32; i++) {
		if (ITinfo.chanset[i] < 16) maxTrack++;
	}
	maxTrack++;
	maxTrack /= 2;
	maxTrack *= 2;
#endif
	// ********************
	// ***** TEMPORAIRE ******
	// ********************
	
	theMAD->header->numChn = 32;
	
	starting = 0;
	
	memset(theMAD->partition, 0, sizeof(theMAD->partition));
	
	for (i = 0; i < theMAD->header->numPat ; i++) {
		ITPatForm *curITPat;
		
		curITPat = (ITPatForm*)(theIT + ITinfo.parappat[i]);
		
		MADLE16(&curITPat->length);
		MADLE16(&curITPat->row);
		
		theMAD->partition[i] = (PatData*)calloc(sizeof(PatHeader) + theMAD->header->numChn * curITPat->row * sizeof(Cmd), 1);
		if (theMAD->partition[i] == NULL) {
			for (i = 0; i < MAXPATTERN; i++) {
				if (theMAD->partition[i]) {
					free(theMAD->partition[i]);
				}
			}
			for (i = 0; i < MAXSAMPLE * MAXINSTRU; i++) {
				if (theMAD->sample[i]) {
					if (theMAD->sample[i]->data) {
						free(theMAD->sample[i]->data);
					}
					free(theMAD->sample[i]);
				}
			}
			
			free(ITinfo.orders);
			free(ITinfo.parapins);
			free(ITinfo.parapsamp);
			free(ITinfo.parappat);
			free(ITinfo.insdata);
			free(ITinfo.sampdata);
			
			free(theMAD->header);
			free(theMAD->sets);
			free(theMAD->fid);
			
			return MADNeedMemory;
		}
		
		theMAD->partition[i]->header.size		= curITPat->row;
		theMAD->partition[i]->header.compMode	= 'NONE';
		
		for (x = 0; x < 20; x++) theMAD->partition[i]->header.name[x] = 0;
		
		MaxPtr = (char*) theMAD->partition[i];
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
			char	*curDataPat = curITPat->data;
			MADByte	maskvariable = 0;
			bool	NeedChannelToRead = true;
			
			Row = 0;
			while(Row < curITPat->row) {
				tempChar = *curDataPat;
				curDataPat++;
				
				if (tempChar == 0) {
					Row++;
				} else {
					if (NeedChannelToRead) {
						// Channel
						channel = (tempChar-1) & 63;
						if (channel >= 0 && channel < theMAD->header->numChn)
							aCmd = GetMADCommand(Row, channel, theMAD->partition[i]);
						else
							aCmd = NULL;
					}
					
					if (tempChar & 128) {
						maskvariable = *curDataPat;
						curDataPat++;
					}
					
					// NOTE
					if (maskvariable & 1 || maskvariable & 16) {
						if (aCmd != NULL) {
							aCmd->note = *curDataPat;
							if (aCmd->note == 255)
								aCmd->note = 0xFE;
							else if (aCmd->note < 0 || aCmd->note >= NUMBER_NOTES)
								aCmd->note = 0xFF;
						}
						curDataPat ++;
					}
					
					// INSTRUMENT
					if (maskvariable & 2 || maskvariable & 32) {
						if (aCmd != NULL) {
							aCmd->ins = 1 + *curDataPat;
						}
						curDataPat++;
					}
					
					// VOLUME
					if (maskvariable & 4 || maskvariable & 64) {
						if (aCmd != NULL) {
							aCmd->vol = *curDataPat;
							if (aCmd->vol > 64)
								aCmd->vol = 64;
							aCmd->vol += 0x10;
						}
						curDataPat++;
					} else
						aCmd->vol = 255;
					
					// PARAMETER
					if (tempChar & 8 || maskvariable & 128) {
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
	
	free(ITinfo.orders);
	free(ITinfo.parapins);
	free(ITinfo.parappat);
	free(ITinfo.insdata);
	free(ITinfo.parapsamp);
	free(ITinfo.sampdata);
	
	return MADNoErr;
}

static MADErr ExtractITInfo(MADInfoRec *info, char* AlienFile)
{
	//short		i, maxInstru, tracksNo;
	ITForm		ITinfo;
	/********************************/
	
	/**** Header principal *****/
	memcpy(&ITinfo, AlienFile, 96);
	
	/*** Signature ***/
	
	info->signature = 'DMF ';
	
	/*** Internal name ***/
	
	ITinfo.name[25] = '\0';
	strcpy(info->internalFileName, ITinfo.name);
	
	/*** Total Patterns ***/
	MADLE16(&ITinfo.patNum);
	info->totalPatterns = ITinfo.patNum;
	
	/*** Partition Length ***/
	
	MADLE16(&ITinfo.orderNum);
	info->partitionLength = ITinfo.orderNum;
	
	/*** Total Instruments ***/
	
	MADLE16(&ITinfo.insNum);
	info->totalInstruments = ITinfo.insNum;
	
	/*** Tracks ***/
	
	//info->totalTracks	 = Tdecode16( &ITinfo.insNum);
	
	strcpy(info->formatDescription, "DMF Plug");
	
	return MADNoErr;
}

static inline MADErr TestITFile(void *AlienFile)
{
	if (memcmp(AlienFile, "DDMF", 4) == 0)
		return MADNoErr;
	else
		return MADFileNotSupportedByThisPlug;
}

#ifndef _MAC_H

EXP MADErr FillPlug(PlugInfo *p);
EXP MADErr PPImpExpMain(MADFourChar order, char* AlienFileName, MADMusic *MadFile, MADInfoRec *info, MADDriverSettings *init);

EXP MADErr FillPlug(PlugInfo *p)		// Function USED IN DLL - For PC & BeOS
{
	strcpy(p->type, 		"DMF ");
	strcpy(p->MenuName, 	"DMF Files");
	p->mode	=	MADPlugImport;
	p->version = 2 << 16 | 0 << 8 | 0;
	
	return MADNoErr;
}
#endif


#if defined(EMBEDPLUGS) && EMBEDPLUGS
MADErr mainDMF(MADFourChar order, char* AlienFileName, MADMusic *MadFile, MADInfoRec *info, MADDriverSettings *init)
#else
extern MADErr PPImpExpMain(MADFourChar order, char* AlienFileName, MADMusic *MadFile, MADInfoRec *info, MADDriverSettings *init)
#endif
{
	MADErr	myErr = MADNoErr;
	char*		AlienFile;
	UNFILE	iFileRefI;
	long	sndSize;
	
	switch (order) {
		case MADPlugImport:
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI) {
				sndSize = iGetEOF(iFileRefI);
				
				AlienFile = malloc(sndSize);
				if (AlienFile == NULL) {
					myErr = MADNeedMemory;
				} else {
					myErr = iRead(sndSize, AlienFile, iFileRefI);
					if (myErr == MADNoErr) {
						myErr = TestITFile(AlienFile);
						if (myErr == MADNoErr) {
							myErr = ConvertIT2Mad(AlienFile, sndSize, MadFile, init);
						}
					}
					free(AlienFile); AlienFile = NULL;
				}
				iClose(iFileRefI);
			} else
				myErr = MADReadingErr;
			break;
			
		case MADPlugTest:
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI) {
				sndSize = 1024;
				
				AlienFile = malloc(sndSize);
				if (AlienFile == NULL) {
					myErr = MADNeedMemory;
				} else {
					myErr = iRead(sndSize, AlienFile, iFileRefI);
					if(myErr == MADNoErr)
						myErr = TestITFile(AlienFile);
					
					free(AlienFile); AlienFile = NULL;
				}
				iClose(iFileRefI);
			} else
				myErr = MADReadingErr;
			break;
			
		case MADPlugInfo:
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI) {
				info->fileSize = iGetEOF(iFileRefI);
				
				// Read only 5000 first bytes for optimisation
				sndSize = 5000; 
				
				AlienFile = malloc(sndSize);
				if (AlienFile == NULL) {
					myErr = MADNeedMemory;
				} else {
					myErr = iRead(sndSize, AlienFile,iFileRefI);
					if (myErr == MADNoErr) {
						myErr = ExtractITInfo(info, AlienFile);
					}
					free(AlienFile); AlienFile = NULL;
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
