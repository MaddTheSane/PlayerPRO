
/* Timer.c */
Timer *NewTimer(void);
void DisposeTimer(Timer *);
void StartTimer(Timer *);
long StopTimer(Timer *);
double StopTimerSecs(Timer *);
void KillEveryTimer(void);