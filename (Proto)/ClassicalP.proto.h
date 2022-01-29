
/* ClassicalP.c */
void DoHelpClassic(short **, short *);
void GetInvertRect(short, Rect *);
void WriteLevel(void);
void SetControlHClassic(void);
void SetWClassic(short);
void AdjustZoomClassic2(Rect *);
void DoGrowClassic(void);
void AfficheClassicRect(short, short);
void DrawCurrentNote(short);
void DrawCurrentClassic(void);
void DoNullClassic(void);
void UpdateClassicInfo(void);
void UpdateClassicWindow(DialogPtr);
pascal void actionProcClassic(ControlHandle, short);
void DoItemPressClassic(short, DialogPtr);
void CreateClassicWindow(void);
void CloseClassicWindow(void);
void DoKeyPressClassic(short);
