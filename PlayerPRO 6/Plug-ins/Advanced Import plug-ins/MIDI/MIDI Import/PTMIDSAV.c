/*
 * ptmidsav.c: MOD-format saver module for ptmid. Takes a structure
 * representing a tune and creates a file which contains that tune.
 *
 * Author: Andrew Scott  (c)opyright 1994
 *
 * Date: 17/11/1993 ver 0.0
 *       8/1/1994   ver 0.1
 *       11/2/1994  ver 0.2
 */

//#include <cmath>
#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/MADPlug.h>
#include <Carbon/Carbon.h>
#define MADPlugNewPtrClear(x, y) calloc(x, 1)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>

#include <errno.h>
#include "PTMID.h"
#include "PTMIDZAP.h"

void Erreur(short a, short b)
{
	char errStr[256] = {0};
	snprintf(errStr, sizeof(errStr), "Error called with values %d, %d", a, b);
	MADDebugStr(__LINE__, __FILE__, errStr);
}

#if 0
sData *MADCreateSample(MADMusic *MDriver, short ins, short sample)
{
	sData *curData;
	
	curData = (sData*)NewPtrClear(sizeof(sData));
	if (curData) {
		curData->size		= 0;
		curData->loopBeg	= 0;
		curData->loopSize	= 0;
		curData->vol		= MAX_VOLUME;
		curData->c2spd		= NOFINETUNE;
		curData->loopType	= 0;
		curData->amp		= 8;
		
		curData->relNote	= 0;
		//curData->name
		curData->data		= NULL;
		
		// Install it
		
		MDriver->sample[ins * MAXSAMPLE + sample] = curData;
		MDriver->fid[ins].numSamples++;
	}
	
	return curData;
}

Handle DoExp1to3( Handle sound, unsigned long numSampleFrames)
{
	long	i;
	Ptr		inState, outState;
	Handle	outBuffer;
	
	outState = NewPtrClear( 128);
	inState = NewPtrClear( 128);
	if( inState == nil)
	{
		/* Error */
	}
	
	outBuffer = NewHandle( numSampleFrames*6);
	if( outBuffer == nil)
	{
		/* Error */
	}
	
	HLock( sound);
	HLock( outBuffer);
	
	Exp1to3( *sound, *outBuffer, numSampleFrames, inState, outState, 1, 1);
	
	HUnlock( sound);
	HUnlock( outBuffer);
	
	DisposeHandle( sound);
	sound = outBuffer;
	
	DisposePtr( inState);
	DisposePtr( outState);
	
	return sound;
}

Handle DoExp1to6( Handle sound, unsigned long numSampleFrames)
{
	long	i;
	Ptr		inState, outState;
	Handle	outBuffer;
	
	outState = NewPtrClear( 128);
	inState = NewPtrClear( 128);
	if( inState == nil)
	{
		/* Error */
	}
	
	outBuffer = NewHandle( numSampleFrames * 6);
	if( outBuffer == nil)
	{
		/* Error */
	}
	
	HLock( sound);
	HLock( outBuffer);
	
	Exp1to6( *sound, *outBuffer, numSampleFrames, inState, outState, 1, 1);
	
	HUnlock( sound);
	HUnlock( outBuffer);
	
	DisposeHandle( sound);
	sound = outBuffer;
	
	DisposePtr( inState);
	DisposePtr( outState);
	
	return sound;
}
#endif

Handle NSndToHandle(Handle sound, long *loopStart, long *loopEnd, short *sampleSize, unsigned long *sampleRate, long *baseFreq, Boolean *stereo)
{
	Ptr 			soundPtr;
	short 			soundFormat, numChannels;
	short 			numSynths = 0, numCmds = 0, CompressID;
	long 			offset, MusSize;
	SoundHeaderPtr 	header;
	CmpSoundHeader	*CmpHeader;
	ExtSoundHeader	*ExtHeader;
	long			i;
	
	if (!sound) DebugStr("\pNSndToHandle");
	
	*loopStart = 0;
	*loopEnd = 0;
	*sampleSize = 8;
	
	// make the sound safe to use at interrupt time.
	HLock(sound);
	soundPtr = *sound;
	
	// determine what format sound we have.
	soundFormat = *(short*)soundPtr;
	
	switch(soundFormat)
	{
		case 1:						// format 1 sound.
			// look inside the format 1 resource and deduce offsets.
			numSynths = ((short*)soundPtr)[1];					// get # synths.
			numCmds = *(short*)(soundPtr+4+numSynths*6);		// get # commands.
			break;
			
		case 2:						// format 2 sound.
			numSynths = 0;			// format 2 sounds have no synth's.
			numCmds = ((short*)soundPtr)[2];
			break;
			
		default:					// jack says, what about 12? or 6?
			DebugStr("\p NSndToHandle... Burkk");
			break;
	}
	
	// compute address of sound header.
	offset = 6 + 6*numSynths + 8*numCmds;
	header = (SoundHeaderPtr)((*sound) + offset);
	
	switch (header->encode) {
		case cmpSH:
			CmpHeader = (CmpSoundHeader*) header;
			CompressID = CmpHeader->compressionID;
			numChannels = CmpHeader->numChannels;
			
			*loopStart = CmpHeader->loopStart;
			*loopEnd = CmpHeader->loopEnd;
			*sampleSize = CmpHeader->sampleSize;
			if (numChannels == 2)
				*stereo = true;
			else
				*stereo = false;
			
			if (sampleRate != NULL)
				*sampleRate	= CmpHeader->sampleRate;
			if (baseFreq != NULL)
				*baseFreq 	= CmpHeader->baseFrequency;
			
			MusSize = (*CmpHeader).numFrames;
			if (*stereo) {
				MusSize *= 2;
				*loopStart *=2;
				*loopEnd *=2;
			}
			HLock(sound);
			memmove(*sound, CmpHeader->sampleArea, MusSize);
			HUnlock(sound);
			
			switch(CompressID) {
				/*	case threeToOne:
				 MusSize *= 2;
				 sound = DoExp1to3(sound, MusSize);
				 MusSize *= 3;
				 break;
				 
				 case sixToOne:
				 sound = DoExp1to6(sound, MusSize);
				 MusSize *= 6;
				 break;*/
				
			default:
				Erreur(7, (OSErr) CompressID);
				break;
		}
			
			break;
			
		case extSH:
			ExtHeader = (ExtSoundHeader*) header;
			
			MusSize = ExtHeader->numFrames;
			numChannels = ExtHeader->numChannels;
			
			*loopStart = ExtHeader->loopStart;
			*loopEnd = ExtHeader->loopEnd;
			*sampleSize = ExtHeader->sampleSize;
			
			if (sampleRate != NULL)
				*sampleRate	= ExtHeader->sampleRate;
			if (baseFreq != NULL)
				*baseFreq = ExtHeader->baseFrequency;
			
			if (numChannels == 2)
				*stereo = true;
			else
				*stereo = false;
			
			if (*stereo) {
				MusSize *= 2;
				*loopStart *=2;
				*loopEnd *=2;
			}
			
			if (*sampleSize == 16) {
				MusSize *= 2;
				*loopStart *= 2;
				*loopEnd *= 2;
			}
			
			HLock(sound);
			if (numChannels == 1)
				memcpy(*sound, ExtHeader->sampleArea, MusSize);
			else if (numChannels == 2) {
				memcpy(*sound, ExtHeader->sampleArea, MusSize);
			} else {
				if (*sampleSize == 8) {
					for (i = 0; i < MusSize; i ++) {
						(*sound)[i] = ExtHeader->sampleArea[i * numChannels];
					}
				} else {
					MusSize /= 2;
					for (i = 0; i < MusSize; i ++) {
						((short*)(*sound))[i] = ((short*)ExtHeader->sampleArea)[i * numChannels];
					}
					MusSize *= 2;
				}
			}
			HUnlock(sound);
			break;
			
		default:
		case stdSH:
			*loopStart = header->loopStart;
			*loopEnd = header->loopEnd;
			
			if (sampleRate != NULL)
				*sampleRate	= header->sampleRate;
			if (baseFreq != NULL)
				*baseFreq = header->baseFrequency;
			
			MusSize = header->length;
			memcpy(*sound, (*header).sampleArea, MusSize);
			HUnlock(sound);
			break;
	}
	
	HUnlock(sound);
	SetHandleSize(sound, MusSize);
	
	if (MemError() != noErr) {
		Erreur(2, MemError());
	}
	
	if (*loopEnd - *loopStart < 4) {
		*loopEnd = 0;
		*loopStart = 0;
	}
	
	return sound;
}

Ptr LenOutPfileFn(long *Ssize,  Ptr fnSample);
void ComputeQuicktimeSound(short GMInstruID, sData **sample, InstrData* inst, short ins);
/*
 * WritePfile: Writes division information (sample, pitch, effect) to
 * given file in the format specified by wModfmt.
 *
 * date: 2/7/1994 - added multi-format support
 *       3/7/1994 - now aborts on write error
 */
void WritePfile(PatData *Pat, short pos, short track, unsigned bSam, unsigned wPit, unsigned wEff)
{
	Cmd					*aCmd;
#if 0
	if (-1 == cNote) {
		irgchPos = 0;
		if (1 == wModfmt)
			pchBuff = (char *) malloc(cNote = Buffsiz = 4 * wMaxchan * DIVSPERPAT);
		else
			pchBuff = (char *) malloc(cNote = Buffsiz = 3 * wMaxchan * DIVSPERPAT);
	}
#endif
	
	aCmd = GetMADCommand(pos, track, Pat);
	if (wPit > 0 )
		aCmd->note =  wPit - 12;
	else
		aCmd->note = 0xFF;
	aCmd->ins = bSam;
	aCmd->arg = wEff & 0xFF;
	aCmd->cmd = wEff >> 8;
	aCmd->vol = 0xFF;
	
	if (aCmd->cmd == 0x0C) {
		aCmd->cmd = 0;
		
		if (aCmd->arg == 0) {
			aCmd->note = 0xFE;
		} else {
			aCmd->vol = aCmd->arg + 0x10;
		}
		aCmd->arg = 0;
	}
}

/*
 * PeiNextPtune: Given a pointer to a tune, will start using it if not
 * already using one. Will return next event in quanta, else NULL.
 * If it gets to the end of the tune, will set flag to false and will
 * wait to be given a new tune.
 */
EI *PeiNextPtune(Tune *ptuneMain, int *pf)
{
	static Tune *ptune = NULL;
	static unsigned long quant;
	EI *pei;
	
	if (NULL == ptune) { /** If first time called **/
		if (NULL == ptuneMain) /** If no tune given, quit **/
			return NULL;
		*pf = 1;
		quant = 0;
		ptune = ptuneMain; /** Initialize tune pointer to start of tune **/
	} else /** Else **/
		quant++; /** Advance along tune 1 quantize fraction **/
	if (quant < ptune->count) /** If haven't reached next event **/
		return NULL; /** return nothing-happening **/
	
	pei = ptune->pei; /** Otherwise note next event **/
	if ((ptune = ptune->ptune) == NULL) /** If no more events **/
		*pf = 0; /** register end of tune **/
	return pei; /** Return that event **/
}

/*
 * Convqpm: Converts a given qpm number into a double tempo thingy.
 */
void Convqpm(unsigned qpm, int rgbTempo[2], int ticks)
{
	if (792 / ticks <= qpm && 6144 / ticks - 1 >= qpm) {
		rgbTempo[0] = ticks; /** If can use current ticks/div, do so **/
		rgbTempo[1] = qpm * ticks / 24;
	} else if (33 > qpm) /** Else if qpm is very small **/
		if (26 > qpm) { /** approximate it **/
			rgbTempo[0] = 31;
			rgbTempo[1] = 33;
		} else {
			rgbTempo[0] = 31;
			rgbTempo[1] = qpm * 31 / 24;
		}
		else if (6144 <= qpm) { /** Else if qpm is very big **/
			rgbTempo[0] = 1; /** approximate it too **/
			rgbTempo[1] = 255;
		} else { /** Else look for closest fraction **/
			int j, k, kMax;
			double ratio;
			long double junk;
			
			ratio = qpm / 24.0;
			j = k = 791 / qpm + 1; /** I hope these constraints are Ok **/
			kMax = 6143 / qpm;
			while (k++ < kMax)
				if (fabsl(modfl(ratio * k, &junk) - 0.5) >
					fabsl(modfl(ratio * j, &junk) - 0.5))
					j = k;
			rgbTempo[0] = j;
			rgbTempo[1] = j * ratio + 0.5;
		}
}

/*
 * PutpatternsPtunePfile: Given an output file and a tune, will output the
 * tune as standard Protracker patterns. wMaxchan determines number of
 * channels per division. wPatmax determines maximum number of patterns
 * to write before terminating.
 *
 * date: 3/7/1994 - quiet samples now play sample 0 rather than sample 31
 */

int PutpatternsPtunePfile(Tune *ptune, MADMusic *theMAD, MADDriverSettings *init)
{
	int 				iT, iT2, wPat = 0, cDiv, ipw, ipwMax, fGoing;
	unsigned 			*pwLen, pwNote[3 * MAXCHANS], rgbTempo[2] = {6,125};
	unsigned 			wNewtempo = 120;
	unsigned long 		cDev = 0, i;
	EI 					*pei;
	
	pwLen = (unsigned *)calloc(MAXCHANS, sizeof(unsigned));
	pei = PeiNextPtune(ptune, &fGoing); 			/** Get first event **/
	ipw = wMaxchan;
	ipwMax = wMaxchan * 3;
	
	for (i = 0; i < MAXPATTERN; i++) theMAD->partition[i] = NULL;
	
	for (wPat = 0; fGoing; wPat++)					/** Loop until told to stop **/
	{
		if (wPat == wPatmax)						/** If pattern limit reached, stop **/
		{
			DebugStr("\pWarning -- Pattern limit %d reached. Aborting!");
			break;
		}
		
		/******************************************/
		
		theMAD->partition[wPat] = (PatData*) MADPlugNewPtrClear(sizeof(PatHeader) + wMaxchan * 64L * sizeof(Cmd), init);
		if (theMAD->partition[wPat] == NULL)
			return MADNeedMemory;
		
		theMAD->partition[wPat]->header.size 		= 64L;
		theMAD->partition[wPat]->header.compMode 	= 'NONE';
		theMAD->partition[wPat]->header.patBytes 	= 0;
		theMAD->partition[wPat]->header.unused2 	= 0;
		
		/******************************************/
		
		for (cDiv = 64; cDiv--;) {					/** For each division in a pattern **/
			memset(pwNote, 0, ipwMax * sizeof(unsigned)); /** Clear next notes **/
			
			for (iT = wMaxchan; iT--; ) { 			/** With any currently playing notes **/
				if (pwLen[iT]) {
					if (0 == --pwLen[iT]) {			/** Check if just stopped **/
						pwNote[iT2 = iT * 3] = 0; 	/** Yes.. store quiet command **/
						pwNote[iT2 + 2] = 0xC00;
					}
				}
			}
			
			if (fGoing)								/** If still going in the tune **/
			{
				for (; NULL != pei; pei = pei->pei)	/** For each event at this position **/
				{
					if (-1 != pei->pitch)			/** If note-event **/
					{
						if (-2 != pei->pitch)		/** which is valid **/
						{
							iT = ipwMax - 1; 		/*** Find channel to stick note ***/
							for (; 0 < iT && 0xC00 != pwNote[iT]; iT -= 3);
							if (0 > iT)
							{
								for (iT = ipw; iT--; ) if (0 == pwLen[iT]) break;
								if (0 > iT)
								{
									for (iT = wMaxchan; iT-- > ipw; ) if (0 == pwLen[iT]) break;
									if (0 > iT)
									{
										iT2 = (unsigned) pei->effect / 2;
										for (iT = wMaxchan; iT--; ) if (iT2 <= pwLen[iT]) break;
										if (0 > iT)
										{
											cDev++;
											continue;
										}
									}
								}
								if (--ipw < 0)
									ipw = wMaxchan - 1;
								iT = iT * 3 + 2;
							}
							
							pwNote[iT - 2] = pei->inst + 1; 	/*** Store note ***/
							if (fStats && 0 != pwNote[iT - 1]) cDev++;
							pwNote[iT - 1] = pei->pitch;
							if (pei->vol != rgmsDecided[pei->inst].bDefvol) pwNote[iT] = 0xC00 + pei->vol;
							else pwNote[iT] = 0;
							pwLen[iT / 3] = (unsigned) pei->effect;
						}
					}
					else wNewtempo = (unsigned) pei->effect;	/** Else store new tempo **/
				}
				pei = PeiNextPtune(NULL, &fGoing);
			}
			
			if (0 != wNewtempo)									/** If need to set a tempo this division **/
			{
				int rgbNew[2];
				
				Convqpm(wNewtempo, rgbNew, rgbTempo[0]); 		/** Find protracker equivalent **/
				if (rgbNew[0] != rgbTempo[0] || rgbNew[1] != rgbTempo[1])
				{
					for (iT = ipwMax - 1; iT > 0; iT -= 3) if (0 == pwNote[iT]) break;			/** Find a channel for it **/
					
					if (iT < 0)									/** If no channel.. damn. Have to replace something **/
					{
						unsigned bMin = -1, bTest;
						
						for (iT2 = ipwMax - 1; iT2 > 0; iT2 -= 3)
						{
							bTest = abs((pwNote[iT2] & 0xFF) - rgmsDecided[pwNote[iT2 - 2] - 1].bDefvol);
							if (bTest < bMin)
							{
								bMin = bTest;
								iT = iT2; 						/** Find best thing to replace **/
							}
						}
					}
					
					if (rgbNew[0] != rgbTempo[0])
						if (rgbNew[1] != rgbTempo[1])			/** If need two places **/
						{
							pwNote[iT] = 0xFFF;
							for (iT2 = ipwMax - 1; iT2 > 0; iT2 -= 3) /** Find a channel **/
								if (0 == pwNote[iT2])
									break;
							
							if (iT2 < 0)						/** No channels.. use different new tempo **/
							{
								iT2 = rgbNew[1] * rgbTempo[0] / rgbNew[0];
								if (255 < iT2)
									if (280 < iT2)
										iT2 = 0;
									else
										iT2 = 255;
									else if (33 > iT2)
									{
										if (30 > iT2)
										{
											iT2 = 33;
										} else {
											iT2 = 0;
										}
									}
								
								if (0 != iT2)					/** If we can allow for ~10% bpm variance **/
								{
									pwNote[iT] = 0xF00 + iT2;	/** then use it **/
									rgbTempo[1] = iT2;
								}
								else							/** Else try to find a ticks value **/
								{
									iT2 = rgbNew[0] * rgbTempo[1] / rgbNew[1];
									if (0 == iT2)
										iT2 = 1;
									else if (32 < iT2)
										iT2 = 32;
									pwNote[iT] = 0xF00 + iT2;	/** and use it **/
									rgbTempo[0] = iT2;
								}
							}
							else
							{
								pwNote[iT] = 0xF00 + rgbNew[0]; /** Store 2 value tempo **/
								rgbTempo[0] = rgbNew[0];
								pwNote[iT2] = 0xF00 + rgbNew[1];
								rgbTempo[1] = rgbNew[1];
							}
						}
						else
						{
							pwNote[iT] = 0xF00 + rgbNew[0];		/** Store just ticks/div **/
							rgbTempo[0] = rgbNew[0];
						}
						else
						{
							pwNote[iT] = 0xF00 + rgbNew[1]; 		/** Store just bpm **/
							rgbTempo[1] = rgbNew[1];
						}
				}
				wNewtempo = 0;
			}
			
			for (iT = ipwMax - 1; iT > 0; iT -= 3)				/** Write division to file **/
			{
				WritePfile(theMAD->partition[wPat], 63 - cDiv, (ipwMax -iT) / 3, pwNote[iT - 2], pwNote[iT - 1], pwNote[iT]);
			}
		}
	}
	if (fStats)
		CreateResult("Number of dropped notes.");
	free(pwLen);
	
	return wPat; 												/** Return number of patterns written **/
}

/*
 * SavePtunePfile: Given an output file and a tune, will output the tune to
 * the file using module format specified by wModfmt.
 *
 * date: 30/6/1994 - added call to LenOutPfileFn
 *       2/7/1994 - added MTM saving
 */

MADMusic	*curMusic = NULL;

void SavePtunePfile(Tune *ptune, MADMusic *theMAD, MADDriverSettings *init)
{
	int 		iT, cPatterns, cSamps;
	long	 	i;
	long 		inOutCount, x;
	//SI 		*psi;
#if 0
	long 		finetune[16] =
	{
		8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757,
		7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280
	};
#endif
	MADMusic	*tempCopy;
	
	/********************/
	/********************/
	/** MAD ALLOCATION **/
	/********************/
	/********************/
	
	tempCopy = curMusic;
	curMusic = NULL;
	
	inOutCount = sizeof(MADSpec);
	theMAD->header = (MADSpec*)MADPlugNewPtrClear(inOutCount, init);
	if (theMAD->header == NULL) DebugStr("\pHeader: I NEED MEMORY !!! NOW !");
	
	dispatch_apply(MAXTRACK, dispatch_get_global_queue(0, 0), ^(size_t i) {
		if (i % 2 == 0)
			theMAD->header->chanPan[i] = MAX_PANNING / 4;
		else
			theMAD->header->chanPan[i] = MAX_PANNING - MAX_PANNING / 4;
		
		theMAD->header->chanVol[i] = MAX_VOLUME;
	});
	theMAD->header->generalVol		= 64;
	theMAD->header->generalSpeed	= 80;
	theMAD->header->generalPitch	= 80;
	
	
	//	for (x = 0; x < 20; x++)		theMAD->header->name[x] = szTitle[x];
	theMAD->header->MAD				=	'MADK';
	theMAD->header->speed			= 	6;
	theMAD->header->tempo			=	125;
	
	strlcpy(theMAD->header->infos, "Converted by PlayerPRO MIDI Plug (\251Antoine ROSSET <rossetantoine@bluewin.ch>)", sizeof(theMAD->header->infos));
	
	for (cSamps = 0, x = 0; x < 129; x++) {
		if (MIDIInstMOD[x] != -1) cSamps++;
	}
	
	theMAD->header->numChn			=	wMaxchan;
	
	theMAD->sets = (FXSets*) calloc(MAXTRACK, sizeof(FXSets));
	for (i = 0; i < MAXTRACK; i++) theMAD->header->chanBus[i].copyId = i;
	
	theMAD->fid = (InstrData*) calloc(sizeof(InstrData), MAXINSTRU);
	if (!theMAD->fid)
		return;
	
	theMAD->sample = (sData**) calloc(sizeof(sData*) * MAXINSTRU, MAXSAMPLE);
	if (!theMAD->sample)
		return;
	
	dispatch_apply(MAXINSTRU, dispatch_get_global_queue(0, 0), ^(size_t i) {
		theMAD->fid[i].firstSample = i * MAXSAMPLE;
	});
	
	for (iT = 0; iT < cSamps; iT++) {
		Str255		tStr;
		short		MidiIns;
		
		MidiIns = 0;
		for (x = 0; x < 129; x++) {
			if (MIDIInstMOD[x] == iT)
				MidiIns = x;
		}
		
		NumToString(MidiIns+1, tStr);
		for (x = 1; x <= tStr[0]; x++) theMAD->fid[iT].name[x-1] = tStr[x];
		
		if (UseQKIns) {
			if (MidiIns == 128)
				MidiIns = 16385;
			ComputeQuicktimeSound(MidiIns, theMAD->sample, &theMAD->fid[iT], iT);
		}
	}
	
	cPatterns = PutpatternsPtunePfile(ptune, theMAD, init); 	/** Write patterns **/
	
	theMAD->header->numPat			=	cPatterns;
	theMAD->header->numPointers		=	cPatterns;
	if (theMAD->header->numPointers > 128)
		theMAD->header->numPointers = 128;
	
	for (x = 0; x < theMAD->header->numPointers; x++)
		theMAD->header->oPointers[x] = x;
	
	curMusic = tempCopy;
}
