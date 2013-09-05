
/* MapPoly.c */
void LoadTexture(short);
void LoadBackground(short);
void ClearScreen(void);
//void ComputeScreenMap2(PixMapPtr, VAGlobalsRecord2 *, Rect *);
void CreateWorld(Rect *);
void CloseMapping(void);
void InitMapping(void);
void DoNullCube(void);
void DoNullPress(void);
void NextStep(void);
void NewFrame(short);
void Compute3DWorld(void);
void NewPos(short *, short *, short *, short);
void Init3DMapping(void);
void Recalc(void);
void InitBresenham(void);
void ComputeOPY(short);
void Fill_In(long,  long, long);
