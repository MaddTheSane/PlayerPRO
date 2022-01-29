
/* VAHeart.c */
void ComputeScreen(PixMapPtr, VAGlobalsRecord *);
void ReComputeVA(void);
void InitBresenham(void);
void VABresenham(short, short, short, short);
void VABresenhamSOsci(short, short, short, short, long **);
void VABresenhamS(short, short, short, short, long **);
void VAStaticLine(register short, register short, register short, register short, long **);
