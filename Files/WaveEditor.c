#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include "Undo.h"
#include <Drag.h>
#include "PPPlug.h"
#include <QDOffscreen.h>
#include "PrivateList.h"

	/******** HELP MODULE ********/
	enum
	{
		HView = 0
	};
	#define	AHELPSIZE	1

	static	short		AHelp[ AHELPSIZE] =
	{ HView};

	void DoHelpWave( short **items, short *lsize)
	{
		*lsize = AHELPSIZE;
		*items = AHelp;
	}

	/*****************************/

#define rate2khz	0x08000000UL

extern	MenuHandle				OsciHMenu;
extern	WindowPtr				oldWindow;
extern	DialogPtr				InstruListDlog;
extern	KeyMap					km;
extern	Boolean					DragManagerUse;
extern	Cursor					PlayCrsr, HandCrsr, CHandCrsr, ZoomInCrsr, ZoomOutCrsr;
extern	RGBColor				theColor;

		DialogPtr				WaveDlog;

static	RGBColor				Gray = {48059, 48059, 48059};
static	ControlHandle			playBut, zoomBut, noteBut;
static	ControlHandle			xScroll, yScroll;
static	long					XSize, YSize;
static	short					CurrentPat, ReaderCopy, CurrentPatSize;
static	Ptr						WavePtr;
static	MADDriverSettings 		WaveDriverType;
static	MADDriverRec			*WaveCopyDriver;
static	Rect					CurRect, WaveRect;
static	short					mode, prevXSize, prevYSize, gThumbPrev;
static	long					doubleClick;

#define WAVESIZE 	1024L
#define BASETOP		24L

enum
{
	playM = 0,
	noteM,
	zoomM
};

#define MODESIZE 2

pascal void actionProcWave(ControlHandle theControl, short ctlPart);
void UPDATE_TrackActive(void);

short GetMaxXWave()
{
short	ret;

	ret = 1 + GetCtlValue( xScroll) + (WaveRect.right - WaveRect.left) / XSize;
	if( ret > curMusic->partition[ CurrentPat]->header.size) ret = curMusic->partition[ CurrentPat]->header.size;
	
	return ret;
}

short GetMaxYWave()
{
short	ret;

	ret = 1 + GetCtlValue( yScroll) + (WaveRect.bottom - WaveRect.top) / YSize;
	if( ret > WaveDriverType.numChn) ret = WaveDriverType.numChn;
	
	return ret;
}

void FinishFrame()
{
	short	i;
	short	mx;
	Rect	caRect;
	
	GetPortBounds( GetDialogPort( WaveDlog), &caRect);
	
	mx = WaveRect.left -1 + (GetMaxXWave() - GetCtlValue( xScroll)) * XSize;
	if( mx > caRect.right-16)
	{
		mx = caRect.right-16;
	}
/*	else
	{
		MoveTo( mx, WaveRect.top);
		LineTo( mx, caRect.bottom - 16);
	}*/
	
	for( i = GetCtlValue( yScroll); i < GetMaxYWave(); i++)
	{
		short temp = WaveRect.top + (i - GetCtlValue( yScroll)) * YSize + YSize;
		
		GetPortBounds( GetDialogPort( WaveDlog), &caRect);
		
		if( temp <= caRect.bottom - 16)
		{
			MoveTo( 0, temp);
			LineTo( mx, temp);
		}
	}
}

void SetWaveControl()
{
short	tt;
Rect	caRect;

	// XScroll
	
	if( xScroll != 0L)
	{
		GetPortBounds( GetDialogPort( WaveDlog), &caRect);
		
		MyMoveControl( xScroll, WaveRect.left-1, caRect.bottom-15);
		MySizeControl( xScroll, caRect.right + 2 - WaveRect.left - 15, 16);
		
		SetCtlMin( xScroll, 0);
		
		tt = curMusic->partition[ CurrentPat]->header.size - ((caRect.right - 14) - WaveRect.left) / XSize;		if( tt < 0) tt = 0;
		SetCtlMax( xScroll, tt);
		
		if( gUseControlSize) SetControlViewSize( xScroll, ((caRect.right - 14) - WaveRect.left) / XSize);
		
		CurrentPatSize = curMusic->partition[ CurrentPat]->header.size;
		prevXSize = XSize;
	}
	
	// YScroll
	
	if( yScroll != 0L)
	{
		GetPortBounds( GetDialogPort( WaveDlog), &caRect);
		
		MyMoveControl( yScroll, caRect.right-15, BASETOP + 14);
		MySizeControl( yScroll, 16, caRect.bottom - (BASETOP + 14) + 1 - 15);
	
		SetCtlMin( yScroll, 0);
	
		tt = WaveDriverType.numChn  - ((caRect.bottom - 14) - WaveRect.top) / YSize;	if( tt < 0) tt = 0;
		SetCtlMax( yScroll, tt);
		
		if( gUseControlSize) SetControlViewSize( yScroll, ((caRect.bottom - 14) - WaveRect.top) / YSize);
		
		prevYSize = YSize;
	}
	
	SetMaxWindow( 	WaveRect.left + curMusic->partition[ CurrentPat]->header.size*XSize + 14,
					WaveRect.top + WaveDriverType.numChn*YSize + 15,
					WaveDlog);
}

void CreateCurWaveRect()
{
short	temp = ReaderCopy - GetCtlValue( xScroll);
Rect	caRect;

	GetPortBounds( GetDialogPort( WaveDlog), &caRect);
	
	WaveRect.right = caRect.right - 15;
	WaveRect.bottom = caRect.bottom - 15;
	
	CurRect.top = WaveRect.top;
	CurRect.bottom = CurRect.top + (GetMaxYWave() - GetCtlValue( yScroll))*YSize;
	CurRect.left = WaveRect.left + temp*XSize;
	CurRect.right = CurRect.left + XSize - 1;
}

void DoNullWave(void)
{
	Str255		String, aStr;
	GrafPtr		SavePort;
	short		temp;
	Point		myPt;
 	Rect		tempRect;
 	Boolean		MakeUpdate = false;
 
 	if( WaveDlog == 0L) return;
 
 	GetPort( &SavePort);
 	SetPortDialogPort( WaveDlog);

	if( CurrentPat != MADDriver->Pat)
	{
		Str255			theStr;
		short			x;
		
		CurrentPat = MADDriver->Pat;
		
		/**/
		
		NumToString( (long) CurrentPat, String);
		pStrcpy( aStr, "\p");
		if( CurrentPat < 10) pStrcat( aStr, "\p0");
		if( CurrentPat < 100) pStrcat( aStr, "\p0");
		pStrcat( aStr, String);
		
		pStrcpy( String, "\pPattern: ");
		pStrcat( String, aStr);
		
		theStr[ 0] = 20;
		for( x = 0; x < 20; x++) theStr[ x + 1] = curMusic->partition[ CurrentPat]->header.name[ x];
		for( x = 1; x < 20; x++) if( theStr[ x] == 0) { theStr[ 0] = x - 1; break;}
		pStrcat( String, "\p ");
		pStrcat( String, theStr);
		
		SetWTitle( GetDialogWindow( WaveDlog), String);
		
		/**/
		
		MakeUpdate = true;
	}
	
	if( WaveDriverType.numChn != MADDriver->DriverSettings.numChn) MakeUpdate = true;
	
	if( ReaderCopy != MADDriver->PartitionReader)
	{
		ReaderCopy = MADDriver->PartitionReader;
		
		if( ReaderCopy < curMusic->partition[ CurrentPat]->header.size)
		{
			short		temp;
			RgnHandle	saveClipRgn;
			
			saveClipRgn = NewRgn();
		 	GetClip( saveClipRgn);
			
		//	WaveRect.left++;
			WaveRect.top++;
			ClipRect( &WaveRect);
		//	WaveRect.left--;
			WaveRect.top--;
			
			BackColor( whiteColor);
			
			LMSetHiliteMode( LMGetHiliteMode() & hiliteBit);
			InvertRect( &CurRect);					// previous
			
			CreateCurWaveRect();
			
			LMSetHiliteMode( LMGetHiliteMode() & hiliteBit);
			InvertRect( &CurRect);					// current
			RGBBackColor( &theColor);
			
			SetClip( saveClipRgn);
 			DisposeRgn( saveClipRgn);
		}
	}
	
	if( oldWindow == GetDialogWindow( WaveDlog))
	{
		RgnHandle	visibleRegion;
		
		myPt = theEvent.where;
		GlobalToLocal( &myPt);
		
		visibleRegion = NewRgn();
		
		GetPortVisibleRegion( GetWindowPort( oldWindow), visibleRegion);
		
		if( PtInRgn( myPt, visibleRegion))
		{
			if( PtInRect( myPt, &WaveRect))
			{
				switch( mode)
				{
					case zoomM:
						if( theEvent.modifiers & optionKey) SetCursor( &ZoomOutCrsr);
						else SetCursor( &ZoomInCrsr);
					break;
					
					case playM:
						SetCursor( &PlayCrsr);
					break;
					
					case noteM:
						SetCursor( GetQDGlobalsArrow( &qdarrow));
					break;
				}
			}
			else SetCursor( GetQDGlobalsArrow( &qdarrow));
		}
		else SetCursor( GetQDGlobalsArrow( &qdarrow));
		
		DisposeRgn( visibleRegion);
	}
	
	if( MakeUpdate) UpdateWaveInfo();
	
	SetPort( SavePort);
}

void DoGrowWave(void)
{
long		lSizeVH;
GrafPtr		SavePort;
Rect		caRect, temp, cellRect;
Point		theCell = { 0, 0}, aPt = { 0, 0};
short		tempA, tempB;
Handle		itemHandle;
short		itemType;
BitMap		screenBits;

	GetPort( &SavePort);
 	SetPortDialogPort( WaveDlog);
	
	temp.left = 100;
	temp.right = WaveRect.left + curMusic->partition[ CurrentPat]->header.size*XSize + 15;
	temp.top = 110;
	
	temp.bottom = WaveRect.top + WaveDriverType.numChn*YSize + 16;
	
	LocalToGlobal( &aPt);
	
	GetQDGlobalsScreenBits( &screenBits);
	
	if( temp.bottom < temp.top) temp.bottom = temp.top;
	else if( temp.bottom > screenBits.bounds.bottom - aPt.v) temp.bottom = screenBits.bounds.bottom - aPt.v -2;
	
	lSizeVH = 0;
	if( theEvent.what == mouseDown) lSizeVH = GrowWindow( GetDialogWindow( WaveDlog), theEvent.where, &temp);
	
	if( lSizeVH != 0)
	{
		tempA = LoWord( lSizeVH);
		tempB = HiWord( lSizeVH);
	}
	else
	{
		GetPortBounds( GetDialogPort( WaveDlog), &caRect);
		
		if( caRect.right >= temp.right) tempA = temp.right-1;
		else tempA = caRect.right;
		
		if( caRect.bottom >= temp.bottom) tempB = temp.bottom-1;
		else tempB = caRect.bottom;
	}
	
	MySizeWindow( WaveDlog, tempA, tempB , true);
	
	CreateCurWaveRect();
	
	GetPortBounds( GetDialogPort( WaveDlog), &caRect);
	
	EraseRect( &caRect);
	InvalWindowRect( GetDialogWindow( WaveDlog), &caRect);
	
	SetWaveControl();

	SetPort( SavePort);
}

void GetMinMax( long BS, long BE, Ptr ptr, long *mi, long *mx)
{
long	temp, minY, maxY;

	BS /= WaveDriverType.numChn;	BS *= WaveDriverType.numChn;
	BE /= WaveDriverType.numChn;	BE *= WaveDriverType.numChn;
	
	temp =   ptr[ BS*WaveDriverType.numChn];
	if( temp < 0) temp += 128;
	else temp -= 128;
	minY = maxY = temp;
				
	if( BS != BE)
	{
		long	x;
		
		for( x = BS; x < BE; x++)
		{
			temp =   ptr[ x*WaveDriverType.numChn];
			if( temp < 0) temp += 128;
			else temp -= 128;
			
			if( temp > maxY) maxY = temp;
			if( temp < minY) minY = temp;
		}
	}
	
	*mi = minY;
	*mx = maxY;
}

void UpdateCmdWave( short Pos, short track)
{
	GrafPtr		SavePort;
	short		maxX, tt;
	Cmd 		*theCommand;
	
	if( WaveDlog == 0L) return;
	
	GetPort( &SavePort);
	SetPortDialogPort( WaveDlog);
	
	/**/ // SCAN FOR SPEED CHANGE
	for( maxX = GetCtlValue( xScroll); maxX < GetMaxXWave(); maxX++)
	{
		for( tt = 0; tt < curMusic->header->numChn; tt++)
		{
			theCommand = GetMADCommand( maxX, tt, curMusic->partition[ CurrentPat]);
			
			if( theCommand->cmd == 0x0F)
			{
				ComputeWave( GetCtlValue( xScroll), GetMaxXWave(), track);
				
				SetPort( SavePort);
				
				return;
			}
		}
	}
	
	for( maxX = Pos+1; maxX < GetMaxXWave(); maxX++)
	{
		theCommand = GetMADCommand( maxX, track, curMusic->partition[ CurrentPat]);
		
		if( theCommand->ins != 0)
		{
			if( theCommand->note != 0xFF || theCommand->note != 0xFE)
			{
				break;
			}
		}
	}
	
	ComputeWave( Pos-1, maxX, track);
	
	SetPort( SavePort);
}

OSErr ComputeWave( short fromX, short toX, short chan)
{
	long				destReader, i;
	OSErr				iErr;
	Boolean				OnContinue, ReadyToGoOut;
	long				count, CellXBytes, dataX;
	long				XOffSet, YOffSet;
	RgnHandle			saveClipRgn;
	Rect 				cRect;
	
	if( WaveCopyDriver->curMusic->musicUnderModification) return noErr;
	
	BackColor( whiteColor);
		
	/***********/
	
	if( fromX < 0) fromX = 0;
	if( toX > curMusic->partition[ CurrentPat]->header.size) toX = curMusic->partition[ CurrentPat]->header.size;
	
	if( fromX == -1)		fromX 	= GetCtlValue( xScroll);
	if( toX == -1)			toX 	= GetMaxXWave();
	
	XOffSet = WaveRect.left	- GetCtlValue( xScroll)*XSize;
	YOffSet = WaveRect.top	- GetCtlValue( yScroll)*YSize;
	
	/***********/
	
	saveClipRgn = NewRgn();
 	GetClip( saveClipRgn);
	
	{
	
	cRect.left = fromX*XSize + XOffSet;		if( cRect.left < WaveRect.left) 	cRect.left = WaveRect.left;
											if( cRect.left > WaveRect.right) 	cRect.left = WaveRect.right;
	
	cRect.right = toX*XSize + XOffSet;		if( cRect.right < WaveRect.left) 	cRect.right = WaveRect.left;
											if( cRect.right > WaveRect.right) 	cRect.right = WaveRect.right;
	
	if( chan != -1)
	{
		cRect.top = chan*YSize + YOffSet + 1;	if( cRect.top <= WaveRect.top) 		cRect.top = WaveRect.top+1;
												if( cRect.top > WaveRect.bottom) 	cRect.top = WaveRect.bottom;
		
		cRect.bottom = (chan+1)*YSize  + YOffSet+1;	if( cRect.bottom <= WaveRect.top) 	cRect.bottom = WaveRect.top+1;
													if( cRect.bottom > WaveRect.bottom) cRect.bottom = WaveRect.bottom;
	}
	else
	{
		cRect.top = YOffSet+1;					if( cRect.top <= WaveRect.top) 		cRect.top = WaveRect.top+1;
												if( cRect.top > WaveRect.bottom) 	cRect.top = WaveRect.bottom;
												
		cRect.bottom = (WaveDriverType.numChn)*YSize + YOffSet+1;	if( cRect.bottom <= WaveRect.top) 	cRect.bottom = WaveRect.top+1;
																if( cRect.bottom > WaveRect.bottom) cRect.bottom = WaveRect.bottom;
	}
	
	ClipRect( &cRect);
	
	}
	
	
	EraseRect( &WaveRect);
	
	/***********/
	
	RGBForeColor( &theColor);
	for( i = fromX; i < toX; i++)
	{
		MoveTo( i*XSize + XOffSet - 1, cRect.top);
		LineTo( i*XSize + XOffSet - 1, cRect.bottom);
	}
	
	for( i = GetCtlValue( yScroll); i < GetMaxYWave(); i++)
	{
		MoveTo( cRect.left, i*YSize + YSize/2 + YOffSet);
		LineTo( cRect.right, i*YSize + YSize/2 + YOffSet);
	}
	
	ForeColor( blackColor);
	
	if( toX == curMusic->partition[ CurrentPat]->header.size)
	{
		MoveTo( toX*XSize + XOffSet - 1, cRect.top);
		LineTo( toX*XSize + XOffSet - 1, cRect.bottom);
	}
	
	/***********/
	
	if( XSize < 8) 			WaveDriverType.outPutRate =	rate2khz;
	else if( XSize < 32) 	WaveDriverType.outPutRate =	rate5khz;
	else 					WaveDriverType.outPutRate =	rate11khz;
	
	WaveCopyDriver->DriverSettings		=	WaveDriverType;
	WaveCopyDriver->Reading				=	true;
	WaveCopyDriver->ASCBUFFER			=	WAVESIZE;
	WaveCopyDriver->ASCBUFFERReal		=	WAVESIZE;
	WaveCopyDriver->VSYNC				= 	((WaveDriverType.outPutRate>>16) * 125L) / (50L);
	
	WaveCopyDriver->FREQBASE = AMIGA_CLOCKFREQ2 / ((WaveDriverType.outPutRate>>16));
	WaveCopyDriver->FREQBASE ++;
	
	WaveCopyDriver->BytesToGenerate		= 	0L;
	WaveCopyDriver->BufCounter			= 	0L;
	WaveCopyDriver->JumpToNextPattern	= 	false;
	
	WaveCopyDriver->smallcounter			= 128;
	WaveCopyDriver->PartitionReader		= 0;
	WaveCopyDriver->Pat					= MADDriver->Pat;
	WaveCopyDriver->PL					= MADDriver->PL;
/*	{
		short 		pos, tt;
		Cmd 		*theCmd;
		Boolean		okT[ MAXTRACK];
	
		for( tt = 0; tt < curMusic->header->numChn; tt++) okT[ tt] = false;
	
		for( pos = 0; pos < fromX; pos++)
		{
			for( tt = 0; tt < curMusic->header->numChn; tt++)
			{
				theCmd = GetMADCommand( pos, tt, curMusic->partition[ CurrentPat]);
				
				if( theCommand->note != 0xFF || theCommand->note != 0xFE)
				{
					short xt;
				
					okT[ tt] = true;
					
					for( xt = 0; xt < curMusic->header->numChn; xt++)
					{
						if( 
					}
				}
			}
		}
	}
	*/
	
	destReader 					= toX;
	dataX 						= 0L;
	
	CellXBytes 		= WaveCopyDriver->VSYNC;
	CellXBytes 		/= WaveCopyDriver->finespeed;
	CellXBytes		*= WaveCopyDriver->speed;
	
	OnContinue = true;
	WaveCopyDriver->endPattern = false;
	ReadyToGoOut = false;
		
	do
	{
		long			i, whichTrack, ptrsize, ttt, prevReader;
		Byte			*alpha = (Byte*) WavePtr;
		
		ttt		= WaveCopyDriver->VSYNC;
		ttt		/= WaveCopyDriver->finespeed;
		ttt		*= WaveCopyDriver->speed;
	
		if( ttt != CellXBytes)
		{
			CellXBytes					= ttt;
			
			WaveCopyDriver->smallcounter		= 128;
			WaveCopyDriver->PartitionReader	= prevReader;
			WaveCopyDriver->BytesToGenerate	= 0L;
			WaveCopyDriver->BufCounter		= 0L;
			dataX 						= 0L;
			
			XOffSet = WaveRect.left	- GetCtlValue( xScroll)*XSize + prevReader * XSize;
		}
		
		if( WaveCopyDriver->endPattern) ReadyToGoOut = true;
		
		WaveCopyDriver->Equalizer = false;
		WaveCopyDriver->DriverSettings.TickRemover = false;
		prevReader = WaveCopyDriver->PartitionReader;
		
		ptrsize = GetPtrSize( WavePtr);
		while( ptrsize-- > 0) *alpha++ = 0x80;
		
		OnContinue = DirectSave( (Ptr) WavePtr, &WaveDriverType, WaveCopyDriver);
	//	if( WaveCopyDriver->curMusic->musicUnderModification) MyDebugStr( __LINE__, __FILE__, "");
		
		if( OnContinue == false) ReadyToGoOut = true;
		
		// ** DRAW PARTICLE
		
		if( (dataX + WaveCopyDriver->ASCBUFFER)/CellXBytes >= fromX)
		{
			short sy, ey;
			
			if( chan == -1)
			{
				sy = GetCtlValue( yScroll);
				ey = GetMaxYWave();
			}
			else
			{
				sy = ey = chan;
				ey++;
			}
			
			for( whichTrack = sy; whichTrack < ey; whichTrack++)
			{
				long	temp, minY, maxY, BS, BE;
				
				if( MADDriver->Active[ whichTrack]) ForeColor( blackColor);
				else RGBForeColor( &Gray);

				
				temp = (dataX*XSize)/CellXBytes + XOffSet;
			//	temp = position;
				
				MoveTo( temp, whichTrack*YSize + YSize/2 + YOffSet);
				
				for( i = 0, BE = 0; BE < WaveCopyDriver->ASCBUFFER; i ++, temp++)			//i < XSize
				{
					BS = (i * CellXBytes) / XSize;
					BE = ((i+1) * CellXBytes) / XSize;
					
					if( BS < WaveCopyDriver->ASCBUFFER)
					{
						if( BE >= WaveCopyDriver->ASCBUFFER) BE = WaveCopyDriver->ASCBUFFER;
						
						GetMinMax( BS, BE, WavePtr + whichTrack, &minY, &maxY);
						
						minY += 128;		maxY += 128;
						
						maxY *= YSize;		maxY >>= 8L;
						minY *= YSize;		minY >>= 8L;
						
						minY += whichTrack*YSize + YOffSet;
						maxY += whichTrack*YSize + YOffSet;
						
						LineTo( temp, minY);
						if( minY != maxY)
						{
							MoveTo( temp, minY);	LineTo( temp, maxY);
						}
					}
					else MyDebugStr( __LINE__, __FILE__, "");
				}
				
				ForeColor( blackColor);
			}
		}
		
		dataX += WaveCopyDriver->ASCBUFFER;
	}
	while( ReadyToGoOut == false && WaveCopyDriver->PartitionReader <= destReader);
	
	/**********/
		
	
	/**********/
	
	FinishFrame();
	
	LMSetHiliteMode( LMGetHiliteMode() & hiliteBit);
	InvertRect( &CurRect);
	
	RGBBackColor( &theColor);
	
	SetClip( saveClipRgn);
 	DisposeRgn( saveClipRgn);
	
	return noErr;
}


void DrawFrame()
{
	short			i, itemType, temp, add;
	Rect			caRect, fRect;
	Handle			itemHandle;
	char			String[ 200];
	Rect			tRect;
	FontInfo		fInfo;
	RgnHandle		savedClip;
	
	savedClip = NewRgn();
 	GetClip( savedClip);
 	
	GetDialogItem( WaveDlog, 2, &itemType, &itemHandle, &fRect);
	
	/**/
	
	tRect = fRect;
	tRect.bottom++;
	tRect.right = WaveRect.right;
	ClipRect( &tRect);
	
	// X BARRE
	
	GetPortBounds( GetDialogPort( WaveDlog), &caRect);
	
	MoveTo( 0, fRect.top);
	LineTo( caRect.right, fRect.top);
	
	MoveTo( 0, WaveRect.top);
	LineTo( caRect.right, WaveRect.top);
	
	if( curMusic->partition[ CurrentPat]->header.size > 100) temp = 3;
	else temp = 2;
	
	if( temp <= 2)
	{
		if( XSize > 8) 			add = 1;
		else if( XSize > 4)		add = 2;
		else 					add = 4;
	}
	else if( temp == 3)
	{
		if( XSize > 16) 		add = 1;
		else if( XSize > 8) 	add = 2;
		else if( XSize > 4)		add = 4;
		else 					add = 8;
	}
	
	tRect.top	= fRect.top + 1;
	tRect.bottom= fRect.bottom;
	
	tRect.left = 0;
	tRect.right = WaveRect.left + XSize/2 - (add*XSize)/2;
	
	EraseRect( &tRect);
	MoveTo( tRect.right-1, tRect.top);
	LineTo( tRect.right-1, tRect.bottom);
	
	for( i = GetCtlValue( xScroll); i < GetMaxXWave(); i+= add)
	{
		tRect.left	= WaveRect.left + (i - GetCtlValue( xScroll)) * XSize + XSize/2 - (add*XSize)/2;
		tRect.right	= tRect.left + (add*XSize);
		
		NTStr( temp, i , (Ptr) String);
		TETextBox( String, strlen( String), &tRect, teCenter);
		
		MoveTo( tRect.right-1, tRect.top);
		LineTo( tRect.right-1, tRect.bottom);
		
		if( add != 1)
		{
			MoveTo( tRect.left + (tRect.right-tRect.left)/2, tRect.bottom-1);
			LineTo( tRect.left + (tRect.right-tRect.left)/2, tRect.bottom);
		}
	}
	
	GetPortBounds( GetDialogPort( WaveDlog), &caRect);
	
	if( tRect.right < caRect.right)
	{
		tRect.left = tRect.right;
		tRect.right = caRect.right;
		
		EraseRect( &tRect);
	}

	/**/

	// Y BARRE
	
	tRect.left	= 0;
	tRect.right	= WaveRect.left;
	tRect.bottom = WaveRect.bottom;
	tRect.top	= WaveRect.top;
	ClipRect( &tRect);
	
	for( i = GetCtlValue( yScroll); i < GetMaxYWave(); i++)
	{
		tRect.left	= 0;
		tRect.right	= WaveRect.left;
		
		tRect.top	= WaveRect.top + 1 + (i - GetCtlValue( yScroll)) * YSize;
		tRect.bottom= tRect.top + YSize - 1;
		
		SwitchColor( i);
		PaintRect( &tRect);
		
		ForeColor( blackColor);
		NTStr( 2, i +1, (Ptr) String);
		
		if( MADDriver->Active[ i]) ForeColor( blackColor);
		else RGBForeColor( &Gray);
		
		MoveTo( 1, tRect.top + 3 +(tRect.bottom - tRect.top)/2);
		DrawString( MyC2PStr( String));
		
		ForeColor( blackColor);
		
		MoveTo( 0, tRect.bottom);
		LineTo( WaveRect.left, tRect.bottom);
	}
	
	GetPortBounds( GetDialogPort( WaveDlog), &caRect);
	
	MoveTo( WaveRect.left-1, WaveRect.top);
	LineTo( WaveRect.left-1, caRect.bottom);
	
	if( tRect.bottom < caRect.bottom)
	{
		tRect.top = tRect.bottom+1;
		tRect.bottom = caRect.bottom;
		
		EraseRect( &tRect);
	}	
	
	SetClip( savedClip);
	DisposeRgn( savedClip);
	
	/**/
}

void UpdateWaveInfo()
{
	GrafPtr		SavePort;
	Rect		caRect, cellRect;
	Boolean		ctlUpdate = false;
	short		x;

	if( WaveDlog == 0L) return;
	
	/**/
	
	/**/
	
 	GetPort( &SavePort);
 	SetPortDialogPort( WaveDlog);
	
	ReaderCopy = MADDriver->PartitionReader;		CurrentPat = MADDriver->Pat;
	
	if( XSize != prevXSize) ctlUpdate = true;
	if( YSize != prevYSize) ctlUpdate = true;
	
	if( CurrentPatSize != curMusic->partition[ CurrentPat]->header.size)
	{
		EraseRect( &WaveRect);
		ctlUpdate = true;
	}
	
	if( WaveDriverType.numChn != 	MADDriver->DriverSettings.numChn)
	{
		EraseRect( &WaveRect);
		WaveDriverType.numChn = 	MADDriver->DriverSettings.numChn;
		ctlUpdate = true;
	}
	
	if( ctlUpdate) SetWaveControl();
	
	CreateCurWaveRect();
	
	GetPortBounds( GetDialogPort( WaveDlog), &caRect);
	
	cellRect.left = 0;
	cellRect.top = WaveRect.top - 14;
	cellRect.right = caRect.right;
	cellRect.bottom = caRect.bottom;
	InvalWindowRect( GetDialogWindow( WaveDlog), &cellRect);
	
	
	//
	MADDisposeVolumeTable( WaveCopyDriver);
	
	BlockMoveData( MADDriver, WaveCopyDriver, sizeof( MADDriverRec));
	WaveCopyDriver->DriverSettings	=	WaveDriverType;
	MADCreateVolumeTable( WaveCopyDriver);
	//
	
	SetPort( SavePort);
}

void  UpdateWaveWindow(DialogPtr GetSelection)
{ 
	Rect   		caRect, tempRect, itemRect;
	GrafPtr		SavePort;
	Point		theCell;
	Ptr			theStr;
	Str255		tempStr, aStr;
	short		i, itemType;
	Handle		itemHandle;
	
	GetPort( &SavePort);
	SetPortDialogPort( WaveDlog);
	
	BeginUpdate( GetDialogWindow( WaveDlog));
	
	DrawDialog( WaveDlog);
	
	GetPortBounds( GetDialogPort( WaveDlog), &caRect);
	
	WaveRect.right = caRect.right - 15;
	WaveRect.bottom = caRect.bottom - 15;
	
	EraseRect( &WaveRect);
	
	DrawFrame();
	
	SetCursor( &watchCrsr);
	
	ComputeWave( -1, -1, -1);
	
	SetCursor( GetQDGlobalsArrow( &qdarrow));
	
	DrawGrowIcon( GetDialogWindow( WaveDlog));
	
	EndUpdate( GetDialogWindow( WaveDlog));

	SetPort( SavePort);
}

void DoItemPressWave( short whichItem, DialogPtr whichDialog)
{
		Cell				theCell;
		long				tempLong;
 		short				temp, temp2, i, bogus, ctlPart;
 		Rect				caRect, cellRect, tempRect;
 		GrafPtr				SavePort;
 		Cmd					*theCommand;
 		Boolean				DoubleClick;
 		Point				myPt, lastCell;
 		ControlHandle		theControl;
 		ControlActionUPP	MyControlUPP;
 		
 		GetPort( &SavePort);
 		SetPortDialogPort( WaveDlog);
 		
		if (theEvent.what == mouseDown)
		{
			myPt = theEvent.where;
			GlobalToLocal( &myPt);
						
			DoubleClick = false;
			
			tempRect = WaveRect;
			tempRect.left = 0;
			tempRect.right = WaveRect.left;
			
			if (PtInRect( myPt, &tempRect))
			{
				GetMouse( &myPt);
				
				myPt.v -= WaveRect.top;
				myPt.v /= YSize;
				myPt.v += GetCtlValue( yScroll);
				
				if( myPt.v < 0) myPt.v = 0;
				if( myPt.v > curMusic->header->numChn)
					myPt.v = curMusic->header->numChn;
						
				
				if( (theEvent.modifiers & cmdKey) != 0)			// Mute
				{
					MADDriver->Active[ myPt.v] = !MADDriver->Active[ myPt.v];
					
					UPDATE_TrackActive();
				}
				else if( (theEvent.modifiers & optionKey) != 0)	// Solo
				{
					short	noActive;
					
					for( i = 0, noActive = 0; i < curMusic->header->numChn; i++)
					{
						if( MADDriver->Active[ i] == true)
						{
							noActive++;
						}
					}
					
					if( noActive <= 1 && MADDriver->Active[ myPt.v] == true)
					{
						for( i = 0, noActive = 0; i < curMusic->header->numChn; i++) MADDriver->Active[ i] = true;
					}
					else
					{
						for( i = 0; i < curMusic->header->numChn; i++) MADDriver->Active[ i] = false;
						MADDriver->Active[ myPt.v] = true;
					}
					
					UPDATE_TrackActive();
				}
			}
			
			if (PtInRect( myPt, &WaveRect))
			{
				switch( mode)
				{
					case playM:
					{
						Boolean		IsPlay = MusicPlayActive;
						Boolean		IsJump = MADDriver->JumpToNextPattern;
						
						MADDriver->JumpToNextPattern = false;
						
						GetMouse( &myPt);
						myPt.h -= WaveRect.left;
						myPt.h /= XSize;
						myPt.h += GetCtlValue( xScroll);
						
						if( myPt.h < 0) myPt.h = 0;
						else if( myPt.h >= curMusic->partition[ CurrentPat]->header.size) myPt.h = curMusic->partition[ CurrentPat]->header.size - 1;
						MADDriver->PartitionReader = myPt.h;
						
						if( IsPlay == false) DoPlay();
						
						while( Button())
						{
							DoGlobalNull();
							WaitNextEvent( everyEvent, &theEvent, 1, 0L);
						}
						
						MADDriver->JumpToNextPattern = IsJump;
						
						if( IsPlay == false) DoPause();
					}
					break;
					
					case zoomM:
							if( theEvent.modifiers & optionKey)
							{
								if( XSize > 4)
								{
									EraseRect( &WaveRect);
									XSize /= 2;
									
									UpdateWaveInfo();
								}
							}
							else
							{
								if( XSize < 128)
								{
									GetMouse( &myPt);
							
									myPt.h -= WaveRect.left;
									myPt.h /= XSize;
									myPt.h += GetCtlValue( xScroll);
									
									EraseRect( &WaveRect);
									XSize *= 2;
									
									SetWaveControl();
									
									GetPortBounds( GetDialogPort( WaveDlog), &caRect);
									
									myPt.h -= (caRect.right + 1 - WaveRect.left - 15)/(XSize*2);
									if( myPt.h < 0) myPt.h = 0;
									SetCtlValue( xScroll, myPt.h);
									
									UpdateWaveInfo();
								}
							}
					break;
					
					case noteM:
						GetMouse( &myPt);
						
						myPt.h -= WaveRect.left;
						myPt.h /= XSize;
						myPt.h += GetCtlValue( xScroll);
						
						if( myPt.h < 0) myPt.h = 0;
						if( myPt.h > curMusic->partition[ CurrentPat]->header.size)
							myPt.h = curMusic->partition[ CurrentPat]->header.size;
							
						
						
						myPt.v -= WaveRect.top;
						myPt.v /= YSize;
						myPt.v += GetCtlValue( yScroll);
						
						if( myPt.v < 0) myPt.v = 0;
						if( myPt.v > curMusic->header->numChn)
							myPt.v = curMusic->header->numChn;
						
						MADDriver->PartitionReader = myPt.h;
						
						ShowCurrentCmdNote();
						SetCommandTrack( myPt.v, -1);
					break;
				}
			}
			else
			{
			//	ctlPart = FindControl( myPt, GetDialogWindow( whichDialog), &theControl);
				
				theControl = 0L;
				if( TestControl(  xScroll, myPt)) theControl = xScroll;
				if( TestControl(  yScroll, myPt)) theControl = yScroll;
				
				if( theControl == xScroll || theControl == yScroll)
				{
				/*	if( ctlPart == kControlIndicatorPart)
					{
						bogus = TrackControl( theControl, myPt, 0L);
						if( bogus != 0)
						{
							GetPortBounds( GetDialogPort( WaveDlog), &caRect);
							InvalWindowRect( GetDialogWindow( WaveDlog), &caRect);
							CreateCurWaveRect();
							
							UpdateWaveWindow( WaveDlog);
						}
					}
					else if( ctlPart > 0)*/
					{
						MyControlUPP = NewControlActionUPP( actionProcWave);
						gThumbPrev = GetCtlValue( theControl);
						TrackControl(theControl, myPt, MyControlUPP);
						DisposeControlActionUPP( MyControlUPP);
					}
				}
			}
		}   						/* End of mouseDown */
		
	switch( whichItem)
	{
		case 7:
		case 5:
		case 6:
			HiliteControl( playBut, 0);
			HiliteControl( zoomBut, 0);
			HiliteControl( noteBut, 0);
			
			switch( whichItem)
			{
				case 7:		mode = playM;	HiliteControl( playBut, kControlButtonPart);	break;
				case 5:		mode = noteM;	HiliteControl( noteBut, kControlButtonPart);	break;
				case 6:		mode = zoomM;	HiliteControl( zoomBut, kControlButtonPart);	break;
			}
			 
			if( whichItem == 6)
			{
				if( TickCount() - doubleClick <= GetDblTime())
				{
					XSize = 8;
					EraseRect( &WaveRect);
					UpdateWaveInfo();
				}
				doubleClick = TickCount();
			}
		break;
		
		case 10:
		{
			Str255	aStr;
			long	mresult;
			short 	itemType, curSelec;
			Handle	itemHandle;
			Rect	tempRect;
		
			
		
			InsertMenu( OsciHMenu, hierMenu);
			GetDialogItem( WaveDlog, whichItem, &itemType, &itemHandle, &tempRect);
			
			switch( YSize)
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
					case 1:	YSize = 16;		break;
					case 2:	YSize = 32;		break;
					case 3:	YSize = 64;		break;
					case 4:	YSize = 128;	break;
					case 5:	YSize = 256;	break;
				}
				
				NumToString( YSize, aStr);
				SetDText( WaveDlog, 11, aStr);
				
				EraseRect( &WaveRect);
				
				UpdateWaveInfo();
			}
			DeleteMenu( GetMenuID( OsciHMenu));
		}
		break;
	}
	SetPort( SavePort);
}

pascal void actionProcWave(ControlHandle theControl, short ctlPart)
{
long			lRefCon;
short			maxValue, minValue, curVal, XX, sVal, itemType;
RgnHandle		aRgn;
Handle			itemHandle;

if( ctlPart <= 0) return;

lRefCon = GetCRefCon( theControl);
maxValue = GetCtlMax( theControl);
minValue = GetCtlMin( theControl);
curVal = sVal = GetCtlValue( theControl);

	switch( ctlPart)
	{
		case kControlUpButtonPart:
			curVal -= 1;
			if( curVal < minValue) curVal = minValue;
		break;
		
		case kControlDownButtonPart:
			curVal += 1;
			if( curVal > maxValue) curVal = maxValue;
		break;
		
		case kControlPageUpPart:
			switch( GetCRefCon( theControl))
			{
				case yScrollNum:	curVal -= GetMaxYWave() - GetCtlValue( yScroll) + 1;	break;
				case xScrollNum:	curVal -= GetMaxXWave() - GetCtlValue( xScroll) + 1;	break;
			}
			
			if( curVal < minValue) curVal = minValue;
		break;
		
		case kControlPageDownPart:
			switch( GetCRefCon( theControl))
			{
				case yScrollNum:	curVal += GetMaxYWave() - GetCtlValue( yScroll) - 1;	break;
				case xScrollNum:	curVal += GetMaxXWave() - GetCtlValue( xScroll) - 1;	break;
			}
			if( curVal > maxValue) curVal = maxValue;
		break;
		
		case kControlIndicatorPart:
			sVal = gThumbPrev;
			gThumbPrev = curVal;
		break;
	}
	
	if( sVal != curVal)
	{
		RgnHandle		aRgn;
		
		SetCtlValue( theControl, curVal);
		
		aRgn = NewRgn();
		
		switch( lRefCon)
		{
			case yScrollNum:
				WaveRect.left -= 14;
				WaveRect.top++;
				ScrollRect( &WaveRect, 0, (sVal - curVal) *YSize, aRgn);
				WaveRect.top--;
				WaveRect.left += 14;
				
				CreateCurWaveRect();
				InvalWindowRgn( GetDialogWindow( WaveDlog), aRgn);
				UpdateWaveWindow( WaveDlog);
			break;
			
			case xScrollNum:
				WaveRect.top -= 14;
				ScrollRect( &WaveRect, (sVal - curVal) *XSize, 0, aRgn);
				WaveRect.top += 14;
				
				CreateCurWaveRect();
				
			/*	if( sVal - curVal == 1 && (*aRgn)->rgnSize == 10)
				{
					ComputeWave( curVal, curVal+2, -1);
					FinishFrame();
				}
				else if( sVal - curVal == -1 && (*aRgn)->rgnSize == 10)
				{
					ComputeWave( GetMaxXWave()-2, GetMaxXWave(), -1);
				}
				else*/
				{
					InvalWindowRgn( GetDialogWindow( WaveDlog), aRgn);
					UpdateWaveWindow( WaveDlog);
				}
			break;
		}
		DrawFrame();
		
		DisposeRgn( aRgn);
	}
}

void CreateWaveWindow(void)
{
	Rect			itemRect, tempRect, dataBounds;
	Handle			itemHandle;
	short			itemType, itemHit, temp, iWidth;
	Point			cSize;
	FontInfo		ThisFontInfo;
	Str255			String, aStr;
	Cmd				aCmd;
	OSErr			iErr;

	SetItemMark( EditorMenu, 4, checkMark);
	
	if( WaveDlog != 0L)
	{
		SetWindEtat( GetDialogWindow(WaveDlog));
		return;
	}
	
	WaveDlog = GetNewDialog( 153, 0L, GetDialogWindow( ToolsDlog));
	SetWindEtat( GetDialogWindow(WaveDlog));
	
	SetPortDialogPort( WaveDlog);

	/**/
	doubleClick = 0;
	
	GetDialogItem( WaveDlog , 7, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	playBut = NewControl( 	GetDialogWindow( WaveDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							160,
							kControlBevelButtonNormalBevelProc,
							0);
							
	GetDialogItem( WaveDlog , 5, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	noteBut = NewControl( 	GetDialogWindow( WaveDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							140,
							kControlBevelButtonNormalBevelProc,
							0);
							
	GetDialogItem( WaveDlog , 6, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	zoomBut = NewControl( 	GetDialogWindow( WaveDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							177,
							kControlBevelButtonNormalBevelProc,
							0);
	
	/**/


	mode = playM;

	HiliteControl( playBut, kControlButtonPart);
	
	SetRect( &itemRect, 0, 0, 30, 16);
	xScroll = NewControl(		 	GetDialogWindow( WaveDlog),
									&itemRect,
									"\p.",
									true,
									0,
									0,
									0,
									gScrollBarID,
									xScrollNum);

	SetRect( &itemRect, 0, 0, 30, 16);
	yScroll = NewControl( 			GetDialogWindow( WaveDlog),
									&itemRect,
									"\p.",
									true,
									0,
									0,
									0,
									gScrollBarID,
									yScrollNum);

	
	TextFont(4);		TextSize(9);
	
	YSize = 32;
	XSize = 8;
	GetDialogItem( WaveDlog, 1, &itemType, &itemHandle, &WaveRect);
	NumToString( YSize, aStr);		SetDText( WaveDlog, 11, aStr);
	
	ReaderCopy = MADDriver->PartitionReader;		CurrentPat = MADDriver->Pat;
	WaveDriverType.numChn = 	MADDriver->DriverSettings.numChn;
	
	SetWaveControl();
	
	CreateCurWaveRect();
	
	ShowWindow( GetDialogWindow( WaveDlog));
	SelectWindow2( GetDialogWindow( WaveDlog));
	
	WaveDriverType.numChn			= 	MADDriver->DriverSettings.numChn;
	WaveDriverType.outPutBits		= 	8;
	WaveDriverType.outPutRate		=	rate11khz;
	WaveDriverType.outPutMode		=	PolyPhonic;
	WaveDriverType.driverMode		=	SoundManagerDriver;
	//WaveDriverType.antiAliasing		=	false;
	WaveDriverType.repeatMusic		=	false;
	WaveDriverType.sysMemory		=	false;
	WaveDriverType.surround			=	false;
	//WaveDriverType.Interpolation	= 	false;
	WaveDriverType.MicroDelaySize	=	0;
	WaveDriverType.Reverb			=	false;
	WaveDriverType.ReverbSize		=	0;
	WaveDriverType.ReverbStrength	=	0;
	WaveDriverType.oversampling		=	1;
	
	WavePtr = (char*) NewPtr( 100L + WAVESIZE * MAXTRACK * 2L);
	if( WavePtr == 0L) MyDebugStr( __LINE__, __FILE__, "Not enough memory");
	
	WaveCopyDriver = (MADDriverRec*) NewPtrClear( sizeof( MADDriverRec));
	
	BlockMoveData( MADDriver, WaveCopyDriver, sizeof( MADDriverRec));
	
//	iErr = MADCreateDriver( &WaveDriverType, gMADLib, &WaveCopyDriver);
//	if( iErr) MyDebugStr( __LINE__, __FILE__, "MADCreateDriver in WavePreview");
	
	WaveCopyDriver->DriverSettings	=	WaveDriverType;
	MADCreateVolumeTable( WaveCopyDriver);	
}

void CloseWave(void)
{
Point	Start;
Rect	caRect;

	if( WaveDlog != 0L)
	{
		SetPortDialogPort( WaveDlog);
	
		DisposePtr( (Ptr) WavePtr);
		WavePtr = 0L;
		
		MADDisposeVolumeTable( WaveCopyDriver);
		
		DisposePtr( (Ptr) WaveCopyDriver);
		
	//	MADDisposeDriver( WaveCopyDriver);
		
		GetPortBounds( GetDialogPort( WaveDlog), &caRect);
		
		thePrefs.WinHi[ GetWRefCon( GetDialogWindow( WaveDlog))] = caRect.bottom;
		thePrefs.WinLarg[ GetWRefCon( GetDialogWindow( WaveDlog))] = caRect.right;
		thePrefs.WinEtat[ GetWRefCon( GetDialogWindow( WaveDlog))] = 1;
		
		Start.h = Start.v = 0;
		LocalToGlobal( &Start);
		thePrefs.WinPos[ GetWRefCon( GetDialogWindow( WaveDlog))].v = Start.v;
		thePrefs.WinPos[ GetWRefCon( GetDialogWindow( WaveDlog))].h = Start.h;
		
		DisposDialog( WaveDlog);
	}
	WaveDlog = 0L;
	
	SetItemMark( EditorMenu, 4, noMark);
}

void DoKeyPressWave( short theChar)
{
	Point 				theCell, aCell;
	Rect				cellRect, eRect, sRect;
	Str255				theStr;
	short				theNo, temp;
	Cmd					*theCommand;
	GrafPtr				SavePort;
	
 	if( curMusic == 0L) return;
 	
 	GetPort( &SavePort);
 	SetPortDialogPort( WaveDlog);
 	
	if( theChar ==  9)
 	{
 		HiliteControl( playBut, 0);
		HiliteControl( zoomBut, 0);
		HiliteControl( noteBut, 0);
		
		mode++;
		if( mode > MODESIZE) mode = 0;
		switch( mode)
		{
			case playM:		HiliteControl( playBut, kControlButtonPart);	break;
			case zoomM:		HiliteControl( zoomBut, kControlButtonPart);	break;
			case noteM:		HiliteControl( noteBut, kControlButtonPart);	break;
		}
 	}
 	
	SetPort( SavePort);
}
