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

#ifndef __DRIVERCLASSH__
#define __DRIVERCLASSH__

//	System headers.
#include <MediaDefs.h>
#include <SoundPlayer.h>
#include <OS.h>
#include <Entry.h>
#include <File.h>
#include <Bitmap.h>
#include <NodeInfo.h>

//	Low-level music driver.
#include "RDriver.h"

//	Debugging.
//	Set to 1 if debugging messages are needed.
#define	DRIVERCLASS_DEBUG	1


//	Audio formats available

enum {	/* for "format" */
	B_AUDIO_UCHAR = 0x11,	/* 128 == mid, 1 == bottom, 255 == top */
	B_AUDIO_SHORT = 0x2,	/* 0 == mid, -32767 == bottom, +32767 == top */
	B_AUDIO_FLOAT = 0x24,	/* 0 == mid, -1.0 == bottom, 1.0 == top */
	B_AUDIO_INT = 0x4		/* 0 == mid, 0x80000001 == bottom, 0x7fffffff == top */
};

//	Inline functions.
//	Convert an OSType (MacOS) to a string.
static inline char* TypeToString(OSType type, char *string)
{
	OSType2Ptr(type, string);
	return string;
}

//	Main MADDriver class:
#pragma	mark	MADDriverClass
_EXPORT	class	MADDriverClass
{
public:
	//	Flavourful constructors.
	MADDriverClass();
	MADDriverClass(MADDriverSettings *init);
	
	//	Destructor.
	virtual ~MADDriverClass();
	
	//	Loading/checking music files.
	bool	LoadMusic(entry_ref* ref, OSType type, bool playIt = true);
	bool	LoadMADHMusic(BFile& file, bool playIt = true);
	bool	ValidMusicFile(entry_ref* ref, OSType* type);
	bool	ValidMADHFile(BFile& file, OSType* type);
	BList*	FindMusics(const char *musicPath);
	
	//	Music control.
	void			StartMusic(void);
	void			PauseMusic(void);
	void			StopMusic(bool discardIt = false);
	bool			MusicEnd(void);
	inline	bool	MusicPlaying(void)
	{
		return	musicPlay;
	}
	inline	bool	MusicLoaded(void)
	{
		return curMusic != NULL;
	}
	
	//	Data members:
	MADErr				libraryError;	//	Last error returned by MADLibrary.
	bool				inited;			//	Successful initialization.
	bool				musicPlay;		//	Music playing flag.
	MADLibrary 			*MADLib;
	MADDriverRecPtr		curDriverRec;	//	Current driver.
	MADMusic			*curMusic;		//	Current music.
	MADDriverSettings	settings;		//	Current settings for driver.
	
private:
	
	//	Library initialization.
	static MADDriverSettings	CreateDefaultDriver(void);
	bool						InitLibrary(MADDriverSettings &init);
	
	//	System streamers.
	BSoundPlayer				*streamPlayer;
};
#endif
