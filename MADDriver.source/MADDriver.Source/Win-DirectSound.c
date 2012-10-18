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

#include "RDriver.h"
#include "RDriverInt.h"
#include "OBJBASE.h"

#include <cguid.h>

//#define DEBUG

static UINT gwID;
static HWND hwnd;

/*static LPDIRECTSOUND lpDirectSound;
static LPDIRECTSOUNDBUFFER lpDirectSoundBuffer;
static LPDIRECTSOUNDBUFFER lpSwSamp;*/

static 	Ptr 					currentBuf;
static 	Boolean					OnOff;
static 	long					WIN95BUFFERSIZE;

char *TranslateDSError( HRESULT hr )
    {
    switch( hr )
    {
    case DSERR_ALLOCATED:
        return "DSERR_ALLOCATED";

    case DSERR_CONTROLUNAVAIL:
        return "DSERR_CONTROLUNAVAIL";

    case DSERR_INVALIDPARAM:
        return "DSERR_INVALIDPARAM";

    case DSERR_INVALIDCALL:
        return "DSERR_INVALIDCALL";

    case DSERR_GENERIC:
        return "DSERR_GENERIC";

    case DSERR_PRIOLEVELNEEDED:
        return "DSERR_PRIOLEVELNEEDED";

    case DSERR_OUTOFMEMORY:
        return "DSERR_OUTOFMEMORY";

    case DSERR_BADFORMAT:
        return "DSERR_BADFORMAT";

    case DSERR_UNSUPPORTED:
        return "DSERR_UNSUPPORTED";

    case DSERR_NODRIVER:
        return "DSERR_NODRIVER";

    case DSERR_ALREADYINITIALIZED:
        return "DSERR_ALREADYINITIALIZED";

    case DSERR_NOAGGREGATION:
        return "DSERR_NOAGGREGATION";

    case DSERR_BUFFERLOST:
        return "DSERR_BUFFERLOST";

    case DSERR_OTHERAPPHASPRIO:
        return "DSERR_OTHERAPPHASPRIO";

    case DSERR_UNINITIALIZED:
        return "DSERR_UNINITIALIZED";

    default:
        return "Unknown HRESULT";
    }
    }


BOOL AppCreateWritePrimaryBuffer(
    LPDIRECTSOUND lpDirectSound,
    LPDIRECTSOUNDBUFFER *lplpDsb,
    HWND hwnd,
    MADDriverRec	*WinMADDriver)
{
    DSBUFFERDESC dsbdesc;
    HRESULT hr;
    WAVEFORMATEX pcmwf;

    // Set up wave format structure.
    memset(&pcmwf, 0, sizeof(PCMWAVEFORMAT));
    pcmwf.wFormatTag 		= WAVE_FORMAT_PCM;
    
    pcmwf.nChannels = 2;
    pcmwf.nSamplesPerSec		= WinMADDriver->DriverSettings.outPutRate;
    pcmwf.wBitsPerSample		= WinMADDriver->DriverSettings.outPutBits;
    pcmwf.nBlockAlign			= pcmwf.nChannels * (pcmwf.wBitsPerSample/8);
    pcmwf.nAvgBytesPerSec		= pcmwf.nSamplesPerSec * pcmwf.nBlockAlign;
    
    // Set up DSBUFFERDESC structure.
    memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); // Zero it out.
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;	//;	//DSBCAPS_GLOBALFOCUS;	//;
    dsbdesc.dwBufferBytes = 0; // Buffer size is determined
                               // by sound hardware.
    dsbdesc.lpwfxFormat = NULL; // Must be NULL for primary buffers.
    // Obtain write-primary cooperative level.
    hr = lpDirectSound->lpVtbl->SetCooperativeLevel(lpDirectSound, hwnd, DSSCL_PRIORITY);
    if(DS_OK == hr)
    {
        // Succeeded! Try to create buffer.
        hr = lpDirectSound->lpVtbl->CreateSoundBuffer(lpDirectSound,
            &dsbdesc, lplpDsb, NULL);
        if(DS_OK == hr) {
            // Succeeded! Set primary buffer to desired format.
            hr = (*lplpDsb)->lpVtbl->SetFormat(*lplpDsb, &pcmwf);
            (*lplpDsb)->lpVtbl->Play(*lplpDsb,0,0,DSBPLAY_LOOPING);
            return TRUE;
        }
        //DEBUG else debugger( TranslateDSError( hr));
    }
    // If we got here, then we failed SetCooperativeLevel.
    // CreateSoundBuffer, or SetFormat.
    *lplpDsb = NULL;
    return FALSE;
}

AppDetermineHardwareCaps(LPDIRECTSOUND lpDirectSound)
{
    DSCAPS dscaps;
    HRESULT hr;
    dscaps.dwSize = sizeof(DSCAPS);
    hr = lpDirectSound->lpVtbl->GetCaps(lpDirectSound, &dscaps);
    if(DS_OK == hr)
    {
    	printf( "dwFlags = %ld", dscaps.dwFlags);
    	
    //DEBUG	if( dscaps.dwFlags & DSBCAPS_GLOBALFOCUS)	debugger("global");
    //DEBUG	if( dscaps.dwFlags & DSBCAPS_STICKYFOCUS)	debugger("sticky");
  	//DEBUG	if( dscaps.dwFlags & DSBCAPS_PRIMARYBUFFER)	debugger("DSBCAPS_PRIMARYBUFFER");
  		  
        // Succeeded, now parse DSCAPS structure.
        
       /*  printf("Hardware Buffers:      %i\n",dscaps.dwSize);
         printf("Hardware Memory:       %i\n",dscaps.dwMinSecondarySampleRate);
         printf("Hardware Free Memory:  %i\n",dscaps.dwFreeHwMemBytes);
         printf( "Byebye");*/
    }
    // .
    // .
    // .
}

BOOL WriteDataToBuffer(
    LPDIRECTSOUNDBUFFER lpDsb,
    DWORD dwOffset,
    LPBYTE lpbSoundData,
    DWORD dwSoundBytes)
{
    LPVOID lpvPtr1;
    DWORD dwBytes1;
    LPVOID lpvPtr2;
    DWORD dwBytes2;
    HRESULT hr;
    
	//	lpDirectSound->lpVtbl->SetCooperativeLevel( lpDirectSound, hwnd, DSSCL_EXCLUSIVE);
		
    // Obtain write pointer.
    hr = lpDsb->lpVtbl->Lock(lpDsb, dwOffset, dwSoundBytes, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);

    // If we got DSERR_BUFFERLOST, restore and retry lock.
    if(DSERR_BUFFERLOST == hr) {
        lpDsb->lpVtbl->Restore(lpDsb);
        hr = lpDsb->lpVtbl->Lock(lpDsb, dwOffset, dwSoundBytes, &lpvPtr1,
             &dwBytes1, &lpvPtr2, &dwBytes2, 0);
    }
    if(DS_OK == hr) {
        // Write to pointers.
        CopyMemory(lpvPtr1, lpbSoundData, dwBytes1);
        if(NULL != lpvPtr2) {
            CopyMemory(lpvPtr2, lpbSoundData+dwBytes1, dwBytes2);
        }
        // Release the data back to DirectSound.
        hr = lpDsb->lpVtbl->Unlock(lpDsb, lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);
        if(DS_OK == hr) {
            // Success!
            return TRUE;
        }
    }
    
//		lpDirectSound->lpVtbl->SetCooperativeLevel( lpDirectSound, hwnd, DSSCL_NORMAL);
    
    // If we got here, then we failed Lock, Unlock, or Restore.
    return FALSE;
}

BOOL LoadSamp(LPDIRECTSOUND lpDirectSound,
              LPDIRECTSOUNDBUFFER *lplpDsb,
              LPBYTE samp, UINT length, UINT flags, MADDriverRec *WinMADDriver)
{
    DSBUFFERDESC dsbdesc;
    HRESULT hr;
    WAVEFORMATEX pcmwf;

    // Set up wave format structure.
    memset(&pcmwf, 0, sizeof(PCMWAVEFORMAT));
    pcmwf.wFormatTag			= WAVE_FORMAT_PCM;
    pcmwf.nChannels = 2;
    pcmwf.nSamplesPerSec		= WinMADDriver->DriverSettings.outPutRate;
    pcmwf.wBitsPerSample		= WinMADDriver->DriverSettings.outPutBits;
    pcmwf.nBlockAlign			= pcmwf.nChannels * (pcmwf.wBitsPerSample/8);
    pcmwf.nAvgBytesPerSec		= pcmwf.nSamplesPerSec * pcmwf.nBlockAlign;
    
    // Set up DSBUFFERDESC structure.
    memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); // Zero it out.
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
//    dsbdesc.dwFlags =	DSBCAPS_STATIC | DSBCAPS_CTRLDEFAULT | DSBCAPS_GETCURRENTPOSITION2;	//;	//;	// | flags;	//DSBCAPS_STATIC | DSBCAPS_CTRLPAN|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLFREQUENCY| DSBCAPS_GLOBALFOCUS|DSBCAPS_GETCURRENTPOSITION2|flags;
  
//  	dsbdesc.dwFlags =	DSBCAPS_CTRLPAN|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLFREQUENCY|flags;
  	
  	dsbdesc.dwFlags = 0;
    dsbdesc.dwBufferBytes = length;
    dsbdesc.dwReserved = 0;
    dsbdesc.lpwfxFormat = &pcmwf;
    hr = lpDirectSound->lpVtbl->CreateSoundBuffer(lpDirectSound, &dsbdesc, lplpDsb, NULL);
    if(hr == DS_OK)
    {
//        lpDirectSound->lpVtbl->SetCooperativeLevel(
//                lpDirectSound,hwnd, DSSCL_EXCLUSIVE);
        // Succeeded! Valid interface is in *lplpDsb.
  //      WriteDataToBuffer(*lplpDsb, 0, samp,length);
//        lpDirectSound->lpVtbl->SetCooperativeLevel(
//                lpDirectSound,hwnd, DSSCL_NORMAL);
    }
    else
    {
    	//DEBUG debugger( TranslateDSError( hr));
    	
      *lplpDsb=NULL;
      return 0;
    }
    return 1;
}

static void CALLBACK TimeProc(
	 UINT  IDEvent,			/* identifies timer event */
	 UINT  uReserved,		/* not used */
	 DWORD  dwUser,			/* application-defined instance data */
	 DWORD  dwReserved1,	/* not used */
	 DWORD  dwReserved2		/* not used */
)
{
	DWORD 		pos, posp, i;
	static volatile int timersema=0;

	/* use semaphore to prevent entering the mixing routines twice.. do we need this ? */

	if(++timersema==1)
	{
		MADDriverRec	*WinMADDriver = (MADDriverRec*) dwUser;
		
		WinMADDriver->lpSwSamp->lpVtbl->GetCurrentPosition( WinMADDriver->lpSwSamp, &pos, &posp);
		
		if(pos > WIN95BUFFERSIZE/2 && OnOff == true)
		{
			OnOff = false;
			
			if( !DirectSave( currentBuf, 0L, WinMADDriver))
			{
				switch( WinMADDriver->DriverSettings.outPutBits)
				{
					case 8:
						for( i = 0; i < WIN95BUFFERSIZE/2; i++) currentBuf[ i] = 0x80;
					break;
					
					case 16:
						for( i = 0; i < WIN95BUFFERSIZE/2; i++) currentBuf[ i] = 0;
					break;
				}
			}
			
			if( !WriteDataToBuffer( WinMADDriver->lpSwSamp, 0, (unsigned char*) currentBuf, WIN95BUFFERSIZE/2))
			{
				//DEBUG 	debugger("ERR");
			}
		}
		else if( OnOff == false && (pos < WIN95BUFFERSIZE/2))
		{
			OnOff = true;
			
			if( !DirectSave( currentBuf + WIN95BUFFERSIZE/2, 0L, WinMADDriver))
			{
				switch( WinMADDriver->DriverSettings.outPutBits)
				{
					case 8:
						for( i = WIN95BUFFERSIZE/2; i < WIN95BUFFERSIZE; i++) currentBuf[ i] = 0x80;
					break;
					
					case 16:
						for( i = WIN95BUFFERSIZE/2; i < WIN95BUFFERSIZE; i++) currentBuf[ i] = 0;
					break;
				}
			}
			
			if( !WriteDataToBuffer( WinMADDriver->lpSwSamp, WIN95BUFFERSIZE/2, (unsigned char*) (currentBuf + WIN95BUFFERSIZE/2), WIN95BUFFERSIZE/2))
			{
				//DEBUG 	debugger("ERR");
			}
		}
		
		if( WIN95BUFFERSIZE - pos > 1700)	WinMADDriver->OscilloWavePtr = currentBuf + pos;
		else WinMADDriver->OscilloWavePtr = currentBuf;
	}
	timersema--;
}


Boolean DirectSoundInit( MADDriverRec* WinMADDriver)
{
	OnOff					= false;
	
	WIN95BUFFERSIZE = WinMADDriver->BufSize;
	WIN95BUFFERSIZE *= 2L;								// double buffer system
	
	currentBuf 		= calloc( WIN95BUFFERSIZE, 1);
	
	hwnd = GetForegroundWindow();	//GetForegroundWindow();
	if( !hwnd) return false;
	
	if(DS_OK == DirectSoundCreate(NULL, &WinMADDriver->lpDirectSound, NULL))
	{
		if( !AppCreateWritePrimaryBuffer( WinMADDriver->lpDirectSound, &WinMADDriver->lpDirectSoundBuffer, hwnd, WinMADDriver))
		{
			WinMADDriver->lpDirectSound = 0L;
			return false;
		}
		if( !WinMADDriver->lpDirectSoundBuffer) return false;
		
		// Creation succeeded.
		WinMADDriver->lpDirectSound->lpVtbl->SetCooperativeLevel(WinMADDriver->lpDirectSound, hwnd, DSSCL_NORMAL);
		
		WinMADDriver->lpSwSamp = 0L;
		if( !LoadSamp(WinMADDriver->lpDirectSound, &WinMADDriver->lpSwSamp, 0L, WIN95BUFFERSIZE, DSBCAPS_LOCSOFTWARE, WinMADDriver))
		{
			//DEBUG debugger( "Error 2\n");		//DSBCAPS_LOCSOFTWARE
			WinMADDriver->lpDirectSound = 0L;
			return false;
		}
		
		if( !WinMADDriver->lpSwSamp) return false;
		
		WinMADDriver->lpSwSamp->lpVtbl->Play(WinMADDriver->lpSwSamp, 0, 0, DSBPLAY_LOOPING);
		
		///////////
		
		timeBeginPeriod(20);      /* set the minimum resolution */
		
		/*  Set up the callback event.  The callback function
		 *  MUST be in a FIXED CODE DLL!!! -> not in Win95
		 */
		 
		// debugger( "timeSetEvent\n");
		 
		gwID = timeSetEvent(	  40,   												/* how often                 */
													  40,   							/* timer resolution          */
													  TimeProc,  						/* callback function         */
													  (unsigned long) WinMADDriver,		/* info to pass to callback  */
													  TIME_PERIODIC); 					/* oneshot or periodic?      */
		
		if( gwID == 0) return false;
		else return true;
	}
	
	WinMADDriver->lpDirectSound = 0L;
	
	return false;
}

void DirectSoundClose( MADDriverRec* WinMADDriver)
{
	if( WinMADDriver->lpDirectSound)
	{
		/* stop the timer */
		timeKillEvent( gwID);
		timeEndPeriod( 20);
		
		WinMADDriver->lpSwSamp->lpVtbl->Stop( WinMADDriver->lpSwSamp);
		WinMADDriver->lpSwSamp->lpVtbl->Release( WinMADDriver->lpSwSamp);
		WinMADDriver->lpSwSamp = 0L;
		
		WinMADDriver->lpDirectSoundBuffer->lpVtbl->Stop(WinMADDriver->lpDirectSoundBuffer);
  		WinMADDriver->lpDirectSoundBuffer->lpVtbl->Release(WinMADDriver->lpDirectSoundBuffer);
  
		WinMADDriver->lpDirectSound->lpVtbl->Release( WinMADDriver->lpDirectSound);
		WinMADDriver->lpDirectSound = NULL;
	}
}