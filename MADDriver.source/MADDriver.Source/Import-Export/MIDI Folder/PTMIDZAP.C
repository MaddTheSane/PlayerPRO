/*
 * ptmidzap.c: Resolution module for ptmid. Takes a structure representing
 * a tune and tries to make it fit into 4 channels.
 *
 * Author: Andrew Scott  (c)opyright 1994
 *
 * Date: 17/11/1993 ver 0.0
 *       8/1/1994   ver 0.1
 *       11/2/1994  ver 0.2
 */

#include <PlayerPROCore/PlayerPROCore.h>
#include <stdio.h>
#include <stdlib.h>
#include "PTMID.H"

MS rgmsDecided[ MAXSAMPS];
int cmsDecided, wMinpitch, wMaxpitch;

/**
 ** The midivolume array holds Protracker equivants of MIDI velocities.
 ** It allows fast velocity-volume conversion.
 **/
unsigned midivolume[128] = {
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
  };

/*
 * Init: Initializes stuff that should be initialized.
 *
 * date: 1/7/1994 - added init of min/max pitches & default period array
 */
static void Init(void)
{
  int ims = MAXSAMPS;

	/** Put default values in sample array **/

	while (ims--)
	{ 
		sprintf(rgmsDecided[ims].szName, "");
    	rgmsDecided[ims].psi = NULL;
		rgmsDecided[ims].bDefvol = 64;
	}
	
  cmsDecided = 0;
  if (fExtend)
  {
		wMinpitch = MIDDLEC - 24;
		wMaxpitch = MIDDLEC + 36;
  }
  else
  {
		wMinpitch = MIDDLEC - 12;
		wMaxpitch = MIDDLEC + 24;
  }
}

/*
 * FitSzBFn: Given a filename, will fit it into the given sample name,
 * and prepend given number, ensuring that it doesn't overflow the
 * 22 character array.
 */
void FitSzBFn(Sz szName, int bPos, Sz fnSample)
{
	int iT = 18;

	sprintf(szName, "%02d: ", bPos); /** First the number **/
	szName += 4;
	while (iT-- && (*(szName++) = *(fnSample++))); /** Then the name **/
}

/*
 * WConvertMidipitch: Given a MIDI pitch, returns a pitch in the range
 * allowed for output. If none can be found (only for wRgmode = 0), -2 is
 * returned. If the pitch is out of range, then the value of the given
 * pointer is incremented.
 *
 * date: 1/7/1994
 */
int WConvertMidipitch(int pitch, unsigned long *cDev)
{
  switch (wRgmode) {
    case 0:
      if (pitch < wMinpitch) {
        if (fStats)
          (*cDev)++;
        pitch = wMinpitch;
      } else if (pitch >= wMaxpitch) {
        if (fStats)
          (*cDev)++;
        pitch = wMaxpitch - 1;
      }
      break;
    case 1:
      if (pitch < wMinpitch || pitch >= wMaxpitch) {
        if (fStats)
          (*cDev)++;
        pitch = -2;
      }
      break;
    case 2:
      if (pitch < wMinpitch) {
        if (fStats)
          (*cDev)++;
        pitch += ((wMinpitch - pitch + 11) / 12) * 12;
      } else if (pitch >= wMaxpitch) {
        if (fStats)
          (*cDev)++;
        pitch -= ((pitch - wMaxpitch + 12) / 12) * 12;
      }
  }
  return pitch;
}

/*
 * AnalyzePtune: Given a tune, searches through and allocates samples
 * to sample array rgmsDecided (includes determining mixes).
 * Eventually (?) this will become much more extensive, and include
 * compacting by changing tempo, filtering of quiet notes, and identification
 * of common chords and replacing these with a separate sample.
 *
 * date: 1/7/1994 - added calls to WConvertMidipitch
 */
 
 short 				MIDIInstMOD[ 129];
 
void AnalyzePtune(Tune *ptune)
{
	unsigned long			cDev = 0;
	EI						*pei;
	SI						*psi = NULL, **ppsi;
	
	short					i, NewIns;

	for( i = 0; i < 129; i++) MIDIInstMOD[ i] = -1;
	NewIns = 0;

	while (NULL != ptune)											/** While not at end of tune **/
	{
	    for (pei = ptune->pei; NULL != pei; pei = pei->pei) 		/** With each event **/
	    {
	    	if (-1 != pei->pitch)
	    	{
		        pei->vol = midivolume[ pei->vol];					// ** Convert volumes **
				
				if( pei->inst < 0)
				{
					pei->inst = 128;
				//	pei->pitch = MIDDLEC;
				}
				if( pei->inst > 128) Debugger();
				
				if( MIDIInstMOD[ pei->inst] == -1)					// A new instrument !!
				{
					MIDIInstMOD[ pei->inst] = NewIns;
					NewIns++;
				}
				
				pei->inst = MIDIInstMOD[ pei->inst];
			}
	    }
		ptune = ptune->ptune;
	}
}

/*
 * UnitifyPtune: Given a tune, will convert lengths into division multiples.
 */
void UnitifyPtune(Tune *ptune)
{
	EI *pei;

	while (NULL != ptune) { /** While not end of tune **/
		pei = ptune->pei;
		ptune->count /= wQuant; /** Divide intervals by quantize amount **/
		ptune = ptune->ptune;
		while (NULL != pei) { /** Go through each event **/
      if (-1 != pei->pitch)
				pei->effect /= wQuant; /** Divide durations by quantize amount **/
      pei = pei->pei;
		}
	}
}

/*
 * ResolvePtune: Given a tune, goes through and finds out useful stuff
 * about it for use in creating a MOD-file. Also trims off chords that
 * are too big, notes that are too quiet, etc.
 */
void ResolvePtune(Tune *ptune)
{
	Init();
/*#if 0
	FilterPtune(ptune);
	Calcchords();
	ConvertPtune(ptune);
	Storevols();
#endif	*/
	AnalyzePtune(ptune);
	UnitifyPtune(ptune);
}
