
/* HelpOnline.c */
void ShowSection(short);
void ChangeMenuName(Str255);
void WriteLineList(void);
void CreateMenuHelp(void);
void UpdateSection(void);
void CreateHelpOnline(short);
void CloseHelpOnline(void);
void DoGrowHelpOnline(void);
void DoItemPressHelpOnline(short, DialogPtr);
void AdjustTextHelp(ControlHandle);
void DoContentHelp(WindowPtr, EventRecord *);
pascal void ScrollProcHelp(ControlHandle, short);
void UpdateHelpOnline(void);
void PrintHelpOnline(void);
