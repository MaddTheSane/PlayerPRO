
/* AEvent.c */
pascal OSErr AEODoc(const AppleEvent *, AppleEvent *, long);
pascal OSErr AEQApp(const AppleEvent *, AppleEvent *, long);
pascal OSErr AEErreur(const AppleEvent *, AppleEvent *, long);
pascal OSErr AEStopMusic(const AppleEvent *, AppleEvent *, long);
pascal OSErr AEPlayMusic(const AppleEvent *, AppleEvent *, long);
pascal OSErr AENextPattern(const AppleEvent *, AppleEvent *, long);
pascal OSErr AEPreviousPattern(const AppleEvent *, AppleEvent *, long);
pascal OSErr AESetSpeed(const AppleEvent *, AppleEvent *, long);
pascal OSErr AESetPitch(const AppleEvent *, AppleEvent *, long);
pascal OSErr AESetVolume(const AppleEvent *, AppleEvent *, long);
pascal OSErr AEGotoPattern(const AppleEvent *, AppleEvent *, long);
pascal OSErr AEGotoPosition(const AppleEvent *, AppleEvent *, long);
pascal OSErr AEGotoPercent(const AppleEvent *, AppleEvent *, long);
OSErr MyGotRequiredParams(const AppleEvent *);
Boolean AppleEventsInstalled(void);
short InstallAE(void);
void AESendOpenFile(FSSpec *);
