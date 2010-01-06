
/* Spectrum.c */
void DoHelpSpectrum(short **, short *);
void GetWorkingZoneSpectrum(Rect *);
void FillInterTextSpectrum(Rect *, short);
void AdjustZoomSpectrum2(Rect *);
void ComputeCurrentQuickPixMapSpec(void);
void SetControlSpectrum(void);
void DoGrowSpectrum(void);
void DrawOsciFreq2(short, short, short, short, Byte);
void UpdateSpectrumData(short, Byte *);
void C8BitSpectrum(short, short, short, Byte *, Byte *);
void C8BitSpectrumDirect(short, short, short, Byte *, Byte *);
void C16BitSpectrum(short, short, short, Byte *, Byte *);
void C16BitSpectrumDirect(short, short, short, Byte *, Byte *);
//void DrawSpectrum(OsciRec *, short);
void DoNullSpectrum(void);
void UpdateSpectrumWindow(DialogPtr);
pascal void actionProcSpectrum(ControlHandle, short);
void DoItemPressSpectrum(short, DialogPtr);
void SetWindowSpectrum(void);
long GetAudioSizeSpectrum(void);
Ptr GetAudioSourceSpectrum(short);
void CreateSpectrumWindow(void);
void CloseSpectrum(void);
void ResetSpectrum(void);
