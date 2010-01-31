//TODO: CoreMIDI back-end
#ifdef MAINPLAYERPRO
#include "Shuddup.h"
#endif
#include "MAD.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include "MIDI.h"
#include <stdio.h>
#include <CoreMIDI/CoreMIDI.h>

#define refConTime			1L
#define refConOutput		3L
#define squidInputBufSize	16384	/* 16K */
#define recordingBufSize	10000	


	extern 		Boolean				PianoRecording, PianoMIDIInput;
	extern 		DialogPtr			InstruListDlog, EditorDlog;
	extern		short				LastCanal;
	extern		WindowPtr			oldWindow;
	
	extern 		short				TouchMem[11];
	extern		short				TrackMem[11];
	extern 		short				TouchIn;
	extern		DialogPtr			PianoDlog;

	Boolean					MIDIHardware, MIDIHardwareAlreadyOpen = false;
	OSErr					CalamariErr, squidLibErr;

	static	ProcPtr			CallBackFunction;

/***/

/* Globals */

Boolean		gSignedInToMIDIMgr;		/* are we signed into MIDI Manager? */
Boolean 	gNodesChanged;
short		gInputPortRefNum;		/* refNum of the OMS input port */
short		gOutputPortRefNum;		/* refNum of the OMS output port */
short		gCompatMode;			/* current OMS compatibility mode */

short 		gChosenInputID = 0;		/* uniqueID of selected input; 0 means none */
short		gChosenOutputID = 0;	/* uniqueID of selected output; 0 means none */
short		gOutNodeRefNum = -1;	/* node refNum of the selected output; -1 means non existant */

unsigned MidiVolume[128] = {
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
  };

/***/

#define MySignature		'SNPL'

void DoPlayInstruInt( short	Note, short Instru, short effect, short arg, short vol, Channel *curVoice, long start, long end);
void NPianoRecordProcess( short i, short, short, short);

static MIDIClientRef MADMIDICliRef;
void MADMIDINotifyProc(const MIDINotification *message, void *refCon);

void MyNullHook()
{

}

void CloseMIDIHarware(void)
{
	/*if( MIDIHardware)
	{
		OSStatus MIDIErr = MIDIClientDispose(MADMIDICliRef);
	}*/
	
	MIDIHardware = false;
}

void OpenMIDIHardware( void)	
{
	/*OSStatus MIDIErr = MIDIClientCreate(CFSTR("PlayerPRO"), MADMIDINotifyProc, NULL, &MADMIDICliRef);
	if(MIDIErr == noErr) MIDIHardware = TRUE;
	else */MIDIHardware = FALSE;
}

void InitMIDIHarware(void)
{
	MIDIHardware = false;
	MIDIHardwareAlreadyOpen = false;
}

void NDoPlayInstru(short Note, short Instru, short effect, short arg, short vol);

void DoMidiSpeaker( short note, short Instru, long arg)
{
	
}

/*void SquidAllNotesOff(short PortRefNum)
{
	int KeyNum;
	
	// AddLine("Turning off all notes...");
	
	for (KeyNum=0; KeyNum<128; KeyNum++)
	{
		SendKeyOn(PortRefNum, 0x80000000, (Byte) KeyNum, (Byte) 0);
	}
	
}*/

void OpenOrCloseConnection(Boolean opening)
{
	
}

void SelectOMSConnections( Boolean Input)
{
	
}

void SendMIDIClock( MADDriverRec *intDriver, Byte MIDIByte)
{
	
}

void SendMIDITimingClock( MADDriverRec *MDriver)
{
	
}

void MADMIDINotifyProc(const MIDINotification *message, void *refCon)
{
	
}
