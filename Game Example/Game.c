/********************************************************/
/*
	Player PRO 5.0 -- Music Driver EXAMPLE
	
	To use with MADLibrary 5.x for Think C & CodeWarrior

	Antoine ROSSET
	16 Tranchees
	1206 GENEVA
	SWITZERLAND
	
	FAX: (+41 22) 346 11 97
	PHONE: (+41 79) 203 74 62
	Email: rosset@dial.eunet.ch
*/
/********************************************************/

#include <Carbon/Carbon.h>
#include <PlayerPROCore/PlayerPROCore.h>
#include "Game.h"

static MADDriverRec	*MADDriver;
static MADMusic		*Music;
static MADLibrary	*MADLib;

/*****************************/
/****** MAIN FUNCTION ********/
/*****************************/

int main( int argc, char* argv[])
{
	DialogPtr		TheDia;
	Handle			itemHandle;
	Rect			itemRect;
	int				i;
	short			itemType, itemHit;
	Handle			mySound[4];			// my 4 sound resources
	
	/* Initialisation de la toolbox */
	
	InitCursor();
	
	/******************************************/
	/*** Load and prepare my sound resource ***/
	/******************************************/
	
	for( i = 0 ; i < 4; i++)
	{
		mySound[ i] = GetResource( 'snd ', kStartSound + i);
		DetachResource( mySound[ i]);
		HLock( mySound[ i]);							// VERY IMPORTANT !!!!!!
	}
	
	
	/*******************************************************************************************/
	/****** MAD Library Initialization : choose the best driver for the current hardware  ******/
	/*******************************************************************************************/
	
	{
		MADDriverSettings	init;
		OSErr				err;
		
		err = MADInitLibrary(NULL, false, &MADLib);
		if( err) DebugStr("\pMADInitLibrary Err");
		
		MADGetBestDriver( &init);
		
		err = MADCreateDriver( &init, MADLib, &MADDriver);
		if( err) DebugStr("\pMADCreateDriver Err");
	}
	
	/***************************************************/
	/***   Open MADK Resource ID 3214 and play it !   **/
	/***************************************************/
	
	if( MADLoadMusicRsrc( &Music, kMADType, kMADKmusic) != noErr)
		DebugStr("\pMADLoadMusicRsrc Err");
	
	MADAttachDriverToMusic( MADDriver, Music, NULL);
	
	if( MADStartDriver( MADDriver) != noErr) DebugStr("\pMADStartDriver Err");
	
	/******************************************/
	/***        Open my dialog              ***/
	/******************************************/
	
	TheDia = GetNewDialog( DiagNum, NULL, (WindowPtr) -1L);
	SetPortDialogPort(TheDia);
	
	do{
		ModalDialog( NULL, &itemHit);
		
		switch( itemHit)
		{
			case 2:
				GetDialogItem( TheDia, 2, &itemType, &itemHandle, &itemRect);
				SetControlValue( (ControlHandle) itemHandle, !GetControlValue( (ControlHandle) itemHandle));
				
				if( GetControlValue( (ControlHandle) itemHandle)) MADPlayMusic( MADDriver);
				else
				{
					MADStopMusic( MADDriver);
					MADCleanDriver( MADDriver);
				}
				break;
				
			case 3:
				MADPlaySndHandle( MADDriver, mySound[ 0], 0, 48);			// On track ID 0, C 3 = 48
				break;
				
			case 4:
				MADPlaySndHandle( MADDriver, mySound[ 1], 1, 0xFF);		// On track ID 0, 0xFF = normal rate (sample rate)
				break;
				
			case 5:
				MADPlaySndHandle( MADDriver, mySound[ 2], 2, 36);			// On track ID 1, at C 2 = 36
				break;
				
			case 6:
				MADPlaySndHandle( MADDriver, mySound[ 3], 3, 60);			// On track ID 2, at C 4 = 60
				break;
		}
		
	}while( itemHit != 1);
	
	DisposeDialog( TheDia);
	
	MADStopMusic( MADDriver);					// Stop the music
	MADStopDriver( MADDriver);					// Stop the driver
	MADDisposeMusic( &Music, MADDriver);		// Clear music
	MADDisposeDriver( MADDriver);				// Dispose driver
	MADDisposeLibrary( MADLib);					// Close Music Library
	
	for( i = 0 ; i < 4; i++)
	{
		HUnlock( mySound[ i]);
		DisposeHandle( mySound[ i]);
	}
	return EXIT_SUCCESS;
}

