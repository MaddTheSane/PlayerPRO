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
#include "mmsystem.h"

#include "RDriver.h"
#include "RDriverInt.h"

////////////

#define MICROBUF		10L


static	char*			myerr;
//static 	MADDriverRec	*WinMADDriver;

////////////

static WAVEOUTCAPS 	woc;
static HWAVEOUT 		hWaveOut;
static LPVOID 			mydata;
static HGLOBAL 			hglobal;
static WAVEHDR 			WaveOutHdr;
static UINT 				gwID;
static char 				*mydma;
static Boolean 			OnOff;
static long 				WIN95BUFFERSIZE;
static long					MICROBUFState;


void W95_PlayStop( MADDriverRec*);

BOOL W95_IsThere(void)
{
	UINT numdevs = waveOutGetNumDevs();
	
	return numdevs>0;
}

void W95_Exit( MADDriverRec *WinMADDriver)
{
	W95_PlayStop( WinMADDriver);

	while(waveOutClose(hWaveOut)==WAVERR_STILLPLAYING) Sleep(20);
	GlobalUnlock(hglobal);
	GlobalFree(hglobal);
}

ULONG GetPos(void)
{
	MMTIME mmt;
	mmt.wType = TIME_BYTES;
	
	waveOutGetPosition( hWaveOut, &mmt,sizeof(MMTIME));
	
	return( mmt.u.cb & 0xfffffff0);		// A cause du 16 bits??
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
			
			if( !DirectSave( mydma, &WinMADDriver->DriverSettings, WinMADDriver))
			{
				for( i = 0; i < WIN95BUFFERSIZE/2; i++) mydma[ i] = 0;
			}
		}
		else if( OnOff == false && (pos < WIN95BUFFERSIZE/2))
		{
			OnOff = true;
			
			if( !DirectSave( mydma + WIN95BUFFERSIZE/2, &WinMADDriver->DriverSettings, WinMADDriver))
			{
				for( i = 0; i < WIN95BUFFERSIZE/2; i++) mydma[ i] = 0;
			}
		}
	}
	timersema--;
}*/

void CALLBACK TimeProc(
	 UINT  IDEvent,
	 UINT  uReserved,
	 DWORD  dwUser,
	 DWORD  dwReserved1,
	 DWORD  dwReserved2
)
{
	char	*dmaDst;
	long	todo, pos, i;
	static volatile int timersema=0;

	/* use semaphore to prevent entering
		the mixing routines twice.. do we need this ? */

	if(++timersema==1)
	{
		MADDriverRec	*WinMADDriver = (MADDriverRec*) dwUser;
		
		pos = GetPos() % WIN95BUFFERSIZE;
		
		if(pos > WIN95BUFFERSIZE/2L && OnOff == true)
		{
			if( MICROBUFState < MICROBUF)
			{
				dmaDst = mydma + MICROBUFState*WinMADDriver->BufSize;
				
				if( !DirectSave( dmaDst, &WinMADDriver->DriverSettings, WinMADDriver))
				{
					for( i = 0; i < WinMADDriver->BufSize; i++) dmaDst[ i] = 0;
				}
				
				MICROBUFState++;
			}
			else
			{
				OnOff = false;
				MICROBUFState = 0;
			}
		}
		else if( OnOff == false && (pos < WIN95BUFFERSIZE/2L))
		{
			if( MICROBUFState < MICROBUF)
			{
				dmaDst = mydma + WIN95BUFFERSIZE/2L + MICROBUFState*WinMADDriver->BufSize;
				
				if( !DirectSave( dmaDst, &WinMADDriver->DriverSettings, WinMADDriver))
				{
					for( i = 0; i < WinMADDriver->BufSize; i++) dmaDst[ i] = 0;
				}
				
				MICROBUFState++;
			}
			else
			{
				OnOff = true;
				MICROBUFState = 0;
			}
		}
	}
	timersema--;
}

void W95_PlayStart( MADDriverRec *WinMADDriver)
{
	waveOutSetVolume(0,0xffffffff);
	
	WaveOutHdr.lpData= (char*) mydata;
	WaveOutHdr.dwBufferLength=WIN95BUFFERSIZE;
	WaveOutHdr.dwFlags=WHDR_BEGINLOOP|WHDR_ENDLOOP;
	WaveOutHdr.dwLoops=0xffffffff;
	WaveOutHdr.dwUser=0;
	waveOutPrepareHeader(hWaveOut,&WaveOutHdr,sizeof(WAVEHDR));
	waveOutWrite(hWaveOut,&WaveOutHdr,sizeof(WAVEHDR));
	mydma= (char*) mydata;
	
	MICROBUFState = 0;
	
	timeBeginPeriod( 20);      /* set the minimum resolution */
	
	gwID = timeSetEvent(	  40,   											/* how often                 */
							  40,   																/* timer resolution          */
							  TimeProc,  														/* callback function         */
							 (unsigned long)  WinMADDriver,    			/* info to pass to callback  */
							  TIME_PERIODIC); 											/* oneshot or periodic?      */
							  
							  
	//////
}

void W95_PlayStop( MADDriverRec *WinMADDriver)
{
	WinMADDriver->Reading = false;
	MADStopDriver( WinMADDriver);
	
	
	/* stop the timer */
	timeKillEvent(gwID);
	timeEndPeriod(20);
	/* stop playing the wave */
	waveOutReset(hWaveOut);
	waveOutUnprepareHeader(hWaveOut,&WaveOutHdr,sizeof(WAVEHDR));
//	VC_PlayStop();
}

	#define DMODE_STEREO    1
	#define DMODE_16BITS    2
	#define DMODE_INTERP    4


Boolean W95_Init( MADDriverRec *WinMADDriver)
{
	MMRESULT err;
	PCMWAVEFORMAT wf;
	long	i;
	
	WIN95BUFFERSIZE = WinMADDriver->BufSize;
	WIN95BUFFERSIZE *= 2L;								// double buffer system
	WIN95BUFFERSIZE *= MICROBUF;					// multi internal buffer system
	
	MICROBUFState = 0;
	
	if( WIN95BUFFERSIZE < 0) return false;
	
	hglobal= GlobalAlloc( GMEM_FIXED, WIN95BUFFERSIZE);	//GMEM_MOVEABLE | GMEM_SHARE
	if( hglobal == NULL) return false;
	
	mydata = GlobalLock(hglobal);
	
	/* get audio device name and put it into the driver structure: */
	waveOutGetDevCaps( 0, &woc, sizeof(WAVEOUTCAPS));
	
	wf.wf.wFormatTag		=WAVE_FORMAT_PCM;
	wf.wf.nChannels = 2;
	wf.wf.nSamplesPerSec	= WinMADDriver->DriverSettings.outPutRate;
	wf.wBitsPerSample		= WinMADDriver->DriverSettings.outPutBits;
	wf.wf.nBlockAlign		= wf.wf.nChannels * (wf.wBitsPerSample/8);
	wf.wf.nAvgBytesPerSec	= wf.wf.nSamplesPerSec * wf.wf.nBlockAlign;
	
	err=waveOutOpen( &hWaveOut, 0, (LPCWAVEFORMATEX)&wf, (unsigned long) 0, (unsigned long) NULL,0L);
	
	if(err)
	{
		if(err==WAVERR_BADFORMAT)
			myerr="This output format is not supported (Try another sampling rate?)";
		else if(err==MMSYSERR_ALLOCATED)
			myerr="Audio device already in use";
		else
			myerr="Can't open audio device";
		GlobalUnlock(hglobal);
		GlobalFree(hglobal);
		return false;
	}
	
	W95_PlayStart( WinMADDriver);
	
	return true;
}
