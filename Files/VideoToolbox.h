/*
VideoToolbox.h
This file contains the necessary prototypes for use of all the VideoToolbox
files except Luminance.c, which has its own header file, Luminance.h.

Any C file that might be used as a MATLAB file should #include <VideoToolbox.h>
as it's first C statement.

Copyright 1989-1993 © Denis G. Pelli

HISTORY: (omitting changes documented in *.c files)
2/20/93 dhb	Added various commonly used headers.  
			Restructured so that only VideoToolbox.h has to be included,
			whether one is creating stand-alone THINK C or code resources
			to be called by MatLab. Everything that cares is properly conditioned
			by symbol MATLAB or UCSB.
			
*/
#pragma once			// suppress multiple inclusions of this file
#ifndef _VIDEOTOOLBOX_	// suppress multiple inclusions of this file
#define _VIDEOTOOLBOX_

/*
Header files that are required for succesful compilation of this file are marked
"required". The VideoToolbox sources can be linked to create a stand-alone C
application, or a MatLab external code resource (a "MEX" or "MATLAB" file), to
be invoked by typing the function name from within MatLab. The symbol MATLAB, if
defined, enables the creation of a MATLAB file. We suggest that if you want to
create a MATLAB file, then you should type "#define MATLAB", without the quotes,
into the Prefix window of your THINK C project (try menu Edit:Options:Prefix).
This will have the same effect as typing it at the top of every C file in your
project, but without modifying the source files. dhb & dgp.
*/
#include <stdio.h>		// required
#include <stdlib.h>
#ifdef MATLAB
	// Must come after stdlib.h and stdio.h, and before everything else.
	#include <cmex.h>
#endif
// ANSI headers
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <time.h>
// Macintosh headers
#include <Devices.h>
#include <Events.h>
//#include <FixMath.h>	// Must come before mc68881.h
#include <Gestalt.h>
#include <Memory.h>
#include <Events.h>
#include <OSUtils.h>	// required
#include <Palettes.h>	// formerly Palette.h
#include <QDOffscreen.h>// formerly QuickDraw32Bit.h
#include <QuickDraw.h>	// required
//#include <Retrace.h>	// required
//#include <Slots.h>
#include <Sound.h>
#if THINK_C				// c2pstr(),CtoPStr()
	#include <pascal.h>
#else
//	#include <Strings.h>// MPW C
#endif
#include <Timer.h>		// required
#include <ToolUtils.h>
#include <Video.h>
#include <Windows.h>	// required
// VideoToolbox headers
//#include "mc68881.h"	// defines mc68881 and mc68020

/*
This next bit is only for MATLAB files, to deal with QuickDraw globals. In THINK
C we have to explicitly allocate space for QuickDraw globals and InitGraf them
when our code resource is first invoked. VideoToolbox.h will be included in all
the C files that make up your MATLAB project. The symbol "MAIN" determines
whether the global structure mex_qd is allocated or just declared as extern.
THINK C complains if space is allocated more than once or never allocated.  So
you need to "#define MAIN 1" at the start of one file in every MATLAB project. I
always do it in the file that contains the entry point (e.g. main or
user_fcn).-- dhb.
*/
#ifdef MATLAB
	#ifdef THINK_C
		#ifdef MAIN
			THINK_C_QD mex_qd;	// Allocate storage for QD globals.
		#else
			extern THINK_C_QD mex_qd;
		#endif
		#define qd mex_qd
	#else
		#define mex_qd qd
	#endif
#endif

/*
TRUE and FALSE
*/
#ifndef TRUE
	#define FALSE	0
	#define TRUE	1
#endif

/*
NAN & INF
If you're using NAN and INF you'll be interested in the IsNan(), IsInf(), and
IsFinite() definitions & prototypes below. Note that the THINK C compiler is
timid, worried about possible exception events, so the INF and NAN expressions
below are evaluated at runtime. (Drat!) If you use these expressions in a tight
loop, you'd be better off putting the value in a variable (preferably a
register) and using that instead.
*/
#if !defined(INF)
	#define INF	(1.0/0.0)		/* Infinity */
#endif
#if !defined(NAN)
	#define NAN	(0.0/0.0)		/* Not a Number */
#endif
/*
NUMERICAL CONSTANTS
It's sad to say, but THINK C 5.02 loses two bits of precision converting between
doubles and ascii text, in either direction. Thus you will obtain more accurate
results by computing the numerical constants below at runtime rather than using
these predefined constants. The ANSI Numerical C Extensions group is moving to
require C compilers to preserve precision, so there is hope for the future. Of
course most applications would never notice a loss of two bits precision out of
the total double precision given by the 64 bits in the mantissa.
*/
#if !defined(PI)
	#define PI		M_PI	/* available in math.h  Far more accurate */
#endif
#if !defined(LOGPI)
	#define LOGPI	1.14472988584940017414	/* computed in Mathematica */
#endif
#if !defined(LOG2)
	#define LOG2	0.69314718055994530942	/* computed in Mathematica */
#endif
#if !defined(LOG10)
	#define LOG10	2.30258509299404568402	/* computed in Mathematica */
#endif

/*
COLORS
The number of colors that a video device can display, in its current mode.
*/
#define GDCOLORS(device) ((**(**(**device).gdPMap).pmTable).ctSize+1)

/*
FIXED POINT ARITHMETIC
Apple defines a handy data type called Fixed that is stored in a long, but is
assumed to have a decimal point in the middle. Many operations, e.g. adding two
Fixed numbers or multiplying or dividing a Fixed by an integer, can be performed
directly. To multiply or divide two Fixed numbers use Apple's FixMul() and
FixDiv(). FixRatio(n,m) returns the Fixed ratio of two integers. Macintosh C
compilers define double in various ways, depending on whether a floating point
unit is to be used. The Apple-provided routines for doing type conversion to and
from Fixed are only appropriate if you are NOT using the floating point unit.
The ones defined below are faster and work with or without the FPU.
*/

#ifdef _MAC_H
#define LongToFix(x) IntToFixed(x)
#define FixToLong(x) FixedToInt(x)
#define DoubleToFix(x) FloatToFixed(x)
#define FixToDouble(x) FixedToFloat(x)
#else
#define LongToFix(x) ((long)(x)<<16)
#define FixToLong(x) ((x)>>16)
#define DoubleToFix(x) ((Fixed)((x)*65536.+0.5))
#define FixToDouble(x) ((double)(x)*(1./65536.))
#endif

/*
STACK
StackGrow(n) increases the stack allocation by n bytes. You'll also want to use
Apple's StackSpace(), declared in Memory.h, that returns the number of bytes 
allocated for the stack.
*/

#define StackGrow(extraBytes) SetApplLimit(GetApplLimit()-(extraBytes))

/* Binomial.c */

long	BinomialSample(double p,long n);
int		BinomialSampleQuickly(int n);
double	BinomialLowerBound(double P,long k,long n);
double	BinomialUpperBound(double P,long k,long n);
double	BinomialPdf(double p,long k,long n);
double	Binomial(double p,long k,long n);
double	IncompleteBeta(double x,double a,double b);
double	InverseBinomial(double P,long k,long n);
double	InverseIncompleteBeta(double p,double a,double b);

/* CardSlot.c */

OSErr	CardSlot(char *cardName);

/* CenterRectInRect.c */

void	CenterRectInRect(Rect *moveableRectPtr,Rect *fixedRectPtr);
void	OffsetRectTile(Rect *r,int nx,int ny);
Boolean	RectInRect(Rect *r,Rect *R);

/* ChiSquare.c */

double	PChiSquare (double chiSquare,int n);


/* ConvolveX.c */

void	ConvolveX(double f[],int dim,BitMap *srcBits,BitMap *dstBits,
			Rect *srcRectPtr,Rect *dstRectPtr);

/* ConvolveY.c */

void ConvolveY(double f[],int dim,BitMap *srcBits,BitMap *dstBits,
			Rect *srcRectPtr,Rect *dstRectPtr);

/* CopyBitsQuickly.c */

enum{mulOver=128};
int	CopyBitsQuickly(BitMap *srcBits,BitMap *dstBits,
			Rect *srcRectPtr,Rect *dstRectPtr,int srcMode,RgnHandle maskRgn);

/* CopyQuickDrawGlobals.c */

void CopyQuickDrawGlobals(void);
#define CopyQuickdrawGlobals CopyQuickDrawGlobals	// old spelling

/* CreateTrialSnds.c */

void CreateTrialSnds(void);


/* DrawPrintf.c */

void DrawPrintf(char *s, ...);

/* ffprintf.c */

int ffprintf(FILE *stream[2],char *format,...);

/* FlushCacheRange.c */

void FlushCacheRange (void *address, unsigned long count);

/* GDOpenWindow.c */

void AddExplicitPalette(WindowPtr window);
WindowPtr GDOpenWindow1(GDHandle device);
void GDDisposeWindow1(WindowPtr window);
CWindowPtr GDOpenWindow(GDHandle device);					// Old. Use GD...1 instead.
void GDDisposeWindow(GDHandle device,CWindowPtr myWindow);	// Old. Use GD...1 instead.

/* GDPrintf.c */

void	GDPrintf(char *s, ...);

/* GDFrameRate.c */

double GDClutUpdateRate(GDHandle device,long clutEntries);
double GDFrameRate(GDHandle device);
double GDFramesPerClutUpdate(GDHandle device,long clutEntries);
double GDMovieRate(GDHandle device,int quickly);
double GDMovieSize(GDHandle device,int quickly);
void GDTimeClutUpdate(GDHandle device,long clutEntries
	,double *framesPtr,double *sPtr);
double GDVBLRate(GDHandle device);
double TickRate(void);

/* GDVideo.c */

unsigned char	*GDName(GDHandle device);
int		GDVersion(GDHandle device);
char 	*GDCardName(GDHandle device);
char	*GDModeName(short mode);
#define ModeName GDModeName			/* old name, before 1/18/93 */
int		PatchMacIIciVideoDriver(void);
short	GDClutSize(GDHandle device);
OSErr	GDRestoreDeviceClut(GDHandle device);
OSErr	GDUncorrectedGamma(GDHandle device);
#define GDLinearGamma GDUncorrectedGamma	/* old name, before 12/16/92 */
OSErr	GDReset(GDHandle device,short *modePtr,short *pagePtr,Ptr *baseAddrPtr);
OSErr	GDSetMode(GDHandle device,short mode,short page,Ptr *baseAddrPtr);
OSErr	GDSetEntries(GDHandle device,short start,short count,ColorSpec *aTable);
OSErr	GDSetGamma(GDHandle device,GammaTbl *myGammaTblPtr);
OSErr	GDGrayPage(GDHandle device,short page);
OSErr	GDSetGray(GDHandle device,Boolean flag);
OSErr	GDSetInterrupt(GDHandle device,Boolean flag);
OSErr	GDDirectSetEntries(GDHandle device,short start,short count,ColorSpec *aTable);
OSErr	GDSetDefaultMode(GDHandle device,short mode);
OSErr	GDGetMode(GDHandle device,short *modePtr,short *pagePtr,Ptr *baseAddrPtr);
OSErr	GDGetEntries(GDHandle device,short start,short count,ColorSpec *aTable);
OSErr	GDGetPageCnt(GDHandle device,short mode,short *pagesPtr);
OSErr	GDGetPageBase(GDHandle device,short page,Ptr *baseAddrPtr);
OSErr	GDGetGray(GDHandle device,Boolean *flagPtr);
OSErr	GDGetInterrupt(GDHandle device,Boolean *flagPtr);
OSErr	GDGetGamma(GDHandle device,GammaTbl **myGammaTblHandle);
OSErr	GDGetDefaultMode(GDHandle device,short *modePtr);
OSErr	GDControl(int refNum,int csCode,Ptr csParamPtr);
OSErr	GDStatus(int refNum,int csCode,Ptr csParamPtr);

/* GetClicks.c */

short	GetClicks(void);

/* GetScreenDevice.c */

GDHandle	GetScreenDevice(int n);
GDHandle	GetWindowDevice(WindowPtr theWindow);
GDHandle	AddressToScreenDevice(Ptr address);
GDHandle	SlotToScreenDevice(int n);
short int	GetDeviceSlot(GDHandle device);
short int	AddressToSlot(Ptr address);
void		LocalToGlobalRect(Rect *r);
void		GlobalToLocalRect(Rect *r);


/* GetVoltage.c */

double	VoltsDuringFrame(double frames);
double	GetVoltage(short channel,double *gainPtr,double *frequencyPtr,long n
			,double *sdPtr);
short	GetVoltages(short channel,double *gainPtr,double *frequencyPtr,long *nPtr
			,unsigned short readings[],double *voltDeltaPtr,double *voltZeroPtr);
short	InitiateVoltageSampling(short channel,double *gainPtr,double *frequencyPtr
			,double *voltDeltaPtr,double *voltZeroPtr);
short	RetrieveVoltages(long *nPtr,unsigned short readings[]);
int		ForeRunnerSlot(void);
enum{voltageBufferOverflow=1<<0,voltageOverflow=1<<1,voltageUnderflow=1<<2};

/* HideMenuBar.c */

/* Identify....c */

char *IdentifyMachine(void);
char *IdentifyCompiler(void);

/* IsNan.c */

int IsNan(double x);
int IsInf(double x);
#ifndef IsFinite
	#define IsFinite(x) ((*(short *)&(x) & 32767)!=32767)	/* neither NAN nor ±INF */
#endif

/* kbhit.c */

int	kbhit(void);
int	getcharUnbuffered(void);

/* Log2L.c */

long Log2L(unsigned long j);

/* MakeNoise.c */

Boolean MakeNoise1(double dx,double dy,Boolean randomPhase,PixMap *frame);
void	MakeNoise(double dx,double dy,PixMap frame[],short *maxFramesPtr);

/* MaximizeConsoleHeight.c */

void MaximizeConsoleHeight(void);

/* Mean.c */

double	Mean(double x[],long n,double *sdPtr);

/* MyFgets.c */

char	*MyFgets(char *s, int length, FILE *stream);

/* Normal.c */

double	NormalPdf(double x);
double	Normal(double x);
double	InverseNormal(double p);
double	NormalSample(void);
double Normal2DPdf(double r);
double Normal2D(double r);
double InverseNormal2D(double p);
double Normal2DSample(void);
double InverseNormal2DPdf(double p);

/* nrand.c */

int nrand(short n);
unsigned short nrandU(unsigned short n);
unsigned long nrandUL(unsigned long n);

/*	OpenDataFiles.c */

unsigned long	OpenDataFiles(FILE **inPtr,FILE **outPtr
					,char *inName,char *outName);

/*	PixMapToPostScript */

void	PixMapToPostScript(char *filename,PixMap *pm,Rect *rectPtr
			,Rect *pageRectPtr,double cellsPerInch,int grayLevels);
void	AppendToFile(char *filename,char *string);

/* PlotXY.c */

typedef struct{
	Boolean continuing;	/* zero to start a new curve */
	long color;			/* e.g. blackColor, blueColor */
	short lineWidth;	/* in pixels, zero for none */
	short symbolWidth;	/* in pixels, zero for none */
	short dashOffset;	/* in pixels */
	short dash[5];		/* in pixels. The array is terminated by a zero element */
	short h,v;			/* reserved for internal use */
	Fixed pathLengthF;	/* reserved for internal use */
} PlotXYStyle;
void PlotXY(WindowPtr window,double x,double y,PlotXYStyle *style);

/* PrintfExit.c */

int PrintfExit(char *format,...);
void Require(long quickDrawVersion);

/* QD32Exists.c */

Boolean 		QD8Exists(void);
Boolean			QD32Exists(void);
Boolean			NewPaletteManager(void);

/* randU.c */

unsigned short	randU(void);
unsigned long	randUL(void);
void			srandU(unsigned n);
void			RandFill(void *address,long bytes);


/* ReadAssignments.c */

enum{echoAssignments=1,echoComments=2,echoFile=4};
enum{charType=1,shortType,longType,unsignedCharType,unsignedShortType,unsignedLongType
	,doubleType,stringType};
typedef struct {
	short type;
	char *name;
	void *ptr;
} Variable;
void PrintAssignment(FILE *file,Variable *v);
Boolean AssignmentLineWasBlank(void);
int ReadAssignmentLine(FILE *file,Variable variable[],int echo);
int ReadAssignmentBlock(FILE *file,Variable variable[],int echo);
int ReadAssignmentFile(char *filename,Variable variable[],int echo);
int InitializeVariables(Variable variable[]);
void InitializeAVariable(Variable *variable);
Variable SetVariable(int type,void *ptr,char *name);

/* ReadMatLabFile.c */

int LoadMatDoubles(FILE *f,char *name,long *rows,long *cols
	,double **real,double **imag);
int LoadMatShorts(FILE *f,char *name,long *rows,long *cols
	,short **real,short **imag);
int SaveMatDoubles(FILE *f,char *name,long rows,long cols,double *real,double *imag);
int SaveMatShorts(FILE *f,char *name,long rows,long cols,short *real,short *imag);

/* RectToAddress.c */

unsigned char	*RectToAddress(PixMap *myPixMapPtr,Rect *myRectPtr
					,short *myRowBytesPtr,short *myPixelSizePtr,short *bitsOffsetPtr);

/* RestoreCluts.c */

void RestoreCluts(void);

/*	SetEntriesQuickly.c */

OSErr SetEntriesQuickly(GDHandle device,short start,short count,ColorSpec *table);
short macltset(GDHandle device,register short start
	,short *red,short *green,short *blue,short count1);
short GetCardType(GDHandle device);
char *GetCardBase(GDHandle device);
OSErr WaitForNextBlanking(GDHandle device);

/* SetFileInfo.c */

void	SetFileInfo(char *fileName,OSType fileType,OSType fileCreator);

/* SetMouse.c */

void	SetMouse(Point mouseLoc);

/* SetOnePixel.c */

void SetPixmapPixel(PixMap *pmHandle,int x,int y,unsigned long value);
unsigned long GetPixmapPixel(PixMap *pmHandle,int x,int y);
unsigned char *GetPixmapPixelAddress(PixMap *pmHandle,int x,int y);
void SetDevicePixel(GDHandle device,int x,int y,unsigned long value);
unsigned long GetDevicePixel(GDHandle device,int x,int y);
void SetOnePixel(int x,int y,unsigned long value);
unsigned long GetOnePixel(int x,int y);
#define SetIPixel SetDevicePixel	// So that old programs won't break.
#define GetIPixel GetDevicePixel	// So that old programs won't break.

/* SetPixelsQuickly.c */

int SetPixelsQuickly(int x,int y,unsigned long value[],short n);
int GetPixelsQuickly(int x,int y,unsigned long value[],short n);
int SetWindowPixelsQuickly(WindowPtr window,int x,int y,unsigned long value[],short n);
int GetWindowPixelsQuickly(WindowPtr window,int x,int y,unsigned long value[],short n);
int SetDevicePixelsQuickly(GDHandle device,int x,int y,unsigned long value[],short n);
int GetDevicePixelsQuickly(GDHandle device,int x,int y,unsigned long value[],short n);
int SetPixmapPixelsQuickly(PixMapPtr pmPtr,int x,int y,unsigned long value[],short n);
int GetPixmapPixelsQuickly(PixMapPtr pmPtr,int x,int y,unsigned long value[],short n);

/* SetPriority.c */

void SwapPriority(char *priority);

/* Shuffle.c */

void	Shuffle(short array[],long size);

/* SndPlay1.c */

OSErr SndPlay1(Handle snd);
void SndStop1(void);
short SndDone1(void);

/* Timer.c */

struct Timer{
	TMTask time;
	long ourA5;
	long interval,elapsed,elapsedIntervals;
	long timeToStartTimer;			// minimum time in µs
	long stopDelay;					// µs from call to stop, re from call to start
	long timeManagerVersion;
	struct Timer *next,*previous;	// doubly linked list of Timers
};
typedef struct Timer Timer;

Timer *NewTimer(void);
void DisposeTimer(Timer *t);
void StartTimer(Timer *t);
long StopTimer(Timer *t);					// µs
double StopTimerSecs(Timer *t);				// s

/* TitleBarHeight.c */

int TitleBarHeight(WindowPtr window);

/* TrapAvailable.c */

Boolean	TrapAvailable(short theTrap);

/* Uniform.c */

double UniformSample(void);

/* VBLInterruptServiceRoutine */
struct VBLTaskAndA5 {
//    volatile VBLTask vbl;
    long ourA5;
    void (*subroutine)(struct VBLTaskAndA5 *vblData);
    GDHandle device;
    long slot;
    volatile long newFrame;				// Boolean
    volatile long framesLeft;			// count down to zero
    long framesDesired;
    Timer *frameTimer;					// time ms since last VBL interrupt, see Timer.c
    void *ptr;							// use this for whatever you want
};
typedef struct VBLTaskAndA5 VBLTaskAndA5;

OSErr VBLInstall(VBLTaskAndA5 *vblData,GDHandle device,int frames);
OSErr VBLRemove(VBLTaskAndA5 *vblData);
void VBLInterruptServiceRoutine(void);
void SimpleVBLSubroutine(VBLTaskAndA5 *vblData);

/* VideoTFB.c */

Boolean TFBInSlot(int slot);
void	SetUpTFB(int slot);
void	RampClutTFB(int slot);
void	GrayClutTFB(int slot);
void	LoadClutTFB(int slot,unsigned char rgb[256][3]);
void	NewBlankingTFB(int slot);
void	NewFieldTFB(int slot);
int		BlankingTFB(int slot);
void	SetDepthTFB(int slot,short int bits);
void	SynchSetDepthTFB(int masterSlot,int slot,short int bits);
void	SynchToMainDeviceTFB(GDHandle device);
void	HaltTFB(int slot);
void	RestartTFB(int slot,short int bits);
void	HaltDeviceTFB(GDHandle device);
void	RestartDeviceTFB(GDHandle device);
void	ScrollTFB(int slot,short int bits,long x,long y);
void	PanTFB(int slot,long int x);

/* VLambda.c */

double VLambda(double nm);
double VLambdaPrime(double nm);

/* Zoom.c */

void Zoom(WindowPtr theWindow,int zoomDir);


#endif

