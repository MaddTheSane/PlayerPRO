void  UpdateQTWindow(DialogPtr GetSelection);
Boolean CreateQTWindow( FSSpec	*file);
void DoItemPressQT( short whichItem, DialogPtr whichDialog);
void CloseQT(void);
Boolean QTTestConversion( FSSpec	*file, OSType fileType);
void InitQTWindow( void);
void DoNullQT( void);
Boolean QTTypeConversion( OSType fileType);
void QTDoAction( Boolean play);