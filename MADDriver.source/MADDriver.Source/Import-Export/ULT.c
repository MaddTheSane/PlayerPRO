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
#include "ULT.h"

#define LOW(para) ((para) & 15)
#define HI(para) ((para) >> 4)

#ifdef _MAC_H
#define Tdecode16(msg_buf) CFSwapInt16LittleToHost(*(short*)msg_buf)
#define Tdecode32(msg_buf) CFSwapInt32LittleToHost(*(int*)msg_buf)
#else
#ifdef __LITTLE_ENDIAN__
#define Tdecode16(msg_buf) *(short*)msg_buf
#define Tdecode32(msg_buf) *(int*)msg_buf
#else

static inline UInt32 Tdecode32(void *msg_buf)
{
	UInt32 toswap = *((UInt32*) msg_buf);
	INT32(&toswap);
	return toswap;
}

static inline UInt16 Tdecode16(void *msg_buf)
{
	UInt16 toswap = *((UInt16*) msg_buf);
	INT16(&toswap);
	return toswap;
}

#endif
#endif

Cmd* GetMADCommand(register short PosX, register short	TrackIdX, register PatData*	tempMusicPat)
{
	if (PosX < 0) PosX = 0;
	else if (PosX >= tempMusicPat->header.size) PosX = tempMusicPat->header.size -1;
		
	return(& (tempMusicPat->Cmds[(tempMusicPat->header.size * TrackIdX) + PosX]));
}

static inline void mystrcpy(Ptr a, BytePtr b)
{
	BlockMoveData(b + 1, a, b[0]);
}

static OSErr ConvertULT2Mad(Ptr theULT, long MODSize, MADMusic *theMAD, MADDriverSettings *init)
{
	long	i, x, z, Row;
	Ptr		MaxPtr;
	Ptr		theInstrument[64];
	Byte	*theULTCopy;
	
	/**** Variables pour le MAD ****/
	Cmd				*aCmd;
	
	/**** Variables pour le ULT ****/
	
	ULTForm			ULTinfo;
	ULTSuite		ULTSuite;
	/********************************/
	
	for (i = 0 ; i < 64; i ++)
	{
		theInstrument[i] = NULL;
	}
	
	/**** Header principal *****/
	theULTCopy = (Byte*) theULT;
	
	BlockMoveData(theULTCopy, &ULTinfo, sizeof(ULTinfo));
	
	//if (ULTinfo.reserved != 0) return MADFileNotSupportedByThisPlug;	// RES in v.1.4 see doc
	
	ULTSuite.NOS = *(theULTCopy + sizeof(ULTinfo) + ULTinfo.reserved * 32L);
	
	
	/**** Ins Num *****/
	if (sizeof(ULTIns) != 64) //DebugStr("\pULTIns != 64");
		return MADIncompatibleFile;
	ULTSuite.ins = (ULTIns*) NewPtrClear(ULTSuite.NOS * sizeof(ULTIns));
	BlockMoveData(theULTCopy + sizeof(ULTinfo) + ULTinfo.reserved * 32L + 1, ULTSuite.ins, ULTSuite.NOS * sizeof(ULTIns));
	
	/**** Copy last infos *****/
	BlockMoveData(theULTCopy + sizeof(ULTinfo) + ULTinfo.reserved * 32L + 1 + (ULTSuite.NOS * sizeof(ULTIns)), &ULTSuite.pattSeq, 256 + 2);
	
	// ******** Le ULT a été lu et analysé ***********
	// ******** Copie des informations dans le MAD ***
	
	theMAD->header = (MADSpec*) MADPlugNewPtrClear(sizeof(MADSpec), init);
	if (theMAD->header == NULL) return MADNeedMemory;
	
	theMAD->header->MAD = 'MADK';
	for(i=0; i<32; i++) theMAD->header->name[i] = 0;
	for(i=0; i<32; i++) theMAD->header->name[i] = ULTinfo.name[i];
	
	mystrcpy(theMAD->header->infos, "\pConverted by PlayerPRO ULT Plug (\xA9\x41ntoine ROSSET <rossetantoine@bluewin.ch>)");
	
	theMAD->header->numPat			= ULTSuite.NOP;
	theMAD->header->numPointers	= 1;					// CHANGE
	theMAD->header->speed				= 6;
	theMAD->header->tempo				= 125;
	
	theMAD->sets = (FXSets*) NewPtrClear(MAXTRACK * sizeof(FXSets));
	for (i = 0; i < MAXTRACK; i++) theMAD->header->chanBus[i].copyId = i;
	
	for(i=0; i<128; i++) theMAD->header->oPointers[i] = 0;
	for(i=0; i<128; i++)
	{
		theMAD->header->oPointers[i] = ULTSuite.pattSeq[i];
		
		if (theMAD->header->oPointers[i] < 0 || theMAD->header->oPointers[i] >= 128) theMAD->header->oPointers[i] = 0;
	}
	
	for (i = 0; i < MAXTRACK; i++)
	{
		if (i % 2 == 0) theMAD->header->chanPan[i] = MAX_PANNING/4;
		else theMAD->header->chanPan[i] = MAX_PANNING - MAX_PANNING/4;
		
		theMAD->header->chanVol[i] = MAX_VOLUME;
	}
	
	theMAD->header->generalVol		= 64;
	theMAD->header->generalSpeed	= 80;
	theMAD->header->generalPitch	= 80;
	
	// ********************
	// ***** INSTRUMENTS *****
	// ********************
	
	theMAD->fid = (InstrData*) MADPlugNewPtrClear(sizeof(InstrData) * (long) MAXINSTRU, init);
	if (!theMAD->fid) return MADNeedMemory;
	
	theMAD->sample = (sData**) MADPlugNewPtrClear(sizeof(sData*) * (long) MAXINSTRU * (long) MAXSAMPLE, init);
	if (!theMAD->sample) return MADNeedMemory;
	
	for (i = 0; i < MAXINSTRU; i++) theMAD->fid[i].firstSample = i * MAXSAMPLE;
	
	for(i  = 0 ; i < MAXINSTRU; i++)
	{
		for (x = 0; x < MAXSAMPLE; x++) theMAD->sample[i*MAXSAMPLE + x] = NULL;
		
		theMAD->fid[i].numSamples	= 0;
	}
	
	for(i=0; i<ULTSuite.NOS; i++)
	{
		InstrData		*curIns = &theMAD->fid[i];
		
		curIns->type	= 0;
		
		{
			sData	*curData;
			
			curIns->numSamples	= 1;
			curIns->volFade			= DEFAULT_VOLFADE;
			
			curData = theMAD->sample[i*MAXSAMPLE +  0] = (sData*) MADPlugNewPtrClear(sizeof(sData), init);
			if (curData == NULL) return MADNeedMemory;
			
			ULTSuite.ins[i].loopStart	= Tdecode32(&ULTSuite.ins[i].loopStart);
			ULTSuite.ins[i].loopEnd		= Tdecode32(&ULTSuite.ins[i].loopEnd);
			ULTSuite.ins[i].sizeStart	= Tdecode32(&ULTSuite.ins[i].sizeStart);
			ULTSuite.ins[i].sizeEnd		= Tdecode32(&ULTSuite.ins[i].sizeEnd);
			ULTSuite.ins[i].finetune	= Tdecode16(&ULTSuite.ins[i].finetune);
			
			curData->size			= ULTSuite.ins[i].sizeEnd - ULTSuite.ins[i].sizeStart;		// * 2 ???
			curData->loopBeg 	= ULTSuite.ins[i].loopStart;
			curData->loopSize	= ULTSuite.ins[i].loopEnd - ULTSuite.ins[i].loopStart;
			curData->vol			= ULTSuite.ins[i].volume;
			curData->c2spd		= ULTSuite.ins[i].finetune;
			curData->loopType	= 0;
			
			switch (ULTSuite.ins[i].Bidi)
			{
				case 4:
				case 12:
				case 28:
					curData->amp = 16;
					break;
					
				default:
					curData->amp			= 8;
					break;
					
			}
			
			
			curData->relNote	= 0;
			for (x = 0; x < 28; x++) theMAD->fid[i].name[x] = ULTSuite.ins[i].name[x];
			
			curData->data 		= MADPlugNewPtr(curData->size, init);
			if (curData->data == NULL) return MADNeedMemory;
			
			else
			{
				BlockMoveData(theULT + ULTSuite.ins[i].sizeStart, curData->data, curData->size);
			}
		}
		//else curIns->numSamples = 0;
	}
	
	theMAD->header->numChn = ULTSuite.NOC;
	
	for (i = 0; i < MAXPATTERN; i++) theMAD->partition[i] = NULL;
	for (i = 0; i < theMAD->header->numPat ; i++)
	{
		theMAD->partition[i] = (PatData*) MADPlugNewPtrClear(sizeof(PatHeader) + theMAD->header->numChn * 64 * sizeof(Cmd), init);
		if (theMAD->partition[i] == NULL) return MADNeedMemory;
		
		theMAD->partition[i]->header.size 			= 64;
		theMAD->partition[i]->header.compMode 	= 'NONE';
		
		for (x = 0; x < 20; x++) theMAD->partition[i]->header.name[x] = 0;
		
		MaxPtr = (Ptr) theMAD->partition[i];
		MaxPtr += sizeof(PatHeader) + theMAD->header->numChn * 64 * sizeof(Cmd);
		
		for (Row = 0; Row < 64; Row++)
		{
			for(z = 0; z < theMAD->header->numChn; z++)
			{
				aCmd = GetMADCommand(Row, z, theMAD->partition[i]);
				
				aCmd->note		= 0xFF;
				aCmd->ins			= 0;
				aCmd->cmd		= 0;
				aCmd->arg		= 0;
				aCmd->vol		= 0xFF;
			}
		}
	}
	
	DisposePtr((Ptr) ULTSuite.ins);
	
	return noErr;
}

static OSErr ExtractULTInfo(PPInfoRec *info, Ptr AlienFile)
{
	//short		i, maxInstru, tracksNo;
	ULTForm		ULTinfo;
	/********************************/
	
	/**** Header principal *****/
	BlockMoveData(AlienFile, &ULTinfo, 49);
	
	/*** Signature ***/
	
	info->signature = 'ULT ';
	
	/*** Internal name ***/
	
	strlcpy(info->internalFileName, ULTinfo.name, sizeof(ULTinfo.name));
	
	/*** Total Patterns ***/
	
	info->totalPatterns = 0;
	//Tdecode16(&ITinfo.patNum);
	
	/*** Partition Length ***/
	
	info->partitionLength = 0;
	//Tdecode16(&ITinfo.orderNum);
	
	/*** Total Instruments ***/
	
	info->totalInstruments = 0;
	
	/*** Tracks ***/
	
	info->totalTracks	 = 0;
	
	strcpy(info->formatDescription, "ULT Plug");

	return noErr;
}

static OSErr TestULTFile(Ptr AlienFile)
{
	ULTForm	*myULT = (ULTForm*) AlienFile;
	OSType ultID = *((OSType *) myULT->ID);
	MOT32(&ultID);
	
	if (ultID == 'MAS_') return noErr;
	else return MADFileNotSupportedByThisPlug;
}

static OSErr mainULT(OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
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
				AlienFile = MADPlugNewPtr(sndSize * 2, init);
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
							myErr = TestULTFile(AlienFile);
							if (myErr == noErr)
							{
								myErr = ConvertULT2Mad(AlienFile,  GetPtrSize(AlienFile), MadFile, init);
							}
						}
					}
					DisposePtr(AlienFile);	AlienFile = NULL;
				}
				iClose(iFileRefI);
			} else myErr = MADReadingErr;
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
					if (myErr == noErr)
						myErr = TestULTFile(AlienFile);
					
					DisposePtr(AlienFile);	AlienFile = NULL;
				}
				iClose(iFileRefI);
			} else myErr = MADReadingErr;
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
					myErr = iRead(sndSize, AlienFile, iFileRefI);
					if (myErr == noErr)
					{
						myErr = ExtractULTInfo(info, AlienFile);
					}
					DisposePtr(AlienFile);	AlienFile = NULL;
				}
				iClose(iFileRefI);
			} else myErr = MADReadingErr;
			break;
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
	return myErr;
}

#define PLUGUUID (CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xC6, 0x59, 0x34, 0xC3, 0x9B, 0x3B, 0x44, 0x84, 0xA0, 0xBF, 0xF0, 0x24, 0x44, 0xE4, 0xD3, 0xFD))
//C65934C3-9B3B-4484-A0BF-F02444E4D3FD

#define PLUGMAIN mainULT
#define PLUGINFACTORY ULTFactory
#include "CFPlugin-bridge.c"
