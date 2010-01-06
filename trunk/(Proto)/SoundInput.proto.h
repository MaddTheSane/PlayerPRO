
/* SoundInput.c */
//pascal void InterruptRoutine(SPBPtr, Ptr, short, long);
//long GetCurrentA1(long GetCurrentA0 (void )=);
void SetUpDeviceInfo(void);
void DeviceFilterMouseDown(EventRecord *);
void DeviceFilterUpdate(void);
void DrawLevelRect(short);
void DrawFillFree(Rect *);
void DrawSmallOscillo(void);
void ConvertInstrument16LL(register short *, register long);
OSErr ActiveSoundInput(Boolean, Handle *, Str255);
OSErr NOpenMicroDevice(Str255);
void MicroOff(void);
void InitSoundInput(void);
