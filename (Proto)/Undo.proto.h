
/* Undo.c */
void UPDATE_Total(void);
void UPDATE_Header(void);
void UPDATE_Note(short, short);
void UPDATE_NoteBOUCLE(short, short);
void UPDATE_NoteFINISH(void);
void UPDATE_Pattern(void);
void InitUndo(void);
void ResetUndo(void);
void SaveUndo(short, short, Str255);
void DoUndo(void);
