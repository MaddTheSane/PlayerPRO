/********************						***********************/
//
//	Player PRO 5.0 -- MAD Class for BeOS -
//
//	Library Version 5.0
//
//	To use with MAD Library for BeOS: CodeWarrior
//
//	Antoine ROSSET
//	16 Tranchees
//	1206 GENEVA
//	SWITZERLAND
//
//	Thank you for your interest in PlayerPRO !
//
//	Special Thanks to:
//
//	Dario Accornero <adario@cs.bu.edu>
//
//	For his BeOS support and help!
//
//	FAX:			(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 		rossetantoine@bluewin.ch
//
/********************						***********************/

//	System headers.

#include <Path.h>
#include <Directory.h>
#include <List.h>
#include <string.h>

//	Local headers.

#define DRIVERCLASS_MODULE	1
#include "Be-DriverClass.h"
#include "MADDriver.h"

//	Streaming function interfacing with BeOS.
static void trackerStreamPlay(void *theCookie, void *buffer, size_t size, const media_raw_audio_format &format);

//	Low-level streaming function from PlayerPROCore.

extern "C" {
Boolean	DirectSave(char	*myPtr, MADDriverSettings *driverType, MADDriverRec *intDriver);
}

//	Constructors.
MADDriverClass::MADDriverClass()
{
	MADDriverSettings init = CreateDefaultDriver();
	inited = InitLibrary(&init);
}

MADDriverClass::MADDriverClass(MADDriverSettings *init)
{
	inited = InitLibrary(init);
}

//	Destructor: remove system streamers before disposing of the library.
MADDriverClass::~MADDriverClass()
{
	if (streamPlayer) {
		streamPlayer->Stop();
		delete streamPlayer;
	}
	
	StopMusic(true);
	
	if (curDriverRec) {
		MADDisposeDriver(curDriverRec);
		MADDisposeLibrary(MADLib);
	}
}

//	Driver initialization: optimal values for BeOS.

static MADDriverSettings MADDriverClass::CreateDefaultDriver(void)
{
	MADDriverSettings	init;
	init.numChn			= 4;
	init.outPutBits 	= 16;
	init.outPutRate		= 44100;
	init.outPutMode		= DeluxeStereoOutPut;
	init.driverMode		= BeOSSoundDriver;
	init.repeatMusic	= true;
	init.MicroDelaySize = 0;
	init.surround		= false;
	init.Reverb			= false;
	init.ReverbSize		= 45;
	init.ReverbStrength	= 60;
	init.TickRemover	= true;

	return	init;
}

//	Library initialization.

bool MADDriverClass::InitLibrary(MADDriverSettings *init)
{
	//	Reset data members to indicate that initialization isn't complete.
	
	libraryError = noErr;
	inited = false;
	musicPlay = false;
	curDriverRec = NULL;
	curMusic = NULL;
	settings = *init;
	
	//	Init the library: default plugins folder is "add-ons".

	if ((libraryError = MADInitLibrary("add-ons", &MADLib)) != noErr) {
#if	DRIVERCLASS_DEBUG
		debugger("Cannot initialize library.");
#else
		return	false;
#endif
	}
	
	//	Init the driver.
	
	if ((libraryError = MADCreateDriver(init, MADLib, &curDriverRec)) != noErr) {
#if	DRIVERCLASS_DEBUG
		debugger("Cannot create driver.");
#else
		return false;
#endif
	}

	//	Init system streamers.
	
	media_raw_audio_format format;
	
	format.frame_rate = init->outPutRate;
	format.channel_count = 2;			// Always stereo !
	format.format = B_AUDIO_FLOAT;		// Always float !
	format.byte_order = 1;
	format.buffer_size = (curDriverRec->BufSize * sizeof(float)) / (init->outPutBits / 8);
	
	streamPlayer = new BSoundPlayer(&format, "PP_player", trackerStreamPlay, NULL, this);
	
	streamPlayer->SetHasData(true);
	streamPlayer->Start();
	
	return true;
}

//	Load a music file into memory and optionally play it.

bool MADDriverClass::LoadMusic(entry_ref* ref, OSType type, bool playIt)
{
	//	Safety check.
	if (!inited)
		return false;
	
	//	Try to import the file.
		
	BEntry		fileEntry(ref);
	BPath		filePath;
	fileEntry.GetPath(&filePath);
	char		plugName[5];
	
	libraryError = MADMusicIdentifyCString(MADLib, plugName, filePath.Path());
	if (libraryError == noErr) {
		libraryError = MADLoadMusicFileCString( MADLib, &curMusic, plugName, filePath.Path() );
		if (libraryError == noErr) {
			MADAttachDriverToMusic(curDriverRec, curMusic);
			if (playIt)
				StartMusic();
		}
	}
	
	if (libraryError)
		return false;
	else
		return true;
}

//	Start playing current music.

void MADDriverClass::StartMusic(void)
{
	//	Safety check.
	
	if (!inited)
		return;
	
	if (MADStartDriver(curDriverRec)) {
#if	DRIVERCLASS_DEBUG
		debugger("Error in MADStartDriver()");
#endif
	}
	MADPlayMusic(curDriverRec);
	musicPlay = true;
}

//	Pause/play music.

void MADDriverClass::PauseMusic(void)
{
	//	Safety check.
	
	if (!inited)
		return;

	if (musicPlay) {
		//	Music is playing: halt it without resetting it.
		
		MADStopMusic(curDriverRec);
		MADStopDriver(curDriverRec);
		musicPlay = false;
	} else {
		//	Start driver and music.
		
		MADStartDriver(curDriverRec);
		MADPlayMusic(curDriverRec);
		musicPlay = true;
	}
}

//	Stop playing current music and optionally discard it.

void MADDriverClass::StopMusic(bool discardIt)
{
	//	Safety check.
	
	if (!inited)
		return;

	//	Stop music and reset its position.
	
	MADStopMusic(curDriverRec);
	MADReset(curDriverRec);
	MADStopDriver(curDriverRec);
	musicPlay = false;

	//	Possibly dispose of current music.
	
	if (discardIt) {
		if (curMusic) {
			MADDisposeMusic(&curMusic);
			curMusic = NULL;
		}
	}
}

//	Check if current music is over.
bool MADDriverClass::MusicEnd(void)
{
	//	Safety checks.
	
	if (!inited)
		return false;
	
	if ( musicPlay)
		return false;
		
	//	Return library flag.
	
	return MADIsDonePlaying(curDriverRec);
}

//	Streamer interfacing with the system.
static void	trackerStreamPlay(void *user, void *inbuffer, size_t count, const media_raw_audio_format &format)
{
	char*	buffer = (char*)inbuffer;
	long	i;
	
	
	//	Driver object.
	
	MADDriverClass* curDriver = (MADDriverClass*)user;
	MADDriverRec*	curMADDriver = curDriver->curDriverRec;
	
	//	If playing is disabled, do not alter the current buffer.
	
	if (!curDriver->musicPlay)
		return;
	
	//	Check buffer size.
	
	if (count/sizeof(float) != curMADDriver->BufSize / (curMADDriver->DriverSettings.outPutBits / 8)) {
#if	DRIVERCLASS_DEBUG
		debugger("count error");
#else
		return;
#endif
	}
	
	DirectSave(curMADDriver->IntDataPtr, &curMADDriver->DriverSettings, curMADDriver);
	
	switch(format.format) {
		case B_AUDIO_FLOAT:
			float 	*floatbuffer = (float*)buffer;
			
			switch(curMADDriver->DriverSettings.outPutBits)
		{
			case 16:
				short *shortbuffer = (short*)curMADDriver->IntDataPtr;
				for(i = 0; i < count/sizeof(float); i++)
					floatbuffer[i] = ((float)shortbuffer[i] * 1.0) / 32767.0;
				break;
				
			case 8:
				Byte *charbuffer = (Byte*)curMADDriver->IntDataPtr;
				for(i = 0; i < count/sizeof(float); i++)
					floatbuffer[i] = (((float)charbuffer[i]- 0x80) * 1.0) / 127.0;
				break;
		}
			break;
			
		default:
			// SUPPORTS ONLY the float format! BeOS driver works in float.
			return;
			break;
	}
}
