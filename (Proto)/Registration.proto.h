
/* Registration.c */
short NumToolboxTraps(void);
//TrapType GetTrapType(short);
Boolean TrapAvailable(short);
void CheckUsedTime(void);
Boolean CheckTimePL(void);
void UNREGISTRED(void);
Boolean IsCodeOK(void);
void ProtectInit(void);
void DecodeText(Ptr, long, Byte);
OSErr CallPlug(short);
OSErr TestCode(void);
void PrintRegistration(void);
void InitPrintRegistration(void);
void PreparePrintRegistration(void);
void EraseResources(OSType, short);
void Registration(void);
