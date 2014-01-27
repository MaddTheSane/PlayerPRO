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
#include "Okta.h"

#ifdef _MAC_H
#define decode16(msg_buf) CFSwapInt16LittleToHost(*(UInt16*)msg_buf)
#define decode32(msg_buf) CFSwapInt32LittleToHost(*(UInt32*)msg_buf)
#else
#ifdef __LITTLE_ENDIAN__
#define decode16(msg_buf) *(UInt16*)msg_buf
#define decode32(msg_buf) *(UInt32*)msg_buf
#else
static inline UInt16 decode16 (void *msg_buf)
{
	UInt16 toswap = *((UInt16*) msg_buf);
	INT16(&toswap);
	return toswap;	
}

static inline UInt32 decode32 (void *msg_buf)
{
	UInt32 toswap = *((UInt32*) msg_buf);
	INT32(&toswap);
	return toswap;
}

#endif
#endif

Cmd* GetMADCommand(register short PosX, register short	TrackIdX, register PatData*	tempMusicPat)
{
	if (PosX < 0) PosX = 0;
	else if (PosX >= tempMusicPat->header.size) PosX = tempMusicPat->header.size -1;
		
	return(& (tempMusicPat->Cmds[ (tempMusicPat->header.size * TrackIdX) + PosX]));
}


static inline void mystrcpy(Ptr a, BytePtr b)
{
	BlockMoveData(b + 1, a, b[ 0]);
}

/*
short FoundNote(short Period)
{
short	NCount = 1;
	
	while (NCount < NUMBER_NOTES+1)
    {
    	if (Period >= MADpitchTable[ NCount]) return(NCount);
    	NCount++;
    }
    
    return 0;
}
*/

static OSErr ConvertOKTA2Mad(Ptr	theOkta, long MODSize, MADMusic *theMAD, MADDriverSettings *init)
{
	short 				i, PatMax, x, z, channel, TrueTracks;
	long 				sndSize, OffSetToSample, OldTicks, temp, starting;
	Ptr					MaxPtr, theOktaPos;
	OSErr				theErr;
	Ptr					theInstrument[ 120], destPtr;
	unsigned short		tempS;
	char				tempChar;
	
	
	/**** Variables pour le MAD ****/
	Cmd				*aCmd;
	
	/**** Variables pour le Okta ****/
	
	OktaHeader		*Okta;
	OktaInstru			*samps, *s, instru[ 120];
	OktaPattern 		*OktaCmd;
	sectheader		*aSect;
	long				SectLength;
	short			pbod_count, sbod_count;
	OSType OKTAHeader = 0;
	/********************************/
	
	for (i = 0 ; i < 64; i ++) theInstrument[ i] = NULL;
	
	theMAD->header = (MADSpec*) MADPlugNewPtrClear(sizeof(MADSpec), init);
	Okta = (struct OktaHeader*) MADPlugNewPtr(sizeof(struct OktaHeader), init);
	
	sbod_count = 0;
	pbod_count = 0;
	
	MaxPtr		= theOkta + MODSize;
	theOktaPos	= theOkta;
	
	OKTAHeader = (*(OSType*)theOkta);
	MOT32(&OKTAHeader);
	if (OKTAHeader != 'OKTA') //DebugStr("\pError in OKTA");
		return MADIncompatibleFile;
	
	theOktaPos += 8L;
	
	while (theOktaPos < MaxPtr)
	{
		aSect = (sectheader*) theOktaPos;
		aSect->length = decode32 (&aSect->length);
		
		theOktaPos += 8L;
		
		MOT32(&aSect->name);
		switch (aSect->name)
		{
			case 'CMOD':
				Okta->splitted[0] = theOktaPos[1];
				Okta->splitted[1] = theOktaPos[3];
				Okta->splitted[2] = theOktaPos[5];
				Okta->splitted[3] = theOktaPos[7];
				Okta->linesize = ((4 +Okta->splitted[0] + Okta->splitted[1]
								   + Okta->splitted[2] + Okta->splitted[3])
								  * 4);
				break;
				
			case 'SAMP':
				samps = (OktaInstru*) theOktaPos;
          		
				for (i = 0; i * sizeof(OktaInstru) < aSect->length; i++)
				{
					instru[i] = samps[i];
					
					instru[ i ].length = decode32 (&instru[ i ].length);
					instru[ i ].repeat = decode16 (&instru[ i ].repeat) * 2;
					instru[ i ].replen = decode16 (&instru[ i ].replen) * 2;
				}
				Okta->samp_count = i;
				break;
				
			case 'SPEE':
				Okta->speed = decode16(theOktaPos);
				break;
				
			case 'SLEN':
				Okta->slen = decode16(theOktaPos);
				break;
				
			case 'PLEN':
				Okta->plen = decode16(theOktaPos);
				break;
				
			case 'PATT':
				Okta->patt = (unsigned char*)theOktaPos;
				break;
				
			case 'PBOD':
				Okta->pbodlen[ pbod_count] = decode16 (theOktaPos);
				
				if (pbod_count == 0) theMAD->header->numChn = (aSect->length - 2L) / (Okta->pbodlen[ pbod_count] * 4L);
				else
				{
					if (theMAD->header->numChn != (aSect->length - 2L) / (Okta->pbodlen[ pbod_count] * 4L))
					{
						//DebugStr("\pNon-standard OKTA - numChn");
						return MADIncompatibleFile;
					}
				}
				
				Okta->pbod[ pbod_count++] = (theOktaPos + 2L);
				break;
				
			case 'SBOD':
				s = &instru[ sbod_count];
				
				if (Okta->splitted[0] || Okta->splitted[1] || Okta->splitted[2] || Okta->splitted[3])
				{
				}
				
				s->length = aSect->length;		//if (s->length < aSect->length)
				
				theInstrument[ sbod_count] = theOktaPos;
				
				if (s->replen == 0)
				{
				}
				else if (s->repeat != 0)
				{
				}
				else
				{
				}
				
				sbod_count++;
				break;
				
			default:
				//DebugStr("\pUnknow section");
				return MADIncompatibleFile;
				break;
		}
		theOktaPos += aSect->length;
	}
	
	/******** Le Okta a été lu et analysé ***********/
	/******** Copie des informations dans le MAD ***/
	
	theMAD->header->MAD = 'MADK';
	for(i=0; i<32; i++) theMAD->header->name[i] = 0;
	for(i=0; i<20; i++) theMAD->header->name[i] = Okta->magic[i];
	
	theMAD->header->numPat 		= pbod_count;
	theMAD->header->numPointers 	= pbod_count;
	theMAD->header->tempo 			= 125;
	theMAD->header->speed 			= Okta->speed;
	
	mystrcpy(theMAD->header->infos, "\pConverted by PlayerPRO OKTA Plug (\xA9\x41ntoine ROSSET <rossetantoine@bluewin.ch>)");
	
	for (i = 0;  i < 128; i++) theMAD->header->oPointers[ i] = 0;
	for (i = 0;  i < pbod_count; i++) theMAD->header->oPointers[ i] = Okta->patt[ i];
	
	for (i = 0; i < MAXTRACK; i++)
	{
		if (i % 2 == 0) theMAD->header->chanPan[ i] = MAX_PANNING/4;
		else theMAD->header->chanPan[ i] = MAX_PANNING - MAX_PANNING/4;
		
		theMAD->header->chanVol[ i] = MAX_VOLUME;
	}
	theMAD->header->generalVol		= 64;
	theMAD->header->generalSpeed	= 80;
	theMAD->header->generalPitch	= 80;
	
	theMAD->sets = (FXSets*) NewPtrClear(MAXTRACK * sizeof(FXSets));
	for (i = 0; i < MAXTRACK; i++) theMAD->header->chanBus[ i].copyId = i;
	
	// INSTRUMENTS
	
	theMAD->fid = (InstrData*) MADPlugNewPtrClear(sizeof(InstrData) * (long) MAXINSTRU, init);
	if (!theMAD->fid) return MADNeedMemory;
	
	theMAD->sample = (sData**) MADPlugNewPtrClear(sizeof(sData*) * (long) MAXINSTRU * (long) MAXSAMPLE, init);
	if (!theMAD->sample) return MADNeedMemory;
	
	for (i = 0; i < MAXINSTRU; i++) theMAD->fid[ i].firstSample = i * MAXSAMPLE;
	
	
	for(i=0; i< sbod_count; i++)
	{
		for (x = 0; x < 22; x++) theMAD->fid[i].name[x] = instru[i].name[ x];
		theMAD->fid[i].type = 0;
		
		if (instru[i].length > 0)
		{
			sData	*curData;
			
			theMAD->fid[i].numSamples = 1;
			theMAD->fid[i].volFade = DEFAULT_VOLFADE;
			
			curData = theMAD->sample[ i*MAXSAMPLE + 0] = (sData*) MADPlugNewPtrClear(sizeof(sData), init);
			
			curData->size		= instru[i].length;
			curData->loopBeg 	= instru[i].repeat;
			curData->loopSize 	= instru[i].replen;
			curData->vol		= instru[i].vol;
			curData->c2spd		= NOFINETUNE;
			curData->loopType	= 0;
			curData->amp		= 8;
			
			curData->relNote	= 0;
			
			curData->data 		= MADPlugNewPtr(curData->size, init);
			if (curData->data == NULL) return MADNeedMemory;
			
			BlockMoveData(theInstrument[i], curData->data, curData->size);
		}
		else theMAD->fid[ i].numSamples = 0;
	}
	
	//*** TEMPORAIRE *****
	
	TrueTracks = theMAD->header->numChn;
	
	theMAD->header->numChn /= 2;
	theMAD->header->numChn *= 2;
	if (theMAD->header->numChn != TrueTracks) theMAD->header->numChn += 2;
	
	for(i=0; i<MAXPATTERN;i++) theMAD->partition[ i] = NULL;
	
	for(i=0; i<theMAD->header->numPat; i++)
	{
		theMAD->partition[ i] = (PatData*) MADPlugNewPtrClear(sizeof(PatHeader) + theMAD->header->numChn * Okta->pbodlen[ i] * sizeof(Cmd), init);
		
		theMAD->partition[ i]->header.size = Okta->pbodlen[ i];
		theMAD->partition[ i]->header.compMode = 'NONE';
		for (x = 0; x < 20; x++) theMAD->partition[ i]->header.name[ x] = 0;
		theMAD->partition[ i]->header.patBytes = 0;
		theMAD->partition[ i]->header.unused2 = 0;
		
		MaxPtr = (Ptr) theMAD->partition[ i];
		MaxPtr += sizeof(PatHeader) + theMAD->header->numChn * Okta->pbodlen[ i] * sizeof(Cmd);
		
		for (x = 0; x < Okta->pbodlen[ i]; x++)
		{
			for(z=0; z<theMAD->header->numChn; z++)
			{
				aCmd = GetMADCommand(x, z, theMAD->partition[ i]);
				if ((Ptr) aCmd >= MaxPtr) //Debugger();
					return MADIncompatibleFile;
				aCmd->note		= 0xFF;
				aCmd->ins			= 0;
				aCmd->cmd		= 0;
				aCmd->arg		= 0;
				aCmd->vol		= 0xFF;
				
				if (z < TrueTracks)
				{
					OktaCmd = (OktaPattern*) (Okta->pbod[i] + x * TrueTracks * 4L + 4L*z);
					
					if (OktaCmd->b1 > 0)
					{
						aCmd->note = 	OktaCmd->b1 - 1;
						aCmd->note += 24;
						
						if (aCmd->note < 0 || aCmd->note >= NUMBER_NOTES) aCmd->note = 0xFF;
						
						aCmd->ins  = OktaCmd->b2 + 1;
					}
					
					switch (OktaCmd->b3)
					{
						case 31:
							if (OktaCmd->b4 <= 0x40)
							{
								aCmd->vol = OktaCmd->b4 + 0x10;
							}
							break;
							
						case 25:
							OktaCmd->b4 &= 0xf;
							if(	OktaCmd->b4 != 0)
							{
								aCmd->cmd = speedE;
								aCmd->arg = OktaCmd->b4;
							}
							break;
					}
					
					/*	if (i == 0 && x == 0 && z == 0)
					 {
					 aCmd->cmd		= speedE;
					 aCmd->arg		= Okta->speed;
					 }*/
				}
			}
		}
	}
	
	DisposePtr((Ptr) Okta);
	
	return noErr;
}

static OSErr ExtractOKTAInfo(PPInfoRec *info, Ptr theOkta, long MODSize)
{
	//long		PatternSize;
	//short	i;
	//short	maxInstru;
	//short	tracksNo;
	
	/*** Signature ***/
	
	info->signature = 'OKTA';
	
	/*** Internal name ***/
	
	mystrcpy(info->internalFileName, "\p");
	
	{
		//OktaInstru			*samps, *s, instru[ 120];
		//OktaPattern 		*OktaCmd;
		sectheader		*aSect;
		//long				SectLength;
		short			pbod_count, sbod_count;
		OSType OKTAHead = 0;
		
		//short 				i, PatMax, x, z, channel, TrueTracks;
		//long 					sndSize, OffSetToSample, OldTicks, temp, starting;
		Ptr					MaxPtr, theOktaPos;
		//OSErr				theErr;
		//Ptr					theInstrument[ 120], destPtr;
		//unsigned	short		tempS;
		//char					tempChar;
		
		
		sbod_count = 0;	pbod_count = 0;
		
		MaxPtr		= theOkta + MODSize;
		theOktaPos	= theOkta;
		
		OKTAHead = (*(uint32_t*)theOkta);
		MOT32(&OKTAHead);
		
		if (OKTAHead != 'OKTA') //DebugStr("\pError in OKTA");
			return MADIncompatibleFile;
		
		theOktaPos += 8L;
		
		while (theOktaPos < MaxPtr)
		{
			aSect = (sectheader*) theOktaPos;
			aSect->length = decode32 (&aSect->length);
			
			theOktaPos += 8L;
			MOT32(&aSect->name);
			
			switch (aSect->name)
			{
				case 'PBOD':
					pbod_count++;
					break;
					
				case 'SBOD':
					sbod_count++;
					break;
			}
			theOktaPos += aSect->length;
		}
		info->totalPatterns = info->partitionLength = pbod_count;
		info->totalInstruments = sbod_count;
	}
	
	strcpy(info->formatDescription, "OKTA Plug");
	
	return noErr;
}

static OSErr TestOKTAFile(Ptr AlienFile)
{
	OSType myOKTA = *((OSType*) AlienFile);
	MOT32(&myOKTA);

	if (myOKTA == 'OKTA') return noErr;
	else return  MADFileNotSupportedByThisPlug;
}

static OSErr mainOKTA(OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
{
	OSErr	myErr = noErr;
	Ptr		AlienFile;
	long	sndSize;
	UNFILE	iFileRefI;
	
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
					myErr = iRead(sndSize, AlienFile, iFileRefI);
					if (myErr == noErr)
					{
						myErr = TestOKTAFile(AlienFile);
						if (myErr == noErr)
						{
							myErr = ConvertOKTA2Mad(AlienFile,  GetPtrSize(AlienFile), MadFile, init);
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
					if(myErr == noErr) myErr = TestOKTAFile(AlienFile);
					
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
				sndSize = info->fileSize;
				AlienFile = MADPlugNewPtr(sndSize, init);
				if (AlienFile == NULL) myErr = MADNeedMemory;
				else
				{
					myErr = iRead(sndSize, AlienFile, iFileRefI);
					if (myErr == noErr)
					{
						myErr = ExtractOKTAInfo(info, AlienFile, sndSize);
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

#define PLUGUUID (CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x08, 0xD9, 0x62, 0x6A, 0xF3, 0xD9, 0x45, 0x36, 0xA2, 0x9B, 0x78, 0x22, 0x2D, 0xFD, 0x8F, 0x32))
//08D9626A-F3D9-4536-A29B-78222DFD8F32

#define PLUGMAIN mainOKTA
#define PLUGINFACTORY OKTAFactory
#include "CFPlugin-bridge.c"
