#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include <stdio.h>
#include "VA.h"
//#include "start.h"
void SetWindowEnviron(void);
void ResetOscilloscope(void);

/******** HELP MODULE ********/
enum
{
	HDisplay	= 5,
	HSize		= 8,
	HSpectr		= 10
};

static short AHelp[] = {HDisplay, HSize, HSpectr};

void DoHelpOscillo(short **items, short *lsize)
{
	*lsize = sizeof(AHelp) / sizeof(AHelp[0]);
	*items = AHelp;
}

/*****************************/

typedef struct
{
	short		VPos;
	Str63		Name;
	Byte		*SavePtr;
	long		Size;
	long		min;
	long		max;
} OsciRec;

enum
{
	OutPutAudio = 0,
	TrackAudio,
	InPutAudio,
	QuicktimeMovie
};

enum
{
	vID,
	hID
};

	extern	EventRecord				theEvent;
	extern	RGBColor				theColor;
	extern	Boolean					MicroPhone;
	extern	Ptr						outPutPtr;
	extern	Str255					InPutName;
	extern	MenuHandle				OsciHMenu, OsciTypeMenu, OsciModeMenu;
	extern	long					deviceBufferSize;
	extern	unsigned long			SoundInputRate;
	extern	short					SoundInputBits;

	static	RgnHandle				dlogptrRgn;
	static	Point					PP, De;
	static	Point					WinZero;
			
			Ptr						AudioDSPPtr;
	static	Ptr						PseudoSilence, ValSaveData;
	static	long					oldNullTicks;
	static	ControlHandle			HControl, VControl;
	static	Ptr						*CurrentQuick;
	static	long					*CurrentQuickPixMap;
	
	static	OsciRec					osci[ MAXTRACK];
	static	short					OsciH, OsciDD, OsciL, OsciType, OsciVStart, OsciNo, SIter;
	static	long					OsciOffSet;
	static	char					OsciColor;
	static	PixMapHandle			osciPixMap[ MAXTRACK];
	static	GWorldPtr				gGWorld = NULL;
	static	short 					gThumbPrev;
	static	RGBColor				myBlueColor = { 13107, 65535, 65535};
	static	long					maxLevelL, maxLevelR, timeLevelL, timeLevelR;
	static	Boolean					VMMode;
	

	static	short					DrawDraw, DrawIn;
	static	Ptr						PreviousOscilloPtr;
	static	long					lastCurTime;
	
//	static	long					fallL, fallR;
	
	extern	Boolean					SpectrumMicrophone;
			Boolean					OscilloMicrophone;
			DialogPtr				OscilloDlog;

#define CourbeColor		0x90
#define RawMouse 		0x82C
#define InterText		12
#define	VALSIZE			300000
#define	OSCILLODEF		10
#define TRACKSIZE		740
#define	AUDIODSPSIZE	4096L

	long 	GetAudioSize();
	Ptr 	GetAudioSource(short);
	Ptr		GetQuicktimeSource(void);
	Byte 	SwitchColorLV(short i);
	OSErr	ActiveSoundInput(Boolean RecordingMode, Handle *RecordedSound, Str255 name);
	short	GetQuicktimeChannels(void);

Byte	SwitchColorLV(short i)
{
	Byte				val;
	CGrafPtr			oldPort;
	GDHandle			oldGDeviceH;
	
	if (gGWorld == NULL) MyDebugStr(__LINE__, __FILE__, "gGWorld = NULL");
	
	GetGWorld(&oldPort, &oldGDeviceH);
	SetGWorld(gGWorld, NULL);
	
	val =  Color2Index(&thePrefs.tracksColor[ i]);
	
	SetGWorld(oldPort, oldGDeviceH);
	
	return val;
}


void GetWorkingZone(Rect	*myRect)
{
	Rect	caRect;
	
	GetPortBounds(GetDialogPort(OscilloDlog), &caRect);
	
	if (OsciType == OutPutAudio) myRect->left = 15;
	else myRect->left = 0;
	myRect->right = caRect.right - 15;
	myRect->top = OsciVStart;
	myRect->bottom = caRect.bottom - 15;
}

void AdjustZoomOscillo2(Rect	*vRect)
{
//	WindowPeek	wPeek;
//	WStateData	*wspd;
	short		tempA, tempB;
	Rect		stdRect;

//	wPeek = (WindowPeek) OscilloDlog;
	
//	wspd = (WStateData*) *(wPeek->dataHandle);
	
	GetWindowStandardState(GetDialogWindow(OscilloDlog), &stdRect);
	
	tempA = OsciL 		+ 15;
	if (thePrefs.osciTile) tempB = OsciVStart 	+ 15 + 1 * (InterText + OsciH);
	else tempB = OsciVStart 	+ 15 + OsciNo * (InterText + OsciH);
	
	if (stdRect.top + tempB > vRect->bottom)
	{
		tempB = vRect->bottom - stdRect.top;
		
		tempB -= OsciVStart + 15;
		tempB /= OsciH + InterText;
		tempB *= OsciH + InterText;
		tempB += OsciVStart + 15;
	}
	
	stdRect.right	= stdRect.left + tempA;
	stdRect.bottom	= stdRect.top + tempB;
	
	SetWindowStandardState(GetDialogWindow(OscilloDlog), &stdRect);
}

void ComputeCurrentQuickPixMap()
{
	long	i, rowBytes = (*osciPixMap[ 0])->rowBytes &0xFFF;
	
	if (CurrentQuickPixMap != NULL) DisposePtr((Ptr) CurrentQuickPixMap);
	
	CurrentQuickPixMap = (long*) NewPtr(sizeof(long) * (*osciPixMap[ 0])->bounds.bottom);
	
	for (i = 0; i < (*osciPixMap[ 0])->bounds.bottom; i++)
	{
		CurrentQuickPixMap[ i] = i * rowBytes;
	}
}

void SetControlOscillo(void)
{
	short	i;
	Rect	caRect;
	
	GetPortBounds(GetDialogPort(OscilloDlog), &caRect);
	
	MyMoveControl(HControl, -1, caRect.bottom-15);
	MySizeControl(HControl, caRect.right -13, 16);
	
	SetControlMinimum(HControl, 0);
	SetControlMaximum(HControl, OsciL - (caRect.right - 15));
	
	if (gUseControlSize) SetControlViewSize(HControl, (caRect.right - 15));
	
	MyMoveControl(VControl, caRect.right -15, OsciVStart);
	MySizeControl(VControl, 16, caRect.bottom - 14 - OsciVStart);
	
	SetControlMinimum(VControl, 0);
	
	if (thePrefs.osciTile) SetControlMaximum(VControl, 0);
	else SetControlMaximum(VControl, OsciNo - (caRect.bottom - 15 - OsciVStart) / (OsciH + InterText));
	
	if (gUseControlSize) SetControlViewSize(VControl, (caRect.bottom - 15 - OsciVStart) / (OsciH + InterText));
	
	ResetOscilloscope();
}

void DoGrowOscillo(void)
{
	long		lSizeVH;
	GrafPtr		SavePort;
	Rect		caRect, temp;
	short		tempA, tempB;

	GetPort(&SavePort);
 	SetPortDialogPort(OscilloDlog);

	temp.left = 149;
	temp.right = OsciL + 15;
	
	temp.top = OsciVStart + InterText + OsciH + 15;
	
	if (thePrefs.osciTile) temp.bottom = OsciVStart + 15 + 1 * (InterText + OsciH);
	else temp.bottom = OsciVStart + 15 + OsciNo * (InterText + OsciH);
	
	lSizeVH = 0;
	if (theEvent.what == mouseDown) lSizeVH = GrowWindow(GetDialogWindow(OscilloDlog), theEvent.where, &temp);
	
	if (lSizeVH != 0) {
		tempA = LoWord(lSizeVH);
		tempB = HiWord(lSizeVH);
	} else {
		GetPortBounds(GetDialogPort(OscilloDlog), &caRect);
		
		tempA = caRect.right;
		if (tempA > temp.right) tempA = temp.right;
		tempB = caRect.bottom;
	}

	tempB -= OsciVStart + 15;
	tempB /= OsciH + InterText;
	tempB *= OsciH + InterText;
	tempB += OsciVStart + 15;

	MySizeWindow(OscilloDlog, tempA, tempB, true);
	SetWindowEnviron();
	
	SetPort(SavePort);
}

Ptr	TransformationDSP(Ptr thePtr, short i)
{
#ifdef __LITTLE_ENDIAN__
	register Ptr		tempPtr = thePtr+1;
#else
    register Ptr		tempPtr = thePtr;
#endif
	register Byte		bb = 0x80;
	
	if (i > AUDIODSPSIZE * 4L) MyDebugStr(__LINE__, __FILE__, "AudioDSPPtr too small");
	
	do
	{
		AudioDSPPtr[ --i] = *tempPtr - bb;
		tempPtr += 4L;
	}
	while (i != 0);
	
	return(AudioDSPPtr);
}

Ptr	Transformation8(Ptr thePtr)
{
	register Ptr		tempPtr = thePtr;
	register short		i = MADDriver->ASCBUFFERReal;
	
	
	if (i > AUDIODSPSIZE * 4L) MyDebugStr(__LINE__, __FILE__, "AudioDSPPtr too small");
	
	do
	{
		AudioDSPPtr[ --i] = *tempPtr;
		tempPtr += 2;
	}
	while (i != 0);
	
	return(AudioDSPPtr);
}

Ptr	Inverse8(register Ptr thePtr, long Size)
{
	register short		i = Size;
	
	if (i > AUDIODSPSIZE * 4L) MyDebugStr(__LINE__, __FILE__, "AudioDSPPtr too small");
	
	do
	{
		AudioDSPPtr[ --i] = *thePtr++ + 0x80;
	}
	while (i != 0);
	
	return(AudioDSPPtr);
}

Ptr	Inverse16(register Ptr thePtr, long Size)
{
	register short		i = Size;
	
	if (i > AUDIODSPSIZE * 4L) MyDebugStr(__LINE__, __FILE__, "AudioDSPPtr too small");
	
	do
	{
		AudioDSPPtr[ --i] = *thePtr + 0x80;
		thePtr += 2;
	}
	while (i != 0);
	
	return(AudioDSPPtr);
}

static	short	MiniSwitch = 0;

Ptr GetAudioChannel(Boolean LeftChannel, long Size)
{
	switch (MADDriver->DriverSettings.outPutBits)
	{
		case 16:
			switch (MADDriver->DriverSettings.outPutMode)
			{
			/*	case MonoOutPut:
					return Inverse16(MADDriver->OscilloWavePtr, Size);
				break;
				*/
				case StereoOutPut:
				case DeluxeStereoOutPut:
				
				/*	if (MADDriver->DriverSettings.driverMode == SoundManagerDriver && VMMode == true)
					{
						Ptr	 tempSoundPtr;
						
						tempSoundPtr = (char*) MADDriver->TheHeader.dbhBufferPtr[ MiniSwitch]->dbSoundData;
						
						MADDriver->TheHeader.dbhBufferPtr[ MiniSwitch]->dbUserInfo[ 1] = false;
						
						if (LeftChannel) return TransformationDSP(tempSoundPtr + 2, Size);
						else return TransformationDSP(tempSoundPtr, Size);
					}
					else */
					{
						Ptr 	OscilloPtr;
						short	i, PreviousDraw = DrawDraw, testMAXDRAW = 0;
						
						MADDriver->useOsciBuffers = true;
						
						OscilloPtr = PreviousOscilloPtr;
						
						if (LeftChannel == 0)
						{
							if (MADDriver->curTime>= TickCount() && lastCurTime != MADDriver->curTime)
							{
								short	PreviousCurDrawPtr;
								
								lastCurTime = MADDriver->curTime;
								
								OscilloPtr = MADDriver->OsciDrawPtr[ MADDriver->curDrawPtr];
								MADDriver->newData[ MADDriver->curDrawPtr] = false;
							}
							else
							{
								VASY:
								
								DrawDraw++;
								if (DrawDraw >= MAXDRAW) DrawDraw = 0;
								
								if (MADDriver->newData[ DrawDraw])
								{
									OscilloPtr = MADDriver->OsciDrawPtr[ DrawDraw];
									MADDriver->newData[ DrawDraw] = false;
								}
								else
								{
									DrawDraw = MADDriver->curDrawPtr;	//PreviousDraw;
								//	testMAXDRAW++;
								//	if (testMAXDRAW < MAXDRAW) goto VASY;
								}
							}
						}
						
						PreviousOscilloPtr = OscilloPtr;
						
						if (LeftChannel) return TransformationDSP(OscilloPtr + 2, Size);
						else return TransformationDSP(OscilloPtr, Size);
					}
				break;
			}
		break;
		
		case 8:
			switch (MADDriver->DriverSettings.outPutMode)
			{
			/*	case MonoOutPut:
					return MADDriver->OscilloWavePtr;
				break;
				*/
				case StereoOutPut:
				case DeluxeStereoOutPut:
					if (LeftChannel) return Transformation8(MADDriver->OscilloWavePtr + 1);
					else return Transformation8(MADDriver->OscilloWavePtr);
				break;
			}
		break;
	}
	
	return (Ptr) -1L;
}

short GetQuicktimeBits(void);

Ptr GetQuicktimeChannel(Boolean LeftChannel, long Size)
{
Ptr	 tempPtr = (Ptr) -1L;

	switch (GetQuicktimeBits())
	{
		case 16:
			tempPtr = Inverse16(GetQuicktimeSource(), 4096);
		break;
		
		case 8:
			tempPtr = GetQuicktimeSource();
		break;
	}
	
	if (LeftChannel) tempPtr = Transformation8(tempPtr + 1);
	else tempPtr = Transformation8(tempPtr);
	
	return tempPtr;
}

Ptr GetAudioInPut(Boolean LeftChannel, long Size)
{
	if (outPutPtr == (Ptr) -1L)
	{
		return GetAudioChannel(false, Size);
	}
	
	switch (SoundInputBits)
	{
		case 16:
			return Inverse16(outPutPtr, Size/2);
		break;
		
		case 8:
			return outPutPtr;
		break;
	}
	
	return (Ptr) -1L;
}

/*void C16BitOsci(short offsetV, short offsetH, short LocalV, Byte *tempPtr, Byte *tempPtr2)
{
Ptr			*CurrentQuickInt = CurrentQuick;
Point		thePt;
short		i, *curQuickShort, color, ioffsetH;

color = 0x1BFF;

i = SIter;
if (i + offsetH >= VA[ 0].frame.right)
{
	i = VA[ 0].frame.right - offsetH - 1;
	if (i < 0) return;
	tempPtr2 += SIter - i;
	tempPtr += SIter - i;
}

CurrentQuickInt += offsetV;
ioffsetH		= offsetH + i;

while (i > 0)
{
	thePt.h = i;
	thePt.v = *tempPtr2 + LocalV;
	if (PtInRgn(thePt, dlogptrRgn))
	{
		curQuickShort = (short*) CurrentQuickInt[ *tempPtr2];
		*(curQuickShort + ioffsetH) = 0x0000;
	}
	
	*tempPtr2 = ((*tempPtr) >> OsciDD);
	thePt.v = *tempPtr2 + LocalV;
	if (PtInRgn(thePt, dlogptrRgn))
	{
		curQuickShort = (short*) CurrentQuickInt[ *tempPtr2];
		*(curQuickShort + ioffsetH) = color;
	}
	tempPtr2++;
	tempPtr++;
	i--;
	ioffsetH--;
}
}*/

/*void C16BitOsciDirect(short offsetV, short offsetH, short LocalV, Byte *tempPtr, Byte *tempPtr2)
{
Ptr			*CurrentQuickInt = CurrentQuick;
short		i, *curQuickShort, color, ioffsetH;

color = 0x1BFF;

i = SIter;
if (i + offsetH >= VA[ 0].frame.right)
{
	i = VA[ 0].frame.right - offsetH - 1;
	if (i < 0) return;
	tempPtr2 += SIter - i;
	tempPtr += SIter - i;
}

CurrentQuickInt += offsetV;
ioffsetH		= offsetH + i;

while (i > 0)
{
	curQuickShort = (short*) CurrentQuickInt[ *tempPtr2];
	*(curQuickShort + ioffsetH) = 0x0000;
	
	*tempPtr2 = ((*tempPtr) >> OsciDD);
	curQuickShort = (short*) CurrentQuickInt[ *tempPtr2];
	*(curQuickShort + ioffsetH) = color;

	tempPtr2++;
	tempPtr++;
	i--;
	ioffsetH--;
}
}

void C8BitOsci(short offsetV, short offsetH, short LocalV, Byte *tempPtr, Byte *tempPtr2)
{
Ptr			*CurrentQuickInt = CurrentQuick;
Point		thePt;
short		i, ioffsetH;

i = SIter;
if (i + offsetH >= VA[ 0].frame.right)
{
	i = VA[ 0].frame.right - offsetH - 1;
	if (i < 0) return;
	tempPtr2 += SIter - i;
	tempPtr += SIter - i;
}

CurrentQuickInt += offsetV;
ioffsetH		= offsetH + i;

while (i > 0)
{
	thePt.h = i;
	thePt.v = *tempPtr2 + LocalV;
	if (PtInRgn(thePt, dlogptrRgn))
	{
		*(CurrentQuickInt[ *tempPtr2] + ioffsetH) = 0xFF;
	}
	
	*tempPtr2 = ((*tempPtr) >> OsciDD);
	thePt.v = *tempPtr2 + LocalV;
	if (PtInRgn(thePt, dlogptrRgn))
	{
		*(CurrentQuickInt[ *tempPtr2] + ioffsetH) = OsciColor;
	}
	
	tempPtr2++;
	tempPtr++;
	i--;
	ioffsetH--;
}
}

void C8BitOsciDirect(short offsetV, short offsetH, short LocalV, Byte *tempPtr, Byte *tempPtr2)
{
Ptr			*CurrentQuickInt = CurrentQuick;
short		i, ioffsetH;

i = SIter;
if (i + offsetH >= VA[ 0].frame.right)
{
	i = VA[ 0].frame.right - offsetH - 1;
	if (i < 0) return;
	tempPtr2 += SIter - i;
	tempPtr += SIter - i;
}

CurrentQuickInt += offsetV;
ioffsetH		= offsetH + i;

while (i > 0)
{
	*(CurrentQuickInt[ *tempPtr2] + ioffsetH) = 0xFF;
	*tempPtr2 = ((*tempPtr) >> OsciDD);
	*(CurrentQuickInt[ *tempPtr2] + ioffsetH) = OsciColor;
	
	tempPtr2++;
	tempPtr++;
	i--;
	ioffsetH--;
}
}*/

void C8BitOsciPixMap(Byte *tempPtr, Byte *tempPtr2, Ptr pixMapPtr)
{
	long		i, x;
	Byte 		*ctempPtr = tempPtr, *ctempPtr2 = tempPtr2;
	Ptr			cpixMapPtr = pixMapPtr;
	
	// Erase pts
	
	i = SIter;
	while (i > 0)
	{
		*(pixMapPtr + CurrentQuickPixMap[ *tempPtr2] + i) = 0xFF;
		
		tempPtr2++;
		i--;
	}
	
	// Draw Frame
	i = SIter;
	pixMapPtr = cpixMapPtr + CurrentQuickPixMap[ 1];
	while (i-- > 0) *(pixMapPtr++) = 0xE5;
	
	i = SIter;
	pixMapPtr = cpixMapPtr + CurrentQuickPixMap[ (*osciPixMap[ 0])->bounds.bottom-2];
	while (i-- > 0) *(pixMapPtr++) = 0xE5;
	
	i = SIter;
	pixMapPtr = cpixMapPtr + CurrentQuickPixMap[ (*osciPixMap[ 0])->bounds.bottom/2];
	while (i-- > 0) *(pixMapPtr++) = 0xE5;
	
	for (x = 0; x < 5; x++)
	{
		i = (*osciPixMap[ 0])->bounds.bottom - 2;
		pixMapPtr = cpixMapPtr + CurrentQuickPixMap[ 1] + (x * ((*osciPixMap[ 0])->bounds.right - (*osciPixMap[ 0])->bounds.left)) / 4;
		if (x == 4) pixMapPtr--;
		while (i-- > 0) *(pixMapPtr + CurrentQuickPixMap[ i]) = 0xE5;
	}
	
	// Draw pts
	tempPtr = ctempPtr;
	tempPtr2 = ctempPtr2;
	pixMapPtr = cpixMapPtr;
	
	i = SIter;
	while (i > 0)
	{
		*tempPtr2 = ((*tempPtr) >> OsciDD);
		*(pixMapPtr + CurrentQuickPixMap[ *tempPtr2] + i) = OsciColor;
		
		tempPtr2++;
		tempPtr++;
		i--;
	}
}

void  Bresenham(long x1, long y1, long x2, long y2, long row, Byte color, Ptr src)
{

register	long			deltaX, deltaY;
register 	long			loopD3, loopD2;
register 	Ptr				outPtr, oPtr;
register	unsigned short	dColor = (color << 8) + color;

if (x1 < x2)
{
	deltaX = x2;
	x2 = x1;
	x1 = deltaX;

	deltaX = y2;
	y2 = y1;
	y1 = deltaX;
}

deltaX = x1 - x2;
deltaY = y1 - y2;
if (deltaY < 0) deltaY = -deltaY;

oPtr = src + CurrentQuickPixMap[ y1] + x1;
outPtr = src + CurrentQuickPixMap[ y2] + x2;

if (y1 < y2) row = -row;

if (deltaY >= deltaX)
{
	loopD3 = loopD2 = deltaY>>1;

	while (loopD3 >= 0)
	{
		//*outPtr	= color;	*(outPtr+1)	= color;	//*(outPtr+row)	= color;	*(outPtr+row+1)	= color;
		*((unsigned short*) outPtr) = dColor;
		outPtr += row;
		
		//*oPtr	= color;	*(oPtr+1)	= color;	//*(oPtr+row)	= color;	*(oPtr+row+1)	= color;
		*((unsigned short*) oPtr) = dColor;
		oPtr   -= row;
		
		loopD2 -= deltaX;
	
		if (loopD2 < 0)
		{
			outPtr ++;
			oPtr --;
			
			loopD2 += deltaY;
		}
		loopD3 --;
	}
	return;
}
else
{
	loopD3 = loopD2 = deltaX>>1;
	
	while (loopD3 >= 0)
	{
		//*(outPtr ++) = color;		*(outPtr)	= color;	//*(outPtr+row-1)	= color;	*(outPtr+row)	= color;
		//*(oPtr --)   = color;		*(oPtr)		= color;		//*(oPtr+row-1)	= color;	*(oPtr+row)	= color;
		
		*((unsigned short*) outPtr) = dColor;	outPtr ++;
		*((unsigned short*) oPtr) = dColor;		oPtr --;
		
		loopD2      -= deltaY;

		if (loopD2 < 0)
		{
			outPtr += row;
			oPtr   -= row;
			loopD2 += deltaX;
		}
		loopD3 --;
	}
	return;
}
}


void C8BitOsciPixMapLine(Byte *tempPtr, Byte *tempPtr2, Ptr pixMapPtr)
{
	long		i, x;
	Byte 		*ctempPtr = tempPtr, *ctempPtr2 = tempPtr2;
	Ptr			cpixMapPtr = pixMapPtr;
	long		lasti, lasttempPtr2, rowBytes = (*osciPixMap[ 0])->rowBytes + 0x8000;
	
	// Erase pts
	
	i = SIter-1;
	lasti = i;
	lasttempPtr2 = *tempPtr2;
	while (i > 0)
	{
		Bresenham(i, *tempPtr2, lasti, lasttempPtr2, rowBytes, 0xFF, pixMapPtr);
		
		lasti = i;
		lasttempPtr2 = *tempPtr2;
		
		tempPtr2++;
		i--;
	}
	
	// Draw Frame
	i = SIter;
	pixMapPtr = cpixMapPtr + CurrentQuickPixMap[ 1];
	while (i-- > 0) *(pixMapPtr++) = 0xE5;
	
	i = SIter;
	pixMapPtr = cpixMapPtr + CurrentQuickPixMap[ (*osciPixMap[ 0])->bounds.bottom-2];
	while (i-- > 0) *(pixMapPtr++) = 0xE5;
	
	i = SIter;
	pixMapPtr = cpixMapPtr + CurrentQuickPixMap[ (*osciPixMap[ 0])->bounds.bottom/2];
	while (i-- > 0) *(pixMapPtr++) = 0xE5;
	
	for (x = 0; x < 5; x++)
	{
		i = (*osciPixMap[ 0])->bounds.bottom - 2;
		pixMapPtr = cpixMapPtr + CurrentQuickPixMap[ 1] + (x * ((*osciPixMap[ 0])->bounds.right - (*osciPixMap[ 0])->bounds.left)) / 4;
		if (x == 4) pixMapPtr--;
		while (i-- > 0) *(pixMapPtr + CurrentQuickPixMap[ i]) = 0xE5;
	}
	
	// Draw pts
	tempPtr = ctempPtr;
	tempPtr2 = ctempPtr2;
	pixMapPtr = cpixMapPtr;
	
	i = SIter-1;
	lasti = i;
	lasttempPtr2 = ((*tempPtr) >> OsciDD);
	while (i > 0)
	{
		*tempPtr2 = ((*tempPtr) >> OsciDD);
		Bresenham(i, *tempPtr2, lasti, lasttempPtr2, rowBytes, OsciColor, pixMapPtr);
		
		lasti = i;
		lasttempPtr2 = *tempPtr2;
		
		tempPtr2++;
		tempPtr++;
		i--;
	}
}

static long gmax, gmin;

void DrawOscilloscope2(OsciRec	*osciPtr, short no, Boolean lastCall)
{
short		temp,i,x;
long		soundlevel;
Rect		tempRect, dstRect, resultRect;
Boolean		doubleView;
Ptr			tempPtrCopy;
short		realno = no;
Rect		caRect, inRect;
	
	if (thePrefs.osciTile)
	{
		no = 0;
	}
	else
	{
		gmax = 0;
		gmin = 512;
	}
	
	GetPortBounds(GetDialogPort(OscilloDlog), &caRect);
	
	if (OsciType == OutPutAudio) tempRect.left	= 16;
	else tempRect.left = 0;
	tempRect.right	=  caRect.right-16;
	
	tempRect.top	= osci[ no].VPos;
	tempRect.bottom	= tempRect.top + OsciH;
	
	dstRect = tempRect;
	
	if (osciPixMap[ no] != NULL)
	{
		for (i = 0; i < SIter; i++)
		{
			if (gmax < osciPtr->SavePtr[ i]) gmax = osciPtr->SavePtr[ i];
			if (gmin > osciPtr->SavePtr[ i]) gmin = osciPtr->SavePtr[ i];
		}
		
		//
		
		
		if (thePrefs.OscilloLine) C8BitOsciPixMapLine(	(Byte*) GetAudioSource(realno) + OsciOffSet,
							(Byte*) osciPtr->SavePtr,
							(*osciPixMap[ no])->baseAddr);
		
		else
		
		C8BitOsciPixMap(	(Byte*) GetAudioSource(realno) + OsciOffSet,
							(Byte*) osciPtr->SavePtr,
							(*osciPixMap[ no])->baseAddr);
		
		//
		
		for (i = 0; i < SIter; i++)
		{
			if (gmax < osciPtr->SavePtr[ i]) gmax = osciPtr->SavePtr[ i];
			if (gmin > osciPtr->SavePtr[ i]) gmin = osciPtr->SavePtr[ i];
		}
		
		inRect = (*osciPixMap[ no])->bounds;
		
		
		if (lastCall || !thePrefs.osciTile)
		{
			gmax++;
			
			inRect.bottom	= inRect.top + gmax;
			inRect.top		= inRect.top + gmin;
			
			tempRect.bottom	= tempRect.top + gmax;
			tempRect.top	= tempRect.top + gmin;
			
			//osciPtr->min = gmin;
			//osciPtr->max = gmax;
			
			BackColor(whiteColor);
			CopyBits ( (BitMap *) (*osciPixMap[ no]),
						(BitMap*) *GetPortPixMap(GetDialogPort(OscilloDlog)),
						&inRect,
						&tempRect,
						srcCopy,
						nil);
						
		/*	ForeColor(redColor);
			MoveTo(tempRect.left, tempRect.top);
			LineTo(tempRect.right, tempRect.top);
			ForeColor(blackColor);*/
			
			RGBBackColor(&theColor);
		}
		
		// compute level
			if (OsciType == OutPutAudio)
			{
					if (realno != 0) soundlevel = MADDriver->levelR;
					else soundlevel = MADDriver->levelL;
					
					soundlevel = (soundlevel * (long) (OsciH-3)) / 65536;
					
					if (!thePrefs.osciTile)
					{
						inRect.left  = 2;
						inRect.right = 13;
					}
					else
					{
						if (realno == 0)
						{
							inRect.left  = 2;
							inRect.right = 7;
						}
						else
						{
							inRect.left  = 8;
							inRect.right = 13;
						}
					}
					inRect.bottom = soundlevel;
					inRect.top = osci[ no].VPos +3;
					inRect.bottom = osci[ no].VPos + OsciH -3 - soundlevel;
					if (inRect.bottom < inRect.top) inRect.bottom = inRect.top;
					ForeColor(blackColor);
					PaintRect(&inRect);
					
					RGBForeColor(&myBlueColor);
					if (realno != 0) {if (MADDriver->clipR) ForeColor(redColor);}
					else {if (MADDriver->clipL) ForeColor(redColor);}
					
					if (realno != 0)
					{
						timeLevelR--;
						maxLevelR -= 1;
					//	if (timeLevelR < 0) {timeLevelR = 60; maxLevelR = soundlevel;}
						if (soundlevel > maxLevelR) {maxLevelR = soundlevel;  timeLevelR = 60;}
						
						temp = osci[ no].VPos + OsciH -3 - maxLevelR;
						if (temp < inRect.top) temp = inRect.top;
						
						MoveTo(inRect.left, temp);
						LineTo(inRect.right-1, temp);
					}
					else
					{
						timeLevelL--;
						maxLevelL -= 1;
					//	if (timeLevelL < 0) {timeLevelL = 60; fallL = 1;	maxLevelL = soundlevel;}
						if (soundlevel > maxLevelL) {maxLevelL = soundlevel; timeLevelL = 60;}
						
						temp = osci[ no].VPos + OsciH -3 - maxLevelL;
						if (temp < inRect.top) temp = inRect.top;
						
						MoveTo(inRect.left, temp);
						LineTo(inRect.right-1, temp);
					}

					
					inRect.top = inRect.bottom;
					inRect.bottom = osci[ no].VPos + OsciH-3;
					PaintRect(&inRect);
										
					ForeColor(blackColor);
			}
	}
	else
	{
		Str255 str;
		
		pStrcpy(str, "\pNot Enough Memory");
		BackColor(blackColor);
		ForeColor(redColor);
		TETextBox(str+1, str[ 0], &tempRect, teCenter);
		ForeColor(blackColor);
		RGBBackColor(&theColor);
	}
}

void DoNull(void)
{
short			temp,i,x, OsciSee;
GrafPtr			myPort;
Rect			caRect, tempRect, CursRect;
Handle			itemHandle;

	if (OscilloDlog == NULL) return;
	
	// ******* ******* ******* ******* ******* ******* ******* *******
	
	
	// ******* ******* ******* ******* ******* ******* ******* *******
	
	if (TickCount() <= oldNullTicks) return;
	oldNullTicks = TickCount()+1;
	
	GetPort(&myPort);
	SetPortDialogPort(OscilloDlog);
	
	GetPortBounds(GetDialogPort(OscilloDlog), &caRect);
	
	if (thePrefs.osciTile) OsciSee = OsciNo-1;
	else OsciSee = (caRect.bottom - 15 - OsciVStart) / (OsciH + InterText);
	
	gmin = 512;
	gmax = 0;
	
/*	if (MADDriver->DriverSettings.driverMode == SoundManagerDriver)
	{
		if (MiniSwitch == 0)
		{
			if (MADDriver->TheHeader.dbhBufferPtr[ 1]->dbUserInfo[ 1] == true) MiniSwitch = 1;
			else goto End;
		}
		else
		{
			if (MADDriver->TheHeader.dbhBufferPtr[ 0]->dbUserInfo[ 1] == true) MiniSwitch = 0;
			else goto End;
		}
	}*/
	
	for (i = GetControlValue(VControl); i < GetControlValue(VControl) + OsciSee; i++)
		DrawOscilloscope2(&osci[ i], i, false);
	
	if (thePrefs.osciTile)
	{
		DrawOscilloscope2(&osci[ i], i, true);
	}
	End:
	SetPort(myPort);
}

void FillInterText(Rect *tempRect, short i)
{
	Str255		aStr, bStr, cStr;
	
	switch (OsciType)
	{
		case OutPutAudio:
			pStrcpy(aStr, "\pAudio OutPut: ");
		break;
		
		case InPutAudio:
			pStrcpy(aStr, "\pAudio InPut: ");
		break;
		
		case TrackAudio:
			pStrcpy(aStr, "\pDriver Tracks: ");
		break;
		
		case QuicktimeMovie:
			pStrcpy(aStr, "\pQuicktime Channel: ");
		break;
	}
	
	pStrcat(aStr, osci[ i].Name);
	pStrcat(aStr, "\p (");
	if (thePrefs.osciTile) NumToString(OsciNo, bStr);
	else NumToString(i + 1, bStr);
	pStrcat(aStr, bStr);
	pStrcat(aStr, "\p)");
	
	pStrcat(aStr, "\p    Buffer Size: ");
	NumToString(osci[ i].Size, bStr);
	pStrcat(aStr, bStr);
	pStrcat(aStr, "\p bytes");

/*	pStrcat(aStr, "\p    Display (");
	NumToString(OsciL, bStr);
	pStrcat(aStr, bStr);
	pStrcat(aStr, "\p) from: ");
	NumToString(OsciOffSet, bStr);
	pStrcat(aStr, bStr);
	pStrcat(aStr, "\p to: ");
	NumToString(OsciOffSet + SIter + 1, bStr);
	pStrcat(aStr, bStr);
*/
	tempRect->left += 2;
	TETextBox(aStr+1, aStr[ 0], tempRect, teJustLeft);
	tempRect->left -= 2;
}

void  UpdateOscilloWindow(DialogPtr GetSelection)
{ 
		Rect   		inRect, caRect, tempRect, itemRect, bRect;
 		GrafPtr		SavePort;
 		Str255		tempStr, aStr;
 		short		itemType, Larg, i ,x, start, end, OsciSee;
 		Handle		itemHandle;
 		RgnHandle	saveClip;
 		RgnHandle	visibleRegion;
 		
 		
 		GetPort(&SavePort);
 		SetPortDialogPort(OscilloDlog);

		TextFont(kFontIDGeneva);	TextSize(9);

		BeginUpdate(GetDialogWindow(OscilloDlog));
		
		OsciOffSet	= GetControlValue(HControl);
		
		/****************************/
		/**** Draw Oscilloscopes ****/
		/****************************/
		
		tempRect.top	= OsciVStart;
		tempRect.bottom = tempRect.top + OsciH;
		
		GetPortBounds(GetDialogPort(OscilloDlog), &caRect);
		
		if (thePrefs.osciTile) OsciSee = 1;			// OscinO
		else OsciSee = (caRect.bottom - 15 - OsciVStart) / (OsciH + InterText);
		
		for (i = GetControlValue(VControl); i < GetControlValue(VControl) + OsciSee; i++)
		{
			if (OsciType == OutPutAudio) tempRect.left	= 15;
			else tempRect.left	= 0;
			tempRect.right	= caRect.right - 15;
			
			
			
			if (thePrefs.osciTile) osci[ i].VPos = OsciVStart;
			else osci[ i].VPos = tempRect.top;
			
			BackColor(whiteColor);
			if (thePrefs.osciTile)
				CopyBits ( (BitMap *) (*osciPixMap[ 0]),
					(BitMap*) *GetPortPixMap(GetDialogPort(OscilloDlog)),
					&(*osciPixMap[ 0])->bounds,
					&tempRect,
					srcCopy,
					nil);
			else
				CopyBits ( (BitMap *) (*osciPixMap[ i]),
					(BitMap*) *GetPortPixMap(GetDialogPort(OscilloDlog)),
					&(*osciPixMap[ i])->bounds,
					&tempRect,
					srcCopy,
					nil);
			RGBBackColor(&theColor);
			
			// Sound Level
			if (OsciType == OutPutAudio)
			{
				inRect.left  = 0;
				inRect.right = 15;
				inRect.top = osci[ i].VPos;
				inRect.bottom = osci[ i].VPos + OsciH;
				ForeColor(blackColor);
				PaintRect(&inRect);
				inRect.top++;
				inRect.bottom--;
				ForeColor(greenColor);
				FrameRect(&inRect);
				ForeColor(blackColor);
			}
			// InterText //
			bRect = tempRect;
			bRect.top = tempRect.bottom;
			bRect.bottom = bRect.top + InterText;
			bRect.right = caRect.right - 16;
			FillInterText(&bRect, i);
			///////////////
			
			tempRect.top	+= OsciH + InterText;
			tempRect.bottom = tempRect.top + OsciH;
		}
		/****************************/
		/****************************/
		
		MoveTo(0, OsciVStart);
		LineTo(caRect.right, OsciVStart);
		
		visibleRegion = NewRgn();
		
		GetPortVisibleRegion(GetDialogPort(GetSelection), visibleRegion);
		
		UpdateDialog(GetSelection, visibleRegion);
		
		DisposeRgn(visibleRegion);
		
		EndUpdate(GetDialogWindow(OscilloDlog));
		
		oldNullTicks = TickCount()-1;
		DoNull();
		
		DrawGrowIconP(GetSelection);

		SetPort(SavePort);
}

static	DialogPtr	theDialogControl;

pascal void actionProcOscillo(ControlHandle theControl, short ctlPart)
{
long			lRefCon;
short			CurWin, maxValue, minValue, curVal, sVal, OsciSee;
Rect			caRect, aRect;

if (ctlPart <= 0) return;

lRefCon = GetControlReference(theControl);
maxValue = GetControlMaximum(theControl);
minValue = GetControlMinimum(theControl);
curVal = sVal = GetControlValue(theControl);

GetPortBounds(GetDialogPort(OscilloDlog), &caRect);

OsciSee = (caRect.bottom - 15 - OsciVStart) / (OsciH + InterText);

	switch (ctlPart)
	{
		case kControlUpButtonPart:
			if (lRefCon == hID) curVal -= OSCILLODEF;
			else curVal -= 1;
			if (curVal < minValue) curVal = minValue;
		break;
		
		case kControlDownButtonPart:
			if (lRefCon == hID) curVal += OSCILLODEF;
			else curVal += 1;
			if (curVal > maxValue) curVal = maxValue;
		break;
		
		case kControlPageUpPart:
			if (lRefCon == hID) curVal -= SIter;
			else curVal -= OsciSee;
			if (curVal < minValue) curVal = minValue;
		break;
		
		case kControlPageDownPart:
			if (lRefCon == hID) curVal += SIter;
			else curVal += OsciSee;
			if (curVal > maxValue) curVal = maxValue;
		break;
		
		case kControlIndicatorPart:
			sVal = gThumbPrev;
			gThumbPrev = curVal;
		break;
	}
	
	SetControlValue(theControl, curVal);
	
	if (sVal != curVal)
	{
		if (lRefCon == vID)
		{
			GetWorkingZone(&aRect);
			EraseRect(&aRect);
			InvalWindowRect(GetDialogWindow(theDialogControl), &aRect);
			UpdateOscilloWindow(theDialogControl);			/* Important pour la mise à jour des VPos !!! */
		}
		else
		{
			OsciOffSet	= GetControlValue(HControl);
			DoGlobalNull();
		}
	}
	else DoGlobalNull();
}

void DoItemPressOscillo(short whichItem, DialogPtr whichDialog)	/* Item hit ID to pass to Dialog function */
{
	Cell				theCell;
	long				mresult;
	short				temp, bogus, itemType, ctlPart, curSelec;
	Rect				tempRect;
	Handle				itemHandle;
	GrafPtr				SavePort;
	ControlHandle		theControl;
	Str255				aStr;
	Point				myPt;
	ControlActionUPP	MyControlUPP;
	
	GetPort(&SavePort);
	SetPortDialogPort(OscilloDlog);
	
	if (theEvent.what == mouseDown)
	{
		myPt = theEvent.where;
		GlobalToLocal(&myPt);
		
		theControl = NULL;
		if (TestControl(HControl, myPt))
			theControl = HControl;
		if (TestControl(VControl, myPt))
			theControl = VControl;
		
		if (theControl == HControl || theControl == VControl) {
			/*	if (ctlPart == kControlIndicatorPart && gUseControlSize == false)
			 {
			 bogus = TrackControl(theControl, myPt, NULL);
			 if (bogus != 0)
			 {
			 Rect aRect;
			 
			 GetWorkingZone(&aRect);
			 EraseRect(&aRect);
			 InvalWindowRect(GetDialogWindow(theDialogControl), &aRect);
			 UpdateOscilloWindow(theDialogControl);
			 }
			 }
			 else if (ctlPart > 0)*/
			{
				theDialogControl = whichDialog;
				MyControlUPP = NewControlActionUPP(actionProcOscillo);
				gThumbPrev = GetControlValue(theControl);
				TrackControl(theControl, myPt, MyControlUPP);
				DisposeControlActionUPP(MyControlUPP);
			}
		}
	}
	
	
	switch (whichItem)
	{
		case 5:
			InsertMenu(OsciTypeMenu, hierMenu);
			GetDialogItem(whichDialog, whichItem, &itemType, &itemHandle, &tempRect);
			curSelec = OsciType;
			
			myPt.v = tempRect.top;	myPt.h = tempRect.left;
			LocalToGlobal(&myPt);
			
			SetItemMark(OsciTypeMenu, curSelec + 1, 0xa5);
			
			mresult = PopUpMenuSelect(	OsciTypeMenu,
									  myPt.v,
									  myPt.h,
									  curSelec + 1);
			
			SetItemMark(OsciTypeMenu, curSelec + 1, 0);
			
			if (HiWord(mresult) != 0)
			{
				switch (LoWord(mresult) - 1)
				{
					case OutPutAudio:
						OsciType = OutPutAudio;
						if (SpectrumMicrophone == false) MicroOff();
						OscilloMicrophone = false;
						SetDText(OscilloDlog, 6, "\pAudio OutPut");
						break;
						
					case InPutAudio:
						if (ActiveSoundInput(false, NULL, "\p") == noErr)
						{
							OsciType = InPutAudio;
							OscilloMicrophone = true;
							SetDText(OscilloDlog, 6, "\pAudio InPut");
						}
						
						CloseSoundInput();
						break;
						
					case TrackAudio:
						OsciType = TrackAudio;
						if (MicroPhone == true && SpectrumMicrophone == false) MicroOff();
						OscilloMicrophone = false;
						SetDText(OscilloDlog, 6, "\pDriver Tracks");
						break;
						
					case QuicktimeMovie:
						OsciType = QuicktimeMovie;
						if (MicroPhone == true && SpectrumMicrophone == false) MicroOff();
						OscilloMicrophone = false;
						SetDText(OscilloDlog, 6, "\pQuicktime Sound Channel");
						break;
				}
				SetWindowEnviron();
			}
			DeleteMenu(GetMenuID(OsciTypeMenu));
 			break;
 			
		case 12:
			InsertMenu(OsciModeMenu, hierMenu);
			GetDialogItem(whichDialog, whichItem, &itemType, &itemHandle, &tempRect);
			
			if (thePrefs.osciTile) curSelec = 0;
			else curSelec = 1;
			
			myPt.v = tempRect.top;	myPt.h = tempRect.left;
			LocalToGlobal(&myPt);
			
			SetItemMark(OsciModeMenu, curSelec + 1, 0xa5);
			
			mresult = PopUpMenuSelect(	OsciModeMenu,
									  myPt.v,
									  myPt.h,
									  curSelec + 1);
			
			SetItemMark(OsciModeMenu, curSelec + 1, 0);
			
			if (HiWord(mresult) != 0)
			{
				switch (LoWord(mresult))
				{
					case 1:	thePrefs.osciTile = true;	pStrcpy(aStr, "\pStack");	break;
					case 2:	thePrefs.osciTile = false;	pStrcpy(aStr, "\pTile");	break;
				}
				
				SetDText(OscilloDlog, 13, aStr);
				
				SetWindowEnviron();
			}
			DeleteMenu(GetMenuID(OsciModeMenu));
 			break;
 			
		case 8:
			InsertMenu(OsciHMenu, hierMenu);
			GetDialogItem(whichDialog, whichItem, &itemType, &itemHandle, &tempRect);
			
			switch (OsciH)
		{
			default:
			case 16:	curSelec = 0;		break;
			case 32:	curSelec = 1;		break;
			case 64:	curSelec = 2;		break;
			case 128:	curSelec = 3;		break;
			case 256:	curSelec = 4;		break;
		}
			
			myPt.v = tempRect.top;	myPt.h = tempRect.left;
			LocalToGlobal(&myPt);
			
			SetItemMark(OsciHMenu, curSelec + 1, 0xa5);
			
			mresult = PopUpMenuSelect(	OsciHMenu,
									  myPt.v,
									  myPt.h,
									  curSelec + 1);
			
			SetItemMark(OsciHMenu, curSelec + 1, 0);
			
			if (HiWord(mresult) != 0)
			{
				switch (LoWord(mresult))
				{
					case 1:	OsciH = 16;		OsciDD = 4; 	break;
					case 2:	OsciH = 32;		OsciDD = 3;		break;
					case 3:	OsciH = 64;		OsciDD = 2; 	break;
					case 4:	OsciH = 128;	OsciDD = 1;		break;
					case 5:	OsciH = 256;	OsciDD = 0;		break;
				}
				
				NumToString(OsciH, aStr);
				SetDText(OscilloDlog, 9, aStr);
				
				SetWindowEnviron();
			}
			DeleteMenu(GetMenuID(OsciHMenu));
 			break;
	}
	
	SetPort(SavePort);
}

void SetWindowEnviron(void)
{
	short		itemType, tempB, tempA, prevRight, prevBot;
	Handle		itemHandle;
	long		i;
	Rect		caRect, itemRect;
	GrafPtr		savePort;
	Str255		tempStr, aStr;
	
	if (OscilloDlog == NULL)
		return;
	
	GetPort(&savePort);
	SetPortDialogPort(OscilloDlog);
	
	/**** Oscillo ******/
	
	OsciL		= GetAudioSize();
	
	switch (OsciType)
	{
		case OutPutAudio:
			osci[ 0].VPos		=	OsciVStart ;
			osci[ 0].SavePtr	=	(Byte*) ValSaveData;
			osci[ 0].Size		=	GetAudioSize();
			
			//if (MADDriver->DriverSettings.outPutMode != MonoOutPut)
		{
			if (thePrefs.osciTile)
				pStrcpy(osci[0].Name, "\pLeft/Right Channels");
			else
				pStrcpy(osci[0].Name, "\pLeft Channel");
			
			if (thePrefs.osciTile)
				osci[1].VPos		=	OsciVStart;
			else
				osci[1].VPos		=	OsciVStart + OsciH + InterText;
			osci[1].SavePtr		=	osci[ 0].SavePtr + osci[ 0].Size;
			osci[1].Size		=	GetAudioSize();
			pStrcpy(osci[ 1].Name, "\pRight Channel");
			
			OsciNo = 2;
		}
#if 0
			else
			{
				pStrcpy(osci[ 0].Name, "\pMono Channel");
				OsciNo = 1;
			}
#endif
			break;
			
		case InPutAudio:
			osci[0].VPos		=	OsciVStart;
			osci[0].SavePtr		=	(Byte*) ValSaveData;
			osci[0].Size		=	GetAudioSize();
			pStrcpy(osci[ 0].Name, InPutName);
			
			OsciNo = 1;
			break;
			
		case TrackAudio:
			for (i = 0; i < MADDriver->DriverSettings.numChn; i++) {
				osci[i].VPos		=	OsciVStart;
				osci[i].SavePtr	=	(Byte*) (ValSaveData + i * GetAudioSize());
				if (i * GetAudioSize() > VALSIZE) MyDebugStr(__LINE__, __FILE__, "");
				osci[ i].Size		=	GetAudioSize();
				
				if(thePrefs.osciTile)
					pStrcpy(osci[i].Name, "\pAll Channels");
				else
					pStrcpy(osci[i].Name, "\pTrack");
			}
			
			OsciNo = MADDriver->DriverSettings.numChn;
			break;
			
		case QuicktimeMovie:
			osci[ 0].VPos		=	OsciVStart;
			osci[ 0].SavePtr	=	(Byte*) ValSaveData;
			osci[ 0].Size		=	GetAudioSize();
			
			if (thePrefs.osciTile)
				pStrcpy(osci[ 0].Name, "\pLeft/Right Channels");
			else
				pStrcpy(osci[ 0].Name, "\pLeft Channel");
			
			if (thePrefs.osciTile) osci[ 1].VPos		=	OsciVStart;
			else osci[ 1].VPos		=	OsciVStart + OsciH + InterText;
			
			osci[ 1].SavePtr	=	osci[ 0].SavePtr + osci[ 0].Size;
			osci[ 1].Size		=	GetAudioSize();
			pStrcpy(osci[ 1].Name, "\pRight Channel");
			
			OsciNo = 2;	//GetQuicktimeChannels();
			break;
	}
	
	/**** Check Window Size ****/
	
	GetPortBounds(GetDialogPort(OscilloDlog), &caRect);
	
	prevRight	= caRect.right;
	prevBot		= caRect.bottom;
	
	if (thePrefs.osciTile) {
		if (prevBot > OsciVStart + 15 + 1 * (InterText + OsciH))
			prevBot = OsciVStart + 15 + 1 * (InterText + OsciH);
	} else {
		if (prevBot > OsciVStart + 15 + OsciNo * (InterText + OsciH))
			prevBot = OsciVStart + 15 + OsciNo * (InterText + OsciH);
	}
	if (prevBot < OsciVStart + InterText + OsciH + 15)
		prevBot = OsciVStart + InterText + OsciH + 15;
	
	if (prevRight > OsciL + 15)
		prevRight = OsciL + 15;
	
	
	prevBot -= OsciVStart + 15;
	prevBot /= OsciH + InterText;
	prevBot *= OsciH + InterText;
	prevBot += OsciVStart + 15;
	
	MySizeWindow(OscilloDlog, prevRight, prevBot, false);
	
	/**** SIter ******/
	
	GetPortBounds(GetDialogPort(OscilloDlog), &caRect);
	EraseRect(&caRect);
	InvalWindowRect(GetDialogWindow(OscilloDlog), &caRect);
	
	SIter = caRect.right - 16 - 15;
	//if (SIter > 740) MyDebugStr(__LINE__, __FILE__, "SIter is VERY BIG !");
	for (i = 0; i < VALSIZE; i++) {
		if (i % 2 == 0)
			ValSaveData[i] = 0;
		else
			ValSaveData[i] = OsciH-1;
	}
	
	/********/
	
	SetControlOscillo();
	OsciOffSet	= GetControlValue(HControl);
	
	/********/
	switch (OsciType)
	{
		case OutPutAudio:
			SetDText(OscilloDlog, 6, "\pAudio OutPut");
			break;
			
		case InPutAudio:
			SetDText(OscilloDlog, 6, "\pAudio InPut");
			break;
			
		case TrackAudio:
			SetDText(OscilloDlog, 6, "\pDriver Tracks");
			break;
			
		case QuicktimeMovie:
			SetDText(OscilloDlog, 6, "\pQuicktime Channel");
			break;
	}
	
	NumToString(OsciH, aStr);
	SetDText(OscilloDlog, 9, aStr);
	
	if (thePrefs.osciTile)
		pStrcpy(aStr, "\pStack");
	else
		pStrcpy(aStr, "\pTile");
	SetDText(OscilloDlog, 13, aStr);
	
	//AdjustZoomOscillo();
	
	if (thePrefs.osciTile)
		SetMaxWindow(OsciL + 15, OsciVStart + 15 + 1 * (InterText + OsciH), OscilloDlog);
	else
		SetMaxWindow(OsciL + 15, OsciVStart + 15 + OsciNo * (InterText + OsciH), OscilloDlog);
	
	GetWorkingZone(&itemRect);
	EraseRect(&itemRect);
	InvalWindowRect(GetDialogWindow(OscilloDlog), &itemRect);
	
	SetPort(savePort);
}

void InitOscillo(void)
{
	long		result, i;
	Rect		picRect;
	OSErr		iErr;
	short		myBit;
	
	SetRect(&picRect, 0, 0, 2, 2);
	
	maxLevelL = maxLevelR = timeLevelL = timeLevelR = 0;
	
	NewGWorld(	&gGWorld,
				8,
				&picRect,
				GetCTable(8),				// CTabHandle
				nil,
				(GWorldFlags) 0);
	
	PseudoSilence = MyNewPtr(750);
	for (i = 0; i < 740; i++) PseudoSilence[ i] = 0x80;
	
	AudioDSPPtr = (Ptr) MyNewPtr(AUDIODSPSIZE * 4L);
	
	
	
	iErr = Gestalt(gestaltVMAttr, &result);
	if (iErr) Debugger();
	
	myBit = gestaltVMPresent;
	VMMode = BitTst(&result, 31-myBit);
	VMMode = true;
	
	PreviousOscilloPtr = MADDriver->OsciDrawPtr[ 0];
	
/*	for (i = 0; i < DRAWBUFFERSIZE; i++)
	{
		DrawBuf[ i] = NewPtrClear(MADDriver->BufSize);
		AlreadyDraw[ i] = false;
	}
	
	DrawDraw = 0;
	DrawIn = 0;*/
}

long GetAudioSize(void)
{
	switch (OsciType)
	{
		case OutPutAudio:
			return MADDriver->ASCBUFFERReal;
		break;
		
		case TrackAudio:
			return TRACKSIZE;
		break;
		
		case InPutAudio:
			return (deviceBufferSize * 8) / SoundInputBits;
		break;
		
		case QuicktimeMovie:
			return 1024;
		break;
	}
	
	return 0;
}

Ptr GetAudioSource(short item)
{
	Ptr	sourcePtr;

	switch (OsciType)
	{
		case OutPutAudio:
			OsciColor = 0x90;
			
			if (item != 0) {
				if (MADDriver->clipR) OsciColor = 215;
				//MADDriver->clipR = false;
			} else {
				if (MADDriver->clipL) OsciColor = 215;
				//MADDriver->clipL = false;
			}

			
			if (item == 0)
				return GetAudioChannel(false, MADDriver->ASCBUFFERReal);
			else
				return GetAudioChannel(true, MADDriver->ASCBUFFERReal);
		break;
		
		case InPutAudio:
			OsciColor = 0x90;
			return GetAudioInPut(false, deviceBufferSize);
		break;
		
		case QuicktimeMovie:
			OsciColor = 0x90;
			
			if (item == 0)
				return GetQuicktimeChannel(false, GetAudioSize());
			else
				return GetQuicktimeChannel(true, GetAudioSize());
		break;
		
		case TrackAudio:
			OsciColor = SwitchColorLV(item);
			if (MADDriver->chan[item].curPtr + TRACKSIZE >= MADDriver->chan[item].maxPtr) {
				if(MADDriver->chan[item].loopSize > 2 && MADDriver->chan[item].curPtr < MADDriver->chan[item].maxPtr) {
					sourcePtr = MADDriver->chan[item].begPtr + MADDriver->chan[item].loopBeg;
					if (sourcePtr + TRACKSIZE >= MADDriver->chan[item].maxPtr)
						return PseudoSilence;
					else
						goto RENVOISOURCE;
				} else
					return PseudoSilence;
			} else {
				sourcePtr = MADDriver->chan[item].curPtr;
				
				RENVOISOURCE:
				
				if (MADDriver->chan[item].amp == 8)
					return Inverse8(sourcePtr, TRACKSIZE);
				else
					return Inverse16(sourcePtr, TRACKSIZE);
			}
		break;
	}
	
	return (Ptr)-1L;
}

void CreateOscilloWindow(void)
{
	Rect		itemRect, tempRect, dataBounds;
	Handle		itemHandle;
	short		itemType, itemHit, temp, i;
	Point		cSize;
	FontInfo	ThisFontInfo;
	Str255		String;
	GrafPtr		savePort;

	if (OscilloDlog != NULL) {
		SetWindEtat(GetDialogWindow(OscilloDlog));
		return;
	}


	ValSaveData	= (Ptr)NewPtrClear(VALSIZE);
	if (ValSaveData == NULL)
		return;

	GetPort(&savePort);

	for (i = 0 ; i < MAXTRACK; i++)
		osciPixMap[i] = NULL;
	CurrentQuickPixMap = NULL;
	
	SetItemMark(ViewsMenu, mOscilloV, checkMark);

	OscilloDlog = GetNewDialog(128, NULL, GetDialogWindow(ToolsDlog));
	
	GetDialogItem(OscilloDlog, 1, &itemType, &itemHandle, &itemRect);
	OsciVStart = itemRect.top;
	
	
	SetWindEtat(GetDialogWindow(OscilloDlog));
	SetPortDialogPort(OscilloDlog);

	TextFont(kFontIDGeneva);
	TextSize(9);
	
	ShowWindow(GetDialogWindow(OscilloDlog));
	SelectWindow2(GetDialogWindow(OscilloDlog));
	
	SetRect(&itemRect, 0, 0, -30, -16);
	HControl = NewControl(GetDialogWindow(OscilloDlog),
							&itemRect,
							"\p.",
							true,
							0,
							0,
							0,
							gScrollBarID,
							hID);
							
	SetRect(&itemRect, 0, 0, -16, -30);
	VControl = NewControl(GetDialogWindow(OscilloDlog),
							&itemRect,
							"\p.",
							true,
							0,
							0,
							0,
							gScrollBarID,
							vID);

	OsciType	= thePrefs.OscilloType;
	if (OsciType == InPutAudio)
		OsciType = OutPutAudio;
	OsciH		= thePrefs.OscilloSize;
	
	switch (OsciH)
	{
		case 16:
			OsciDD = 4;
			break;
			
		case 32:
			OsciDD = 3;
			break;
			
		case 64:
			OsciDD = 2;
			break;
			
		case 128:
			OsciDD = 1;
			break;
			
		case 256:
			OsciDD = 0;
			break;
	}

	SetWindowEnviron();

	SetPort(savePort);
}

void CloseOscillo(void)
{
	short i;
	
	if (OscilloDlog != NULL)  {
		thePrefs.OscilloType = OsciType;
		thePrefs.OscilloSize = OsciH;
		DisposePtr(ValSaveData);
		DisposeDialog(OscilloDlog);
		
		for (i = 0; i < MAXTRACK; i++) {
			if (osciPixMap[i] != NULL)
				ZapPixMap(&osciPixMap[i]);
		}
		if (CurrentQuickPixMap != NULL)
			DisposePtr((Ptr)CurrentQuickPixMap);
		
		SetItemMark(ViewsMenu, mOscilloV, noMark);
		
		MADDriver->useOsciBuffers = false;
		
		for (i = 0; i < MAXDRAW; i++)
			MADDriver->newData[i] = false;
	}
	OscilloDlog = NULL;
}

void ResetOscilloscope(void)
{
	GrafPtr	savePort;
	long	i;
	Rect	caRect;

	if (OscilloDlog == NULL)
		return;

	GetPort(&savePort );
	SetPortDialogPort(OscilloDlog );
	GetPortBounds(GetDialogPort(OscilloDlog), &caRect);
	InvalWindowRect(GetDialogWindow(OscilloDlog), &caRect);
	
	{
		for (i = 0; i < OsciNo; i++) {
			Rect	tempRect;
			long	bb, rowbytes;
			
			if (osciPixMap[i] != NULL)
				ZapPixMap(&osciPixMap[i]);
			
			tempRect.left	= 15;
			tempRect.right	= caRect.right-15;
			tempRect.top	= 0;
			tempRect.bottom	= OsciH;
			
			if (NewOffscreenPixMap(&osciPixMap[i], &tempRect) != noErr) {
				osciPixMap[ i] = NULL;
			} else {
				rowbytes = (*osciPixMap[i])->rowBytes + 0x8000;
				for (bb = 0; bb < (*osciPixMap[i])->bounds.bottom * rowbytes; bb++) {
					*((*osciPixMap[i])->baseAddr + bb) = 0xFF;
				}
			}
		}
		
		ComputeCurrentQuickPixMap();
	}
	
	for (i=0; i < VALSIZE; i++) {
		if (i % 2 == 0)
			ValSaveData[i] = 0;
		else
			ValSaveData[i] = OsciH - 1;
	}
	
	SetPort(savePort);
}
