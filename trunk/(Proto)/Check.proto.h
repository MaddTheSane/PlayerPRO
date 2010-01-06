
/* Check.c */
long OpenHeaderMOD(Ptr, Str255, long);
Boolean OpenableFile(OSType, FSSpec *);
Boolean CheckFileType(FSSpec, OSType);
Ptr ConvertCurrentMusicToPtr(void);
void ExportFile(OSType, FSSpec *);
void CheckInstrument(void);
Boolean ImportFile(Str255, short, long, OSType);
