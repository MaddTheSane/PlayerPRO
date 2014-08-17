//TODO: CoreMIDI back-end
#include "MAD.h"
#include "RDriver.h"
#include "RDriverInt.h"
//#include "MIDI.h"
#include <stdio.h>
#include "PPPrivate.h"

#include <CoreMIDI/CoreMIDI.h>

#define refConTime			1
#define refConOutput		3
#define squidInputBufSize	16384	/* 16K */
#define recordingBufSize	10000	

Boolean MIDIHardware = false, MIDIHardwareAlreadyOpen = false;

/***/

/* Globals */

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

#define MySignature 'SNPL'

void DoPlayInstruInt(short Note, short Instru, short effect, short arg, short vol, Channel *curVoice, int start, int end);
void NPianoRecordProcess(short i, short, short, short);

static MIDIClientRef MADMIDICliRef;
static MIDIPortRef MADMIDIPortInRef;
//static MIDIPortRef MADMIDIPortOutRef;
static MIDIEndpointRef MADMIDIKeyboardEndRef;
static void MADMIDINotifyProc(const MIDINotification *message, void *refCon);
static void MADMIDIPortProc(const MIDIPacketList *pktlist, void *readProcRefCon, void *srcConnRefCon);
//static void OpenOrCloseConnection(Boolean opening);

void CloseMIDIHarware()
{
	if (MIDIHardware) {
		OSStatus MIDIErr = MIDIEndpointDispose(MADMIDIKeyboardEndRef);
		MIDIErr = MIDIClientDispose(MADMIDICliRef);
		MADMIDICliRef = 0L;
		MADMIDIPortInRef = 0L;
	}
	
	MIDIHardware = false;
}

void OpenMIDIHardware(MADDriverRec *rec)
{
	if(MIDIHardware || MIDIHardwareAlreadyOpen == FALSE) {
		OSStatus MIDIErr = MIDIClientCreate(CFSTR("PlayerPRO"), MADMIDINotifyProc, rec, &MADMIDICliRef);
		if(MIDIErr == noErr) {
			MIDIHardware = TRUE;
			MIDIHardwareAlreadyOpen = TRUE;
			MIDIErr = MIDIInputPortCreate(MADMIDICliRef, CFSTR("PlayerPRO Keyboard In"), MADMIDIPortProc, rec, &MADMIDIPortInRef);
			if (MIDIErr == noErr) {
				MIDIErr = MIDIDestinationCreate(MADMIDICliRef, CFSTR("PlayerPRO destination"), MADMIDIPortProc, rec, &MADMIDIKeyboardEndRef);
			}
			if (MIDIErr == noErr) {
				MIDIErr = MIDIPortConnectSource(MADMIDIPortInRef, MADMIDIKeyboardEndRef, rec);
			}
		} else
			MIDIHardware = FALSE;
	}
}

void InitMIDIHarware(void)
{
	MIDIHardware = TRUE;
	MIDIHardwareAlreadyOpen = false;
}

void NDoPlayInstru(short Note, short Instru, short effect, short arg, short vol);

void DoMidiSpeaker(short note, short Instru, int arg)
{
	
}

#if 0
void SquidAllNotesOff(short PortRefNum)
{
	int KeyNum;
	
	// AddLine("Turning off all notes...");
	
	for (KeyNum = 0; KeyNum < 128; KeyNum++) {
		SendKeyOn(PortRefNum, 0x80000000, KeyNum, 0);
	}
}

void OpenOrCloseConnection(Boolean opening)
{
	
}
#endif

void SendMIDIClock(MADDriverRec *intDriver, Byte MIDIByte)
{
	
}

void SendMIDITimingClock(MADDriverRec *MDriver)
{
	
}

static void MADMIDINotifyProc(const MIDINotification *message, void *refCon)
{
	
}

static void MADMIDIPortProc(const MIDIPacketList *pktlist, void *readProcRefCon, void *srcConnRefCon)
{
	
}
