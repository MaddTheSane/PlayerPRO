
/* Adaptators.c */
void DoHelpAdap(short **, short *);
void SetUpScroll(void);
void DoGrowAdap(DialogPtr);
void FillBarControl(short, short, short);
void FillBarStereo(short, short, short);
void UpdateAdapWindow(DialogPtr);
void ResetAdaptators(void);
void InitAdapWindow(void);
void CreateAdapWindow(void);
void CloseAdapWindow(void);
pascal void actionProcAdap(ControlHandle, short);
void LoadAdaptatorsInt(FSSpec *);
void LoadAdaptators(void);
void LoadAdaptatorsRsrc(FSSpec *);
void SaveAdaptatorsRsrc(FSSpec *);
void SaveAdaptorsFile(FSSpec *);
void SaveAdaptators(void);
void DoItemPressAdap(short, DialogPtr);
pascal OSErr MyTrackingAdap(short, WindowPtr, void *, DragReference);
pascal OSErr MyReceiveAdap(WindowPtr, void*, DragReference);
