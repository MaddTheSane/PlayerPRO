/* WaveEditor.c */void DoHelpWave(short **, short *);short GetMaxXWave(void);short GetMaxYWave(void);void FinishFrame(void);void SetWaveControl(void);void CreateCurWaveRect(void);void DoNullWave(void);void DoGrowWave(void);void GetMinMax(long, long, Ptr, long *, long *);void UpdateCmdWave(short, short);OSErr ComputeWave(short, short, short);void DrawFrame(void);void UpdateWaveInfo(void);void UpdateWaveWindow(DialogPtr);void DoItemPressWave(short, DialogPtr);pascal void actionProcWave(ControlHandle, short);void CreateWaveWindow(void);void CloseWave(void);void DoKeyPressWave(short);