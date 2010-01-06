
/* ScrollTextTT.c */
pascal long GetVBLRec(void);
void DesactiveScrollText(void);
void ActiveScrollEtoile(void);
OSErr InstallScrollTextVBL(VBLTask *, ProcPtr, Boolean);
void RemoveScrollTextVBL(VBLTask *);
void ScrollTextVBLProc(void);
//pascal void Scroll(VBLTaskPtr);
//pascal void NOWScroll(VBLTaskPtr);
//void Scroll(void);
//void NOWScroll(void);
void DATAtoRSRC(void);
void ResetScrollTXT(void);
void MigoText(Ptr);
void HideBottom(void);
void ShowBottom(void);
void ScrollTextInit(Boolean, Boolean);
void ScrollTextClose(Boolean);
