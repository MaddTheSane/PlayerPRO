
/* MIDI-Hardware.c */
char *StrCpy(char *, char *);
char *PortType2String(short, char *);
char *C2PStrCpy(char *, Str255);
short AddInputPort(OSType, OSType, short, short, long, Ptr, long, char[], short);
void CloseMIDIHarware(void);
void DoMIDIHarwareRecord(void);
void InitMIDIHarware(void);
void DoMidiSpeaker(short, short, long);
//pascal short Reader(MIDIPacketPtr, long);
//int ExtractFlags(short, short, MIDIPacketPtr);
void SquidAllNotesOff(short);
