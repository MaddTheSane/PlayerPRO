
/* Functions.c */
void SetCtlName(DialogPtr, short, Str255);
void InitFindReplace(void);
void NReplaceInt(Cmd *, short, short, Boolean);
void ReplaceCommand(void);
Boolean FindInternal(short);
void SetUpButtons(void);
void CreateFind(void);
short ExtractFunctionArgu(Str255);
Boolean SetUpFind(void);
void DoItemPressFind(short, DialogPtr);
void DoKeyPressFind(short);