/*	XI				*/
/*  IMPORT/EXPORT	*/
/*	v 1.0			*/
/*	1996 by ANR		*/

#include <PlayerPROCore/PlayerPROCore.h>
#include <Carbon/Carbon.h>
#include "XM.h"

static OSErr TestXI(void *CC)
{
	OSType Ident = *((OSType*)CC);
	MOT32(&Ident);
	
	if (Ident == 'Exte')
		return noErr;
	else
		return MADFileNotSupportedByThisPlug;
}

static OSErr MAD2KillInstrument(InstrData *curIns, sData **sample)
{
	short			i;
	//Boolean			IsReading;

	for (i = 0; i < curIns->numSamples; i++)
	{
		if (sample[i] != NULL)
		{
			if (sample[i]->data != NULL)
			{
				DisposePtr((Ptr) sample[i]->data);
				sample[i]->data = NULL;
			}
			DisposePtr((Ptr) sample[i]);
			sample[i] = NULL;
		}
	}
	
	memset(curIns->name, 0, sizeof(curIns->name));
	curIns->type		= 0;
	curIns->numSamples	= 0;
	
	/**/
	memset(curIns->what, 0, sizeof(curIns->what));
	memset(curIns->volEnv, 0, sizeof(curIns->volEnv));
	memset(curIns->pannEnv, 0, sizeof(curIns->pannEnv));
	memset(curIns->pitchEnv, 0, sizeof(curIns->pitchEnv));
	curIns->volSize		= 0;
	curIns->pannSize	= 0;
	
	curIns->volSus		= 0;
	curIns->volBeg		= 0;
	curIns->volEnd		= 0;
	
	curIns->pannSus		= 0;
	curIns->pannBeg		= 0;
	curIns->pannEnd		= 0;

	curIns->volType		= 0;
	curIns->pannType	= 0;
	
	curIns->volFade		= 0;
	curIns->vibDepth	= 0;
	curIns->vibRate		= 0;
	
	return noErr;
}

static OSErr mainXI(OSType		order,				// Order to execute
					InstrData	*InsHeader,			// Ptr on instrument header
					sData		**sample,			// Ptr on samples data
					short		*sampleID,			// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
													// If sampleID == -1 : add sample else replace selected sample.
					FSSpec		*AlienFileFSSpec,	// IN/OUT file
					PPInfoPlug	*thePPInfoPlug)
{
	OSErr	myErr;
	UNFILE	iFileRefI;
	short	x;
	long	inOutCount;
		
	switch (order) {
		case 'IMPL':
		{
			Ptr				theXI;
			XMPATCHHEADER	*pth;
			XMWAVHEADER		*wh;
			short			numSamples;
			
			myErr = FSpOpenDF(AlienFileFSSpec, fsCurPerm, &iFileRefI);
			if (myErr == noErr) {
				GetEOF(iFileRefI, &inOutCount);
				
				theXI = NewPtr(inOutCount);
				if (theXI == NULL)
					myErr = MADNeedMemory;
				else {
					MAD2KillInstrument(InsHeader, sample);
					
					for (x = 0; x < 32; x++) {
						if (x < AlienFileFSSpec->name[0])
							InsHeader->name[x] = AlienFileFSSpec->name[x + 1];
						else
							InsHeader->name[x] = '\0';
					}
					
					GetEOF(iFileRefI, &inOutCount);
					
					myErr = FSRead(iFileRefI, &inOutCount, theXI);
					
					// READ instrument header
					
					pth = (XMPATCHHEADER*)(theXI + 0x42);
					
					numSamples = *((short*)(theXI + 0x42 + sizeof(XMPATCHHEADER)));
					INT16(&numSamples);
					
					INT16(&pth->volfade);
					
					BlockMoveData(pth->what, 	InsHeader->what, 	96);
					BlockMoveData(pth->volenv,	InsHeader->volEnv, 	48);
					
					InsHeader->volSize	= pth->volpts;
					InsHeader->volType	= pth->volflg;
					InsHeader->volSus	= pth->volsus;
					InsHeader->volBeg	= pth->volbeg;
					InsHeader->volEnd	= pth->volend;
					InsHeader->volFade	= pth->volfade;
					
					BlockMoveData(pth->panenv, InsHeader->pannEnv, 	48);
					for (x = 0; x < 12; x++) {
						INT16(&InsHeader->volEnv[x].pos);
						INT16(&InsHeader->volEnv[x].val);

						INT16(&InsHeader->pannEnv[x].pos);
						INT16(&InsHeader->pannEnv[x].val);
					}
					
					InsHeader->pannSize	= pth->panpts;
					InsHeader->pannType	= pth->panflg;
					InsHeader->pannSus	= pth->pansus;
					InsHeader->pannBeg	= pth->panbeg;
					InsHeader->pannEnd	= pth->panend;
					
					// Read SAMPLE HEADERS
					
					for (x = 0; x < InsHeader->numSamples; x++) {
						sData	*curData;
						long	i;
						long 	finetune[16] = {
							7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280,
							8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757
						};
						
						wh = (XMWAVHEADER*)(theXI + 0x42 + 0x02 + sizeof(XMPATCHHEADER) + x * sizeof(XMWAVHEADER));
						
						INT32(&wh->length);
						INT32(&wh->loopstart);
						INT32(&wh->looplength);
						
						///////////
						
						curData = sample[x] = inMADCreateSample();
						
						curData->size		= wh->length;
						curData->loopBeg 	= wh->loopstart;
						curData->loopSize 	= wh->looplength;
						
						curData->vol		= wh->volume;
						curData->c2spd		= finetune[(wh->finetune + 128)/16];
						curData->loopType	= 0;
						curData->amp		= 8;
						if (wh->type & 0x10) {		// 16 Bits
							curData->amp = 16;
						}
						
						if (!(wh->type & 0x3)) {
							curData->loopBeg = 0;
							curData->loopSize = 0;
						}
						
						//curData->panning	= wh->panning;
						curData->relNote	= wh->relnote;
						for (i = 0; i < 22; i++) curData->name[i] = wh->samplename[i];
					}
					
					// Read SAMPLE DATA
					{
						Ptr reader = (Ptr)wh;
						
						reader += sizeof(XMWAVHEADER);
						
						for (x = 0; x < InsHeader->numSamples; x++) {
							sData *curData = sample[x];
							
							curData->data = NewPtr(curData->size);
							if (curData->data != NULL) {
								BlockMoveData(reader, curData->data, curData->size);
								
								if (curData->amp == 16) {
									short	*tt;
									long	tL;
									
									tt = (short*)curData->data;

									for (tL = 0; tL < curData->size/2; tL++) {
										INT16((Ptr)(tt + tL));
									}
									
									{
									/* Delta to Real */
									long	oldV, newV;
									long	xL;
									
									oldV = 0;
									
									for (xL = 0; xL < curData->size/2; xL++) {
										newV = tt[xL] + oldV;
										tt[xL] = newV;
										oldV = newV;
									}
									}
								} else {
									/* Delta to Real */
									long	oldV, newV;
									long	xL;
									
									oldV = 0;

									for (xL = 0; xL < curData->size; xL++) {
										newV = curData->data[xL] + oldV;
										curData->data[xL] = newV;
										oldV = newV;
									}
								}
							}
							
							reader += curData->size;
						}
					}
				}
				
				FSCloseFork(iFileRefI);
			}
		}
		break;
		
		case 'TEST':
		{
			Ptr	theSound;
			
			myErr = FSpOpenDF(AlienFileFSSpec, fsCurPerm, &iFileRefI);
			if (myErr == noErr) {
				inOutCount = 50;
				theSound = NewPtr(inOutCount);
				if (theSound == NULL)
					myErr = MADNeedMemory;
				else {
					FSRead(iFileRefI, &inOutCount, theSound);
					
					myErr = TestXI(theSound);
				}
				
				DisposePtr(theSound);
				
				FSCloseFork(iFileRefI);
			}
		}
		break;
		
		case 'EXPL':
			myErr = FSpCreate(AlienFileFSSpec, 'SNPL', 'XI  ', smCurrentScript);
			if(myErr == noErr)
				myErr = FSpOpenDF(AlienFileFSSpec, fsCurPerm, &iFileRefI);
			
			if (myErr == noErr) {
				// Write instrument header
				
				short			u, i, x;
				long			inOutCount = 0;
				//long			ihsizecopy, ihssizecopy;
				XMPATCHHEADER	pth;
				char			start[0x42];
				
				BlockMoveData("Extended Instrument:                       \xA1\x46\x61stTracker v2.00   \x02\x01", start, 0x42);
				
				inOutCount = 0x42;
				FSWriteFork(iFileRefI, fsAtMark, 0, inOutCount, start, NULL);
				
				BlockMoveData(InsHeader->what, pth.what, 96);
				BlockMoveData(InsHeader->volEnv, pth.volenv, 48);
				for (x = 0; x < 24; x++) {
					INT16(&pth.volenv[x]);
				}
				
				pth.volpts = InsHeader->volSize;
				pth.volflg = InsHeader->volType;
				pth.volsus = InsHeader->volSus;
				pth.volbeg = InsHeader->volBeg;
				pth.volend = InsHeader->volEnd;
				pth.volfade = InsHeader->volFade;
				INT16(&pth.volfade);
				
				BlockMoveData(InsHeader->pannEnv, pth.panenv, 48);
				for (x = 0; x < 24; x++) {
					INT16(&pth.panenv[x]);
				}
				
				pth.panpts = InsHeader->pannSize;
				pth.panflg = InsHeader->pannType;
				pth.pansus = InsHeader->pannSus;
				pth.panbeg = InsHeader->pannBeg;
				pth.panend = InsHeader->pannEnd;
				
				inOutCount = sizeof(XMPATCHHEADER);
				FSWriteFork(iFileRefI, fsAtMark, 0, inOutCount, &pth, NULL);
				
				inOutCount = 2;
				x = InsHeader->numSamples;
				INT16(&x);
				FSWriteFork(iFileRefI, fsAtMark, 0, inOutCount, &x, NULL);
				
				/** WRITE samples */
				
				for (u = 0 ; u < InsHeader->numSamples ; u++) {
					XMWAVHEADER		wh;
					sData			*curData;
					long 	finetune[16] = {
						7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280,
						8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757
					};
					
					curData = sample[u];
					
					if (curData->stereo)
						wh.length = curData->size / 2;
					else
						wh.length = curData->size;
					
					inOutCount += wh.length;
					
					wh.loopstart	= curData->loopBeg;
					wh.looplength	= curData->loopSize;
					
					if (curData->stereo) {
						wh.loopstart /= 2;
						wh.looplength /= 2;
					}
					
					wh.volume = curData->vol;
					
					wh.finetune = -128;
					if (curData->c2spd > 8757)
						wh.finetune = 127;
					else {
						while (finetune[(wh.finetune + 128) / 16] < curData->c2spd) {
							wh.finetune += 16;
						}
					}
					wh.type = 0;
					if (curData->amp == 16)
						wh.type |= 0x10;
					if (curData->loopSize > 0)
						wh.type |= 0x3;
					
					wh.relnote = curData->relNote;
					for (x = 0; x < 22; x++) wh.samplename[x] = curData->name[x];
					
					INT32(&wh.length);
					INT32(&wh.loopstart);
					INT32(&wh.looplength);
					
					inOutCount = sizeof(wh);
					FSWriteFork(iFileRefI, fsAtMark, 0, inOutCount, &wh, NULL);
				}
				
				for (u = 0; u < InsHeader->numSamples; u++) {
					sData 	*curData = sample[u];
					Ptr		tempPtr;
					long	dstSize;
					
					tempPtr = NewPtr(curData->size);
						
					/// WriteData
					if (tempPtr != NULL) {
						BlockMoveData(curData->data, tempPtr, curData->size);
						
						dstSize = curData->size;
						
						if (curData->amp == 16) {
							short	*tt = (short*) tempPtr;
							long	tL;
							
							/* Real to Delta */
							long	oldV, newV;
							long	xL;
							
							if (curData->stereo) {
								for (i = 0; i < dstSize; i++) tt[i] = tt[i*2];
								dstSize /= 2;
							}
							
							oldV = 0;
							
							for (xL = 0; xL < dstSize / 2; xL++) {
								newV = tt[xL];
								tt[xL] -= oldV;
								oldV = newV;
							}
							
							for (tL = 0; tL < dstSize / 2; tL++) {
								INT16((Ptr) (tt + tL));
							}
						} else {
							/* Real to Delta */
							long	oldV, newV;
							long	xL;
							Ptr		tt = (Ptr) tempPtr;
							
							if (curData->stereo) {
								for (i = 0; i < dstSize; i++) tt[i] = tt[i * 2];
								dstSize /= 2;
							}
							
							oldV = 0;
							
							for (xL = 0; xL < dstSize; xL++) {
								newV = tt[xL];
								tt[xL] -= oldV;
								oldV = newV;
							}
						}
						
						inOutCount = dstSize;
						FSWriteFork(iFileRefI, fsAtMark, 0, inOutCount, tempPtr, NULL);
						
						DisposePtr(tempPtr);
					}
				}
				
				FSCloseFork(iFileRefI);
			}
		break;
		
		default:
			myErr = MADOrderNotImplemented;
		break;
	}
		
	return myErr;
}

//620875F5-1E38-45EF-9FBA-AAE929502D63
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x62, 0x08, 0x75, 0xF5, 0x1E, 0x38, 0x45, 0xEF, 0x9F, 0xBA, 0xAA, 0xE9, 0x29, 0x50, 0x2D, 0x63)
#define PLUGINFACTORY XIFactory
#define PLUGMAIN mainXI

#include "CFPlugin-InstrBridge.c"
