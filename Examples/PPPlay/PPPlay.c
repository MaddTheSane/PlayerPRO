/*
REQUIRE THESE LIBRARIES:

DSOUND.LIB
WINMM.LIB
COMDLG32.LIB
GDI32.lib
kernel32.lib
user32.lib
winspool.lib
*/

#ifdef __APPLE__
#include <PlayerPROCore/PlayerPROCore.h>
#else
#include "RDriver.h"
#endif

#define PPbanner \
"=======================================================================\n" \
"PlayerPRO Driver v 5.9  - Portable version - Programmed by A.ROSSET '02\n" \
"=======================================================================\n" \
"       DirectSound or Wave Version for Windows 95 or Windows NT        \n" \
"\n" \
"=======================================================================\n" \
"       This program is FREEWARE - Read README.TXT for more info\n" \
"\n" \
"     E-Mail : rossetantoine@bluewin.ch    Phone : (+41 79) 203 74 62\n"\
"=======================================================================\n" \
" Current settings: 44Khz-16bits-Stereo-MicroDelay 35ms-Interpolation \n" \
"=======================================================================\n"

void debugger( Ptr a)
{
	printf( a);
//	getchar();
}

int main( int argc, char *argv[])
{
	MADDriverSettings	init;
	MADDriverRec		*aDriver;
	MADLibrary			*lib;
	MADMusic 			*aMusic;
  	long				fT, cT;
  	char 				type[ 5];
  	
  	puts( PPbanner);
  	
  	if (argc < 2)
  	{
		puts("Usage: PlayerPRO MyMusicFile\n");
	}
  	else
  	{
		init.numChn				= 4;
		init.outPutBits 		= 16;
		init.outPutRate			= 44100;
		init.outPutMode			= DeluxeStereoOutPut;
		init.driverMode			= DirectSound95NT;
		
		// You can also try: init.driverMode = Wave95NT;
		
		init.repeatMusic		= false;
		init.MicroDelaySize 	= 0;
		init.surround		 	= false;
		init.Reverb				= false;
		init.ReverbSize			= 45;
		init.ReverbStrength		= 50;
		init.TickRemover		= false;
		init.oversampling		= 1;
		
		if( MADInitLibrary( "Plugins", &lib)) {debugger( "ERR MADInitLibrary\n");}
		else
		{
			if( MADCreateDriver( &init, lib, &aDriver)) {debugger( "ERR MADCreateDriver\n");}
			else
			{
				if( MADMusicIdentifyCString( lib, type, argv[ 1]))  {debugger( "ERR MADMusicIdentifyCString\n");}
				else
				{
					if( MADLoadMusicFileCString( lib, &aMusic, type, argv[ 1])) {debugger( "ERR MADLoadMusicFile\n");}
					else
					{
						double fTd = 0;
						MADAttachDriverToMusic( aDriver, aMusic, 0L);
						
						MADStartDriver( aDriver);
						
						MADPlayMusic( aDriver);
						
						MADGetMusicStatus( aDriver, &fT, &cT);
						
						fTd = fT / 60.0;
						
						printf("\n\nMusic length: %.2f s", fTd);
						printf("\nPress Return to exit\n");
						
						getchar();
						
						MADDisposeMusic( &aMusic, aDriver);
					}
				}
				MADStopMusic( aDriver);
				MADStopDriver( aDriver);
						
				MADDisposeDriver( aDriver);
			}
			MADDisposeLibrary( lib);
		}
	}
}
