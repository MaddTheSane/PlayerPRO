#ifdef MAINPLAYERPRO
#include "Shuddup.h"
#endif
#include "MAD.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include "MADPrivate.h"

#include <stdio.h>

#define MySignature		'SNPL'

void DoPlayInstruInt(short Note, short Instru, short effect, short arg, short vol, Channel *curVoice, int start, int end);
void NPianoRecordProcess(short i, short, short, short);

void MyNullHook()
{
	
}

void CloseMIDIHarware(void)
{

}

void OpenMIDIHardware(MADDriverRec *rec)
{

}

void InitMIDIHarware(void)
{
	//MIDIHardware = false;
	//MIDIHardwareAlreadyOpen = false;
}

void NDoPlayInstru(short Note, short Instru, short effect, short arg, short vol);

void DoMidiSpeaker(short note, short Instru, int arg)
{

}

void SelectOMSConnections(bool Input)
{

}

void SendMIDIClock(MADDriverRec *intDriver, MADByte MIDIByte)
{

}

void SendMIDITimingClock(MADDriverRec *MDriver)
{

}
