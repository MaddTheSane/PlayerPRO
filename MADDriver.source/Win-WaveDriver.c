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

#include <stdio.h>

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include "mmreg.h"
#include <mmsystem.h>

#ifdef _MSC_VER
#pragma comment(lib, "winmm.lib")
#endif

#include "RDriver.h"
#include "RDriverInt.h"
#include "MADPrivate.h"

////////////

#define MICROBUF		10L


static char* myerr;

////////////

void W95_PlayStop(MADDriverRec*);

BOOL W95_IsThere(void)
{
	UINT numdevs = waveOutGetNumDevs();
	
	return numdevs>0;
}

void W95_Exit(MADDriverRec *WinMADDriver)
{
	W95_PlayStop(WinMADDriver);

	while(waveOutClose(WinMADDriver->hWaveOut)==WAVERR_STILLPLAYING) Sleep(20);
	GlobalUnlock(WinMADDriver->hglobal);
	GlobalFree(WinMADDriver->hglobal);
}

ULONG GetPos(MADDriverRec *WinMADDriver)
{
	MMTIME mmt;
	mmt.wType = TIME_BYTES;
	
	waveOutGetPosition(WinMADDriver->hWaveOut, &mmt,sizeof(MMTIME));
	
	return(mmt.u.cb & 0xfffffff0);		// A cause du 16 bits??
}

//static Ptr 		currentBuf, currentBuf2;
//static long		currentBufPos, currentBufSize;
//static Boolean	currentBuf2Ready;

/*void CALLBACK TimeProc(
	 UINT  IDEvent,					// identifies timer event 
	 UINT  uReserved,				// not used 
	 DWORD  dwUser,					// application-defined instance data 
	 DWORD  dwReserved1,		// not used 
	 DWORD  dwReserved2			// not used 
)
{
	long todo, pos, i;
	static volatile int timersema=0;
	
	if(++timersema==1)
	{
		pos = GetPos() % WIN95BUFFERSIZE;
		
		if(pos > WIN95BUFFERSIZE/2 && OnOff == true)
		{
			OnOff = false;
			
			if (!DirectSave(mydma, &WinMADDriver->DriverSettings, WinMADDriver))
			{
				for (i = 0; i < WIN95BUFFERSIZE/2; i++) mydma[i] = 0;
			}
		}
		else if (OnOff == false && (pos < WIN95BUFFERSIZE/2))
		{
			OnOff = true;
			
			if (!DirectSave(mydma + WIN95BUFFERSIZE/2, &WinMADDriver->DriverSettings, WinMADDriver))
			{
				for (i = 0; i < WIN95BUFFERSIZE/2; i++) mydma[i] = 0;
			}
		}
	}
	timersema--;
}*/

//TODO: we should probably do something to prevent thread contention
void CALLBACK TimeProc(
	 UINT  IDEvent,
	 UINT  uReserved,
	 DWORD_PTR  dwUser,
	 DWORD_PTR  dwReserved1,
	 DWORD_PTR  dwReserved2
)
{
	char	*dmaDst;
	long	pos /*, todo*/;
	static volatile int timersema=0;

	/* use semaphore to prevent entering
		the mixing routines twice.. do we need this ? */

	if(++timersema==1)
	{
		MADDriverRec	*WinMADDriver = (MADDriverRec*) dwUser;
		
		pos = GetPos(WinMADDriver) % WinMADDriver->WIN95BUFFERSIZE;
		
		if(pos > WinMADDriver->WIN95BUFFERSIZE/2L && WinMADDriver->OnOff == true)
		{
			if (WinMADDriver->MICROBUFState < MICROBUF)
			{
				dmaDst = WinMADDriver->mydma + WinMADDriver->MICROBUFState*WinMADDriver->BufSize;
				
				if (!DirectSave(dmaDst, &WinMADDriver->DriverSettings, WinMADDriver))
				{
					memset(dmaDst, 0, WinMADDriver->BufSize);
				}
				
				WinMADDriver->MICROBUFState++;
			}
			else
			{
				WinMADDriver->OnOff = false;
				WinMADDriver->MICROBUFState = 0;
			}
		}
		else if (WinMADDriver->OnOff == false && (pos < WinMADDriver->WIN95BUFFERSIZE/2L))
		{
			if (WinMADDriver->MICROBUFState < MICROBUF)
			{
				dmaDst = WinMADDriver->mydma + WinMADDriver->WIN95BUFFERSIZE/2L + WinMADDriver->MICROBUFState*WinMADDriver->BufSize;
				
				if (!DirectSave(dmaDst, &WinMADDriver->DriverSettings, WinMADDriver))
				{
					memset(dmaDst, 0, WinMADDriver->BufSize);
				}
				
				WinMADDriver->MICROBUFState++;
			}
			else
			{
				WinMADDriver->OnOff = true;
				WinMADDriver->MICROBUFState = 0;
			}
		}
	}
	timersema--;
}

void W95_PlayStart(MADDriverRec *WinMADDriver)
{
	waveOutSetVolume(0,0xffffffff);
	
	WinMADDriver->WaveOutHdr.lpData= (char*) WinMADDriver->mydata;
	WinMADDriver->WaveOutHdr.dwBufferLength = WinMADDriver->WIN95BUFFERSIZE;
	WinMADDriver->WaveOutHdr.dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
	WinMADDriver->WaveOutHdr.dwLoops = 0xffffffff;
	WinMADDriver->WaveOutHdr.dwUser = 0;
	waveOutPrepareHeader(WinMADDriver->hWaveOut, &WinMADDriver->WaveOutHdr, sizeof(WAVEHDR));
	waveOutWrite(WinMADDriver->hWaveOut, &WinMADDriver->WaveOutHdr, sizeof(WAVEHDR));
	WinMADDriver->mydma = (char*) WinMADDriver->mydata;
	
	WinMADDriver->MICROBUFState = 0;
	
	timeBeginPeriod(20);	/* set the minimum resolution */
	
	WinMADDriver->gwID = timeSetEvent(40,								/* how often                 */
									  40,								/* timer resolution          */
									  TimeProc,							/* callback function         */
									  (unsigned long)  WinMADDriver,	/* info to pass to callback  */
									  TIME_PERIODIC);					/* oneshot or periodic?      */
							  
							  
	//////
}

void W95_PlayStop(MADDriverRec *WinMADDriver)
{
	WinMADDriver->Reading = false;
	MADStopDriver(WinMADDriver);
	
	
	/* stop the timer */
	timeKillEvent(WinMADDriver->gwID);
	timeEndPeriod(20);
	/* stop playing the wave */
	waveOutReset(WinMADDriver->hWaveOut);
	waveOutUnprepareHeader(WinMADDriver->hWaveOut,&WinMADDriver->WaveOutHdr,sizeof(WAVEHDR));
//	VC_PlayStop();
}

	//#define DMODE_STEREO    1
	//#define DMODE_16BITS    2
	//#define DMODE_INTERP    4


Boolean W95_Init(MADDriverRec *WinMADDriver)
{
	MMRESULT err;
	PCMWAVEFORMAT wf;
	//long	i;
	
	WinMADDriver->WIN95BUFFERSIZE = WinMADDriver->BufSize;
	WinMADDriver->WIN95BUFFERSIZE *= 2L;		// double buffer system
	WinMADDriver->WIN95BUFFERSIZE *= MICROBUF;	// multi internal buffer system
	
	WinMADDriver->MICROBUFState = 0;
	
	if (WinMADDriver->WIN95BUFFERSIZE < 0) return false;
	
	WinMADDriver->hglobal= GlobalAlloc(GMEM_FIXED, WinMADDriver->WIN95BUFFERSIZE);	//GMEM_MOVEABLE | GMEM_SHARE
	if (WinMADDriver->hglobal == NULL) return false;
	
	WinMADDriver->mydata = GlobalLock(WinMADDriver->hglobal);
	
	/* get audio device name and put it into the driver structure: */
	waveOutGetDevCaps(0, &WinMADDriver->woc, sizeof(WAVEOUTCAPS));
	
	wf.wf.wFormatTag		=WAVE_FORMAT_PCM;
	wf.wf.nChannels = 2;
	wf.wf.nSamplesPerSec	= WinMADDriver->DriverSettings.outPutRate;
	wf.wBitsPerSample		= WinMADDriver->DriverSettings.outPutBits;
	wf.wf.nBlockAlign		= wf.wf.nChannels * (wf.wBitsPerSample/8);
	wf.wf.nAvgBytesPerSec	= wf.wf.nSamplesPerSec * wf.wf.nBlockAlign;
	
	err = waveOutOpen(&WinMADDriver->hWaveOut, 0, (LPCWAVEFORMATEX)&wf, (unsigned long) 0, (unsigned long) NULL, 0L);
	
	if(err) {
		if(err==WAVERR_BADFORMAT)
			myerr="This output format is not supported (Try another sampling rate?)";
		else if(err==MMSYSERR_ALLOCATED)
			myerr="Audio device already in use";
		else
			myerr="Can't open audio device";
		GlobalUnlock(WinMADDriver->hglobal);
		GlobalFree(WinMADDriver->hglobal);
		return false;
	}
	
	W95_PlayStart(WinMADDriver);
	
	return true;
}
