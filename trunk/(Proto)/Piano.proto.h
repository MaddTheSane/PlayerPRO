
/* Piano.c */
void DoHelpPiano(short **, short *);
Boolean DragCommand(RgnHandle, Cmd, EventRecord *);
void CreatePianoPixMap(void);
void GetToucheRect(Rect *, short);
void SetControlPiano(void);
void DoGrowPiano(void);
void UpdatePianoInfo(void);
void AfficheTouche(short, Rect *, short, short);
void EffaceTouche(short, Rect *);
void SelectTouche(short, short);
void ResetPiano(void);
void DoNullPiano(void);
void UpdatePianoWindow(DialogPtr);
void TrackRect(Rect *);
pascal void actionPiano(ControlHandle, short);
void RecordButtonPiano(void);
void DoItemPressPiano(short, DialogPtr);
void UpdatePressChar(void);
void AdjustZoomPiano2(Rect *);
void CreatePianoWindow(void);
void ClosePiano(void);
void NPianoRecordProcess(short, short, short, short);
Boolean DoRecordPiano(void);
