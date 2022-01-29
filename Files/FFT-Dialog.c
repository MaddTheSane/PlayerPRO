#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include "Undo.h"
#include "math.h"

static	double		ShiftFilter[ EQPACKET*2], HzFilter[ EQPACKET*2], loggedFilter[ EQPACKET*2];
static	Boolean		EQPreview;
static	Boolean		logMode;

extern	KeyMap		km;
static	short		*logoScale, *ilogoScale;
extern	Cursor		PlayCrsr;


double EQInterpolate(double p,double p1,double p2,double v1,double v2);

void InitFFTSampleFilter( void)
{
	long	i;
	
	for( i = 0; i < EQPACKET*2; i++)
	{
		ShiftFilter[ i] = i;
		HzFilter[ i] = 1.0;
	}
	
	EQPreview = true;
	logMode = true;
}

void FFTSampleUpdate( DialogPtr TheDia, Boolean filter)
{
	long		Result, mresult;
	short		PourCent, itemHit, i, z, itemType;
	Rect		itemRect;
	Handle		itemHandle;
	Str255		theStr;
	GDHandle	oldGDeviceH;
	CGrafPtr	oldPort;
	Boolean		IsPlaying;
	GWorldPtr	theGWorld = 0L;
	
	GetDialogItem( TheDia, 3, &itemType, &itemHandle, &itemRect);
	
	
	GetGWorld( &oldPort, &oldGDeviceH);
	theGWorld = 0L;
	NewGWorld( &theGWorld, 8, &itemRect, nil, nil, (GWorldFlags) 0);
	LockPixels( GetPortPixMap( theGWorld));
	
	SetGWorld( theGWorld, 0L);
	
	ForeColor( blackColor);
	PaintRect( &itemRect);
	
	ForeColor( greenColor);
	
	if( logMode && filter)
	{
		for( i = 0; i < 16; i++)
		{
			z = 256-logoScale[  (256 * i) / 16];
			
			MoveTo( itemRect.left, itemRect.top + ((itemRect.bottom-itemRect.top)*z)/256);
			LineTo( itemRect.right, itemRect.top + ((itemRect.bottom-itemRect.top)*z)/256);
		}
	}
	else
	{
		for( i = 0; i < 10; i++)
		{
			MoveTo( itemRect.left, itemRect.top + ((itemRect.bottom-itemRect.top)*i)/10);
			LineTo( itemRect.right, itemRect.top + ((itemRect.bottom-itemRect.top)*i)/10);
		}
	}
	
	if( logMode)
	{
		for( i = 0; i < 16; i++)
		{
			z = 256-logoScale[  (256 * i) / 16];
		
			MoveTo( itemRect.left + ((itemRect.right-itemRect.left)*z)/256, itemRect.top);
			LineTo( itemRect.left + ((itemRect.right-itemRect.left)*z)/256, itemRect.bottom);
		}
	}
	else
	{
		for( i = 0; i < 10; i++)
		{
			MoveTo( itemRect.left + ((itemRect.right-itemRect.left)*i)/10, itemRect.top);
			LineTo( itemRect.left + ((itemRect.right-itemRect.left)*i)/10, itemRect.bottom);
		}
	}
	
	PenSize( 2, 2);
	
	ForeColor( redColor);
	
	if( filter)
	{
		if( logMode)
		{
			MoveTo( itemRect.left, itemRect.top);
			for( i = 0; i < EQPACKET*2; i++) LineTo( itemRect.left + logoScale[ i/4], itemRect.top + i/4);
		}
		else
		{
			MoveTo( itemRect.left, itemRect.top);
			LineTo( itemRect.right, itemRect.bottom);
		}
		
		ForeColor( yellowColor);
		
		MoveTo( itemRect.left + ShiftFilter[ 0]/4, 0);
		for( i = 0; i < EQPACKET*2; i++) LineTo( itemRect.left + ShiftFilter[ i]/4, itemRect.top + i/4);
	}
	else
	{
		MoveTo( itemRect.left, itemRect.top + (itemRect.bottom-itemRect.top)/2 );
		LineTo( itemRect.right, itemRect.top + (itemRect.bottom-itemRect.top)/2 );
		
		ForeColor( yellowColor);
		
		MoveTo( itemRect.left, itemRect.top + (2.0 - HzFilter[ 0]) * (EQPACKET / 4));
		for( i = 0; i < EQPACKET*2; i++) LineTo( itemRect.left + i/4, itemRect.top + (2.0 - HzFilter[ i]) * (EQPACKET / 4));
	}
	
	PenSize( 1, 1);
	
	FrameRectRelief( &itemRect);
	
	SetGWorld( oldPort, oldGDeviceH);
	
	CopyBits(		(BitMap*) *GetPortPixMap( theGWorld),
					(BitMap*) *GetPortPixMap(GetDialogPort( TheDia)),
					&itemRect,
					&itemRect,
					srcCopy,
					0L);
	
	UnlockPixels( GetPortPixMap( theGWorld));
	DisposeGWorld( theGWorld);
}

void ApplyFilter( Boolean filter, sData *SDataSrc)
{
short	i, last, z, cur;

	if( logMode)
	{
		if( filter)
		{
			for( i = 0; i < 1024; i++) loggedFilter[ i] = 0;
			
			last = 0;
			for( i = 1024-1; i >= 0; i--)
			{
				cur = 4*logoScale[ i/4];
				
				loggedFilter[ i] = ShiftFilter[ cur];
			}
		}
		else
		{
			for( i = 0; i < 256 * 4; i++) loggedFilter[ i] = 1;
			
			last = 0;
			for( i = 1024-1; i >= 0; i--)
			{
				cur = 4*logoScale[ i/4];
				
				loggedFilter[ i] = HzFilter[ cur];
			}
		}
		
		MADCallFFT( SDataSrc, loggedFilter, MADDriver, filter);
	}
	else
	{
		if( filter) MADCallFFT( SDataSrc, ShiftFilter, MADDriver, filter);
		else MADCallFFT( SDataSrc, HzFilter, MADDriver, filter);
	}
}

void FFTSampleFilter( sData *SDataSrc, short instru, Boolean filter)
{
	long		Result, mresult;
	short		PourCent, itemHit, i, itemType, prev;
	Rect		itemRect;
	Handle		itemHandle, rsrc;
	DialogPtr	TheDia;
	Str255		theStr;
	GrafPtr		myPort;
	Point		theCell, myPt;
	long		oldValue, NewValue, lastValue, lastPt;
	Boolean		IsPlaying;
	sData		SData;
	
	GetPort( &myPort);
	
	//////////
	
	rsrc = GetResource( 'LogC', 128);
	DetachResource( rsrc);
	
	logoScale = (short*)		NewPtrClear( 256 * sizeof( short));
	ilogoScale = (short*)	NewPtrClear( 256 * sizeof( short));
	
	HLock( rsrc);
	BlockMoveData( *rsrc, logoScale, 256 * sizeof( short));
	HUnlock( rsrc);
	DisposeHandle( rsrc);
	
	for( i = 0; i < 256; i++) ilogoScale[ i] = -1;

	for( i = 0; i < 256; i++) ilogoScale[ logoScale[ i]] = i;
	
	prev = 0;
	for( i = 0; i < 256; i++)
	{
		if( ilogoScale[ i] == -1) ilogoScale[ i] = prev;
		else prev = ilogoScale[ i];
	}
	
	//////////
	
	
	SData = *SDataSrc;
	SData.data = NewPtr( SDataSrc->size);
	if( SData.data == 0L) return;
	
	BlockMoveData( SDataSrc->data, SData.data, SDataSrc->size);
	
	SetCursor( &watchCrsr);
	
	if( EQPreview) ApplyFilter( filter, SDataSrc);
	
	SetCursor( GetQDGlobalsArrow( &qdarrow));
	
	UpdateSampleWindows();
	
	SaveUndo( USample, instru, "\pUndo 'FFT'");
	
	TheDia = GetNewDialog( 184,0L, (WindowPtr) -1L);
	SetPortDialogPort( TheDia);
	AutoPosition( TheDia);
	ChangeDialogFont( TheDia);
	
	if( filter) SetWTitle( GetDialogWindow( TheDia), "\pShift Filter");
	else SetWTitle( GetDialogWindow( TheDia), "\pHz Filter");
	
	if( !logMode) TurnRadio( 7, TheDia, true);
	else TurnRadio( 8, TheDia, true);
	
	TurnRadio( 6, TheDia, EQPreview);
	
	do
	{
		MyModalDialog( TheDia, &itemHit);
		
		switch( itemHit)
		{
			case 7:
			case 8:
				if( itemHit == 7)
				{
					if( logMode == true)
					{
						if( filter) for( i = 0; i < EQPACKET*2; i++) ShiftFilter[ i] = ShiftFilter[ 4*ilogoScale[ i/4]];
						else for( i = 0; i < EQPACKET*2; i++) HzFilter[ i] = HzFilter[ 4*ilogoScale[ i/4]];
					}
					logMode = false;
				}
				else
				{
					if( logMode == false)
					{
						if( filter) for( i = EQPACKET*2 -1; i >= 0 ; i--) ShiftFilter[ i] = ShiftFilter[ 4*logoScale[ i/4]];
						else for( i = EQPACKET*2 -1; i >= 0 ; i--) HzFilter[ i] = HzFilter[ 4*logoScale[ i/4]];
					}
					logMode = true;
				}
				
				TurnRadio( 7, TheDia, false);
				TurnRadio( 8, TheDia, false);
				
				if( !logMode) TurnRadio( 7, TheDia, true);
				else TurnRadio( 8, TheDia, true);
				
				BlockMoveData( SData.data, SDataSrc->data, SDataSrc->size);
				
				if( EQPreview) ApplyFilter( filter, SDataSrc);
				
				UpdateSampleWindows();
			break;
		
			case 6:
				EQPreview = !EQPreview;
				TurnRadio( 6, TheDia, EQPreview);
				
				BlockMoveData( SData.data, SDataSrc->data, SDataSrc->size);
				
				if( EQPreview) ApplyFilter( filter, SDataSrc);
				
				UpdateSampleWindows();
			break;
		
			case -updateEvt:
				FFTSampleUpdate( TheDia, filter);
			break;
			
			case 5:
				if( filter)
				{
					if( logMode) for( i = 0; i < EQPACKET*2; i++) ShiftFilter[ i] = 4*logoScale[ i/4];
					else for( i = 0; i < EQPACKET*2; i++) ShiftFilter[ i] = i;
				}
				else
				{
					for( i = 0; i < EQPACKET*2; i++) HzFilter[ i] = 1.0;
				}
				FFTSampleUpdate( TheDia, filter);
				
				BlockMoveData( SData.data, SDataSrc->data, SDataSrc->size);
				
				SetCursor( &watchCrsr);
				
				if( EQPreview) ApplyFilter( filter, SDataSrc);
				
				SetCursor( GetQDGlobalsArrow( &qdarrow));
				
				UpdateSampleWindows();
			break;
			
			case 3:
				GetDialogItem( TheDia, 3, &itemType, &itemHandle, &itemRect);
				
				GetMouse( &myPt);
				
				lastPt = myPt.v - itemRect.top;
				lastValue = myPt.h - itemRect.left;
				
				GetKeys( km);
				do
				{
					GetMouse( &myPt);
					
					oldValue = myPt.h - itemRect.left;
					NewValue = myPt.v - itemRect.top;
					
					if( oldValue < 0) oldValue = 0;
					if( oldValue >= EQPACKET/2) oldValue = EQPACKET/2 -1;
					
					if( NewValue < 0) NewValue = 0;
					if( NewValue >= EQPACKET/2) NewValue = EQPACKET/2 -1;
					
					if( filter)
					{
						if( IsPressed( 0x003A))
						{
							for( i = 0; i < EQPACKET*2; i++)
							{
								ShiftFilter[ i] = i + (oldValue - NewValue)*4;
							}
						}
						else
						{
							if( NewValue < lastPt)
							{
								for( i = NewValue*4; i <= (lastPt*4) + 3; i++) ShiftFilter[ i] = EQInterpolate( i, NewValue*4, (lastPt*4), (double) oldValue * 4.0, (double) lastValue * 4.0);
							}
							else
							{
								for( i = NewValue*4 +3; i >= (lastPt*4); i--) ShiftFilter[ i] = EQInterpolate( i, NewValue*4, (lastPt*4), (double) oldValue * 4.0, (double) lastValue * 4.0);
							}
						}
					}
					else
					{
						if( oldValue < lastValue)
						{
							for( i = oldValue*4; i <= (lastValue*4) + 3; i++) HzFilter[ i] = 2.0 - EQInterpolate( i, oldValue*4, lastValue*4, (double) NewValue / (double) (EQPACKET/4), (double) lastPt / (double) (EQPACKET/4));
						}
						else
						{
							for( i = oldValue*4 +3; i >= (lastValue*4); i--) HzFilter[ i] = 2.0 - EQInterpolate( i, oldValue*4, lastValue*4, (double) NewValue / (double) (EQPACKET/4), (double) lastPt / (double) (EQPACKET/4));
						}
					}
					
					FFTSampleUpdate( TheDia, filter);
					
					lastPt = NewValue;
					lastValue = oldValue;
					
					WaitNextEvent( everyEvent, &theEvent, 1, 0L);
					DoGlobalNull();
				}
				while( Button());
				
				BlockMoveData( SData.data, SDataSrc->data, SDataSrc->size);
				
				SetCursor( &watchCrsr);
				
				if( EQPreview) ApplyFilter( filter, SDataSrc);
				
				SetCursor( GetQDGlobalsArrow( &qdarrow));
				
				UpdateSampleWindows();
			break;
			
			case 4:	// TEST
				BlockMoveData( SData.data, SDataSrc->data, SDataSrc->size);
				
				SetCursor( &watchCrsr);
				
				ApplyFilter( filter, SDataSrc);
				
				SetCursor( GetQDGlobalsArrow( &PlayCrsr));
				
				UpdateSampleWindows();
				
				MADPlaySoundDataSYNC( MADDriver, SDataSrc->data, SData.size, 0, 48 + SData.relNote, SData.amp, SData.loopBeg, SData.loopSize, ((long)SData.c2spd) << 16L, SData.stereo);
			
				SetCursor( GetQDGlobalsArrow( &qdarrow));
			break;
		}
	
	}while( itemHit != 1 && itemHit != 2);


	if( itemHit == 1)
	{
		curMusic->hasChanged = true;
	}
	else BlockMoveData( SData.data, SDataSrc->data, SDataSrc->size);
	
	DisposePtr( SData.data);
	
	CreateInstruList();
	DrawInfoInstrument();
	UpdateSampleWindows();
	UpdateInstruMenu();
	
	if( logoScale != 0L) DisposePtr( (Ptr) logoScale);	logoScale = 0L;
	if( ilogoScale != 0L) DisposePtr( (Ptr) ilogoScale);	ilogoScale = 0L;
	
	DisposeDialog( TheDia);
	SetPort( myPort);
}
