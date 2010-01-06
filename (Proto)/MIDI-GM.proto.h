
/* MIDI-GM.c */
void InitQuicktimeInstruments(void);
void OctavesMIDIName(short, Str255);
void SetInstruNameM(short, Str255, short, Ptr);
void SetSampNameM(Str255, Ptr);
short OpenResFileQK(long, short);
void ComputeQuicktimeSound25(short, sData **, InstrData *, short);
void ComputeQuicktimeSound(short, sData **, InstrData *, short);
OSErr CreateNameListQuicktime25(void);
OSErr CreateNameListQuicktime(void);
void UpdateQKInstruWindow(DialogPtr);
void PressSmallPianoQ(DialogPtr, InstrData *, short);
void ComputeInstSize(Str255, InstrData *, short);
void Quicktime2Converter(void);
