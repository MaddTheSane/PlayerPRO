/*
 * ptmid.c: Creates Protracker MODule files from MIDI files.
 * (My first attempt at Hungarian Notation.. wince!)
 *
 * Author: Andrew Scott  (c)opyright 1994
 *
 * Date: 17/11/1993 ver 0.0
 *       8/1/1994   ver 0.1 - first release
 *       11/2/1994  ver 0.2 - added stats + fixed 15 handles limit
 *       29/6/1994  ver 0.3 - second release
 */

void InitQuicktimeInstruments(void);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <PlayerPROCore/PlayerPROCore.h>
#include "PTMID.h"
#include <Carbon/Carbon.h>

#ifndef R_OK
#define R_OK 4
#endif

char		bDrumch = 9;
int			fNocopy = 0, fExtend = 1, fStats = 0;
int			wRgmode = 2, wPatmax = 128, wMaxchan = 8, wQuantval = 16;
SI			*rgpsiDrum[128], **rgppsiIns[129], *psiTree = NULL;
Sz			szQuant = NULL, szProgram;
Fn			fnSampath = {0};
bool		UseQKIns;
extern int	cmsDecided, wVolfract;

/*
 * Init: Does all those initialization things (which aren't too involved).
 */
static void Init(void)
{
	memset(rgppsiIns, 0, sizeof(rgppsiIns));
	memset(rgpsiDrum, 0, sizeof(rgpsiDrum));
}

/*
 * main: Parses arguments to program and opens appropriate MOD and MID files.
 */
void ConvertMidiFile(const char *src, MADMusic *theMAD, MADDriverSettings *init)
{
	short	channels; // TODO: remove
	Tune 	*ptuneMusic;
	
	Init();
	
	if ((ptuneMusic = PtuneLoadFn((Sz)src, &channels)) == NULL) {
		DebugStr("\pNot a legal MIDI file");
		return;
	}
	
	InitQuicktimeInstruments();
	
	channels++;
	channels /= 2;
	channels *= 2;
	//wMaxchan = channels;
	
	ResolvePtune(ptuneMusic);
	
	SavePtunePfile(ptuneMusic, theMAD, init);
}
