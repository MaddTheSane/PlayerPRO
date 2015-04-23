/*
 * ptmidinp.c: MIDI input module for for ptmid. Reads a MIDI file and
 * creates a structure representing it.
 *
 * Author: Andrew Scott  (c)opyright 1994
 *
 * Date: 17/11/1993 ver 0.0
 *       8/1/1994   ver 0.1
 *       11/2/1994  ver 0.2
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <PlayerPROCore/PlayerPROCore.h>
#include <Carbon/Carbon.h>
#include "PTMID.h"
#include <tgmath.h>

#define BUFSIZE 512

#define ODD(x) (x & 1)

typedef unsigned long VLQ; /** VLQ is a variable length quantity **/

typedef struct { /** RF is a record of a file **/
	int fd, ib;
	unsigned char rgb[BUFSIZE];
} RF;

int rgbPatch[16], wDivision;
unsigned wQuant;
NRL *rgpnrl[16][128];
Tune *ptuneMain, *ptuneCurr;
RF *prf;

/////////////////
/////////////////  File patch

#define MAXFILES	300

static long		MADpos[MAXFILES];
static short	MADcur = 0;
static Ptr		MIDIGenPtr = NULL;

short MADopen(void)
{
	MADpos[MADcur] = 0;
	MADcur++;
	
	if (MADcur >= MAXFILES)
		DebugStr("\pMADopen");
	
	return MADcur-1;
}

short MADread(short id, Ptr dst, long size)
{
	if (MADpos[id] >= GetPtrSize(MIDIGenPtr))
		return 0;
	
	if (MADpos[id] + size >= GetPtrSize(MIDIGenPtr))
		size = GetPtrSize(MIDIGenPtr) - MADpos[id];
	
	memcpy(dst, MIDIGenPtr + MADpos[id], size);
	
	MADpos[id] += size;
	
	return size;
}

short MADseek(short id, long size, short mode)
{
	switch (mode) {
		case SEEK_CUR:
			MADpos[id] += size;
			break;
			
		case SEEK_SET:
			MADpos[id]  = size;
			break;
			
		default:
			DebugStr("\pMADseek");
			break;
	}
	
	if (MADpos[id] < 0)
		return -1;
	if (MADpos[id] >= GetPtrSize(MIDIGenPtr))
		return -1;
	
	return 0;
}

long MADtell(short id)
{
	return MADpos[id];
}

short MADclose(int b)
{
	return 0;
}


/*
 * Init: Yes.. you guessed it. Initializes variables and stuff.
 *
 * date: 30/6/1994 - added 16 NRL array init
 */
static void Init(void)
{
	int i, j;
	
	for (i = 16; i--; ) { 			/** Clear current instrument array **/
		rgbPatch[i] = 0;
		for (j = 128; j--; ) 			/** Clear hanging-note arrays **/
			rgpnrl[i][j] = NULL;
	}
}

/*
 * ChGetFd: Given a file, returns next character from it.
 */
int ChGetFd(int fd)
{
	unsigned char b;
	
	if (MADread(fd, (Ptr)&b, 1) < 1)
		return EOF;
	return b;
}

/*
 * ChGetIrf: Given an index, returns next character from corresponding
 * file record.
 */
unsigned char ChGetIrf(unsigned irf)
{
	if (prf[irf].ib == BUFSIZE) {
		if (MADread(prf[irf].fd, (Ptr) prf[irf].rgb, BUFSIZE) == -1) {
			ERROR;
			//	exit(1);
		}
		prf[irf].ib = 1;
		return prf[irf].rgb[0];
	}
	return prf[irf].rgb[prf[irf].ib++];
}

/*
 * SkipIrf: Given an index, skips a given number of bytes forward in
 * that file record.
 *
 * date: 4/7/1994 - fixed longstanding bug: wouldn't read in new buffer
 */
void SkipIrf(unsigned irf, long cb)
{
	if (BUFSIZE - prf[irf].ib >= cb)
		prf[irf].ib += (int)cb;
	else {
		cb -= BUFSIZE - prf[irf].ib;
		if (MADseek(prf[irf].fd, cb, SEEK_CUR) == -1) {
			ERROR;
			//	exit(1);
		}
		prf[irf].ib = BUFSIZE;
	}
}

/*
 * ValidquantSz: Takes a lower-case string and checks to see if it is a
 * legal quantize fraction. If not, zero is returned, else value of
 * string is returned (+1 if a triplet case).
 */
int ValidquantSz(Sz szQuant)
{
	int bFrac;
	Sz szEnd;
	
	if ((bFrac = (int) strtol(szQuant, &szEnd, 10))) /** If a number **/
		if ('t' == *szEnd || 'T' == *szEnd) /** possibly followed by a 't' **/
			bFrac++;
	return bFrac; /** return valid **/
}

/*
 * VlqFromFd: Reads bytes from given file until a variable length
 * quantity is decoded. Returns that vlq.
 */
VLQ VlqFromFd(int fd)
{
	VLQ vlqRead = 0;
	unsigned char b = 0;
	
	while (MADread(fd, (Ptr) &b, 1) == 1 && (b & 0x80))
		vlqRead = (vlqRead << 7) | (b & 0x7F);
	return (vlqRead << 7) | b;
}

/*
 * VlqFromIrf: Reads bytes from file corresponding to given index until
 * a variable length quantity is decoded. Returns that vlq.
 */
VLQ VlqFromIrf(unsigned irf)
{
	VLQ vlqRead = 0;
	unsigned char b = 0;
	
	while ((b = ChGetIrf(irf)) & 0x80)
		vlqRead = (vlqRead << 7) | (b & 0x7F);
	return (vlqRead << 7) | b;
}

/*
 * LongFromFd: Reads number of bytes from Fd until number is in.
 */
unsigned long LongFromFd(int fd, unsigned cb)
{
	unsigned char rgb[4];
	unsigned long longT = 0;
	unsigned ib = 0;
	
	MADread(fd, (Ptr) rgb, cb);
	for (; ib < cb; ib++)
		longT = (longT << 8) + rgb[ib];
	return longT;
}

/*
 * Addnote: Given channel, pitch, instrument, and volume will add that
 * note to the array of playing notes.
 *
 * date: 30/6/1994 - added chan as well as pfx init
 */
void Addnote(int chan, int pitch, int inst, int vol)
{
	NRL *pnrlT;
	
	if (0 > pitch || 127 < pitch || 0 > chan || 15 < chan)
		return;
	pnrlT = (NRL *) malloc(sizeof(NRL)); /** Allocate space **/
	pnrlT->pnrl = rgpnrl[chan][pitch];
	rgpnrl[chan][pitch] = pnrlT; /** Attach to front of list in array **/
	pnrlT->inst = inst;
	pnrlT->vol = vol;
	pnrlT->pfxTail = NULL;
	pnrlT->ptuneNow = ptuneCurr;
}

/*
 * PeiRequestPtune: Returns a pointer to a new event structure at the
 * position in the tune specified.
 */
EI *PeiRequestPtune(Tune *ptune)
{
	EI *pei;
	
	pei = (EI *)malloc(sizeof(EI)); /** Allocate space for event **/
	pei->pei = ptune->pei;
	ptune->pei = pei; /** Attach to front of event list at tune position **/
	return pei;
}

/*
 * Endnote: Given a pitch and instrument, will remove a note from array
 * of playing notes and store it as an event in the tune.
 *
 * date: 30/6/1994 - added support for effects, 16 chans, min. perc. durat.
 */
void Endnote(int chan, int pitch, int inst)
{
	NRL *pnrlT, *pnrlOld = NULL;
	unsigned long durat;
	EI *peiT;
	
	if (0 > pitch || 127 < pitch || 0 > chan || 15 < chan)
		return;
	for (pnrlT = rgpnrl[chan][pitch]; NULL != pnrlT && pnrlT->inst != inst; ) {
		pnrlOld = pnrlT;
		pnrlT = pnrlT->pnrl;
	} /** Find instrument in hanging-note array **/
	if (NULL == pnrlT)
		return;
	
	durat = ptuneCurr->count - pnrlT->ptuneNow->count; /** Calc. duration **/
	if (0 > inst && wDivision / 2 > durat)
		durat = wDivision / 2; /** Percussion sounds have a min. duration **/
	if (durat < wQuant)
		durat = wQuant; /** All sounds have a min. duration **/
	
	peiT = PeiRequestPtune(pnrlT->ptuneNow); /** Get an event **/
	peiT->effect = durat;
	peiT->inst = inst; /** Store the note in it **/
	peiT->pitch = pitch;
	peiT->vol = pnrlT->vol;
	peiT->pfxTail = pnrlT->pfxTail;
	
	if (NULL == pnrlOld) /** Remove note from hanging-note array **/
		rgpnrl[chan][pitch] = pnrlT->pnrl;
	else
		pnrlOld->pnrl = pnrlT->pnrl;
	free(pnrlT);
}

/*
 * VlqInterpIrf: Reads a file pointer and gathers all notes at this
 * instant, storing them on the note stack. Returns ticks until next
 * collection of notes. A running status of current channel is maintained.
 *
 * date: 30/6/1994 - added effect support
 */
VLQ VlqInterpIrf(unsigned irf, unsigned *pbStat)
{
	unsigned bEvent;
	VLQ vlqT;
	FX *pfx;
	
	do { /** Loop.. **/
		
		bEvent = ChGetIrf(irf); /** Get first data byte **/
		
		if (0x80 <= bEvent && 0xEF >= bEvent) {	/** If a command **/
			*pbStat = bEvent;					/** update running-status byte **/
			bEvent = ChGetIrf(irf);				/** and get next data byte **/
		}
		if (0xF0 == bEvent || 0xF7 == bEvent) 		/** If a sys-exclusive message **/
			SkipIrf(irf, VlqFromIrf(irf)); 			/** skip it **/
		else if (0xFF == bEvent) { 					/** Else if a meta event **/
			bEvent = ChGetIrf(irf); 				/** get type of event **/
			vlqT = VlqFromIrf(irf); 				/** and length **/
			if (0x2F == bEvent) { 					/*** If termination event ***/
				MADclose(prf[irf].fd); 				/*** MADclose handle ***/
				prf[irf].fd = -1;
				vlqT = 0;
				break; 								/*** and terminate ***/
			}
			if (0x51 == bEvent) { 					/*** Else if tempo event ***/
				unsigned long t;
				EI *pei;
				
				t = (unsigned long) ChGetIrf(irf) << 16; /*** get value ***/
				t += (unsigned long) ChGetIrf(irf) << 8;
				t += ChGetIrf(irf);
				pei = PeiRequestPtune(ptuneCurr);
				pei->effect = 60000000L / wQuant * wDivision / t; /*** and convert ***/
				pei->pitch = -1;
				
			} else
				SkipIrf(irf, vlqT); 			/*** Else skip event ***/
		} else
			switch (*pbStat & 0xF0)
		{ 			/** Else must be a midi event.. **/
				case 0x80:
				case 0x90: { 					/** Note on/off **/
					unsigned bVol, bChan;
					
					bVol = ChGetIrf(irf);
					bChan = *pbStat & 0x0F;
					if (0 < bVol && 0x90 <= *pbStat)		// ON
					{
						if (bChan != bDrumch)
							Addnote(bChan, bEvent, rgbPatch[bChan], bVol);
						else
							Addnote(bChan, bEvent, -1 - bEvent, bVol);
					}
					else									// OFF
					{
						if (bChan != bDrumch)
							Endnote(bChan, bEvent, rgbPatch[bChan]);
						else
							Endnote(bChan, bEvent, -1 - bEvent);
					}
					break;
				}
				case 0xA0: { 				/** Polyphonic Key Pressure **/
					NRL *pnrlT;
					unsigned bChan;
					
					bChan = *pbStat & 0x0F;
					pnrlT = rgpnrl[bChan][bEvent];
					while (NULL != pnrlT && pnrlT->inst != rgbPatch[bChan])
						pnrlT = pnrlT->pnrl;
					if (NULL != pnrlT) {
						pfx = (FX *) malloc(sizeof(FX));
						pfx->delay = ptuneCurr->count - pnrlT->ptuneNow->count;
						pfx->eff = 1;
						pfx->param = ChGetIrf(irf);
						if (NULL == pnrlT->pfxTail)
							pfx->pfx = pfx;
						else {
							pfx->pfx = pnrlT->pfxTail->pfx;
							pnrlT->pfxTail->pfx = pfx;
						}
						pnrlT->pfxTail = pfx;
					} else
						ChGetIrf(irf);
					break;
				}
				case 0xB0: /** Controller change **/
				case 0xE0: /** Pitch Wheel change **/
					ChGetIrf(irf);
					break;
				case 0xC0: /** Program change **/
					rgbPatch[*pbStat - 0xC0] = bEvent;
					break;
				case 0xD0: /** Channel Pressure **/
					break;
			}
		vlqT = VlqFromIrf(irf);
	} while (0 == vlqT); /** Continue loop for simultaneous notes **/
	return vlqT;
}

/*
 * Freearray: Performs a free on all structures left in rgpnrl array.
 * Assume these notes are rebels.
 *
 * date: 30/6/1994 - now frees remaining fx too
 */
void Freearray(void)
{
	int 	i, j = 16;
	NRL 	*pnrlT, *pnrlT2;
	FX 		*pfxT, *pfxT2;
	
	while (j--) { /** Go through hanging-note array **/
	
		for (i = 128; i--;) {
			for (pnrlT = rgpnrl[j][i]; NULL != pnrlT; ) {		/** freeing each list **/
				pnrlT2 = pnrlT->pnrl;
				
				if (pnrlT->pfxTail != NULL){
					pfxT = pnrlT->pfxTail->pfx;
					if (NULL != pfxT) {
						while (pfxT != pnrlT->pfxTail) {			/** and each fx list **/
							pfxT2 = pfxT;
							pfxT = pfxT->pfx;
							free(pfxT2);
						}
						free(pfxT);
					}
				}
				free(pnrlT);
				pnrlT = pnrlT2;
			}
		}
	}
}

/*
 * PtuneLoadPfile: Given the filename of a MIDI file, parse it and return
 * a pointer to a collection of chords (a Tune structure). If MIDI file
 * cannot be processed, NULL is returned.
 */
Tune *PtuneLoadFn(Ptr MIDIptr, short *channels)
{
	int 			MADfd, crf;
	unsigned long 	cb, *pvlqWait, vlqMin = -1, vlqT, wCount, wNcount, cDev = 0;
	unsigned 		irf, irfMax, *pbStatus, wQuant2, wDev, wMaxdev = 0, cMaxdev = 0;
	char 			rgbHeader[] = {'M', 'T', 'h', 'd', 0, 0, 0, 6, 0}, rgbTest[9];
	
	MIDIGenPtr = MIDIptr;
	
	Init();
	MADfd = MADopen();
	MADread(MADfd, rgbTest, 9);
	if (memcmp(rgbHeader, rgbTest, 9) || ChGetFd(MADfd) > 1) {
		MADclose(MADfd);
		return NULL; /** Only process type 0 or type 1 general MIDI files **/
	}
	
	irfMax = (unsigned) LongFromFd(MADfd, 2); /** Get # tracks **/
	wDivision = (int) LongFromFd(MADfd, 2); /** Get ticks for a beat **/
	
	if (-1 == MADseek(MADfd, 23, SEEK_SET)) {
		MADclose(MADfd);
		return NULL; /** Error if MIDI file is smaller than 23 bytes **/
	}
	if (fNocopy && ChGetFd(MADfd) == 0xFF && ChGetFd(MADfd) == 0x02) {
		MADclose(MADfd);
		return NULL; /** Error if Nocopy and copyright notice exists **/
	}
	if (32767 < wDivision) {
		MADclose(MADfd);
		return NULL;
	}
	
	if (ODD(wQuantval)) /** Calculate quantize ticks from quantize fraction **/
		wQuant = wDivision * 8 / (3 * (wQuantval - 1));
	else
		wQuant = wDivision * 4 / wQuantval;
	wQuant2 = wQuant/2;
	if (fStats) {
		CreateResult("Ticks to quantize");
		CreateResult(" Number of tracks");
	}
	
	prf = (RF *) malloc(sizeof(RF) * irfMax);
	pvlqWait = (VLQ *) malloc(sizeof(VLQ) * irfMax);
	pbStatus = (unsigned *) malloc(sizeof(unsigned) * irfMax);
	
	MADseek(MADfd, 18, SEEK_SET);
	/** Put file descriptors at the start of each track in file **/
	for (irf = 0; irf < irfMax; irf++) {
		cb = LongFromFd(MADfd, 4);
		if ((prf[irf].fd = MADopen()) == -1) {
			ERROR;
			//	exit(1);
		}
		MADseek(prf[irf].fd, MADtell(MADfd), SEEK_SET);
		MADread(prf[irf].fd, (Ptr) prf[irf].rgb, BUFSIZE);
		prf[irf].ib = 0;
		if ((pvlqWait[irf] = VlqFromIrf(irf)) < vlqMin)
			vlqMin = pvlqWait[irf]; /** Find minimum time to first event **/
		pbStatus[irf] = 0x90;
		MADseek(MADfd, cb + 4, SEEK_CUR);
	}
	MADclose(MADfd);
	
	if (irf != irfMax) {	/** If things look a bit suspicious **/
		CreateResult("MIDI file ends prematurely **");
		free(prf); /** Free prf - NB. not closed **/
		free(pvlqWait);
		free(pbStatus);
		return NULL;
	}
	
	ptuneMain = ptuneCurr = (Tune *) malloc(sizeof(Tune));
	wCount = vlqMin; /** Start from first event **/
	wNcount = (wCount + wQuant2) % wQuant;
	if (fStats) {
		wDev = (wQuant2 - (int) wNcount);
		
		if (wDev > 2) cDev++;
		
		if (wDev > wMaxdev) {
			wMaxdev = wDev;
			cMaxdev = 1;
		} else if (wDev == wMaxdev)
			cMaxdev++;
	}
	ptuneMain->count = wCount + wQuant2 - wNcount;
	wNcount = wQuant - wNcount;
	ptuneMain->ptune = NULL;
	ptuneMain->pei = NULL;
	crf = 1;
	while (0 < crf) { /** While still tracks in file to process **/
		crf = 0;
		vlqT = -1;
		
		for (irf = 0; irf < irfMax; irf++) { /** With each track.. **/
			if (-1 != prf[irf].fd) {
			 /** If not finished **/
				crf++;
				/**
				 ** Must keep all tracks in sync, if events occurring on this track
				 ** at this instant, then interpret them. Also note when next event
				 ** will occur (ie. minimum ticks to next event)
				 **/
				if ((pvlqWait[irf] -= vlqMin) == 0)
					pvlqWait[irf] = VlqInterpIrf(irf, pbStatus + irf);
				if (pvlqWait[irf] < vlqT)
					vlqT = pvlqWait[irf];
			}
		}
		vlqMin = vlqT;
		wCount += vlqMin;
		if (wNcount <= vlqMin) { /** If need to advance to new quanta **/
			if ((ptuneCurr->ptune = (Tune *) malloc(sizeof(Tune))) == NULL) {
				ERROR;
				//	exit(1);
			} /** allocate **/
			ptuneCurr = ptuneCurr->ptune; /** and initialize **/
			wNcount = (wCount + wQuant2) % wQuant;
			if (fStats) {
				wDev = abs(wQuant2 - (int) wNcount);
				if (wDev > 2)
					cDev++;
				if (wDev > wMaxdev) {
					wMaxdev = wDev;
					cMaxdev = 1;
				} else if (wDev == wMaxdev)
					cMaxdev++;
			}
			ptuneCurr->count = wCount + wQuant2 - wNcount;
			wNcount = wQuant - wNcount;
			ptuneCurr->ptune = NULL;
			ptuneCurr->pei = NULL;
		}
		else
			wNcount -= vlqMin; /** Else decrememnt "new quanta" count **/
	}
	
	Freearray();
	free(pvlqWait);
	free(pbStatus);
	free(prf);
	
	*channels = irfMax;
	
	return ptuneMain;
}
