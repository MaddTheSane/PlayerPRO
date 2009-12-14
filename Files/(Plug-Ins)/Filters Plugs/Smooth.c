/*	Smooth			*//*	v 0.2			*//*	1995 by Liane	*///	Usage://	Works like a low pass filter, removing high//	harmonics generated by a low sampling rate.//	Works on the selected part or all the waveform//	if there is no selection.//	Not very accurate, but pretty fast to write !!!#include <PlayerPROCore/MAD.h>#include <PlayerPROCore/FileUtils.h>#include <PlayerPROCore/PPPlug.h>OSErr main( 	sData					*theData,				long					SelectionStart,				long					SelectionEnd,				PPInfoPlug				*thePPInfoPlug,				long					StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel{	long	i, length, temp, prevtemp, nexttemp, work;	length = SelectionEnd - SelectionStart - 1;	switch( theData->amp)	{		case 8:		{			Ptr	SamplePtr = (theData->data) + SelectionStart;						prevtemp = *SamplePtr++;			temp = *SamplePtr++;			for( i = 1; i < length; i++)			{				nexttemp = *SamplePtr--;								work = ((prevtemp + nexttemp) + (temp * 6)) >> 3;								*SamplePtr++ = work;				prevtemp = temp;				temp = nexttemp;				SamplePtr++;			}		} break;		case 16:		{			short	*SamplePtr = (short*) theData->data + (SelectionStart / 2);						prevtemp = *SamplePtr++;			temp = *SamplePtr++;			for( i = 1; i < length / 2; i++)			{				nexttemp = *SamplePtr--;								work = ((prevtemp + nexttemp) + (temp * 6)) >> 3;								*SamplePtr++ = work;				prevtemp = temp;				temp = nexttemp;				SamplePtr++;			}		} break;	}	return noErr;}