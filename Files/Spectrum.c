#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include "stdio.h"
#include "VA.h"
#include "start.h"

	/******** HELP MODULE ********/
	enum
	{
		HDisplay	= 5,
		HSize		= 8,
		HSpectr		=10,
		HLog		=11
	};
	#define	AHELPSIZE	4

	static	short					AHelp[ AHELPSIZE] = { HDisplay, HSize, HSpectr, HLog};

	void DoHelpSpectrum( short **items, short *lsize)
	{
		*lsize = AHELPSIZE;
		*items = AHelp;
	}

	/*****************************/


typedef struct
{
	short		VPos;
	Str63		Name;
	Ptr			SavePtr;
	long		Size;
	long		Freq;
	Rect		rect;
	Ptr			Spot;
	Ptr			SpotTime;
}	OsciRec;

enum
{
	OutPutAudio = 0,
	InPutAudio
};

enum
{
	vID,
	hID
};

enum
{
	linear = 1,
	logarithmic = 2
};

	extern	unsigned long			SoundInputRate;
	extern	EventRecord				theEvent;
	extern	RGBColor				theColor;
	extern	Boolean					MicroPhone;
	extern	Ptr						outPutPtr;
	extern	Str255					InPutName;
	extern	MenuHandle				OsciHMenu, SpectrumTypeMenu;
	extern	long					deviceBufferSize;

	static	RgnHandle				dlogptrRgn;
	static	Point					PP, De;
	static	Point					WinZero;
			
	static	Ptr						PseudoSilence, ValSaveData, SpotData, SpotTimeData;
	static	long					oldNullTicks;
	static	ControlHandle			HControl, VControl;
	static	Ptr						*CurrentQuick;
	static	Boolean					KOKO;
	static	PixMapHandle			specPixMap[ 2];
	static	long					*CurrentQuickPixMap;
	
	static	OsciRec					osci[ MAXTRACK];
	static	short					OsciH, OsciDD, OsciL, OsciType, OsciVStart, OsciNo, SIter, OsciScale;
	static	long					OsciOffSet;
	static	short 					gThumbPrev;
	
	extern	Boolean					OscilloMicrophone;
			Boolean					SpectrumMicrophone;
			DialogPtr				SpectrumDlog;

	#define CourbeColor		0x90
	#define RawMouse 		0x82C
	#define InterText		12
	#define	VALSIZE			32000L
	#define	OSCILLODEF		10L
	#define TRACKSIZE		512L

	long 	GetAudioSizeSpectrum();
	Ptr 	GetAudioSourceSpectrum( short item);
	Byte 	SwitchColorLV( short i);
	void 	SetWindowSpectrum( void);
	Ptr 	MakeCalculusSpectrum( Ptr srcPtr, Boolean);
	OSErr	ActiveSoundInput( Boolean RecordingMode, Handle *RecordedSound, Str255 name);
	Ptr 	GetAudioInPut( Boolean LeftChannel, long Size);
	long	LogChangePos( long x);
	void DrawOsciFreq2( short OsciFreq, short LocalV, short OffSet, short no, Byte);
	void C8BitSpectrumPixMap( Byte *tempPtr, Byte *tempPtr2, Ptr pixMapPtr, Byte *spot, Byte *spotTime);

	
void GetWorkingZoneSpectrum( Rect	*myRect)
{
Rect	caRect;
	
	GetPortBounds( GetDialogPort( SpectrumDlog), &caRect);

	myRect->left = 0;
	myRect->right = caRect.right - 15;
	myRect->top = OsciVStart;
	myRect->bottom = caRect.bottom - 15;

}

void FillInterTextSpectrum( Rect *tempRect, short i)
{
	Str255		aStr, bStr, cStr;
	long		defV;
	
	switch( OsciType)
	{
		case OutPutAudio:
			pStrcpy( aStr, "\pAudio OutPut: ");
		break;
		
		case InPutAudio:
			pStrcpy( aStr, "\pAudio InPut: ");
		break;
	}
	
	pStrcat( aStr, osci[ i].Name);
	pStrcat( aStr, "\p (");
	NumToString( i + 1, bStr);
	pStrcat( aStr, bStr);
	pStrcat( aStr, "\p)");
	
	pStrcat( aStr, "\p    Frequency: ");
	
	switch( OsciScale)
	{
		case linear:
			defV = osci[ i].Freq;
		break;
		
		case logarithmic:
			defV = LogChangePos( osci[ i].Freq/2);
			defV *= 2;
		break;
	}
	
	switch( OsciType)
	{
		case OutPutAudio:
			NumToString( (defV * (MADDriver->DriverSettings.outPutRate / 65535) / 1000), bStr);
		break;
		
		case InPutAudio:
			NumToString( (defV * ( rate22khz/ 65535) / 1000), bStr);			//SoundInputRate
		break;
	}
	pStrcat( aStr, bStr);
	pStrcat( aStr, "\p.00 Hertz");

	tempRect->left += 2;
	TETextBox( aStr+1, aStr[ 0], tempRect, teJustLeft);
	tempRect->left -= 2;
}

/*void AdjustZoomSpectrum2( Rect	*vRect)
{
//	WindowPeek	wPeek;
//	WStateData	*wspd;
	short		tempA, tempB;
	Rect		stdRect;
	
//	wPeek = ( WindowPeek) SpectrumDlog;
	
//	wspd = (WStateData*) *(wPeek->dataHandle);
	
	GetWindowStandardState( GetDialogWindow( SpectrumDlog), &stdRect);
	
	tempA = OsciL 		+ 15;
	tempB = OsciVStart 	+ 15 + OsciNo * (InterText + OsciH);

	if( stdRect.top + tempB > vRect->bottom)
	{
		tempB = vRect->bottom - stdRect.top;
		
		tempB -= OsciVStart + 15;
		tempB /= OsciH + InterText;
		tempB *= OsciH + InterText;
		tempB += OsciVStart + 15;
	}
	
	stdRect.right	= stdRect.left + tempA;
	stdRect.bottom	= stdRect.top + tempB;
	
	SetWindowStandardState( GetDialogWindow( SpectrumDlog), &stdRect);
}*/

void ComputeCurrentQuickPixMapSpec()
{
	long	i, rowBytes = (*specPixMap[ 0])->rowBytes + 0x8000;
	
	if( CurrentQuickPixMap != 0L) DisposePtr( (Ptr) CurrentQuickPixMap);
	
	CurrentQuickPixMap = (long*) NewPtr( sizeof( long) * (*specPixMap[ 0])->bounds.bottom);
	
	for( i = 0; i < (*specPixMap[ 0])->bounds.bottom; i++)
	{
		CurrentQuickPixMap[ i] = i * rowBytes;
	}
}

void SetControlSpectrum(void)
{
	Rect	caRect;
	
	GetPortBounds( GetDialogPort( SpectrumDlog), &caRect);
	
	MyMoveControl( HControl, -1, caRect.bottom-15);
	MySizeControl( HControl, caRect.right -13, 16);
	
	SetCtlMin( HControl, 0);
	SetCtlMax( HControl, OsciL - (caRect.right - 15));
	
	if( gUseControlSize) SetControlViewSize( HControl, (caRect.right - 15));
	
	MyMoveControl( VControl, caRect.right -15, OsciVStart);
	MySizeControl( VControl, 16, caRect.bottom - 14 - OsciVStart);
	
	SetCtlMin( VControl, 0);
	SetCtlMax( VControl, OsciNo - (caRect.bottom - 15 - OsciVStart) / (OsciH + InterText));
	
	if( gUseControlSize) SetControlViewSize( VControl, (caRect.bottom - 15 - OsciVStart) / (OsciH + InterText));
	
//	if( !thePrefs.DirectVideo)
	{
		short i;
		
		for( i = 0; i < OsciNo; i++)
		{
			Rect	tempRect;
			long	bb, rowbytes;
			
			if( specPixMap[ i] != 0L) ZapPixMap( &specPixMap[ i]);
			
			tempRect.left	= 0;
			tempRect.right	= caRect.right-15;
			tempRect.top	= 0;
			tempRect.bottom	= OsciH;
			
			NewOffscreenPixMap( &specPixMap[ i], &tempRect);
			ErasePixMap( specPixMap[ i]);
			
			if( i == 0) ComputeCurrentQuickPixMapSpec();
			
			ForeColor( redColor);
			DrawOsciFreq2( osci[ i].Freq, osci[ i].VPos, (OsciOffSet/2)*2, i, 215);
			ForeColor( blackColor);
		}
	}
}

void DoGrowSpectrum(void)
{
long		lSizeVH;
GrafPtr		SavePort;
Rect		caRect, temp;
short		tempA, tempB;

	GetPort( &SavePort);
 	SetPortDialogPort( SpectrumDlog);

	temp.left = 150;
	temp.right = OsciL + 15;
	
	temp.top = OsciVStart + InterText + OsciH + 15;
	temp.bottom = OsciVStart + 15 + OsciNo * (InterText + OsciH);
	
	#if MACOS9VERSION
	temp.left++;
	temp.right++;
	temp.top++;
	temp.bottom++;
	#endif

	
	lSizeVH = 0;
	if( theEvent.what == mouseDown) lSizeVH = GrowWindow( GetDialogWindow( SpectrumDlog), theEvent.where, &temp);
	
	if( lSizeVH != 0)
	{
		tempA = LoWord( lSizeVH);
		tempB = HiWord( lSizeVH);
	}
	else
	{
		GetPortBounds( GetDialogPort( SpectrumDlog), &caRect);
		
		tempA = caRect.right;
		if( tempA > temp.right) tempA = temp.right;
		tempB = caRect.bottom;
	}

	tempB -= OsciVStart + 15;
	tempB /= OsciH + InterText;
	tempB *= OsciH + InterText;
	tempB += OsciVStart + 15;

	MySizeWindow( SpectrumDlog, tempA, tempB, true);
	SetWindowSpectrum();
	
	SetPort( SavePort);
}

void DrawOsciFreq2( short OsciFreq, short LocalV, short OffSet, short no, Byte col)
{
	OffSet /=2;
	OffSet *=2;
	
	if( OsciFreq - OffSet <= 1) return;
	if( OsciFreq - OffSet >= (*specPixMap[ no])->bounds.right - 2) return;
	
	{
		Ptr		pixmap = (*specPixMap[ no])->baseAddr;
		short	i;
		
		for( i = 0; i < (*specPixMap[ no])->bounds.bottom; i++)
		{
			*(pixmap + CurrentQuickPixMap[ i] + OsciFreq + 1 - OffSet) = col;
			*(pixmap + CurrentQuickPixMap[ i] + OsciFreq - 1 - OffSet) = col;
		}
	}
}

void UpdateSpectrumData( short LocalV, Byte *tempPtr2)
{
	short		i;
	RGBColor	r = { 13107, 65535, 65535};
	
	RGBForeColor( &r);
	
	for( i = 0; i < SIter; i+= 2)
	{
		MoveTo( i, LocalV + OsciH - 1);
		LineTo( i, LocalV + OsciH - *tempPtr2++);
	}
}

/*void C8BitSpectrum( short offsetV, short offsetH, short LocalV, Byte *tempPtr, Byte *tempPtr2)
{
short		i, high, ioffsetH, VFast, SIterCopy = SIter;
Point		thePt;
Ptr			*CurrentQuickInt = CurrentQuick;

i = SIterCopy;
if( i + offsetH >= VA[ 0].frame.right)
{
	SIterCopy = VA[ 0].frame.right - offsetH;
	if( SIterCopy < 0) return;
}

offsetV			+= OsciH - 1;
LocalV			+= OsciH - 1;
CurrentQuickInt	+= offsetV;
VFast			= OsciH >> 5;	if( VFast == 0) VFast = 1;

for( i = 0, ioffsetH = offsetH; i < SIterCopy; i+= 2, ioffsetH += 2)
{
	thePt.h		= i;
	
	high		= VFast;
	thePt.v		= LocalV - *tempPtr2;
	while( high-- > 0 && *tempPtr2 > 0)
	{
		if( PtInRgn( thePt, dlogptrRgn)) *(CurrentQuickInt[ -*tempPtr2] + ioffsetH) = 0xFF;
		thePt.v++;
		(*tempPtr2)--;			// Descente de la fr子uence
	}
	
	(*tempPtr) >>= OsciDD;
	if( *tempPtr > *tempPtr2)
	{
		high = *tempPtr - *tempPtr2;
		thePt.v = LocalV - *tempPtr2 - high;
		while( high-- > 0)
		{
			thePt.v++;
			if( PtInRgn( thePt, dlogptrRgn)) *(CurrentQuickInt[ -*tempPtr2 - high] + ioffsetH) = 0x90;
		}
		*tempPtr2 = *tempPtr;
	}
	
	tempPtr2++;		tempPtr++;
}
}*/

void C8BitSpectrumPixMap( Byte *tempPtr, Byte *tempPtr2, Ptr pixMapPtr, Byte *spot, Byte *spotTime)
{
	short		i, high, VFast;
	long		*CurrentQuickInt = CurrentQuickPixMap + OsciH - 1;
	VFast			= OsciH >> 5;	if( VFast == 0) VFast = 1;
	
	for( i = 0; i < SIter; i+= 2)
	{
		high		= VFast;
		while( high-- > 0 && *tempPtr2 > 0)
		{
			*(pixMapPtr + CurrentQuickInt[ -*tempPtr2] + i) = 0xFF;
			(*tempPtr2)--;			// Descente de la fr子uence
		}
		
	/*	if( *spotTime > 0) (*spotTime)--;
		else
		{
			if( *spot != 0) *(pixMapPtr + CurrentQuickInt[ -*spot -1] + i) = 0xFF;	// Erase previous spot
			*spot = 0;
		}	*/
		
		(*tempPtr) >>= OsciDD;
		
		if( *tempPtr > *tempPtr2)
		{
		/*	if( *spot < *tempPtr2)
			{
				if( *spot != 0)
					*(pixMapPtr + CurrentQuickInt[ -*spot -1] + i) = 0xFF;	// Erase previous spot
			}*/
			
			high = *tempPtr - *tempPtr2;
			while( high-- > 0)
			{
				*(pixMapPtr + CurrentQuickInt[ -*tempPtr2 - high] + i) = 0x90;
			}
			*tempPtr2 = *tempPtr;
			
			
		/*		if( *spot < *tempPtr2)
			{
			*spotTime = 40;
			*spot = *tempPtr2;
			
			*(pixMapPtr + CurrentQuickInt[ -*spot -1] + i) = 215;	// Draw spot
			}*/
		}
		
		tempPtr2++;		tempPtr++;
		
		spot++;
		spotTime++;
	}
}

/*void C8BitSpectrumDirect( short offsetV, short offsetH, short LocalV, Byte *tempPtr, Byte *tempPtr2)
{
short		i, high, ioffsetH, VFast, SIterCopy = SIter;
Ptr			*CurrentQuickInt = CurrentQuick;

i = SIterCopy;
if( i + offsetH >= VA[ 0].frame.right)
{
	SIterCopy = VA[ 0].frame.right - offsetH;
	if( SIterCopy < 0) return;
}

offsetV 		+= OsciH - 1;
LocalV 			+= OsciH - 1;
CurrentQuickInt	+= offsetV;
VFast			= OsciH >> 5;	if( VFast == 0) VFast = 1;

for( i = 0, ioffsetH = offsetH; i < SIterCopy; i+= 2, ioffsetH += 2)
{
	high		= VFast;
	while( high-- > 0 && *tempPtr2 > 0)
	{
		*(CurrentQuickInt[ -*tempPtr2] + ioffsetH) = 0xFF;
		(*tempPtr2)--;			// Descente de la fr子uence
	}
	
	(*tempPtr) >>= OsciDD;
	
	if( *tempPtr > *tempPtr2)
	{
		high = *tempPtr - *tempPtr2;
		while( high-- > 0)
		{
			*(CurrentQuickInt[ -*tempPtr2 - high] + ioffsetH) = 0x90;
		}
		*tempPtr2 = *tempPtr;
	}
	
	tempPtr2++;		tempPtr++;
}
}

void C16BitSpectrum( short offsetV, short offsetH, short LocalV, Byte *tempPtr, Byte *tempPtr2)
{
short		i, high, *curQuickShort, ioffsetH, VFast, SIterCopy = SIter;
Point		thePt;
Ptr			*CurrentQuickInt = CurrentQuick;

i = SIterCopy;
if( i + offsetH >= VA[ 0].frame.right)
{
	SIterCopy = VA[ 0].frame.right - offsetH;
	if( SIterCopy < 0) return;
}

offsetV 		+= OsciH - 1;
LocalV 			+= OsciH - 1;
CurrentQuickInt	+= offsetV;
VFast			= OsciH >> 5;	if( VFast == 0) VFast = 1;

for( i = 0, ioffsetH = offsetH; i < SIterCopy; i+= 2, ioffsetH += 2)
{
	thePt.h = i;

	high		= VFast;
	thePt.v		= LocalV - *tempPtr2;
	while( high-- > 0 && *tempPtr2 > 0)
	{
		if( PtInRgn( thePt, dlogptrRgn))
		{
			curQuickShort = (short*) CurrentQuickInt[ -*tempPtr2];
			*(curQuickShort + ioffsetH) = 0x0000;
		}
		thePt.v++;
		(*tempPtr2)--;			// Descente de la fr子uence
	}
	
	(*tempPtr) >>= OsciDD;
	
	if( *tempPtr > *tempPtr2)
	{
		high = *tempPtr - *tempPtr2;
		while( high-- > 0)
		{
			thePt.v = LocalV - *tempPtr2 - high;
			if( PtInRgn( thePt, dlogptrRgn))
			{
				curQuickShort = (short*) CurrentQuickInt[ -*tempPtr2 - high];
				*(curQuickShort + ioffsetH) = 0x1BFF;
			}
		}
		*tempPtr2 = *tempPtr;
	}
	
	tempPtr2++;		tempPtr++;
}
}

void C16BitSpectrumDirect( short offsetV, short offsetH, short LocalV, Byte *tempPtr, Byte *tempPtr2)
{
short		i, high, *curQuickShort, ioffsetH, VFast, SIterCopy = SIter;
Ptr			*CurrentQuickInt = CurrentQuick;

i = SIterCopy;
if( i + offsetH >= VA[ 0].frame.right)
{
	SIterCopy = VA[ 0].frame.right - offsetH;
	if( SIterCopy < 0) return;
}

offsetV 		+= OsciH - 1;
LocalV 			+= OsciH - 1;
CurrentQuickInt	+= offsetV;
VFast			= OsciH >> 5;	if( VFast == 0) VFast = 1;

for( i = 0, ioffsetH = offsetH; i < SIterCopy; i+= 2, ioffsetH += 2)
{
	high		= VFast;
	while( high-- > 0 && *tempPtr2 > 0)
	{
		curQuickShort = (short*) CurrentQuickInt[ -*tempPtr2];
		*(curQuickShort + ioffsetH) = 0x0000;
		(*tempPtr2)--;			// Descente de la fr子uence
	}
	
	(*tempPtr) >>= OsciDD;
	
	if( *tempPtr > *tempPtr2)
	{
		high = *tempPtr - *tempPtr2;
		while( high-- > 0)
		{
			curQuickShort = (short*) CurrentQuickInt[ -*tempPtr2 - high];
			*(curQuickShort + ioffsetH) = 0x1BFF;
		}
		*tempPtr2 = *tempPtr;
	}
	
	tempPtr2++;		tempPtr++;
}
}*/

void DrawSpectrum( OsciRec	*osciPtr, short no)
{
short		i;
Rect		tempRect, resultRect;
Boolean		doubleView;
Ptr			spectrumPtr;

	switch( OsciScale)
	{
		case linear:
			spectrumPtr = MakeCalculusSpectrum( GetAudioSourceSpectrum( no), false);
		break;
		
		case logarithmic:
			spectrumPtr = MakeCalculusSpectrum( GetAudioSourceSpectrum( no), true);
		break;
	}
	
/*	if( thePrefs.DirectVideo)
	{
		switch( theDepth)
		{
			case 8:
				if( Direct) C8BitSpectrumDirect( WinZero.v + osciPtr->VPos, WinZero.h, osciPtr->VPos, (Byte*) spectrumPtr + OsciOffSet/2, (Byte*) osciPtr->SavePtr + OsciOffSet/2);
				else C8BitSpectrum( WinZero.v + osciPtr->VPos, WinZero.h, osciPtr->VPos, (Byte*) spectrumPtr + OsciOffSet/2, (Byte*) osciPtr->SavePtr + OsciOffSet/2);
			break;
			
			case 16:
				if( Direct) C16BitSpectrumDirect( WinZero.v + osciPtr->VPos, WinZero.h, osciPtr->VPos, (Byte*) spectrumPtr + OsciOffSet/2, (Byte*) osciPtr->SavePtr + OsciOffSet/2);
				else C16BitSpectrum( WinZero.v + osciPtr->VPos, WinZero.h, osciPtr->VPos, (Byte*) spectrumPtr + OsciOffSet/2, (Byte*) osciPtr->SavePtr + OsciOffSet/2);
			break;
		}
	}
	else*/
	{
		Rect	caRect, tempRect;
		
		GetPortBounds( GetDialogPort( SpectrumDlog), &caRect);
		
		tempRect.left	= 0;
		tempRect.right	= caRect.right-15;
		
		tempRect.top	= osci[ no].VPos;
		tempRect.bottom	= tempRect.top + OsciH;
		
		C8BitSpectrumPixMap(	(Byte*) spectrumPtr + OsciOffSet/2,
								(Byte*) osciPtr->SavePtr + OsciOffSet/2,
								(*specPixMap[ no])->baseAddr,
								(Byte*) osciPtr->Spot + OsciOffSet/2,
								(Byte*) osciPtr->SpotTime + OsciOffSet/2);
		
		BackColor( whiteColor);
		CopyBits (  (BitMap *) (*specPixMap[ no]),
					(BitMap*) *GetPortPixMap(GetDialogPort( SpectrumDlog)),
					&(*specPixMap[ no])->bounds,
					&tempRect,
					srcCopy,
					nil);
		RGBBackColor( &theColor);
	}
}

void DoNullSpectrum(void)
{
short			temp,i,x, OsciSee;
GrafPtr			myPort;
Rect			caRect, tempRect,CursRect;
Boolean			Test;
Handle			itemHandle;

	if( SpectrumDlog == 0L) return;

	if( TickCount() <= oldNullTicks) return;
	oldNullTicks = TickCount();
	
	GetPort( &myPort);
	SetPortDialogPort( SpectrumDlog);	
	
/*	if( thePrefs.DirectVideo)
	{
		Direct = false;

		WinZero.v = 0;	WinZero.h = 0;
		LocalToGlobal( &WinZero);
		
		tempRect.left = 0 + WinZero.h - 16;
		tempRect.right = caRect.right + 8 + WinZero.h;
		tempRect.top = caRect.top + WinZero.v - 16;
		tempRect.bottom = caRect.bottom + WinZero.v;

	//	#define CrsrRect  0x83C
		
		CursRect.left = ((Point*) RawMouse)->h - WinZero.h;
		CursRect.right = CursRect.left + 16;
		CursRect.top = ((Point*) RawMouse)->v - WinZero.v;
		CursRect.bottom = CursRect.top + 16;
		
		if( RectInRgn( &CursRect, SpectrumDlog->visRgn))
		{
			Test = PtInRect( (((Point*) RawMouse)[0]), &tempRect);
		}
		else Test = false;
		
		if( (**(*SpectrumDlog).visRgn).rgnSize == 10)
		{
			if( (**(*SpectrumDlog).visRgn).rgnBBox.right == caRect.right &&
				(**(*SpectrumDlog).visRgn).rgnBBox.left == caRect.left &&
				(**(*SpectrumDlog).visRgn).rgnBBox.top == caRect.top &&
				(**(*SpectrumDlog).visRgn).rgnBBox.bottom == caRect.bottom) Direct = true;
		}

		dlogptrRgn = SpectrumDlog->visRgn;
		
		if( Test) HideCursor();

		CurrentQuick = VA[ 0].quickrow;
	}
	else*/ Test = false;
	
	GetPortBounds( GetDialogPort( SpectrumDlog), &caRect);
	
	OsciSee = (caRect.bottom - 15 - OsciVStart) / (OsciH + InterText);
	for( i = GetCtlValue( VControl); i < GetCtlValue( VControl) + OsciSee; i++) DrawSpectrum( &osci[ i], i);
	
	if( Test) ShowCursor();
	
	SetPort( myPort);
}

void  UpdateSpectrumWindow(DialogPtr GetSelection)
{ 
		Rect   		caRect, tempRect, itemRect, bRect;
 		GrafPtr		SavePort;
 		Str255		tempStr, aStr;
 		short		itemType, Larg, i ,x, start, end, OsciSee;
 		Handle		itemHandle;
 		RgnHandle	visibleRegion, saveClip;
 		
 		GetPort( &SavePort);
 		SetPortDialogPort( SpectrumDlog);

		TextFont( kFontIDGeneva);	TextSize( 9);

		BeginUpdate( GetDialogWindow( SpectrumDlog));
		
		OsciOffSet	= GetCtlValue( HControl);
		
		/****************************/
		/**** Draw Oscilloscopes ****/
		/****************************/
		
		tempRect.top	= OsciVStart;
		tempRect.bottom = tempRect.top + OsciH;
		
		GetPortBounds( GetDialogPort( SpectrumDlog), &caRect);
		
		OsciSee = (caRect.bottom - 15 - OsciVStart) / (OsciH + InterText);
		for( i = GetCtlValue( VControl); i < GetCtlValue( VControl) + OsciSee; i++)
		{
			tempRect.left	= 0;
			tempRect.right	= caRect.right - 15;
			
			osci[ i].VPos = tempRect.top;
			
		/*	if( thePrefs.DirectVideo)
			{
				if( theDepth != 8 && theDepth != 16)
				{
					PaintRect( &tempRect);
				
					TextSize( 16);
					TextFont( kFontIDHelvetica);
					TextFace( bold);
					ForeColor( redColor);
					BackColor( blackColor);
					pStrcpy( aStr, "\pSpectrum window doesn't support this video mode...");
					bRect = tempRect;
					InsetRect( &bRect, 2, 2);
					TETextBox( aStr + 1,aStr[ 0], &bRect, teJustCenter);
					ForeColor( blackColor);
					RGBBackColor( &theColor);
					TextSize( 9);
					TextFont( kFontIDGeneva);
					TextFace( 0);
				}
				else
				{
					PaintRect( &tempRect);
					UpdateSpectrumData( osci[ i].VPos, (Byte*) osci[ i].SavePtr + OsciOffSet/2);
					
					ForeColor( redColor);
					DrawOsciFreq2( osci[ i].Freq, osci[ i].VPos, (OsciOffSet/2)*2, i, 215);
					ForeColor( blackColor);
				}
			}
			else*/
			{
				BackColor( whiteColor);
					CopyBits (  (BitMap *) (*specPixMap[ i]),
						(BitMap*) *GetPortPixMap(GetDialogPort( SpectrumDlog)),
						&(*specPixMap[ i])->bounds,
						&tempRect,
						srcCopy,
						nil);
				RGBBackColor( &theColor);
			}
			osci[ i].rect = tempRect;
			
			// InterText //
			bRect = tempRect;
			bRect.top = tempRect.bottom;
			bRect.bottom = bRect.top + InterText;
			bRect.right = OsciL;
			FillInterTextSpectrum( &bRect, i);
			///////////////
			
			tempRect.top	+= OsciH + InterText;
			tempRect.bottom = tempRect.top + OsciH;
		}
		/****************************/
		/****************************/
		
		MoveTo( 0, OsciVStart);
		LineTo( caRect.right, OsciVStart);
		
		visibleRegion = NewRgn();
		
		GetPortVisibleRegion( GetDialogPort( GetSelection), visibleRegion);
		
		UpdtDialog( GetSelection, visibleRegion);
		
		DisposeRgn( visibleRegion);
		
		EndUpdate( GetDialogWindow( SpectrumDlog));
		
		DrawGrowIconP( GetSelection);
		
		SetPort( SavePort);
}

static	DialogPtr	theDialogControl;

pascal void actionProcSpectrum(ControlHandle theControl, short ctlPart)
{
long			lRefCon;
short			CurWin, maxValue, minValue, curVal, sVal, OsciSee;
Rect			caRect, aRect;

if( ctlPart <= 0) return;

lRefCon = GetCRefCon( theControl);
maxValue = GetCtlMax( theControl);
minValue = GetCtlMin( theControl);
curVal = sVal = GetCtlValue( theControl);

GetPortBounds( GetDialogPort( SpectrumDlog), &caRect);

OsciSee = (caRect.bottom - 15 - OsciVStart) / (OsciH + InterText);

	switch( ctlPart)
	{
		case kControlUpButtonPart:
			if( lRefCon == hID) curVal -= OSCILLODEF;
			else curVal -= 1;
			if( curVal < minValue) curVal = minValue;
		break;
		
		case kControlDownButtonPart:
			if( lRefCon == hID) curVal += OSCILLODEF;
			else curVal += 1;
			if( curVal > maxValue) curVal = maxValue;
		break;
		
		case kControlPageUpPart:
			if( lRefCon == hID) curVal -= SIter;
			else curVal -= OsciSee;
			if( curVal < minValue) curVal = minValue;
		break;
		
		case kControlPageDownPart:
			if( lRefCon == hID) curVal += SIter;
			else curVal += OsciSee;
			if( curVal > maxValue) curVal = maxValue;
		break;
		
		case kControlIndicatorPart:
			sVal = gThumbPrev;
			gThumbPrev = curVal;
		break;
	}
	
	SetCtlValue( theControl, curVal);
	
	if( sVal != curVal)
	{
		if( lRefCon == vID)
		{
			GetWorkingZoneSpectrum( &aRect);
			EraseRect( &aRect);
			InvalWindowRect( GetDialogWindow( theDialogControl), &aRect);
			
			UpdateSpectrumWindow( theDialogControl);			/* Important pour la mise � jour des VPos !!! */
		}
		else
		{
			short i;
			
			GetWorkingZoneSpectrum( &aRect);
			InvalWindowRect( GetDialogWindow( theDialogControl), &aRect);
			UpdateSpectrumWindow( theDialogControl);
			
			for( i = 0; i < 2;i++) ErasePixMap( specPixMap[ i]);
			for( i = 0; i < VALSIZE; i++) ValSaveData[ i] = 0;
			for( i = 0; i < VALSIZE; i++) SpotData[ i] = 0;
			for( i = 0; i < VALSIZE; i++) SpotTimeData[ i] = 0;
			
			OsciOffSet	= GetCtlValue( HControl);
			
			for( i =0 ; i < 2;i++)
			{
				ForeColor( redColor);
				DrawOsciFreq2( osci[ i].Freq, osci[ i].VPos, (OsciOffSet/2)*2, i, 215);
				ForeColor( blackColor);
			}
			//DoGlobalNull();
		}
	}
//	else DoGlobalNull();
}

void DoItemPressSpectrum( short whichItem, DialogPtr whichDialog)
{
		Cell				theCell;
		long				mresult;
 		short				temp, bogus, itemType, ctlPart, curSelec, i, tempB, OsciSee;
 		Rect				caRect, tempRect;
 		Handle				itemHandle;
 		GrafPtr				SavePort;
 		ControlHandle		theControl;
 		Str255				aStr;
 		Point				myPt;
		ControlActionUPP	MyControlUPP;
		
 		GetPort( &SavePort);
 		SetPortDialogPort( SpectrumDlog);
 
	 	if (theEvent.what == mouseDown)
		{
			myPt = theEvent.where;
			GlobalToLocal(&myPt);
			
			theControl = 0L;
			if( TestControl(  HControl, myPt)) theControl = HControl;
			if( TestControl(  VControl, myPt)) theControl = VControl;
			
			if( theControl == HControl || theControl == VControl)
			{
			/*	if( ctlPart == kControlIndicatorPart && gUseControlSize == false)
				{
					bogus = TrackControl( theControl, myPt, 0L);
					if( bogus != 0)
					{
					}
				}
				else if( ctlPart > 0)*/
				{
					theDialogControl = whichDialog;
					MyControlUPP = NewControlActionUPP( actionProcSpectrum);
					gThumbPrev = GetCtlValue( theControl);
					TrackControl(theControl, myPt, MyControlUPP);
					DisposeControlActionUPP( MyControlUPP);
				}
			}
		}
 
 
 		switch( whichItem)
 		{
 			case 11:
 			{
 				MenuHandle	tempMenu = GetMenu( 160);
 			
				InsertMenu( tempMenu, hierMenu);
				GetDialogItem( whichDialog, whichItem, &itemType, &itemHandle, &tempRect);
				curSelec = OsciScale;
				
				myPt.v = tempRect.top;	myPt.h = tempRect.left;
				LocalToGlobal( &myPt);
				
				SetItemMark( tempMenu, curSelec, 0xa5);
				
				mresult = PopUpMenuSelect(	tempMenu,
											myPt.v,
											myPt.h,
											curSelec);
				
				SetItemMark( tempMenu, curSelec, 0);
				
				if( HiWord( mresult) != 0)
				{
					OsciScale = LoWord( mresult);
					switch( OsciScale)
					{
						case linear:
							SetDText( SpectrumDlog, 3, "\pLinear");
						break;
						
						case logarithmic:
							SetDText( SpectrumDlog, 3, "\pLog");
						break;
					}
					SetWindowSpectrum();
				}
				DeleteMenu( GetMenuID( tempMenu));
				DisposeMenu( tempMenu);
			}
 			break;
 		
 			case 5:
				InsertMenu( SpectrumTypeMenu, hierMenu);
				GetDialogItem( whichDialog, whichItem, &itemType, &itemHandle, &tempRect);
				curSelec = OsciType;
				
				myPt.v = tempRect.top;	myPt.h = tempRect.left;
				LocalToGlobal( &myPt);
				
				SetItemMark( SpectrumTypeMenu, curSelec + 1, 0xa5);
				
				mresult = PopUpMenuSelect(	SpectrumTypeMenu,
											myPt.v,
											myPt.h,
											curSelec + 1);
				
				SetItemMark( SpectrumTypeMenu, curSelec + 1, 0);
				
				if( HiWord( mresult) != 0)
				{
					switch( LoWord( mresult) - 1)
					{
						case OutPutAudio:
							OsciType = OutPutAudio;
							SpectrumMicrophone = false;
							if( OscilloMicrophone == false) MicroOff();
							SetDText( SpectrumDlog, 6, "\pAudio OutPut");
						break;
						
						case InPutAudio:
							if( ActiveSoundInput( false, 0L, "\p") == noErr)
							{
								SpectrumMicrophone = true;
								OsciType = InPutAudio;
								SetDText( SpectrumDlog, 6, "\pAudio InPut");
							}
							
							CloseSoundInput();
						break;
					}
					SetWindowSpectrum();
				}
				DeleteMenu( GetMenuID( SpectrumTypeMenu));
 			break;
 			
 			case 8:
				InsertMenu( OsciHMenu, hierMenu);
				GetDialogItem( whichDialog, whichItem, &itemType, &itemHandle, &tempRect);
				
				switch( OsciH)
				{
					case 16:	curSelec = 0;		break;
					case 32:	curSelec = 1;		break;
					case 64:	curSelec = 2;		break;
					case 128:	curSelec = 3;		break;
					case 256:	curSelec = 4;		break;
				}
				
				myPt.v = tempRect.top;	myPt.h = tempRect.left;
				LocalToGlobal( &myPt);
				
				SetItemMark( OsciHMenu, curSelec + 1, 0xa5);
				
				mresult = PopUpMenuSelect(	OsciHMenu,
											myPt.v,
											myPt.h,
											curSelec + 1);
				
				SetItemMark( OsciHMenu, curSelec + 1, 0);
				
				if( HiWord( mresult) != 0)
				{
					switch( LoWord( mresult))
					{
						case 1:	OsciH = 16;		OsciDD = 4; 	break;
						case 2:	OsciH = 32;		OsciDD = 3;		break;
						case 3:	OsciH = 64;		OsciDD = 2; 	break;
						case 4:	OsciH = 128;	OsciDD = 1;		break;
						case 5:	OsciH = 256;	OsciDD = 0;		break;
					}
					
					NumToString( OsciH, aStr);
					SetDText( SpectrumDlog, 9, aStr);
					
					SetWindowSpectrum();
				}
				DeleteMenu( GetMenuID( OsciHMenu));
 			break;
 			
 			case 10:
 				myPt = theEvent.where;
 				GlobalToLocal( &myPt);
 				
 				GetPortBounds( GetDialogPort( SpectrumDlog), &caRect);
 				
 				OsciSee = (caRect.bottom - 15 - OsciVStart) / (OsciH + InterText);
				for( i = GetCtlValue( VControl); i < GetCtlValue( VControl) + OsciSee; i++)
 				{
 					if( PtInRect( myPt, &osci[ i].rect))
 					{
 						while( Button())
 						{
 							DoGlobalNull();
 						
 							GetMouse( &myPt);
 							if( PtInRect( myPt, &osci[ i].rect) && OsciOffSet + (myPt.h/2)*2 != osci[ i].Freq)
 							{
 								ForeColor( blackColor);
 								DrawOsciFreq2( osci[ i].Freq, osci[ i].VPos, (OsciOffSet/2)*2, i, 0xFF);
 								
 								osci[ i].Freq = (OsciOffSet/2)*2 + (myPt.h/2)*2;
								
  								ForeColor( redColor);
 								DrawOsciFreq2( osci[ i].Freq, osci[ i].VPos, (OsciOffSet/2)*2, i, 215);
 								ForeColor( blackColor);
 								
 								GetPortBounds( GetDialogPort( SpectrumDlog), &caRect);
 								
 								tempRect = osci[ i].rect;
								tempRect.top = osci[ i].rect.bottom;
								tempRect.bottom = tempRect.top + InterText;
								tempRect.right = caRect.right - 16;
								FillInterTextSpectrum( &tempRect, i);
 							}
 						}
 					}
 				}	
 			break;
 		}
 
		SetPort( SavePort);
}

void SetWindowSpectrum(void)
{
short		itemType, i, tempB, tempA;
Handle		itemHandle;
Rect		caRect, itemRect;
GrafPtr		savePort;
Str255		tempStr, aStr;
short		prevRight, prevBot;

	if( SpectrumDlog == 0L) return;
	
	GetPort( &savePort);
	SetPortDialogPort( SpectrumDlog);
	
	/**** Oscillo ******/
	
	OsciL		= GetAudioSizeSpectrum();
	
	switch( OsciType)
	{
		case OutPutAudio:
			osci[ 0].VPos		=	OsciVStart ;
			osci[ 0].SavePtr	=	ValSaveData;
			osci[ 0].Spot		=	SpotData;
			osci[ 0].SpotTime	=	SpotTimeData;
			osci[ 0].Size		=	GetAudioSizeSpectrum();
			
		//	if( MADDriver->DriverSettings.outPutMode != MonoOutPut)
			{
				pStrcpy( osci[ 0].Name, "\pLeft Channel");
			
				osci[ 1].VPos		=	OsciVStart + OsciH + InterText;
				osci[ 1].SavePtr	=	osci[ 0].SavePtr + osci[ 0].Size;
				osci[ 1].Spot		=	SpotData + osci[ 0].Size;
				osci[ 1].SpotTime	=	SpotTimeData + osci[ 0].Size;
				osci[ 1].Size		=	GetAudioSizeSpectrum();
				pStrcpy( osci[ 1].Name, "\pRight Channel");
				
				OsciNo = 2;
			}
		/*	else
			{
				pStrcpy( osci[ 0].Name, "\pMono Channel");
				OsciNo = 1;
			}*/
		break;
	
		case InPutAudio:
			osci[ 0].VPos		=	OsciVStart;
			osci[ 0].SavePtr	=	ValSaveData;
			osci[ 0].Spot		=	SpotData;
			osci[ 0].SpotTime	=	SpotTimeData;
			osci[ 0].Size		=	GetAudioSizeSpectrum();
			pStrcpy( osci[ 0].Name, InPutName);
			
			OsciNo = 1;
		break;
	}
	
	for( i = 0; i < OsciNo; i++)
	{
		for( tempB = 0; tempB < osci[ i].Size; tempB++) osci[ i].SavePtr[ tempB] = 0;
	}
	
	/**** Check Window Size ****/
	
	GetPortBounds( GetDialogPort( SpectrumDlog), &caRect);
	
	prevRight	= caRect.right;
	prevBot		= caRect.bottom;
	
	if( caRect.bottom > OsciVStart + 15 + OsciNo * (InterText + OsciH)) prevBot = OsciVStart + 15 + OsciNo * (InterText + OsciH);
	if( caRect.bottom < OsciVStart + InterText + OsciH + 15) prevBot = OsciVStart + InterText + OsciH + 15;
	
	if( caRect.right > OsciL + 15) prevRight = OsciL + 15;
	
	
	prevBot -= OsciVStart + 15;
	prevBot /= OsciH + InterText;
	prevBot *= OsciH + InterText;
	prevBot += OsciVStart + 15;
	
	MySizeWindow( SpectrumDlog, prevRight, prevBot, false);

	/**** SIter ******/
	GetPortBounds( GetDialogPort( SpectrumDlog), &caRect);
	SIter = caRect.right - 16;
	/********/
	
	SetControlSpectrum();
	OsciOffSet	= GetCtlValue( HControl);
	
	/********/
	switch( OsciType)
	{
		case OutPutAudio:
			SetDText( SpectrumDlog, 6, "\pAudio OutPut");
		break;
		
		case InPutAudio:
			SetDText( SpectrumDlog, 6, "\pAudio InPut");
		break;
	}
	
	NumToString( OsciH, aStr);
	SetDText( SpectrumDlog, 9, aStr);

	/*********/
	switch( OsciScale)
	{
		case linear:
			SetDText( SpectrumDlog, 3, "\pLinear");
		break;
		
		case logarithmic:
			SetDText( SpectrumDlog, 3, "\pLog");
		break;
	}
	
//	AdjustZoomSpectrum();

	SetMaxWindow( OsciL + 15,OsciVStart + 15 + OsciNo * (InterText + OsciH), SpectrumDlog);
	
	GetWorkingZoneSpectrum( &itemRect);
	EraseRect( &itemRect);
	InvalWindowRect( GetDialogWindow( SpectrumDlog), &itemRect);
	
	SetPort( savePort);
}

long GetAudioSizeSpectrum(void)
{
	switch( OsciType)
	{
		case OutPutAudio:
			return 512;
		break;
		
		case InPutAudio:
			if( deviceBufferSize*2 >= 512) return 512;
			else return 256;
		break;
	}
	
	return 0;
}

Ptr GetAudioSourceSpectrum( short item)
{
Ptr	sourcePtr;

	switch( OsciType)
	{
		case OutPutAudio:
			if( item == 0) return GetAudioChannel( false, MADDriver->ASCBUFFERReal);
			else return GetAudioChannel( true, MADDriver->ASCBUFFERReal);
		break;
		
		case InPutAudio:
			return GetAudioInPut( false, deviceBufferSize);
		break;
	}
	
	return 0L;
}

void CreateSpectrumWindow(void)
{
	Rect		itemRect, tempRect, dataBounds;
	Handle		itemHandle;
	short		itemType, itemHit, temp, i;
	Point		cSize;
	FontInfo	ThisFontInfo;
	Str255		String;
	GrafPtr		savePort;
	
	if( SpectrumDlog != 0L)
	{
		SetWindEtat( GetDialogWindow(SpectrumDlog));
		return;
	}
	
	SetItemMark( ViewsMenu, mSpectrumV, checkMark);

	GetPort( &savePort);
	
	for( i = 0 ; i < 2; i++) specPixMap[ i] = 0L;
	CurrentQuickPixMap = 0L;
	
	SpectrumDlog = GetNewDialog( 164, 0L, (WindowPtr) ToolsDlog);
	
	KOKO = false;
	
	GetDialogItem( SpectrumDlog, 1, &itemType, &itemHandle, &itemRect);
	OsciVStart = itemRect.top;
	
	SetWindEtat( GetDialogWindow(SpectrumDlog));
	SetPortDialogPort( SpectrumDlog);

	ValSaveData		= (Ptr) NewPtrClear( VALSIZE);
	SpotData		= (Ptr) NewPtrClear( VALSIZE);
	SpotTimeData	= (Ptr) NewPtrClear( VALSIZE);
	
	TextFont( kFontIDGeneva);	TextSize(9);
	
	ShowWindow( GetDialogWindow( SpectrumDlog));
	SelectWindow2( GetDialogWindow( SpectrumDlog));
	
	SetRect( &itemRect, 0, 0, -30, -16);
	HControl = NewControl( 	GetDialogWindow( SpectrumDlog),
							&itemRect,
							"\p.",
							true,
							0,
							0,
							0,
							gScrollBarID,
							hID);
	
	SetRect( &itemRect, 0, 0, -16, -30);
	VControl = NewControl( 	GetDialogWindow( SpectrumDlog),
							&itemRect,
							"\p.",
							true,
							0,
							0,
							0,
							gScrollBarID,
							vID);

	if( thePrefs.SpectrumScale) OsciScale = linear;
	else OsciScale = logarithmic;
	
	OsciType			= thePrefs.SpectrumType;
	if( OsciType == InPutAudio) OsciType = OutPutAudio;
	OsciH				= thePrefs.SpectrumSize;
	
	switch( OsciH)
	{
		case 16:	OsciDD = 4; 	break;
		case 32:	OsciDD = 3;		break;
		case 64:	OsciDD = 2; 	break;
		case 128:	OsciDD = 1;		break;
		case 256:	OsciDD = 0;		break;
	}
	
	osci[ 0].Freq = 256;
	osci[ 1].Freq = 256;
	
	SetWindowSpectrum();

	//InitFourier();

	SetPort( savePort);
}

void CloseSpectrum(void)
{
	if( SpectrumDlog != 0L) 
	{
		short	i;
		
		thePrefs.SpectrumType = OsciType;
		thePrefs.SpectrumSize = OsciH;
		if( OsciScale == linear) thePrefs.SpectrumScale = true;
		else thePrefs.SpectrumScale = false;
	
		//CloseFourier();
		DisposePtr( ValSaveData);	ValSaveData = 0L;
		DisposePtr( SpotData);		SpotData = 0L;
		DisposePtr( SpotTimeData);	SpotTimeData = 0L;
		DisposDialog( SpectrumDlog);
		
		SetItemMark( ViewsMenu, mSpectrumV, noMark);
		
		for( i = 0 ; i < 2; i++) if( specPixMap[ i] != 0L) ZapPixMap( &specPixMap[ i]);
		if( CurrentQuickPixMap != 0L) DisposePtr( (Ptr) CurrentQuickPixMap);
		
		MADDriver->useOsciBuffers = false;
		for( i = 0; i < MAXDRAW; i++) MADDriver->newData[ i] = false;
	}
	SpectrumDlog = 0L;
}

void ResetSpectrum(void)
{
GrafPtr	savePort;
long	i;
Rect	caRect;

	if( SpectrumDlog == 0L) return;

	GetPort( &savePort );
	SetPortDialogPort( SpectrumDlog);
	GetPortBounds( GetDialogPort( SpectrumDlog), &caRect);
	InvalWindowRect( GetDialogWindow( SpectrumDlog), &caRect);
	
	for( i =0 ; i < 2;i++) ErasePixMap( specPixMap[ i]);
//	for( i = 0; i < VALSIZE; i++) ValSaveData[ i] = 0;
	
	SetPort( savePort);
}
