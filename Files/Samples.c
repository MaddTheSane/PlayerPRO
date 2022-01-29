#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include "Undo.h"
#include <QDOffscreen.h>
#include <Drag.h>
#include <Aliases.h>

	/******** HELP MODULE ********/
	enum
	{
		HInfo	= 25,
		HSel	= 27,
		HPenc	= 26,
		Hzoom	= 18,
		HPlug	= 14,
		HWave	= 1,
		HEnv	= 13,
		HSus	= 36,
		HLoop	= 37
	};
	#define	AHELPSIZE	9

	static	short					AHelp[ AHELPSIZE] = { HInfo, HSel, HPenc, Hzoom, HPlug, HWave, HEnv, HSus, HLoop};

	void DoHelpSamples( short **items, short *lsize)
	{
		*lsize = AHELPSIZE;
		*items = AHelp;
	}

	/*****************************/
void			VSTSampleEditor( short item, sData	*curData, long Start, long End, Boolean StereoMode);
void			NAppelVSTPlug( short InstruNo, short samp, short whichPlug);
short			GetMaxSoundFilterPlugs( void);
void			MoveLoopSelection( short InstruNo);
void			NEditInstruInfo( short ins, short samp);
void			SetMaxBottomWindow( short , DialogPtr);
void			NCOPYSample( long, long, short, short);
void			DoKeyPressSample( DialogPtr, short);
short			PressSampleMenu( Rect	*PopUpRect);
void			InternalUpdate( short whichWindow);
void			DoKeyPlay(short temp);
short			NewOffscreenPixMap(PixMapHandle *thePixMap, Rect *picRect);
short			NewOffscreenBitMap(BitMap *thePixMap, Rect *picRect);
void			ZapBitMap(BitMap *offscreenPixMap);
void			ZapPixMap(PixMapHandle *offscreenPixMap);
void			GetInstruName( short, Ptr);
void			GetQualityString( short ID, Str255 str);
Boolean			DragSample( RgnHandle myRgn, short theNo, EventRecord *theEvent);
pascal OSErr 	MySendSampleProc(FlavorType theFlavor,  void *refCon, ItemReference theItem,  DragReference theDrag);
Handle			SoundQualityExportSnd( short , OSType	*, short *, long *, Str255);
void			SetMobiusRect( Rect*, short, short, short, short);
void			DrawSample( short pSt, short pEn, short Instru, DialogPtr theDia);
pascal OSErr 	MyTrackingSample(short message, WindowPtr theWindow, void *handlerRefCon, DragReference theDrag);
void			NSelectInstruList( short , short);
long			AmpToByte( short Pos, short InstruNo);
void			ConvertInstrumentOut16( register	short	*tempPtr,	register long sSize);
void			FFTSampleFilter( sData *SData, short instru, Boolean filter);
void			SampleUpdateNow( void);

extern	DialogPtr	InstruListDlog;
extern	Boolean		PianoRecording, DragManagerUse;
extern	RGBColor	theColor;
extern	Cursor		HandCrsr, beamCrsr, pencilCrsr, pencilCrsrStereo, CHandCrsr;

static	DragTrackingHandlerUPP	MyTrackingHandlerUPP;
static	DragReceiveHandlerUPP	MyReceiveDropHandlerUPP;

struct	IdleSample	{
		short			oldPos;
		short			oldInstru;
};
typedef		struct IdleSample	IdleSample;

struct	LPoint	{
		long		start;
		long		end;
};
typedef		struct LPoint	LPoint;

enum
{
	eSelect = 0,
	ePencil,
	eZoom
};
#define MODESIZE 2

enum
{
	panningEnv = -2,
	volumeEnv = -1
};

	extern	KeyMap					km;
	extern	WindowPtr				oldWindow;
	extern	short					TouchIn, TouchMem[11], TrackMem[11];
	extern	Cursor					ZoomInCrsr, ZoomOutCrsr;
	
	static 	Rect					SampleRect;
			LPoint					SelecRect[ MAXINSTRU];
	static	GWorldPtr				BitMapSample[ MAXINSTRU];
	static	Boolean					Clignote[ MAXINSTRU], TokTak[ MAXINSTRU];
	static	long					ZoomLevel[ MAXINSTRU];
	static	IdleSample				theIdle[ MAXTRACK];
	static	ControlHandle			InfoBut[ MAXINSTRU], PencilBut[ MAXINSTRU], SelectBut[ MAXINSTRU], ZoomBut[ MAXINSTRU];
	static	short					DragInstruSource, SelectMode[ MAXINSTRU];
	static	ControlHandle			xScroll[ MAXINSTRU];
	static	short					curSample[ MAXINSTRU];
	static  Byte					curMode[ MAXINSTRU];
	static	ControlHandle			LoopBut[ MAXINSTRU], SustainBut[ MAXINSTRU], EnvBut[ MAXINSTRU], FixedBut[ MAXINSTRU];
	static	short 					gThumbPrev;
	static	Boolean					gNowMovingLoop = false;
	
			DialogPtr				SampleDlog[ MAXINSTRU];
			
	extern 	Boolean		Direct;
	extern	RGBColor	theColor;

	#define	SAMPLEDEF		10L
	#define SAMPLERECTTOP	60
	#define ENVSIZE			300L


void PrintPixMap( PixMapHandle aPix);

long GetStartSampleValue( long val, short InstruNo)
{
	long	start;
	long	size;
	short	samp;
	
	samp = curSample[ InstruNo];
	if( samp < 0) samp = 0;
	
	size = curMusic->sample[ curMusic->fid[ InstruNo].firstSample + samp]->size;
	
	val = GetCtlValue( xScroll[ InstruNo]);
	
	if( val == GetCtlMax( xScroll[ InstruNo]))
	{
		start 		= size - size / ZoomLevel[ InstruNo];
	}
	else
	{
		start 		= (val * (size / SAMPLEDEF)) / ZoomLevel[ InstruNo];
	}
	
	return start;
}

/*inline long min( long a, long b)
{
	if( a < b) return a;
	else return b;
}

inline long max( long a, long b)
{
	if( a < b) return b;
	else return a;
}*/

void DrawSampleInt( long	sampleSize,
					long 	start,
					long 	tSS,
					long 	tSE,
					long 	high,
					long	larg,
					long	trueV,
					long	trueH,
					short	channel,
					sData	*curData)
{
	long long		temp, i;
	Ptr				theSample = curData->data;
	short			*theShortSample = (short*) curData->data;
	long long		BS, BE, minY, maxY, x;
	
	if( curData->amp == 16)
	{
		sampleSize /= 2;
		start /= 2;
		
		BS = start + (tSS * sampleSize) / larg;
		if( curData->stereo)
		{
			BS /= 2;	BS *=2;
			BS += channel;
		}
		temp = (theShortSample[ BS]  + 0x8000);
		temp *= high;	temp >>= 16;
		MoveTo( trueH + tSS, trueV + temp);
		
		for( i = tSS; i < tSE; i++)
		{
			BS = start + (i * sampleSize) / larg;
			BE = start + ((i+1) * sampleSize) / larg;
			
			if( curData->stereo)
			{
				BS /=2;		BS *=2;
				BE /=2;		BE *=2;
				
				BS += channel;
				BE += channel;
			}
			
			temp =(theShortSample[ BS]  + 0x8000);
			minY = maxY = temp;
			temp *= high;		temp >>= 16;
			LineTo( trueH + i, temp + trueV);
			
			if( BS != BE)
			{
				for( x = BS; x < BE; x++)
				{
					temp = (theShortSample[ x]  + 0x8000);
					
					if( temp > maxY) maxY = temp;
					if( temp < minY) minY = temp;
					
					if( curData->stereo) x++;
				}
				
				maxY *= high;		maxY >>= 16;
				minY *= high;		minY >>= 16;
				
				MoveTo( trueH + i, minY + trueV);
				LineTo( trueH + i, maxY + trueV);
			}
		}
	}
	else
	{
		BS = start + (tSS * sampleSize) / larg;
		if( curData->stereo)
		{
			BS /= 2;	BS *=2;
			BS += channel;
		}
		
		temp = (unsigned char) (theSample[ BS] - 0x80);
		temp *= high;	temp >>= 8;
		MoveTo( trueH + tSS, trueV + temp);
		
		for( i = tSS; i < tSE; i++)
		{
			BS = start + (i * sampleSize) / larg;
			BE = start + ((i+1) * sampleSize) / larg;
			
			if( curData->stereo)
			{
				BS /=2;		BS *=2;
				BE /=2;		BE *=2;
				
				BS += channel;
				BE += channel;
			}
			
			temp = (unsigned char) (theSample[ BS] - 0x80);
			minY = maxY = temp;
			temp *= high;		temp >>= 8;
			LineTo( trueH + i, temp + trueV);
			
			if( BS != BE)
			{
				for( x = BS; x < BE; x++)
				{
					temp = (unsigned char) (theSample[ x] - 0x80);
					
					if( temp > maxY) maxY = temp;
					if( temp < minY) minY = temp;
					
					if( curData->stereo) x++;
				}
				maxY *= high;		maxY >>= 8;
				minY *= high;		minY >>= 8;
			
				MoveTo( trueH + i, minY + trueV);
				LineTo( trueH + i, maxY + trueV);
			}
		}
	}
}

void SetSelectionZero( short no)
{
	SelecRect[ no].start	= 0;
	SelecRect[ no].end		= 0;
	ZoomLevel[ no]			= 1;
}

void SetSelection( short no, long start, long end)
{
	SelecRect[ no].start	= start;
	SelecRect[ no].end		= end;
	Clignote[ no]			= false;
}

void EraseTikTak( short ins)
{
	Rect	tempRect;

	if( Clignote[ ins])
	{
		if( TokTak[ ins])
		{
			TokTak[ ins] = false;
			
			tempRect.left = ByteToPos( SelecRect[ ins].start, ins);
			tempRect.right = tempRect.left +1;
			tempRect.top = SampleRect.top;
			tempRect.bottom = SampleRect.bottom;
			
			InvertRect( &tempRect);
		}
	}
	
	Clignote[ ins] = false;
}

void DoGrowSample( DialogPtr theDia)
{
long		lSizeVH;
GrafPtr		SavePort;
Rect		caRect, temp;
Point		theCell = { 0, 0}, aPt = { 0, 0};
short		tempA, tempB;
BitMap		screenBits;


	GetPort( &SavePort);
 	SetPortDialogPort( theDia);
	
	GetQDGlobalsScreenBits( &screenBits);
	
	temp.left = 100;
	temp.right = screenBits.bounds.right - 2;
	temp.top = 80;
	temp.bottom = 256 + SAMPLERECTTOP + 16;
	
	LocalToGlobal( &aPt);
	
	lSizeVH = 0;
	if( theEvent.what == mouseDown) lSizeVH = GrowWindow( GetDialogWindow( theDia), theEvent.where, &temp);
	
	if( lSizeVH != 0)
	{
		tempA = LoWord( lSizeVH);
		tempB = HiWord( lSizeVH);
	}
	else
	{
		GetPortBounds( GetDialogPort( theDia), &caRect);
		
		tempA = caRect.right;
		tempB = caRect.bottom;
	}
	
	MySizeWindow( theDia, tempA, tempB , true);
	
	GetPortBounds( GetDialogPort( theDia), &caRect);
	
	EraseRect( &caRect);
	InvalWindowRect( GetDialogWindow( theDia), &caRect);
	
	InternalUpdate( FindSample( theDia));
	
	SetPort( SavePort);
}

void UpdateDisplaySize( short InstruNo)
{
	Str255	StrTemp;

	if( !SampleDlog[ InstruNo]) return;

	switch( curSample[ InstruNo])
	{
		case panningEnv:
			NumToString( (long) curMusic->fid[ InstruNo].pannSize, StrTemp);
			pStrcat( StrTemp, "\p Pts");
			SetDText( SampleDlog[ InstruNo], 3, StrTemp);
		break;
		
		case volumeEnv:
			NumToString( (long) curMusic->fid[ InstruNo].volSize, StrTemp);
			pStrcat( StrTemp, "\p Pts");
			SetDText( SampleDlog[ InstruNo], 3, StrTemp);
		break;
		
		default:
			GetSizeString( (long) SampleDataS( InstruNo, curSample[ InstruNo])->size, StrTemp, false);
			SetDText( SampleDlog[ InstruNo], 3, StrTemp);
		break;
	}
}

void UpdateDisplay( short InstruNo)
{
	short	itemType;
	Rect	itemRect;
	Handle	itemHandle;
	long	start, sampleSize, val;
	Str255	StrTemp, aStr, theStr;
	
	if( !SampleDlog[ InstruNo]) return;
	
	val = GetCtlValue( xScroll[ InstruNo]);
	
	switch( curSample[ InstruNo])
	{
		case panningEnv:
		case volumeEnv:	
			start 		= (val * (ENVSIZE / SAMPLEDEF)) / ZoomLevel[ InstruNo];
			sampleSize	= ENVSIZE / ZoomLevel[ InstruNo];
			
			NumToString( start, StrTemp);
			SetDText( SampleDlog[ InstruNo], 20, StrTemp);
			
			NumToString( start + sampleSize, StrTemp);
			SetDText( SampleDlog[ InstruNo], 22, StrTemp);
		break;
		
		default:
			start = GetStartSampleValue( val, InstruNo); //(val * (SampleDataD( InstruNo)->size / SAMPLEDEF)) / ZoomLevel[ InstruNo];
			sampleSize = SampleDataD( InstruNo)->size / ZoomLevel[ InstruNo];
			
			
			// Real Note
			
			NumToString( SampleDataD( InstruNo)->relNote, StrTemp);
			pStrcat( StrTemp, "\p/");
			
			NumToString( 60 - SampleDataD( InstruNo)->relNote, aStr);
			pStrcat( StrTemp, aStr);
			pStrcat( StrTemp, "\p/");
			
			OctavesName( 48 - SampleDataD( InstruNo)->relNote, (Ptr) aStr);
			MyC2PStr(  (Ptr) aStr);
			pStrcat( StrTemp, aStr);
			
			SetDText( SampleDlog[ InstruNo], 33, StrTemp);
			
			// c4spd
			
			NumToString( SampleDataD( InstruNo)->c2spd, StrTemp);
			pStrcat( StrTemp, "\p Hz");
			
			SetDText( SampleDlog[ InstruNo], 32, StrTemp);
			
			GetSizeString( start, StrTemp, false);
			SetDText( SampleDlog[ InstruNo], 20, StrTemp);
			
			GetSizeString( start + sampleSize, StrTemp, false);
			SetDText( SampleDlog[ InstruNo], 22, StrTemp);
		break;
	}
		
	NumToString( ZoomLevel[ InstruNo], StrTemp);
	pStrcat( StrTemp, "\p x");
	SetDText( SampleDlog[ InstruNo], 24, StrTemp);
}

void UpdateDisplayQuality( short InstruNo)
{
	Str255	StrTemp;

	if( !SampleDlog[ InstruNo]) return;

	switch( curSample[ InstruNo])
	{
		case panningEnv:
		case volumeEnv:
			SetDText( SampleDlog[ InstruNo], 17, "\p-");
			SetDText( SampleDlog[ InstruNo], 17, "\p-");
		break;
		
		default:
			NumToString( SampleDataD( InstruNo)->amp, StrTemp);
			pStrcat( StrTemp, "\p bits");
			SetDText( SampleDlog[ InstruNo], 17, StrTemp);
		break;
	}
}

void UpdateDisplayPosition( short InstruNo)
{
	Str255	StrTemp, StrTemp2, dst;
	Point	myPt;
	long	val;
	char	dest[ 80];

	if( !SampleDlog[ InstruNo]) return;
	
	GetMouse( &myPt);
	
	switch( curSample[ InstruNo])
	{
		case panningEnv:
		case volumeEnv:
			NumToString( PosToByte( myPt.h, InstruNo), StrTemp);
			pStrcat( StrTemp, "\p/");
			NumToString( AmpToByte( myPt.v, InstruNo), StrTemp2);
			pStrcat( StrTemp, StrTemp2);
			
			SetDText( SampleDlog[ InstruNo], 38, StrTemp);
		break;
		
		default:
			GetSizeString( PosToByte( myPt.h, InstruNo), StrTemp, false);
			pStrcat( StrTemp, "\p/0x");
			
			val = (255UL * PosToByte( myPt.h, InstruNo)) / SampleDataD( InstruNo)->size;
			sprintf( dest, "%02X", val);
			MyC2PStr( dest);
			pStrcat( StrTemp, (unsigned char*) dest);
			
			pStrcat( StrTemp, "\p%/0x");
			val = PosToByte( myPt.h, InstruNo);
			val /= 256;
			if( SampleDataD( InstruNo)->amp == 16) val /= 2;
			if( SampleDataD( InstruNo)->stereo == true) val /= 2;
			sprintf( dest, "%02X", val);
			MyC2PStr( dest);
			pStrcat( StrTemp, (unsigned char*) dest);
			SetDText( SampleDlog[ InstruNo], 38, StrTemp);
		break;
	}
}

void UpdateDisplayLoop( short InstruNo)
{ 
	Str255	StrTemp;
	long 	temp;

	if( !SampleDlog[ InstruNo]) return;

	switch( curSample[ InstruNo])
	{
		case panningEnv:
			if( curMusic->fid[ InstruNo].pannType & EFLOOP)
			{
				NumToString( (long) curMusic->fid[ InstruNo].pannBeg+1, StrTemp);
				pStrcat( StrTemp, "\p Pt");
				SetDText( SampleDlog[ InstruNo], 9, StrTemp);
			
				NumToString( (long) curMusic->fid[ InstruNo].pannEnd+1, StrTemp);
				pStrcat( StrTemp, "\p Pt");
				SetDText( SampleDlog[ InstruNo], 11, StrTemp);
			}
			else
			{
				SetDText( SampleDlog[ InstruNo], 9, "\p-");
				SetDText( SampleDlog[ InstruNo], 11, "\p-");
			}
		break;
	
		case volumeEnv:
			if( curMusic->fid[ InstruNo].volType & EFLOOP)
			{
				NumToString( (long) curMusic->fid[ InstruNo].volBeg+1, StrTemp);
				pStrcat( StrTemp, "\p Pt");
				SetDText( SampleDlog[ InstruNo], 9, StrTemp);
			
				NumToString( (long) curMusic->fid[ InstruNo].volEnd+1, StrTemp);
				pStrcat( StrTemp, "\p Pt");
				SetDText( SampleDlog[ InstruNo], 11, StrTemp);
			}
			else
			{
				SetDText( SampleDlog[ InstruNo], 9, "\p-");
				SetDText( SampleDlog[ InstruNo], 11, "\p-");
			}
		break;
		
		default:
			temp = SampleDataD( InstruNo)->loopBeg;
			GetSizeString( temp, StrTemp, false);
			SetDText( SampleDlog[ InstruNo], 9, StrTemp);
			
			temp += SampleDataD( InstruNo)->loopSize;
			GetSizeString( temp, StrTemp, false);
			SetDText( SampleDlog[ InstruNo], 11, StrTemp);
		break;
	}
}

long	FindByteStart( short InstruNo)
{
	long	tempL;
	
//	if( curSample[ InstruNo] < 0) return 0L;
	
	tempL = SelecRect[ InstruNo].start;
	
	switch( curSample[ InstruNo])
	{
		case panningEnv:
		
		break;
		
		case volumeEnv:
		
		break;
		
		default:
			if( SampleDataD( InstruNo)->stereo)
			{
				if( SampleDataD( InstruNo)->amp == 16)
				{
					tempL /= 4;		tempL *= 4;
					if( curMode[ InstruNo]) tempL += 2;
				}
				else
				{
					tempL /= 2;		tempL *= 2;
					if( curMode[ InstruNo]) tempL++;
				}
			}
		break;
	}
	
	return tempL;
}

short		ByteToPos( long bytePosi, short InstruNo)
{
	long long	bytePos, val, start, sampleSize;
	short	itemType, tempInt;
	Handle	itemHandle;
	Rect	itemRect;

	bytePos = bytePosi;

	val = GetCtlValue( xScroll[ InstruNo]);
	
	switch( curSample[ InstruNo])
	{
		case panningEnv:
			start = (val * (ENVSIZE / SAMPLEDEF)) / ZoomLevel[ InstruNo];
			sampleSize = ENVSIZE / ZoomLevel[ InstruNo];
		break;
		
		case volumeEnv:
			start = (val * (ENVSIZE / SAMPLEDEF)) / ZoomLevel[ InstruNo];
			sampleSize = ENVSIZE / ZoomLevel[ InstruNo];
		break;
		
		default:
			start = GetStartSampleValue( val, InstruNo);	//(val * (SampleDataD( InstruNo)->size / SAMPLEDEF)) / ZoomLevel[ InstruNo];
			sampleSize = SampleDataD( InstruNo)->size / ZoomLevel[ InstruNo];
		break;
	}
	
	bytePos -= start;
	bytePos *= (long long) SampleRect.right;
	
	if( sampleSize == 0) bytePos = 0;
	else bytePos /= sampleSize;
	
	if( bytePos > 30000L) tempInt = 30000L;
	else if( bytePos < -30000L) tempInt = -30000L;
	else tempInt = bytePos;
	
//	if( curSample[ InstruNo] >= 0) if( SampleDataD( InstruNo)->stereo) tempInt *= 2;
	
	return tempInt;
}

long AmpToByte( short Pos, short InstruNo)
{
	long	tempL, sampleSize;
	short	itemType;
	Handle	itemHandle;
	Rect	itemRect;
	
	switch( curSample[ InstruNo])
	{
		case panningEnv:
			sampleSize = 256;
		break;
		
		case volumeEnv:
			sampleSize = 256;
		break;
		
		default:
			sampleSize = 256;
		break;
	}
	
	tempL = Pos - SampleRect.top;
	
	tempL *= sampleSize;
	tempL /= SampleRect.bottom - SampleRect.top;
	
	if( tempL < 0) tempL = 0;
	if( tempL > 256) tempL = 256;
	
	switch( curSample[ InstruNo])
	{
		case panningEnv:
			tempL = 128 - tempL;
		break;
		
		case volumeEnv:
			tempL = 256 - tempL;
		break;
		
		default:
			if( SampleDataD( InstruNo)->amp == 16)
			{
				tempL -= 128;
				tempL *= 256;
			}
		break;
	}
	
	return (tempL);

}

long PosToByte( short Pos, short InstruNo)
{
	long long	tempL, start, val, sampleSize;
	short	itemType;
	Handle	itemHandle;
	Rect	itemRect;
	
	val = GetCtlValue( xScroll[ InstruNo]);
	
	switch( curSample[ InstruNo])
	{
		case panningEnv:
			start = (val * (ENVSIZE / SAMPLEDEF)) / ZoomLevel[ InstruNo];
			sampleSize = ENVSIZE / ZoomLevel[ InstruNo];
		break;
		
		case volumeEnv:
			start = (val * (ENVSIZE / SAMPLEDEF)) / ZoomLevel[ InstruNo];
			sampleSize = ENVSIZE / ZoomLevel[ InstruNo];
		break;
		
		default:
			start = GetStartSampleValue( val, InstruNo);	//(val * (SampleDataD( InstruNo)->size / SAMPLEDEF)) / ZoomLevel[ InstruNo];
			sampleSize = SampleDataD( InstruNo)->size / ZoomLevel[ InstruNo];
			
			if( val == GetCtlMax( xScroll[ InstruNo]))
			{
				//Debugger();
			}
		break;
	}
	
	tempL = Pos;
	
	tempL *= sampleSize;
	tempL /= SampleRect.right;
	
	tempL += start;
	
	switch( curSample[ InstruNo])
	{
		case panningEnv:
		break;
		
		case volumeEnv:
		break;
		
		default:
			if( SampleDataD( InstruNo)->amp == 16)
			{
				tempL /= 2;	tempL *= 2;
			}
		
			if( SampleDataD( InstruNo)->stereo)
			{
				if( SampleDataD( InstruNo)->amp == 16)
				{
					tempL /= 4;		tempL *= 4;
				//	if( curMode[ InstruNo]) tempL += 2;
				}
				else
				{
					tempL /= 2;		tempL *= 2;
				//	if( curMode[ InstruNo]) tempL++;
				}
			}
			
			if( tempL < 0 ) tempL = 0;
			if( tempL >= SampleDataD( InstruNo)->size) tempL = SampleDataD( InstruNo)->size;
		break;
	}
	
	return tempL;
}

void GetSampleRect( DialogPtr theDia)
{
	Rect caRect;
	
	if( theDia == 0L) MyDebugStr( __LINE__, __FILE__, "Big big ERROR");
	
	GetPortBounds( GetDialogPort( theDia), &caRect);
	
	SampleRect.top = SAMPLERECTTOP;
	SampleRect.bottom = caRect.bottom - 15;
	SampleRect.left = 0;
	SampleRect.right = caRect.right;
}

long	FindByteEnd( short InstruNo)
{
	long	tempL;
	
//	if( curSample[ InstruNo] < 0) return 0L;
	
	tempL = SelecRect[ InstruNo].end;
	
	switch( curSample[ InstruNo])
	{
		case panningEnv:
		
		break;
		
		case volumeEnv:
		
		break;
		
		default:
			if( SampleDataD( InstruNo)->stereo)
			{
				if( SampleDataD( InstruNo)->amp == 16)
				{
					tempL /= 4;		tempL *= 4;
					if( curMode[ InstruNo]) tempL += 2;
				}
				else
				{
					tempL /= 2;		tempL *= 2;
					if( curMode[ InstruNo]) tempL++;
				}
			}
		break;
	}
	
	return tempL;
}

void Selection2Loop( short InstruNo)
{
InstrData	*curIns = &curMusic->fid[ InstruNo];

	switch( curSample[ InstruNo])
	{
		case panningEnv:
			if( Clignote[ InstruNo])
			{
				if( curMusic->fid[ InstruNo].pannType & EFLOOP) curMusic->fid[ InstruNo].pannType -= EFLOOP;
			
				curIns->pannBeg = 0;
				curIns->pannEnd = 0;
			}
			else
			{
				short i;
				
				if( !(curMusic->fid[ InstruNo].pannType & EFLOOP)) curMusic->fid[ InstruNo].pannType += EFLOOP;
				
				for( i = 0; i < curIns->pannSize; i++)
				{
					if( FindByteStart( InstruNo) <= curIns->pannEnv[ i].pos) break;
				}
				if( i >= curIns->pannSize) i = curIns->pannSize-1;
				curIns->pannBeg = i;
				
				for( i = 0; i < curIns->pannSize; i++)
				{
					if( FindByteEnd( InstruNo) <= curIns->pannEnv[ i].pos) break;
				}
				i--;
				if( i < 0) i = 0;
				curIns->pannEnd = i;
			}
		break;
	
		case volumeEnv:
			if( Clignote[ InstruNo])
			{
				if( curMusic->fid[ InstruNo].volType & EFLOOP) curMusic->fid[ InstruNo].volType -= EFLOOP;
			
				curIns->volBeg = 0;
				curIns->volEnd = 0;
			}
			else
			{
				short i;
				
				if( !(curMusic->fid[ InstruNo].volType & EFLOOP)) curMusic->fid[ InstruNo].volType += EFLOOP;
				
				for( i = 0; i < curIns->volSize; i++)
				{
					if( FindByteStart( InstruNo) <= curIns->volEnv[ i].pos) break;
				}
				if( i >= curIns->volSize) i = curIns->volSize-1;
				curIns->volBeg = i;
				
				for( i = 0; i < curIns->volSize; i++)
				{
					if( FindByteEnd( InstruNo) <= curIns->volEnv[ i].pos) break;
				}
				i--;
				if( i < 0) i = 0;
				curIns->volEnd = i;
			}
		break;
		
		default:
			if( Clignote[ InstruNo])
			{
				SampleDataD( InstruNo)->loopBeg = 0;
				SampleDataD( InstruNo)->loopSize = 0;
			}
			else
			{
				SampleDataD( InstruNo)->loopBeg = FindByteStart( InstruNo);
				SampleDataD( InstruNo)->loopSize = (FindByteEnd( InstruNo) - FindByteStart( InstruNo));
				
				if( SampleDataD( InstruNo)->stereo)
				{
					if( SampleDataD( InstruNo)->amp == 8)
					{
						SampleDataD( InstruNo)->loopBeg 	/= 2;		SampleDataD( InstruNo)->loopBeg 	*= 2;
						SampleDataD( InstruNo)->loopSize	/= 2;		SampleDataD( InstruNo)->loopSize	*= 2;
					}
					else
					{
						SampleDataD( InstruNo)->loopBeg 	/= 4;		SampleDataD( InstruNo)->loopBeg 	*= 4;
						SampleDataD( InstruNo)->loopSize	/= 4;		SampleDataD( InstruNo)->loopSize	*= 4;
					}
				}
			}
		break;
	}
	
	InternalUpdate( InstruNo);
}

void SetSustainPoint( short InstruNo)
{
InstrData	*curIns = &curMusic->fid[ InstruNo];

	switch( curSample[ InstruNo])
	{
		case panningEnv:
			if( Clignote[ InstruNo])
			{
				if( curMusic->fid[ InstruNo].pannType & EFSUSTAIN) curMusic->fid[ InstruNo].pannType -= EFSUSTAIN;
			
				curIns->pannSus = 0;
			}
			else
			{
				short i;
				
				if( !(curMusic->fid[ InstruNo].pannType & EFSUSTAIN)) curMusic->fid[ InstruNo].pannType += EFSUSTAIN;
				
				for( i = 0; i < curIns->pannSize; i++)
				{
					if( FindByteStart( InstruNo) <= curIns->pannEnv[ i].pos) break;
				}
				if( i >= curIns->pannSize) i = curIns->pannSize-1;
				curIns->pannSus = i;
			}
		break;
		
		case volumeEnv:
			if( Clignote[ InstruNo])
			{
				if( curMusic->fid[ InstruNo].volType & EFSUSTAIN) curMusic->fid[ InstruNo].volType -= EFSUSTAIN;
			
				curIns->volSus = 0;
			}
			else
			{
				short i;
				
				if( !(curMusic->fid[ InstruNo].volType & EFSUSTAIN)) curMusic->fid[ InstruNo].volType += EFSUSTAIN;
				
				for( i = 0; i < curIns->volSize; i++)
				{
					if( FindByteStart( InstruNo) <= curIns->volEnv[ i].pos) break;
				}
				if( i >= curIns->volSize) i = curIns->volSize-1;
				curIns->volSus = i;
			}
		break;
		
		default:
		break;
	}
	
	InternalUpdate( InstruNo);
}

void Loop2Selection( short InstruNo)
{
	switch( curSample[ InstruNo])
	{
		case panningEnv:
			if( !(curMusic->fid[ InstruNo].pannType & EFLOOP))
			{
				return;
			}
			else
			{
				InstrData	*curIns = &curMusic->fid[ InstruNo];
				
				SelecRect[ InstruNo].start	= curIns->pannEnv[ curIns->pannBeg].pos;
				SelecRect[ InstruNo].end	= curIns->pannEnv[ curIns->pannEnd].pos+2;
				
				Clignote[ InstruNo] = false;
			}
		break;
	
		case volumeEnv:
			if( !(curMusic->fid[ InstruNo].volType & EFLOOP))
			{
				return;
			}
			else
			{
				InstrData	*curIns = &curMusic->fid[ InstruNo];
				
				SelecRect[ InstruNo].start	= curIns->volEnv[ curIns->volBeg].pos;
				SelecRect[ InstruNo].end	= curIns->volEnv[ curIns->volEnd].pos+2;
				
				Clignote[ InstruNo] = false;
			}
		break;
		
		default:
			if( SampleDataD( InstruNo)->loopSize == 0)
			{
				return;
			}
			else
			{
				SetSelection( InstruNo, SampleDataD( InstruNo)->loopBeg, SampleDataD( InstruNo)->loopBeg + SampleDataD( InstruNo)->loopSize);
				
				Clignote[ InstruNo] = false;
			}
		break;
	}
	
	InternalUpdate( InstruNo);
}

static	long	oldByteStart, oldByteEnd;

void UpdateDisplaySelec( short InstruNo)
{ 
	Str255	StrTemp;
	long	newVal;

	if( !SampleDlog[ InstruNo]) return;

	newVal = FindByteStart( InstruNo);
	if( newVal != oldByteStart)
	{
		GetSizeString( newVal, StrTemp, false);
		SetDText( SampleDlog[ InstruNo], 5, StrTemp);
		oldByteStart = newVal;
	}
	
	newVal = FindByteEnd( InstruNo);
	if( newVal != oldByteEnd)
	{
		GetSizeString( newVal, StrTemp, false);
		SetDText( SampleDlog[ InstruNo], 7, StrTemp);
		oldByteEnd = newVal;
	}
}

void COPYSampleInt( DialogPtr	theDia)
{
short		CurWin;
long		Start, End;
OSErr		anErr;
ScrapRef	scrap;

	CurWin = FindSample( theDia);

	Start = FindByteStart( CurWin);
	End = FindByteEnd( CurWin);

	switch( curSample[ CurWin])
	{
		case panningEnv:
			if( Start != End)
			{
				InstrData		*curIns = &curMusic->fid[ CurWin];
				short			i;
				InstrData		copyEnv;
				
				copyEnv.pannSize = 0;
				copyEnv.numSamples = End - Start;
				
				for( i = 0; i < curIns->pannSize; i++)
				{
					if( curIns->pannEnv[ i].pos >= Start && curIns->pannEnv[ i].pos <= End)
					{
						copyEnv.pannEnv[ copyEnv.pannSize] = curIns->pannEnv[ i];
						copyEnv.pannEnv[ copyEnv.pannSize].pos -= Start;
						
						copyEnv.pannSize++;
					}
				}
				#if MACOS9VERSION
				ZeroScrap();
				PutScrap( sizeof( InstrData), 'pEnv', (Ptr) &copyEnv);
				#else
				anErr = ClearCurrentScrap();
				
				anErr = GetCurrentScrap( &scrap);
				
				anErr = PutScrapFlavor( scrap, 'pEnv', 0, sizeof( InstrData), (Ptr) &copyEnv);
				
				#endif
			}
		break;
		
		case volumeEnv:
			if( Start != End)
			{
				InstrData		*curIns = &curMusic->fid[ CurWin];
				short			i;
				InstrData		copyEnv;
				
				copyEnv.volSize = 0;
				copyEnv.numSamples = End - Start;
				
				for( i = 0; i < curIns->volSize; i++)
				{
					if( curIns->volEnv[ i].pos >= Start && curIns->volEnv[ i].pos <= End)
					{
						copyEnv.volEnv[ copyEnv.volSize] = curIns->volEnv[ i];
						copyEnv.volEnv[ copyEnv.volSize].pos -= Start;
						
						copyEnv.volSize++;
					}
				}
				
				#if MACOS9VERSION
				ZeroScrap();
				PutScrap( sizeof( InstrData), 'vEnv', (Ptr) &copyEnv);
				#else
				anErr = ClearCurrentScrap();
				
				anErr = GetCurrentScrap( &scrap);
				
				anErr = PutScrapFlavor( scrap, 'vEnv', 0, sizeof( InstrData), (Ptr) &copyEnv);
				
				#endif
			}
		break;
		
		default:
			NCOPYSample( Start, End - Start, CurWin, curSample[ CurWin]);
		break;
	}
}

void PASTESampleInt( DialogPtr	theDia)
{
short				CurWin;
long				Start;
ScrapRef			scrap;
ScrapFlavorFlags	flags;
OSErr				anErr;

	CurWin = FindSample( theDia);
	Start = FindByteStart( CurWin);
	
	SaveUndo( USample, CurWin, "\pUndo 'Paste instrument'");
	
	DoKeyPressSample( theDia, (short) 0x0008);

	switch( curSample[ CurWin])
	{
		case panningEnv:
			{
				long scrapOffset, lCntOrErr;
				
				#if MACOS9VERSION
				lCntOrErr = GetScrap( 0L, 'pEnv', &scrapOffset);
				#else
	
				lCntOrErr = 0;
				anErr = GetCurrentScrap( &scrap);
				anErr = GetScrapFlavorFlags( scrap, 'pEnv', &flags);
				if( anErr == noErr) GetScrapFlavorSize( scrap, 'pEnv', &lCntOrErr);
				
				#endif

				
				if( lCntOrErr > 0)
				{
					InstrData		*curIns = &curMusic->fid[ CurWin];
					short			i;
					InstrData		pasteEnv;
					Handle			tempH;
					short			whichPt;
					
					// GetScrap
					
					tempH = MyNewHandle( lCntOrErr);
					
					#if MACOS9VERSION
					GetScrap( tempH, 'pEnv', &scrapOffset);
					#else
					
					HLock( tempH);
					GetScrapFlavorData( scrap, 'pEnv', &lCntOrErr, *tempH);
					HUnlock( tempH);
					
					#endif

					
					HLock( tempH);
					BlockMoveData( *tempH, &pasteEnv, sizeof( InstrData));
					HUnlock( tempH);
					DisposeHandle( tempH);
					
					// Make room on existing volume envelope 
					whichPt = curIns->pannSize;
					for( i = curIns->pannSize-1; i >= 0 ; i--)
					{
						if( curIns->pannEnv[ i].pos >= Start)
						{
							whichPt = i;
							
							if( i + pasteEnv.pannSize < 12)
							{
								curIns->pannEnv[ i + pasteEnv.pannSize].pos = curIns->pannEnv[ i].pos + pasteEnv.numSamples;
								if( curIns->pannEnv[ i + pasteEnv.pannSize].pos >= ENVSIZE) curIns->pannEnv[ i + pasteEnv.pannSize].pos = ENVSIZE-1;
								
								curIns->pannEnv[ i + pasteEnv.pannSize].val = curIns->pannEnv[ i].val;
							}
						}
					}
					
					curIns->pannSize += pasteEnv.pannSize;
					if( curIns->pannSize > 12)
					{
						curIns->pannSize = 12;
						Erreur( 79, -98);
					}
					// Paste volume envelope
					
					for( i = 0; i < pasteEnv.pannSize; i++)
					{
						curIns->pannEnv[ i + whichPt].pos = pasteEnv.pannEnv[ i].pos + Start;
						if( curIns->pannEnv[ i + whichPt].pos >= ENVSIZE) curIns->pannEnv[ i + whichPt].pos = ENVSIZE-1;
						
						curIns->pannEnv[ i + whichPt].val = pasteEnv.pannEnv[ i].val;
					}
					
					SelecRect[ CurWin].start	= Start;
					SelecRect[ CurWin].end		= Start + pasteEnv.numSamples;
					if( SelecRect[ CurWin].end > ENVSIZE) SelecRect[ CurWin].end = ENVSIZE;
					
					Clignote[ CurWin]			= false;
					
					InternalUpdate( CurWin);
				}
			}
		break;
	
		case volumeEnv:
			{
				long scrapOffset, lCntOrErr;
				
				#if MACOS9VERSION
				lCntOrErr = GetScrap( 0L, 'vEnv', &scrapOffset);
				#else
				lCntOrErr = 0;
				anErr = GetCurrentScrap( &scrap);
				anErr = GetScrapFlavorFlags( scrap, 'vEnv', &flags);
				if( anErr == noErr) GetScrapFlavorSize( scrap, 'vEnv', &lCntOrErr);
				#endif
				
				if( lCntOrErr > 0)
				{
					InstrData		*curIns = &curMusic->fid[ CurWin];
					short			i;
					InstrData		pasteEnv;
					Handle			tempH;
					short			whichPt;
					
					// GetScrap
					
					tempH = MyNewHandle( lCntOrErr);
					
					#if MACOS9VERSION
					GetScrap( tempH, 'vEnv', &scrapOffset);
					#else
					HLock( tempH);
					GetScrapFlavorData( scrap, 'vEnv', &lCntOrErr, *tempH);
					HUnlock( tempH);
					#endif
					
					HLock( tempH);
					BlockMoveData( *tempH, &pasteEnv, sizeof( InstrData));
					HUnlock( tempH);
					DisposeHandle( tempH);
					
					// Make room on existing volume envelope 
					whichPt = curIns->volSize;
					for( i = curIns->volSize-1; i >= 0 ; i--)
					{
						if( curIns->volEnv[ i].pos >= Start)
						{
							whichPt = i;
							
							if( i + pasteEnv.volSize < 12)
							{
								curIns->volEnv[ i + pasteEnv.volSize].pos = curIns->volEnv[ i].pos + pasteEnv.numSamples;
								if( curIns->volEnv[ i + pasteEnv.volSize].pos >= ENVSIZE) curIns->volEnv[ i + pasteEnv.volSize].pos = ENVSIZE-1;
								
								curIns->volEnv[ i + pasteEnv.volSize].val = curIns->volEnv[ i].val;
							}
						}
					}
					
					curIns->volSize += pasteEnv.volSize;
					if( curIns->volSize > 12)
					{
						curIns->volSize = 12;
						Erreur( 79, -98);
					}
					// Paste volume envelope
					
					for( i = 0; i < pasteEnv.volSize; i++)
					{
						curIns->volEnv[ i + whichPt].pos = pasteEnv.volEnv[ i].pos + Start;
						if( curIns->volEnv[ i + whichPt].pos >= ENVSIZE) curIns->volEnv[ i + whichPt].pos = ENVSIZE-1;
						
						curIns->volEnv[ i + whichPt].val = pasteEnv.volEnv[ i].val;
					}
					
					SelecRect[ CurWin].start	= Start;
					SelecRect[ CurWin].end		= Start + pasteEnv.numSamples;
					if( SelecRect[ CurWin].end > ENVSIZE) SelecRect[ CurWin].end = ENVSIZE;
					
					Clignote[ CurWin]			= false;
					
					InternalUpdate( CurWin);
				}
			}
		break;
		
		default:
			NPASTESample( Start, CurWin, curSample[ CurWin]);
		break;
	}
}

void AfficheOldPoint(register	short	i)
{
	register	short	 theInstru;
	Rect		UptRect, selRect;
	GrafPtr		SavePort;
	long		SS, EE;
	
	theInstru = theIdle[i].oldInstru;
	if( theInstru == -1) return;
	
	theIdle[i].oldInstru = -1;
		
	GetPort( &SavePort);
	if( SampleDlog[ theInstru] == 0L) return;
	
 	SetPortDialogPort( SampleDlog[ theInstru]);
 	GetSampleRect( SampleDlog[ theInstru]);
	
	UptRect.left = theIdle[i].oldPos;
	UptRect.right = UptRect.left+2;
	UptRect.top = SampleRect.top;
	UptRect.bottom = SampleRect.bottom;
	
	BackColor( whiteColor);
	if( BitMapSample[ theInstru])
	{
		CopyBits(		(BitMap*) *GetPortPixMap( BitMapSample[ theInstru]),
	 					(BitMap*) *GetPortPixMap( GetDialogPort( SampleDlog[ theInstru])),
	 					&UptRect,
	 					&UptRect,
	 					srcCopy,
	 					0L);
 	}
 	
	if( (**(**GetMainDevice()).gdPMap).pixelSize == 1) BackColor( whiteColor);
	else RGBBackColor( &theColor);
	
	SS = ByteToPos( SelecRect[ theInstru].start, theInstru);
	EE = ByteToPos( SelecRect[ theInstru].end, theInstru);
	
	selRect.top = SampleRect.top;
	selRect.bottom = SampleRect.bottom;
	
	if( Clignote[ theInstru] == true)
	{
		if( TokTak[ theInstru] == true)
		{
			selRect.left = SS;
			selRect.right = SS+1;
		}
		else
		{
			selRect.left = selRect.right = 0;		// DO NOTHING !
		}
	}
	else
	{
		selRect.left = SS;
		selRect.right = EE;
	}
	
	if( SectRect( &UptRect, &selRect, &selRect))
	{
		InvertRect( &selRect);
	}
	
/*	if( UptRect.right >= SS &&
		UptRect.left <= EE)
		{
			if( UptRect.left < SS) UptRect.left = SS;
			if( UptRect.right > EE) UptRect.right = EE;
			InvertRect( &UptRect);
		}
	else if( Clignote[ theInstru] == true && TokTak[ theInstru] == true)
	{
		if( UptRect.left >= SS &&
			UptRect.left <= EE)
			{
				UptRect.left = SS;
				UptRect.right = SS+1;
				InvertRect( &UptRect);
			}
	}*/
				
 	SetPort( SavePort);
}

static	long	oldNullTicks, Flashing;


void DoNullInstrument(void)
{
GrafPtr		SavePort;
long		maxChannels, maxVal, curVal;
short		i, InstruNo;
Rect		caRect, tempRect;
Point		pt;
RgnHandle	visibleRegion;


	if( TickCount() <= oldNullTicks) return;
	oldNullTicks = TickCount();

	GetPort( &SavePort);
	
	if( oldWindow)
	{
	if( GetWRefCon( oldWindow) == RefSample && gNowMovingLoop == false)
	{
		SetPortWindowPort( oldWindow);
		
		pt = theEvent.where;
		GlobalToLocal( &pt);
		
		visibleRegion = NewRgn();
		
		GetPortVisibleRegion( GetWindowPort( oldWindow), visibleRegion);
		
		if( PtInRgn( pt, visibleRegion))
		{
			GetPortBounds( GetWindowPort( oldWindow), &caRect);
			
			SampleRect.top = SAMPLERECTTOP;		SampleRect.bottom = caRect.bottom - 15;
			SampleRect.left = 0;				SampleRect.right = caRect.right;
			
			if( PtInRect( pt, &SampleRect))
			{
				InstruNo = FindSample( GetDialogFromWindow( oldWindow));
				
				UpdateDisplayPosition( InstruNo);
				
				GetKeys( km);
				if( IsPressed( 0x003A) && ZoomLevel[ InstruNo] != 1)
				{
					SetCursor( &ZoomOutCrsr);
				}
				else if( IsPressed( 0x0037) && ZoomLevel[ InstruNo] < 1024)
				{
					SetCursor( &ZoomInCrsr);
				}
				else
				{
					switch( curSample[ InstruNo])
					{
						case panningEnv:
							if( SelectMode[ InstruNo] == ePencil)
							{
								InstrData	*curIns = &curMusic->fid[ InstruNo];
								short		tcoc = PosToByte( pt.h, InstruNo);
								
								for( i = 0; i < curIns->pannSize; i++)
								{
									if( tcoc >= curIns->pannEnv[ i].pos - 1 && tcoc <= curIns->pannEnv[ i].pos + 1)
									{
										SetCursor( &HandCrsr);
										goto END;
									}
								}
							}
						break;
						
						case volumeEnv:
							if( SelectMode[ InstruNo] == ePencil)
							{
								InstrData	*curIns = &curMusic->fid[ InstruNo];
								short		tcoc = PosToByte( pt.h, InstruNo);
								
								for( i = 0; i < curIns->volSize; i++)
								{
									if( tcoc >= curIns->volEnv[ i].pos - 1 && tcoc <= curIns->volEnv[ i].pos + 1)
									{
										SetCursor( &HandCrsr);
										goto END;
									}
								}
							}
						break;
						
						default:
							if( SelectMode[ InstruNo] == eSelect)
							{
								InstrData	*curIns = &curMusic->fid[ InstruNo];
								long		start = ByteToPos( SampleDataD( InstruNo)->loopBeg, InstruNo);
								long		end = ByteToPos( SampleDataD( InstruNo)->loopBeg + SampleDataD( InstruNo)->loopSize, InstruNo);
								
								if( pt.h >= start-1 && pt.h <= start+1)
								{
									SetCursor( &HandCrsr);
									goto END;
								}
								
								if( pt.h >= end-1 && pt.h <= end+1)
								{
									SetCursor( &HandCrsr);
									goto END;
								}
							}
						
							if( SampleDataD( InstruNo)->size <= 0)
							{
								SetCursor( GetQDGlobalsArrow( &qdarrow));
								goto END;
							}
						break;
					}
					
					if( SelectMode[ InstruNo] == eSelect) SetCursor( &beamCrsr);
					else if( SelectMode[ InstruNo] == ePencil)
					{
						if( curSample[ InstruNo] >= 0 && IsPressed( 0x38) && SampleDataD( InstruNo)->stereo)
						{
							SetCursor( &pencilCrsrStereo);
						}
						else SetCursor( &pencilCrsr);
					}
					else if( SelectMode[ InstruNo] == eZoom) SetCursor( &ZoomInCrsr);
					
					END:;
				}
				
			}
			else SetCursor( GetQDGlobalsArrow( &qdarrow));
		}
		else SetCursor( GetQDGlobalsArrow( &qdarrow));
		
		DisposeRgn( visibleRegion);
	}
	}
	
	Flashing--;
	if( Flashing <= 0)
	{
		Flashing = 15;
		for(i=0; i<MAXINSTRU; i++)
		{
			if( SampleDlog[ i] != 0L)
			{
				SetPortDialogPort( SampleDlog[ i]);
				
				if( Clignote[ i])
				{
					if( TokTak[ i] == true || oldWindow == GetDialogWindow( SampleDlog[ i]))
					{
						TokTak[ i] = !TokTak[ i];
						GetSampleRect( SampleDlog[ i]);
						
						tempRect.left = ByteToPos( SelecRect[ i].start, i);
						tempRect.right = tempRect.left +1;
						tempRect.top = SampleRect.top;
						tempRect.bottom = SampleRect.bottom;
						InvertRect( &tempRect);
					}
				}
			}
		}
	}
	
	maxChannels = MADDriver->MultiChanNo;
	if( MADDriver->DriverSettings.numChn > maxChannels) maxChannels = MADDriver->DriverSettings.numChn;
	
	for(i=0; i<maxChannels; i++)
	{
		AfficheOldPoint( i);
		
		InstruNo = MADDriver->chan[i].ins;
		
		if( InstruNo >= 0 || InstruNo <= MAXINSTRU)
		{
			if( SampleDlog[ InstruNo] != 0L)
			{
				switch( curSample[ InstruNo])
				{
					case panningEnv:
						if( curMusic->fid[ MADDriver->chan[i].ins].pannType  & EFON)
						{
					 		SetPortDialogPort( SampleDlog[ InstruNo]);
					 		GetSampleRect( SampleDlog[ InstruNo]);
					 		
					 		if( MADDriver->chan[ i].curPtr < MADDriver->chan[ i].maxPtr)
					 		{
						 		if( MADDriver->chan[ i].pp > 0 && MADDriver->chan[ i].pp < ENVSIZE)
						 		{
						 			SwitchColor( MADDriver->chan[ i].TrackID);
						 			
						 			theIdle[i].oldPos = ByteToPos( MADDriver->chan[ i].pp, InstruNo);
						 			theIdle[i].oldInstru = InstruNo;
						 			PenSize( 2, 1);
						 			MoveTo( theIdle[i].oldPos ,SampleRect.top);
						 			LineTo( theIdle[i].oldPos ,SampleRect.bottom-1);
						 			PenSize( 1, 1);
						 			
						 			ForeColor( blackColor);
						 		}
					 		}
					 	}
					break;
				
					case volumeEnv:
						if( curMusic->fid[ MADDriver->chan[i].ins].volType  & EFON)
						{
					 		SetPortDialogPort( SampleDlog[ InstruNo]);
					 		GetSampleRect( SampleDlog[ InstruNo]);
					 		
					 		if( MADDriver->chan[ i].curPtr < MADDriver->chan[ i].maxPtr)
					 		{
						 		if( MADDriver->chan[ i].p > 0 && MADDriver->chan[ i].p < ENVSIZE)
						 		{
						 			SwitchColor( MADDriver->chan[ i].TrackID);
						 			
						 			theIdle[i].oldPos = ByteToPos( MADDriver->chan[ i].p, InstruNo);
						 			theIdle[i].oldInstru = InstruNo;
						 			PenSize( 2, 1);
						 			MoveTo( theIdle[i].oldPos ,SampleRect.top);
						 			LineTo( theIdle[i].oldPos ,SampleRect.bottom-1);
						 			PenSize( 1, 1);
						 			
						 			ForeColor( blackColor);
						 		}
					 		}
					 	}
					break;
					
					default:
						if( curSample[ InstruNo] == MADDriver->chan[i].samp)
						{
					 		SetPortDialogPort( SampleDlog[ InstruNo]);
					 		GetSampleRect( SampleDlog[ InstruNo]);
					 		
					 		maxVal = (long) MADDriver->chan[ i].maxPtr;
					 		maxVal -=(long) SampleDataD( InstruNo)->data;
					 		
					  		curVal = (long) MADDriver->chan[ i].curPtr;
					 		curVal -= (long) SampleDataD( InstruNo)->data;
					 		
					 		if( maxVal > curVal && curVal > 0 && maxVal > 0)
					 		{
					 			SwitchColor( MADDriver->chan[ i].TrackID);
					 			
					 			theIdle[i].oldPos = ByteToPos( curVal, InstruNo);
					 			theIdle[i].oldInstru = InstruNo;
					 			PenSize( 2, 1);
					 			MoveTo( theIdle[i].oldPos ,SampleRect.top);
					 			LineTo( theIdle[i].oldPos ,SampleRect.bottom-1);
					 			PenSize( 1, 1);
					 			
					 			ForeColor( blackColor);
					 		}
				 		}
					break;
				}
			}
		}
	}
		
	SetPort( SavePort);
}

short FindSample( DialogPtr	theDia)
{
short	i;

	for(i=0;i < MAXINSTRU; i++)
	{
		if( SampleDlog[i] == theDia) return( i);
	}
	
	MyDebugStr( __LINE__, __FILE__, "FIND Sample ERROR");
	
	return 0;
}

void DrawLoop( short ins)
{
	long	StartLoop;
	long	SizeLoop, endLine, temp;
	
	if( curSample[ ins] >= 0)
	{
		GetSampleRect( SampleDlog[ ins]);
		
		ForeColor( redColor);
		
		StartLoop = (long) SampleDataD( ins)->loopBeg;
		SizeLoop = (long) SampleDataD( ins)->loopSize;
		endLine = ByteToPos( StartLoop + SizeLoop, ins);
		
		if( endLine > SampleRect.right) endLine = SampleRect.right + 2;
		
		PenSize( 2,2);
		
		if( SizeLoop > 2)
		{
			temp = ByteToPos( StartLoop, ins);
			if( temp < SampleRect.left) temp = SampleRect.left - 2;
	
			MoveTo( temp, SampleRect.top);
			LineTo( temp, SampleRect.bottom-2);
			
			LineTo( endLine-2, SampleRect.bottom-2);
			LineTo( endLine-2, SampleRect.top);
			
			LineTo( temp, SampleRect.top);
			
			if( SampleDataD( ins)->loopType == ePingPongLoop)
			{
				MoveTo( temp + 2, SampleRect.top + 13);
				TextSize( 0);	TextFont( 0);
				DrawString( "\pPingPong");
				TextFont( 4);		TextSize( 9);
			}
		}
		
		PenSize( 1,1);
		
		ForeColor( blackColor);
	}
}

void UpdateSampleWindow( DialogPtr	GetSelection)
{
		Rect   		caRect, tempRect;
 		GrafPtr		SavePort;
 		short		theInstru;
 		Handle		itemHandle;
 		Rect		itemRect;
 		short		itemType;
 		Str255		str, str2;
 		sData		*curData;
 		RgnHandle	saveClipRgn;
 		RgnHandle	visibleRegion;
 		
 		if( GetSelection == 0L) return;
 		
 		GetPort( &SavePort);
 		SetPortDialogPort( GetSelection);
 
 		TextFont( 4);	TextSize( 9);
 		
 		BeginUpdate( GetDialogWindow( GetSelection));
 		
 		visibleRegion = NewRgn();
		
		GetPortVisibleRegion( GetDialogPort( GetSelection), visibleRegion);
 		
 		UpdtDialog( GetSelection, visibleRegion);
 		
 		DisposeRgn( visibleRegion);
 		
 		theInstru = FindSample( GetSelection);
 		BackColor( whiteColor);
 		
 		if( BitMapSample[ theInstru])
		{
	  		CopyBits(	(BitMap*) *GetPortPixMap(BitMapSample[ theInstru]),
	 					(BitMap*) *GetPortPixMap(GetDialogPort( GetSelection)),
	 					&(*GetPortPixMap(BitMapSample[ theInstru]))->bounds,
	 					&(*GetPortPixMap(BitMapSample[ theInstru]))->bounds,
	 					srcCopy,
	 					0L);
 		}
 		
 		if( (**(**GetMainDevice()).gdPMap).pixelSize == 1) BackColor( whiteColor);
		else RGBBackColor( &theColor);
		
		GetPortBounds( GetDialogPort( GetSelection), &caRect);
		
 		MoveTo( 0, SampleRect.top - 1);
 		LineTo( caRect.right, SampleRect.top - 1);
 		
 		if( Clignote[ theInstru])
 		{
 			if( TokTak[ theInstru])
 			{
 				GetSampleRect( GetSelection);
 			
 				tempRect.left = ByteToPos( SelecRect[ theInstru].start, theInstru);
				tempRect.right = tempRect.left +1;
				tempRect.top = SampleRect.top;
				tempRect.bottom = SampleRect.bottom;
				
				InvertRect( &tempRect);
 			}
 		}
 		else
 		{
 			GetSampleRect( GetSelection);
 			
 			tempRect.left = ByteToPos( SelecRect[ theInstru].start, theInstru);
			tempRect.right = ByteToPos( SelecRect[ theInstru].end, theInstru);
			tempRect.top = SampleRect.top;
			tempRect.bottom = SampleRect.bottom;
			
		//	BackColor( blackColor);
		//	LMSetHiliteMode( LMGetHiliteMode() & hiliteBit);
			InvertRect( &tempRect);
		//	RGBBackColor( &theColor);
		}
		
		DrawGrowIconP( SampleDlog[ theInstru]);
		
		GetDialogItem( SampleDlog[ theInstru] , 30, &itemType, &itemHandle, &itemRect);
		itemRect.right = caRect.right;
		switch( curSample[ theInstru])
		{
			case panningEnv:
				pStrcpy( str, "\pPanning Envelope");
			break;
			
			case volumeEnv:
				pStrcpy( str, "\pVolume Envelope");
			break;
			
			default:
				curData = curMusic->sample[ curMusic->fid[ theInstru].firstSample + curSample[ theInstru]];
			
				strcpy( (Ptr) str, curData->name);
				
				MyC2PStr( (Ptr) str);
				
				if( str[ 0] == 0)
				{
					Str255	str2;
					
					pStrcpy( str, "\puntitled sample");
				}
				if( str[ 0] > 27) str[ 0] = 27;
				pStrcat( str, "\p #");
				
				NumToString( curSample[ theInstru] + 1, str2);
				pStrcat( str, str2);
				
				if( curData->stereo)
				{
					if( curMode[ theInstru] == 0) pStrcat( str, "\p Left");
					else pStrcat( str, "\p Right");
				}
			break;
		}
		
		saveClipRgn = NewRgn();
		GetClip( saveClipRgn );

		ClipRect( &itemRect);

		TextFace( bold);
	//	if( StringWidth( str) < (itemRect.right - itemRect.left)/2) itemRect.left = 50;
		itemRect.right -= 10;
		TETextBox( str+1, str[ 0], &itemRect, teJustRight);
		TextFace( 0);
		
		SetClip( saveClipRgn);
		DisposeRgn( saveClipRgn);
		
		EndUpdate( GetDialogWindow( GetSelection));
		
		SetPort(SavePort);
}


extern	short			theDepth;

void DrawPencil( DialogPtr	theDia, short InstruNo)
{
	Point			aPt, prevPt, tempPt;
	long long			BS, BE, sampleSize, x, start, nVal16E, nVal16S;
	Ptr				theSample;
	short			*theShortSample, itemType;
	short			nVal8S, nVal8E, val;
	Rect			itemRect, tRect, tempRect;
	Handle			itemHandle;
	RgnHandle		saveClipRgn;
	Boolean			Inverse, firstTime;
	GDHandle		oldGDeviceH;
	CGrafPtr		oldPort;
	GWorldPtr		theGWorld = 0L;
	PixMapHandle	tempPM;
	short 			editPtEnv;
	Boolean			PreInOutPt;

	curMusic->hasChanged = true;
	SaveUndo( USample, InstruNo, "\pUndo 'Draw Instrument'");

	saveClipRgn = NewRgn();								/* get an empty region */
	GetClip( saveClipRgn );								/* save current */

	GetSampleRect( theDia);
	
	val = GetCtlValue( xScroll[ InstruNo]);
	
	GetMouse( &prevPt);
	if( prevPt.v < SampleRect.top) prevPt.v = SampleRect.top;
	else if( prevPt.v > SampleRect.bottom) prevPt.v = SampleRect.bottom;
	
	if( prevPt.h < SampleRect.left) prevPt.h = SampleRect.left;
	else if( prevPt.h > SampleRect.right) prevPt.h = SampleRect.right;

	firstTime = true;
	
	if( TokTak[ InstruNo])
	{
		Flashing = 0;
		DoNullInstrument();
	}
	
	switch( curSample[ InstruNo])
	{
		case panningEnv:
		case volumeEnv:
			if( ZoomLevel[ InstruNo] == 1)
			{
				start = 0;
				sampleSize = ENVSIZE;
			}
			else
			{
				start = (val * (ENVSIZE / SAMPLEDEF)) / ZoomLevel[ InstruNo];
				sampleSize = ENVSIZE / ZoomLevel[ InstruNo];
			}
			
			editPtEnv = -1;
		break;
		
		default:
			theSample = SampleDataD( InstruNo)->data;
			theShortSample = (short*) theSample;
			
			if( ZoomLevel[ InstruNo] == 1)
			{
				start = 0;
				sampleSize = SampleDataD( InstruNo)->size;
			}
			else
			{
				start = GetStartSampleValue( val, InstruNo);	//(val * (SampleDataD( InstruNo)->size / SAMPLEDEF)) / ZoomLevel[ InstruNo];
				sampleSize = SampleDataD( InstruNo)->size / ZoomLevel[ InstruNo];
			}
		break;
	}

	do
	{
		/**/
		WaitNextEvent( everyEvent, &theEvent, 1, 0L);
	
		//	if (QDIsPortBuffered( GetDialogPort( theDia)))
    	//				QDFlushPortBuffer( GetDialogPort( theDia), NULL);
	
		Boolean		InOutPt;
	
		PreInOutPt = InOutPt;
	
		GetMouse( &aPt);
		if( aPt.v < SampleRect.top) { aPt.v = SampleRect.top;			InOutPt = true;}
		else if( aPt.v > SampleRect.bottom) { aPt.v = SampleRect.bottom;InOutPt = true;}
		else InOutPt = false;
		
		if( InOutPt != PreInOutPt) firstTime = true;
		
		/**/
		
		if( aPt.h < SampleRect.left) aPt.h = SampleRect.left;
		else if( aPt.h > SampleRect.right) aPt.h = SampleRect.right;

		if( prevPt.h != aPt.h || prevPt.v != aPt.v || firstTime == true)
		{
			firstTime = false;
			
			UpdateDisplayPosition( InstruNo);
			
			switch( curSample[ InstruNo])
			{
				case panningEnv:
				{
					
					short	i;
					short 	tcoc;
					
					InstrData	*curIns = &curMusic->fid[ InstruNo];
					
				//	aPt.h = prevPt.h;
					tcoc = PosToByte( aPt.h, InstruNo);
					
					if( InOutPt)
					{
						if( editPtEnv != -1)
						{
							for( i = 0; i < curIns->pannSize; i++)
							{
								if( i == editPtEnv)
								{
									short ww;
									
									for( ww = i; ww < curIns->pannSize-1; ww++)
									{
										curIns->pannEnv[ ww] = curIns->pannEnv[ ww+1];
									}
									
									curIns->pannSize--;
									
									i = curIns->pannSize;
									
									if( curIns->pannBeg >= editPtEnv && curIns->pannBeg > 0) curIns->pannBeg--;
									if( curIns->pannEnd >= editPtEnv && curIns->pannEnd > 0) curIns->pannEnd--;
									if( curIns->pannSus >= editPtEnv && curIns->pannSus > 0) curIns->pannSus--;
								}
							}
							
							if( curIns->pannSize <= 0)
							{
								curIns->pannSize = 0;
								if( curIns->pannType & EFON) curIns->pannType -= EFON;
							}
							
							editPtEnv = -1;
							
							/************/
							
							GetGWorld( &oldPort, &oldGDeviceH);
						//	theGWorld = 0L;
						//	NewGWorld( &theGWorld, 8, &SampleRect, nil, nil, (GWorldFlags) 0);
						//	if( theGWorld)
						//	{
						//		LockPixels( GetPortPixMap( theGWorld));
								
						//		tempPM = GetPortPixMap( theGWorld);
						//		theGWorld->portPixMap = BitMapSample[ InstruNo];
								
								SetGWorld( BitMapSample[ InstruNo], 0L);
								
								DrawSample( -1, -1, InstruNo, SampleDlog[ InstruNo]);
								
						//		theGWorld->portPixMap = tempPM;
								
								SetGWorld( oldPort, oldGDeviceH);
						//	 	UnlockPixels( GetPortPixMap( theGWorld));
						//		DisposeGWorld( theGWorld);
						//	}
							/************/
							
							InvalWindowRect( GetDialogWindow( theDia), &SampleRect);
							UpdateSampleWindow( theDia);
							
							prevPt = aPt;
						}
					}
					else
					{
						if( editPtEnv == -1)
						{
							for( i = 0; i < curIns->pannSize; i++)
							{
								if( tcoc >= curIns->pannEnv[ i].pos - 1 && tcoc <= curIns->pannEnv[ i].pos + 1)
								{
									editPtEnv = i;
									SetCursor( &CHandCrsr);
									break;
								}
								else if( tcoc < curIns->pannEnv[ i].pos)
								{
									if( curIns->pannSize < 12)
									{
										short ww;
										// déplace les pts après ce pt
										
										for( ww = 11; ww >= i; ww--) curIns->pannEnv[ ww] = curIns->pannEnv[ ww-1];
										
										curIns->pannSize++;
										
										editPtEnv = i;
										curIns->pannEnv[ editPtEnv].pos = tcoc;
										
										if( curIns->pannBeg >= editPtEnv) curIns->pannBeg++;
										if( curIns->pannEnd >= editPtEnv) curIns->pannEnd++;
										if( curIns->pannSus >= editPtEnv) curIns->pannSus++;
										
										break;
									}
								}
							}
							
							if( editPtEnv == -1)		// Le Pt est derriere le dernier pt pann ( curIns->pannSize)
							{
								if( curIns->pannSize < 12)
								{
									editPtEnv = curIns->pannSize;
									curIns->pannEnv[ editPtEnv].pos = tcoc;
									
									curIns->pannSize++;
								}
							}
						}
						
						if( editPtEnv != -1)
						{
							if( editPtEnv > 0)
							{
								if( tcoc < curIns->pannEnv[ editPtEnv-1].pos)
								{
									curIns->pannEnv[ editPtEnv].val = curIns->pannEnv[ editPtEnv-1].val;
									curIns->pannEnv[ editPtEnv].pos = curIns->pannEnv[ editPtEnv-1].pos;
									editPtEnv--;
								}
							}
							
							if( editPtEnv < curIns->pannSize-1)
							{
								if( tcoc > curIns->pannEnv[ editPtEnv+1].pos)
								{
									curIns->pannEnv[ editPtEnv].val = curIns->pannEnv[ editPtEnv+1].val;
									curIns->pannEnv[ editPtEnv].pos = curIns->pannEnv[ editPtEnv+1].pos;
									editPtEnv++;
								}
							}
							
							curIns->pannEnv[ editPtEnv].pos = tcoc;
							curIns->pannEnv[ editPtEnv].val = 64 - ((aPt.v - SampleRect.top) * 64) / (SampleRect.bottom - SampleRect.top);
							
							/************/
							
							GetGWorld( &oldPort, &oldGDeviceH);
						/*	theGWorld = 0L;
							NewGWorld( &theGWorld, 8, &SampleRect, nil, nil, (GWorldFlags) 0);
							if( theGWorld)
							{
								LockPixels( GetPortPixMap( theGWorld));
								
								tempPM = GetPortPixMap( theGWorld);
								theGWorld->portPixMap = BitMapSample[ InstruNo];
						*/
								SetGWorld( BitMapSample[ InstruNo], 0L);
								
								DrawSample( -1, -1, InstruNo, SampleDlog[ InstruNo]);
								
						//		theGWorld->portPixMap = tempPM;
								
								SetGWorld( oldPort, oldGDeviceH);
						/*	 	UnlockPixels( GetPortPixMap( theGWorld));
								DisposeGWorld( theGWorld);
							}*/
							/************/
							
							InvalWindowRect( GetDialogWindow( theDia), &SampleRect);
							UpdateSampleWindow( theDia);
							
							prevPt = aPt;
						}
						else Erreur( 79, -98);
					}
				}
				break;
			
				case volumeEnv:
				{
					
					short	i;
					short 	tcoc;
					
					InstrData	*curIns = &curMusic->fid[ InstruNo];
					
				//	aPt.h = prevPt.h;
					tcoc = PosToByte( aPt.h, InstruNo);
					
					if( InOutPt)
					{
						if( editPtEnv != -1)
						{
							for( i = 0; i < curIns->volSize; i++)
							{
								if( i == editPtEnv)
								{
									short ww;
									
									for( ww = i; ww < curIns->volSize-1; ww++)
									{
										curIns->volEnv[ ww] = curIns->volEnv[ ww+1];
									}
									
									curIns->volSize--;
									
									i = curIns->volSize;
									
									if( curIns->volBeg >= editPtEnv && curIns->volBeg > 0) curIns->volBeg--;
									if( curIns->volEnd >= editPtEnv && curIns->volEnd > 0) curIns->volEnd--;
									if( curIns->volSus >= editPtEnv && curIns->volSus > 0) curIns->volSus--;
								}
							}
							
							if( curIns->volSize <= 0)
							{
								curIns->volSize = 0;
								if( curIns->volType & EFON) curIns->volType -= EFON;
							}
							
							editPtEnv = -1;
							
							/************/
							
							GetGWorld( &oldPort, &oldGDeviceH);
						/*	theGWorld = 0L;
							NewGWorld( &theGWorld, 8, &SampleRect, nil, nil, (GWorldFlags) 0);
							if( theGWorld)
							{
								LockPixels( GetPortPixMap( theGWorld));
								
								tempPM = GetPortPixMap( theGWorld);
								theGWorld->portPixMap = BitMapSample[ InstruNo];
						*/
								SetGWorld( BitMapSample[ InstruNo], 0L);
								
								DrawSample( -1, -1, InstruNo, SampleDlog[ InstruNo]);
								
						//		theGWorld->portPixMap = tempPM;
								
								SetGWorld( oldPort, oldGDeviceH);
						/*	 	UnlockPixels( GetPortPixMap( theGWorld));
								DisposeGWorld( theGWorld);
							}*/
							/************/
	
							InvalWindowRect( GetDialogWindow( theDia), &SampleRect);
							UpdateSampleWindow( theDia);
							
							prevPt = aPt;
						}
					}
					else
					{
						if( editPtEnv == -1)
						{
							for( i = 0; i < curIns->volSize; i++)
							{
								if( tcoc >= curIns->volEnv[ i].pos - 1 && tcoc <= curIns->volEnv[ i].pos + 1)
								{
									editPtEnv = i;
									SetCursor( &CHandCrsr);
									break;
								}
								else if( tcoc < curIns->volEnv[ i].pos)
								{
									if( curIns->volSize < 12)
									{
										short ww;
										// déplace les pts après ce pt
										
										for( ww = 11; ww >= i; ww--) curIns->volEnv[ ww] = curIns->volEnv[ ww-1];
										
										curIns->volSize++;
										
										editPtEnv = i;
										curIns->volEnv[ editPtEnv].pos = tcoc;
										
										if( curIns->volBeg >= editPtEnv) curIns->volBeg++;
										if( curIns->volEnd >= editPtEnv) curIns->volEnd++;
										if( curIns->volSus >= editPtEnv) curIns->volSus++;
										
										break;
									}
								}
							}
							
							if( editPtEnv == -1)		// Le Pt est derriere le dernier pt vol ( curIns->volSize)
							{
								if( curIns->volSize < 12)
								{
									editPtEnv = curIns->volSize;
									curIns->volEnv[ editPtEnv].pos = tcoc;
									
									curIns->volSize++;
								}
							}
						}
						
						if( editPtEnv != -1)
						{
							if( editPtEnv > 0)
							{
								if( tcoc < curIns->volEnv[ editPtEnv-1].pos)
								{
									curIns->volEnv[ editPtEnv].val = curIns->volEnv[ editPtEnv-1].val;
									curIns->volEnv[ editPtEnv].pos = curIns->volEnv[ editPtEnv-1].pos;
									editPtEnv--;
								}
							}
							
							if( editPtEnv < curIns->volSize-1)
							{
								if( tcoc > curIns->volEnv[ editPtEnv+1].pos)
								{
									curIns->volEnv[ editPtEnv].val = curIns->volEnv[ editPtEnv+1].val;
									curIns->volEnv[ editPtEnv].pos = curIns->volEnv[ editPtEnv+1].pos;
									editPtEnv++;
								}
							}
							
							curIns->volEnv[ editPtEnv].pos = tcoc;
							curIns->volEnv[ editPtEnv].val = 64 - ((aPt.v - SampleRect.top) * 64) / (SampleRect.bottom - SampleRect.top);
							
							/************/
							
							GetGWorld( &oldPort, &oldGDeviceH);
						/*	theGWorld = 0L;
							NewGWorld( &theGWorld, 8, &SampleRect, nil, nil, (GWorldFlags) 0);
							if( theGWorld)
							{
								LockPixels( GetPortPixMap( theGWorld));
								
								tempPM = GetPortPixMap( theGWorld);
								theGWorld->portPixMap = BitMapSample[ InstruNo];
						*/
								SetGWorld( BitMapSample[ InstruNo], 0L);
								
								DrawSample( -1, -1, InstruNo, SampleDlog[ InstruNo]);
								
						//		theGWorld->portPixMap = tempPM;
								
								SetGWorld( oldPort, oldGDeviceH);
						/*	 	UnlockPixels( GetPortPixMap( theGWorld));
								DisposeGWorld( theGWorld);
							}*/
							/************/
							
							InvalWindowRect( GetDialogWindow( theDia), &SampleRect);
							UpdateSampleWindow( theDia);
							
							prevPt = aPt;
						}
						else Erreur( 79, -98);
					}
				}
				break;
				
				default:
					/** Conversion X - Y **/
					
					if( prevPt.h < aPt.h)
					{
						tempPt = prevPt;
						prevPt = aPt;
						aPt = tempPt;
						Inverse = true;
					}
					else Inverse = false;
					
					BS = start + (aPt.h * sampleSize) / SampleRect.right;
					BE = start + ((prevPt.h) * sampleSize) / SampleRect.right;
					
					if( BS == BE)
					{
						if( SampleDataD( InstruNo)->stereo) BE = BS + 2;
						else BE = BS + 1;
					}
					
					if( BS < 0) BS = 0;
					if( BE > SampleDataD( InstruNo)->size) BE = SampleDataD( InstruNo)->size;
					
					if( SampleDataD( InstruNo)->amp == 16)
					{
						nVal16S = (( (long) (aPt.v - SampleRect.top) * 65535L) / (long) (SampleRect.bottom - SampleRect.top));
						nVal16E = (( (long) (prevPt.v - SampleRect.top) * 65535L) / (long) (SampleRect.bottom - SampleRect.top));
						
						BS /= 2;
						BE /= 2;
						
						if( BS == BE)
						{
							if( SampleDataD( InstruNo)->stereo) BE = BS + 2;
							else BE = BS + 1;
						}
						
						if( BE > SampleDataD( InstruNo)->size/2) BE = SampleDataD( InstruNo)->size/2;		// Where are in 16 bits!
						
						if( SampleDataD( InstruNo)->stereo)
						{
							BS /= 2;		BS *= 2;
							BE /= 2;		BE *= 2;
							if( !IsPressed( 0x38)) if( curMode[ InstruNo]) BS++;
							if( !IsPressed( 0x38)) if( curMode[ InstruNo]) BE++;
							
							for( x = BS; x < BE; x++)
							{
								val = (long) nVal16S +  ( (long) (x - BS) * (long) ( nVal16E - nVal16S)/ (long) (BE - BS));
								theShortSample[ x] = val - 0x8000;
								
								x++;
								
								if( IsPressed( 0x38)) theShortSample[ x] = val - 0x8000;
							}
						}
						else
						{
							for( x = BS; x < BE; x++)
							{
								val = (long) nVal16S +  ( (long) (x - BS) * (long) ( nVal16E - nVal16S)/ (long) (BE - BS));
								theShortSample[ x] = val - 0x8000;
							}
						}
					}
					else
					{
						nVal8S = (((long) (aPt.v - SampleRect.top) * 255L) / (long) (SampleRect.bottom - SampleRect.top));
						nVal8E = (((long) (prevPt.v - SampleRect.top) * 255L) / (long) (SampleRect.bottom - SampleRect.top));
						
						if( BE > SampleDataD( InstruNo)->size) BE = SampleDataD( InstruNo)->size;
						
						if( SampleDataD( InstruNo)->stereo)
						{
							BS /= 2;		BS *= 2;
							BE /= 2;		BE *= 2;
							if( !IsPressed( 0x38)) if( curMode[ InstruNo]) BS++;
							if( !IsPressed( 0x38)) if( curMode[ InstruNo]) BE++;
							
							for( x = BS; x < BE; x++)
							{
								val = (long) nVal8S +  ( (long) (x - BS) * (long) ( nVal8E - nVal8S)/ (long) (BE - BS));
								theSample[ x] = val - 0x80;
								
								x++;
								
								if( IsPressed( 0x38)) theSample[ x] = val - 0x80;
							}
						}
						else
						{
							for( x = BS; x < BE; x++)
							{
								val = (long) nVal8S +  ( (long) (x - BS) * (long) ( nVal8E - nVal8S)/ (long) (BE - BS));
								theSample[ x] = val - 0x80;
							}
						}
					}
					
					if( BS != BE)
					{
						SetRect( &tRect, aPt.h, SampleRect.top, prevPt.h, SampleRect.bottom);
						
						x = SampleRect.right;
						
						if( SampleDataD( InstruNo)->amp == 16) x *=2;
						if( SampleDataD( InstruNo)->stereo) x *= 3;
						
						tRect.left -= (x / sampleSize) + 2;
						tRect.right += (x / sampleSize) + 2;
						
						ClipRect( &tRect);
						
						DrawSample( tRect.left - 4, tRect.right + 4, InstruNo, theDia);
						
						if( Clignote[ InstruNo])
				 		{
				 			if( TokTak[ InstruNo])
				 			{
				 				tempRect.left = ByteToPos( SelecRect[ InstruNo].start, InstruNo);
								tempRect.right = tempRect.left +1;
								tempRect.top = SampleRect.top;
								tempRect.bottom = SampleRect.bottom;
								
								InvertRect( &tempRect);
				 			}
				 		}
				 		else
				 		{
				 			tempRect.left = ByteToPos( SelecRect[ InstruNo].start, InstruNo);
							tempRect.right = ByteToPos( SelecRect[ InstruNo].end, InstruNo);
							tempRect.top = SampleRect.top;
							tempRect.bottom = SampleRect.bottom;
								
							InvertRect( &tempRect);
						}
					}
					if( !Inverse) prevPt = aPt;
				break;
			}
		}
		
	}while( Button());
	
	SetClip( saveClipRgn);
	DisposeRgn( saveClipRgn);
	
	RGBBackColor( &theColor);
	
	InternalUpdate( InstruNo);
}

void DrawEnveloppe( short ins, DialogPtr	theDia)
{
Str255		aStr;
short		itemType;
short		i, high;
InstrData	*curIns = &curMusic->fid[ ins];

	BackColor( whiteColor);
	ForeColor( blackColor);
	
	GetSampleRect( theDia);
	
	//PaintRect( &SampleRect);
	
	high = SampleRect.bottom - SampleRect.top;
	
	// Envelope
	
	RGBForeColor( &theColor);
	for( i = 0; i < curIns->volSize; i++)
	{
		MoveTo( ByteToPos( curIns->volEnv[ i].pos, ins), SampleRect.top);
		LineTo( ByteToPos( curIns->volEnv[ i].pos, ins), SampleRect.bottom);
	}
	
	PenSize( 2, 2);
	ForeColor( yellowColor);
	if( curIns->volSize > 0)
	{
		MoveTo( ByteToPos( 0, ins), SampleRect.top + ((64 - curIns->volEnv[ 0].val) * high) / 64);
		for( i = 0; i < curIns->volSize; i++)
		{
			LineTo( ByteToPos( curIns->volEnv[ i].pos, ins), SampleRect.top + ((64 - curIns->volEnv[ i].val) * high) / 64);
		}
		LineTo( SampleRect.right, SampleRect.top + ((64 - curIns->volEnv[ curIns->volSize-1].val) * high) / 64);
	}
	
	// Sustain Pt
	if( curIns->volSize > 0)
	{
		PenSize( 2, 2);
		if( (curIns->volType & EFSUSTAIN))
		{
			ForeColor( greenColor);
		
			MoveTo( ByteToPos( curIns->volEnv[ curIns->volSus].pos, ins), SampleRect.top);
			LineTo( ByteToPos( curIns->volEnv[ curIns->volSus].pos, ins), SampleRect.bottom);
		}
	}
	// Envelope Loop
	if( curIns->volSize > 0)
	{
		if( (curIns->volType & EFLOOP))
		{
			ForeColor( redColor);
			
			MoveTo( ByteToPos( curIns->volEnv[ curIns->volBeg].pos, ins)-2, SampleRect.top);
			LineTo( ByteToPos( curIns->volEnv[ curIns->volBeg].pos, ins)-2 , SampleRect.bottom-2);
				
			LineTo( ByteToPos( curIns->volEnv[ curIns->volEnd].pos, ins)+2, SampleRect.bottom-2);
			LineTo( ByteToPos( curIns->volEnv[ curIns->volEnd].pos, ins)+2, SampleRect.top);
				
			LineTo( ByteToPos( curIns->volEnv[ curIns->volBeg].pos, ins)-2, SampleRect.top);
		}
	}
	ForeColor( blackColor);
	PenSize( 1, 1);
	
	if( !(curIns->volType & EFON))
	{
		ForeColor( redColor);
	
		pStrcpy( aStr, "\pEnvelope NOT active!");
		itemType = StringWidth( aStr);
	
		MoveTo( SampleRect.left + (SampleRect.right - SampleRect.left)/2 - itemType/2,
				SampleRect.top + (SampleRect.bottom - SampleRect.top)/2);
				
		TextFont( 0);		TextSize( 0);
		DrawString( aStr);
		TextFont( 4);		TextSize( 9);
		ForeColor( blackColor);
	}
	
//	SetSelectionZero( ins);
}

void DrawPanningEnveloppe( short ins, DialogPtr	theDia)
{
Str255		aStr;
short		itemType;
short		i, high;
InstrData	*curIns = &curMusic->fid[ ins];

	BackColor( whiteColor);
	ForeColor( blackColor);
	
	GetSampleRect( theDia);
	//PaintRect( &SampleRect);
	
	high = SampleRect.bottom - SampleRect.top;
	
	// Right / Left
	
	ForeColor( greenColor);
	MoveTo( 0, SampleRect.top + high/2);
	LineTo( SampleRect.right, SampleRect.top + high/2);
	
	ForeColor( whiteColor);
	TextFont( 0);		TextSize( 0);
	pStrcpy( aStr, "\pRight");	MoveTo( SampleRect.left + 2, SampleRect.top + 13);		DrawString( aStr);
	pStrcpy( aStr, "\pLeft");	MoveTo( SampleRect.left + 2, SampleRect.bottom - 4);	DrawString( aStr);
	TextFont( 4);		TextSize( 9);
	
	// Envelope
	
	RGBForeColor( &theColor);
	for( i = 0; i < curIns->pannSize; i++)
	{
		MoveTo( ByteToPos( curIns->pannEnv[ i].pos, ins), SampleRect.top);
		LineTo( ByteToPos( curIns->pannEnv[ i].pos, ins), SampleRect.bottom);
	}
	
	PenSize( 2, 2);
	ForeColor( yellowColor);
	if( curIns->pannSize > 0)
	{
		MoveTo( ByteToPos( 0, ins), SampleRect.top + ((64 - curIns->pannEnv[ 0].val) * high) / 64);
		for( i = 0; i < curIns->pannSize; i++)
		{
			LineTo( ByteToPos( curIns->pannEnv[ i].pos, ins), SampleRect.top + ((64 - curIns->pannEnv[ i].val) * high) / 64);
		}
		LineTo( SampleRect.right, SampleRect.top + ((64 - curIns->pannEnv[ curIns->pannSize-1].val) * high) / 64);
	}
	
	// Sustain Pt
	if( curIns->pannSize > 0)
	{
		PenSize( 2, 2);
		if( (curIns->pannType & EFSUSTAIN))
		{
			ForeColor( greenColor);
		
			MoveTo( ByteToPos( curIns->pannEnv[ curIns->pannSus].pos, ins), SampleRect.top);
			LineTo( ByteToPos( curIns->pannEnv[ curIns->pannSus].pos, ins), SampleRect.bottom);
		}
	}
	// Envelope Loop
	if( curIns->pannSize > 0)
	{
		if( (curIns->pannType & EFLOOP))
		{
			ForeColor( redColor);
			
			MoveTo( ByteToPos( curIns->pannEnv[ curIns->pannBeg].pos, ins)-2, SampleRect.top);
			LineTo( ByteToPos( curIns->pannEnv[ curIns->pannBeg].pos, ins)-2 , SampleRect.bottom-2);
				
			LineTo( ByteToPos( curIns->pannEnv[ curIns->pannEnd].pos, ins)+2, SampleRect.bottom-2);
			LineTo( ByteToPos( curIns->pannEnv[ curIns->pannEnd].pos, ins)+2, SampleRect.top);
				
			LineTo( ByteToPos( curIns->pannEnv[ curIns->pannBeg].pos, ins)-2, SampleRect.top);
		}
	}
	ForeColor( blackColor);
	PenSize( 1, 1);
	
	if( !(curIns->pannType & EFON))
	{
		ForeColor( redColor);
	
		pStrcpy( aStr, "\pEnvelope NOT active!");
		itemType = StringWidth( aStr);
	
		MoveTo( SampleRect.left + (SampleRect.right - SampleRect.left)/2 - itemType/2,
				SampleRect.top + (SampleRect.bottom - SampleRect.top)/2);
				
		TextFont( 0);		TextSize( 0);
		DrawString( aStr);
		TextFont( 4);		TextSize( 9);
		ForeColor( blackColor);
	}
	
//	SetSelectionZero( ins);
}

void DrawSample( short	tSS, short tSE, short InstruNo, DialogPtr	theDia)
{
	Point			PP;
	long			sampleSize, x, temp, val, start, XPos, end, i, minY, maxY;
	long			BS, BE;
	short			trueV;
	Ptr				theSample;
	short			*theShortSample;
	short			itemType, high;
	Handle			itemHandle;
	Rect			itemRect;
	Str255			aStr;
	
	sData			*curData;
	short			samp;
	Boolean			grayDraw = false;
	
	ForeColor( blackColor);
	GetSampleRect( theDia);
	PaintRect( &SampleRect);
	
	if( curSample[ InstruNo] == volumeEnv)
	{
		grayDraw = true;
		//if( !(curMusic->fid[ InstruNo].volType & EFNOTE))
		{
			DrawEnveloppe( InstruNo, theDia);
			return;
		}
	}
	if( curSample[ InstruNo] == panningEnv)
	{
		grayDraw = true;
		//if( !(curMusic->fid[ InstruNo].pannType & EFNOTE))
		{
			DrawPanningEnveloppe( InstruNo, theDia);
			return;
		}
	}
	
	/**/
	
	BackColor( whiteColor);
	ForeColor( blackColor);
	

	if( tSS == -1 && tSE == -1)
	{
		tSS = 0;
		tSE = SampleRect.right;
	}
	if( tSS < 0) tSS = 0;
	if( tSE > SampleRect.right) tSE = SampleRect.right;
	
	PP.v = SampleRect.top;	PP.h = 0;
	trueV = PP.v;
	
	/**/
	
	samp = curSample[ InstruNo];
	if( samp < 0)	samp = 0;	// ENVELOPPES !!!!
	
	if( samp >= curMusic->fid[ InstruNo].numSamples) goto RENVOI;
	
	curData = curMusic->sample[ curMusic->fid[ InstruNo].firstSample +  samp];
	if( curData == 0L) MyDebugStr( __LINE__, __FILE__, "Samp ERR");
	theSample = curData->data;
	theShortSample = (short*) theSample;
	
	/********************/
	
	if( curData->size <= 0)
	{
		RENVOI:
		
		ForeColor( redColor);
		
		if( !grayDraw)
		{
			pStrcpy( aStr, "\pNo data in this sample");
			itemType = StringWidth( aStr);
		
			MoveTo( SampleRect.left + (SampleRect.right - SampleRect.left)/2 - itemType/2,
				SampleRect.top + (SampleRect.bottom - SampleRect.top)/2);
			DrawString( aStr);
			ForeColor( blackColor);
		
		SetSelectionZero( InstruNo);
		}
		goto ENDSAMPLE;
		
		return;
	}
	
	val = GetCtlValue( xScroll[ InstruNo]);
	
	if( ZoomLevel[ InstruNo] == 1)
	{
		start = 0;
		sampleSize = curMusic->sample[ curMusic->fid[ InstruNo].firstSample + samp]->size;
	}
	else
	{
		start = GetStartSampleValue( val, InstruNo);	//(val * (SampleDataD( InstruNo)->size / SAMPLEDEF)) / ZoomLevel[ InstruNo];
		sampleSize = curMusic->sample[ curMusic->fid[ InstruNo].firstSample + samp]->size / ZoomLevel[ InstruNo];

		end = start + sampleSize;
	}
	
	high = SampleRect.bottom - SampleRect.top;

	/***/ 	
	ForeColor( greenColor);
	MoveTo( 0, SampleRect.top + high/2);
	LineTo( SampleRect.right, SampleRect.top + high/2);

	MoveTo( 0, SampleRect.top + high/4);
	LineTo( SampleRect.right, SampleRect.top + high/4);

	MoveTo( 0, SampleRect.bottom - high/4);
	LineTo( SampleRect.right, SampleRect.bottom - high/4);
	
	MoveTo( SampleRect.right/2, SampleRect.top);
	LineTo( SampleRect.right/2, SampleRect.bottom);
	
	MoveTo( SampleRect.right/4, SampleRect.top);
	LineTo( SampleRect.right/4, SampleRect.bottom);

	MoveTo( SampleRect.right - SampleRect.right/4, SampleRect.top);
	LineTo( SampleRect.right - SampleRect.right/4, SampleRect.bottom);

	FrameRect( &SampleRect);

	/********************/
	
	if( curData->stereo)
	{
		ForeColor( blueColor);
		DrawSampleInt(	sampleSize,
						start,
						tSS,
						tSE,
						high,
						SampleRect.right,
						trueV,
						0,
						!curMode[ InstruNo],
						curData);
	}
	
	if( grayDraw) ForeColor( blueColor);//RGBForeColor( &blueColor);
	else ForeColor( whiteColor);
	DrawSampleInt(	sampleSize,
					start,
					tSS,
					tSE,
					high,
					SampleRect.right,
					trueV,
					0,
					curMode[ InstruNo],
					curData);
	
	/********************/
	
	DrawLoop( InstruNo);
	
	ENDSAMPLE:
	
	if( curSample[ InstruNo] == volumeEnv)
	{
		if( (curMusic->fid[ InstruNo].volType & EFNOTE)) DrawEnveloppe( InstruNo, theDia);
	}
	if( curSample[ InstruNo] == panningEnv)
	{
		if( (curMusic->fid[ InstruNo].pannType & EFNOTE)) DrawPanningEnveloppe( InstruNo, theDia);
	}
	
	ForeColor( blackColor);
}

void InitSampleWindow(void)
{
	short		i;
	GrafPtr		SavePort;

	GetPort( &SavePort);
 	 
 	Flashing = 200;
 	
 	gNowMovingLoop = false;
 	
 	SampleDlog[ 0] = GetNewDialog( 137, 0L, GetDialogWindow( ToolsDlog));
 	GetSampleRect( SampleDlog[ 0]);
 	DisposDialog( SampleDlog[ 0]);
 	
	for( i = 0; i < MAXINSTRU; i++)
	{
		Clignote[ i] = false;
		TokTak[ i] = false;
		
		SelecRect[ i].start	= 0;
		SelecRect[ i].end	= 0;
		BitMapSample[i]		= 0L;
		ZoomLevel[ i]		= 1;
		SampleDlog[ i]		= 0L;
		curSample[ i]		= volumeEnv;
	}
	
	SetPort( SavePort);
}

void SetControlH( short theInstru)
{
short	itemType;
Handle	itemHandle;
Rect	caRect, itemRect;

	GetPortBounds( GetDialogPort( SampleDlog[ theInstru]), &caRect);
	
	MyMoveControl( xScroll[ theInstru], -1, caRect.bottom-15);
	
	MySizeControl( xScroll[ theInstru], caRect.right -13, 16);
	
	if( ZoomLevel[ theInstru] != 1)
	{
		SetCtlMin( xScroll[ theInstru], 0);
		SetCtlMax( xScroll[ theInstru], ZoomLevel[ theInstru] * SAMPLEDEF - SAMPLEDEF);
		
		if( gUseControlSize) SetControlViewSize( xScroll[ theInstru], SAMPLEDEF);
	}
	else
	{
		SetCtlMin( xScroll[ theInstru], 0);
		SetCtlMax( xScroll[ theInstru], 0);
		SetCtlValue( xScroll[ theInstru], 0);
	}
}

void InternalUpdate( short InstruNo)
{
	Str255			String, mainStr;
	GrafPtr			SavePort;
	GDHandle		oldGDeviceH;
	OSErr			errCode;
	CGrafPtr		oldPort;
	GWorldPtr		theGWorld = 0L;
	PixMapHandle	tempPM;
	Handle			itemHandle;
	Rect			itemRect;
	short			itemType;
	
	GetPort( &SavePort);
	
	UpdateWaveInfo();
	
	if( SampleDlog[ InstruNo] != 0L && IsWindowVisible( GetDialogWindow( SampleDlog[ InstruNo])) == true)
	{
		SetPortDialogPort( SampleDlog[ InstruNo]);
		
		GetDialogItem( SampleDlog[ InstruNo] , 30, &itemType, &itemHandle, &itemRect);
		InvalWindowRect( GetDialogWindow( SampleDlog[ InstruNo]), &itemRect);
		
		GetSampleRect( SampleDlog[ InstruNo]);
		
		TextFont( 4);	TextSize( 9);
		
		recheck:
		switch( curSample[ InstruNo])
		{
			case panningEnv:
				if( SelecRect[ InstruNo].start > ENVSIZE || SelecRect[ InstruNo].end > ENVSIZE)
				{
					SelecRect[ InstruNo].start	= 0;
					SelecRect[ InstruNo].end	= 0;
					ZoomLevel[ InstruNo]		= 1;
				}
			//	HiliteControl( ZoomBut[ InstruNo], 0);
				
				ShowControl( FixedBut[ InstruNo]);
				ShowControl( EnvBut[ InstruNo]);
				ShowControl( SustainBut[ InstruNo]);
				ShowControl( LoopBut[ InstruNo]);
				
				if( !(curMusic->fid[ InstruNo].pannType & EFNOTE))		HiliteControl( FixedBut[ InstruNo], kControlButtonPart);
				else HiliteControl( FixedBut[ InstruNo], 0);
				
				if( curMusic->fid[ InstruNo].pannType & EFON)		HiliteControl( EnvBut[ InstruNo], kControlButtonPart);
				else HiliteControl( EnvBut[ InstruNo], 0);
				
				if( curMusic->fid[ InstruNo].pannType & EFSUSTAIN)	HiliteControl( SustainBut[ InstruNo], kControlButtonPart);
				else HiliteControl( SustainBut[ InstruNo], 0);
				
				if( curMusic->fid[ InstruNo].pannType & EFLOOP)	HiliteControl( LoopBut[ InstruNo], kControlButtonPart);
				else HiliteControl( LoopBut[ InstruNo], 0);
			break;
		
			case volumeEnv:
				if( SelecRect[ InstruNo].start > ENVSIZE || SelecRect[ InstruNo].end > ENVSIZE)
				{
					SelecRect[ InstruNo].start	= 0;
					SelecRect[ InstruNo].end	= 0;
					ZoomLevel[ InstruNo]		= 1;
				}
			//	HiliteControl( ZoomBut[ InstruNo], 0);
				
				ShowControl( FixedBut[ InstruNo]);
				ShowControl( EnvBut[ InstruNo]);
				ShowControl( SustainBut[ InstruNo]);
				ShowControl( LoopBut[ InstruNo]);
				
				if( !(curMusic->fid[ InstruNo].volType & EFNOTE))		HiliteControl( FixedBut[ InstruNo], kControlButtonPart);
				else HiliteControl( FixedBut[ InstruNo], 0);
				
				if( curMusic->fid[ InstruNo].volType & EFON)		HiliteControl( EnvBut[ InstruNo], kControlButtonPart);
				else HiliteControl( EnvBut[ InstruNo], 0);
				
				if( curMusic->fid[ InstruNo].volType & EFSUSTAIN)	HiliteControl( SustainBut[ InstruNo], kControlButtonPart);
				else HiliteControl( SustainBut[ InstruNo], 0);
				
				if( curMusic->fid[ InstruNo].volType & EFLOOP)	HiliteControl( LoopBut[ InstruNo], kControlButtonPart);
				else HiliteControl( LoopBut[ InstruNo], 0);
			break;
			
			default:
				HideControl( FixedBut[ InstruNo]);
				HideControl( EnvBut[ InstruNo]);
				HideControl( SustainBut[ InstruNo]);
				HideControl( LoopBut[ InstruNo]);
			
				if( curSample[ InstruNo] >= curMusic->fid[ InstruNo].numSamples)
				{
					curSample[ InstruNo] = volumeEnv;
					goto recheck;
				}
			
				if( SelecRect[ InstruNo].start > SampleDataD( InstruNo)->size || SelecRect[ InstruNo].end > SampleDataD( InstruNo)->size)
				{
					SelecRect[ InstruNo].start	= 0;
					SelecRect[ InstruNo].end	= 0;
					ZoomLevel[ InstruNo]		= 1;
				}
			//	HiliteControl( ZoomBut[ InstruNo], 0);
			break;
		}
		
		InvalWindowRect( GetDialogWindow( SampleDlog[ InstruNo]), &SampleRect);
		
		SetControlH( InstruNo);
		
		if( BitMapSample[ InstruNo] != 0L)
		{
			DisposeGWorld( BitMapSample[ InstruNo]);
			BitMapSample[ InstruNo] = 0L;
		}
		BitMapSample[ InstruNo] = 0L;
		if( NewGWorld( &BitMapSample[ InstruNo], 8, &SampleRect, nil, nil, (GWorldFlags) 0) == noErr)
	//	if( NewOffscreenPixMap( &BitMapSample[ InstruNo], &SampleRect) == noErr)
		{
			/************/
			
			GetGWorld( &oldPort, &oldGDeviceH);
		/*	theGWorld = 0L;
			NewGWorld( &theGWorld, 8, &SampleRect, nil, nil, (GWorldFlags) 0);
			if( theGWorld)
			{
				LockPixels( GetPortPixMap( theGWorld));
				
				tempPM = GetPortPixMap( theGWorld);
				theGWorld->portPixMap = BitMapSample[ InstruNo];
		*/
				SetGWorld( BitMapSample[ InstruNo], 0L);
				
				DrawSample( -1, -1, InstruNo, SampleDlog[ InstruNo]);
				
		//		theGWorld->portPixMap = tempPM;
				
				SetGWorld( oldPort, oldGDeviceH);
		/*	 	UnlockPixels( GetPortPixMap( theGWorld));
				DisposeGWorld( theGWorld);
			}*/
			/************/
		}
		else BitMapSample[ InstruNo] = 0L;
		
		UpdateDisplaySize( InstruNo);
		UpdateDisplayLoop( InstruNo);
		UpdateDisplaySelec( InstruNo);
		UpdateDisplayQuality( InstruNo);
		UpdateDisplay( InstruNo);
		
		NTStr( (short) 2, (short) (InstruNo+1), (Ptr) String);
		strcpy( (Ptr) mainStr, (Ptr) String);
		strcat( (Ptr) mainStr, " - ");
		GetInstruName( InstruNo, (Ptr) String);
		strcat( (Ptr) mainStr, (Ptr) String);
		MyC2PStr( (Ptr) mainStr);
		{
			WindowPtr	tempWin = GetDialogWindow( SampleDlog[ InstruNo]);
			Str255		winTitle;
				
			GetWTitle( tempWin, winTitle);
			
			if( !EqualString( mainStr, winTitle, true, true))	SetWTitle( tempWin, mainStr);
		}
		DrawInfoInstrument();
	}
	SetPort( SavePort);
}

void UpdateSampleWindows(void)
{
	short		i;
	
	SetCursor( &watchCrsr);
	
	for(i=0; i<MAXINSTRU; i++) InternalUpdate( i);
	
	if( thePrefs.ClassicalProjection) UpdateMozartInfo();
	
	SetCursor( GetQDGlobalsArrow( &qdarrow));
}

void NCreateSampleWindow( short ins, short samp)
{
	Rect		itemRect;
	Handle		itemHandle;
	short		itemType;
	Str255		String, mainStr;
	GrafPtr		SavePort;
//	CGrafPort	aNewPort;
	GDHandle		oldGDeviceH;
	OSErr			errCode;
	CGrafPtr		oldPort;
	GWorldPtr		theGWorld = 0L;
	PixMapHandle	tempPM;
	
	GetPort( &SavePort);

	curSample[ ins] = samp;
	curMode[ ins] = 0;
	
	if( SampleDlog[ ins] == 0L)
	{
		SampleDlog[ ins] = GetNewDialog( 137, 0L, GetDialogWindow( ToolsDlog));
		
		ChangeDialogFont( SampleDlog[ ins]);
		
		SelectMode[ ins] = eSelect;
	
		SetPortDialogPort( SampleDlog[ ins]);
		TextFont( 4);		TextSize( 9);
		
		AutoPosition( SampleDlog[ ins]);
	
		TokTak[ ins] 			= false;
		Clignote[ ins] 			= false;
		SelecRect[ ins].start	= 0;
		SelecRect[ ins].end		= 0;
		ZoomLevel[ ins] 		= 1;
		
		SetRect( &itemRect, -100, -100, -90, -90);
		xScroll[ ins] = NewControl( 	GetDialogWindow( SampleDlog[ ins]),
								&itemRect,
								"\p.",
								true,
								0,
								0,
								0,
								gScrollBarID,
								0);
								
		GetDialogItem( SampleDlog[ ins] , 40, &itemType, &itemHandle, &itemRect);
		//itemRect.right = itemRect.left;
		FixedBut[ ins] = NewControl( 	GetDialogWindow( SampleDlog[ ins]),
								&itemRect,
								"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							213,
							kControlBevelButtonNormalBevelProc,
							0);
		
		GetDialogItem( SampleDlog[ ins] , 13, &itemType, &itemHandle, &itemRect);
		//itemRect.right = itemRect.left;
		EnvBut[ ins] = NewControl( 	GetDialogWindow( SampleDlog[ ins]),
								&itemRect,
								"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							190,
							kControlBevelButtonNormalBevelProc,
							0);
		GetDialogItem( SampleDlog[ ins] , 36, &itemType, &itemHandle, &itemRect);
		//itemRect.right = itemRect.left;
		SustainBut[ ins] = NewControl( 	GetDialogWindow( SampleDlog[ ins]),
								&itemRect,
								"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							192,
							kControlBevelButtonNormalBevelProc,
							0);
		GetDialogItem( SampleDlog[ ins] , 37, &itemType, &itemHandle, &itemRect);
		//itemRect.right = itemRect.left;
		LoopBut[ ins] = NewControl( 	GetDialogWindow( SampleDlog[ ins]),
								&itemRect,
								"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							193,
							kControlBevelButtonNormalBevelProc,
							0);
		
		
		
		GetDialogItem( SampleDlog[ ins] , 25, &itemType, &itemHandle, &itemRect);
		InfoBut[ ins] = NewControl( 	GetDialogWindow( SampleDlog[ ins]),
								&itemRect,
								"\p",
								true,
								0,
								kControlContentIconSuiteRes,
								149,
								kControlBevelButtonNormalBevelProc,
								0);
								
								
		GetDialogItem( SampleDlog[ ins] , 26, &itemType, &itemHandle, &itemRect);
		PencilBut[ ins] = NewControl(	GetDialogWindow( SampleDlog[ ins]),
								&itemRect,
								"\p",
								true,
								0,
								kControlContentIconSuiteRes,
								175,
								kControlBevelButtonNormalBevelProc,
								0);
		GetDialogItem( SampleDlog[ ins] , 27, &itemType, &itemHandle, &itemRect);
		SelectBut[ ins] = NewControl(	GetDialogWindow( SampleDlog[ ins]),
								&itemRect,
								"\p",
								true,
								0,
								kControlContentIconSuiteRes,
								176,
								kControlBevelButtonNormalBevelProc,
								0);

		GetDialogItem( SampleDlog[ ins] , 18, &itemType, &itemHandle, &itemRect);
		ZoomBut[ ins] = NewControl(	GetDialogWindow( SampleDlog[ ins]),
								&itemRect,
								"\p",
								true,
								0,
								kControlContentIconSuiteRes,
								177,
								kControlBevelButtonNormalBevelProc,
								0);
		
		HiliteControl( SelectBut[ ins], kControlButtonPart);
		HiliteControl( PencilBut[ ins], 0);
		HiliteControl( ZoomBut[ ins], 0);

		HideControl( FixedBut[ ins]);
		HideControl( EnvBut[ ins]);
		HideControl( SustainBut[ ins]);
		HideControl( LoopBut[ ins]);

		if( DragManagerUse)
		{
			MyTrackingHandlerUPP 		= NewDragTrackingHandlerUPP( MyTrackingSample);
		//	MyReceiveDropHandlerUPP 	= NewDragReceiveHandlerUPP( MyReceiveDropHandler);
		//	mySendDataUPP 				= NewDragSendDataUPP( MySendSampleProc);
			
		//	InstallTrackingHandler( (DragTrackingHandlerUPP) MyTrackingHandlerUPP, SampleDlog[ ins], (void *) 0L);
		//	InstallReceiveHandler( (DragReceiveHandlerUPP) MyReceiveDropHandlerUPP, SampleDlog[ ins], (void *) 0L);
		}
	}
	
	SetPortDialogPort( SampleDlog[ ins]);
	ShowWindow( GetDialogWindow( SampleDlog[ ins]));
	SelectWindow2( GetDialogWindow( SampleDlog[ ins]));
	GetSampleRect( SampleDlog[ ins]);
//	SetMaxBottomWindow( 256 + SampleRect.top + 17 + SAMPLERECTTOP, SampleDlog[ ins]);
	SetMaxWindow( 0, 256 + 15 + SAMPLERECTTOP, SampleDlog[ ins]);
	
	InternalUpdate( ins);
	SetPort( SavePort);
}

Boolean	IsPressed(unsigned short k);
extern	KeyMap	km;

void CloseSampleWindow( DialogPtr	theDia)
{
	short	i;
	
	GetKeys( km);
	
	if( IsPressed( 0x3A) == true)
	{
		for( i = MAXINSTRU; i >= 0 ; i--)
		{
			if( SampleDlog[ i] != 0L)
			{
				if( DragManagerUse)
				{
				//	RemoveTrackingHandler( MyTrackingHandlerUPP, SampleDlog[ i]);
				//	RemoveReceiveHandler( MyReceiveDropHandlerUPP, SampleDlog[ i]);
				
					DisposeDragTrackingHandlerUPP( MyTrackingHandlerUPP);
				//	DisposeDragReceiveHandlerUPP( MyReceiveDropHandlerUPP);
				//	DisposeDragSendDataUPP( mySendDataUPP);
				}
				if( BitMapSample[ i] != 0L)
				{
					DisposeGWorld( BitMapSample[ i]);
					BitMapSample[ i] = 0L;
				}
				DisposDialog( SampleDlog[ i]);
				SampleDlog[ i] = 0L;
			}
		}
	}
	else
	{
		i = FindSample( theDia);
		
		if( DragManagerUse)
		{
		//	RemoveTrackingHandler( MyTrackingHandlerUPP, SampleDlog[ i]);
		//	RemoveReceiveHandler( MyReceiveDropHandlerUPP, SampleDlog[ i]);
		
			DisposeDragTrackingHandlerUPP( MyTrackingHandlerUPP);
		//	DisposeDragReceiveHandlerUPP( MyReceiveDropHandlerUPP);
		//	DisposeDragSendDataUPP( mySendDataUPP);
		}
		if( BitMapSample[ i] != 0L)
		{
			DisposeGWorld( BitMapSample[ i]);
			BitMapSample[ i] = 0L;
		}
		DisposDialog( SampleDlog[ i]);
		SampleDlog[ i] = 0L;
	}
	
	SetCursor( GetQDGlobalsArrow( &qdarrow));
}

extern 	EventRecord		theEvent;
static	long			doubleClick;
static	Point			lastPosition;

void NAppelPlug( short InstruNo, short samp, short whichPlug)
{
long	End, Start;
sData	*curData = curMusic->sample[ curMusic->fid[ InstruNo].firstSample +  samp];
short	StereoMode;

	Start 	= FindByteStart( InstruNo);
	End 	= FindByteEnd( InstruNo);

	if (Start == End)
	{
		Start	= 0;
		if( curData->stereo)
		{
			if( curMode[ InstruNo])
			{
				if( curData->amp == 16) Start = 2;
				else Start = 1;
			}
		}
		
		End		= curData->size;
	}
	
	if( curData->stereo)
	{
		if( IsPressed( 0x38) && curData->stereo) StereoMode = 0;
		else StereoMode = 1;
	}
	else StereoMode = 0;
	
	NCallPlugIns( 	whichPlug,
					curData,
					Start,
					End,
					StereoMode);
}

static	short 	currentInstru;

void SampleUpdateNow( void)
{
	InternalUpdate( currentInstru);
	UpdateSampleWindow( SampleDlog[ currentInstru]);
}

void NAppelVSTPlug( short InstruNo, short samp, short whichPlug)
{
long	End, Start;
sData	*curData = curMusic->sample[ curMusic->fid[ InstruNo].firstSample +  samp];
short	StereoMode;

	currentInstru = InstruNo;
	
	Start 	= FindByteStart( InstruNo);
	End 	= FindByteEnd( InstruNo);

	if (Start == End)
	{
		Start	= 0;
		if( curData->stereo)
		{
			if( curMode[ InstruNo])
			{
				if( curData->amp == 16) Start = 2;
				else Start = 1;
			}
		}
		
		End		= curData->size;
	}
	
	if( SampleDataD( InstruNo)->stereo)
	{
		if( IsPressed( 0x38) && SampleDataD( InstruNo)->stereo) StereoMode = 0;
		else StereoMode = 1;
	}
	else StereoMode = 0;
	
	SaveUndo( USample, InstruNo, "\pUndo 'VST Sound Effect'");
	
	VSTSampleEditor( 	whichPlug,
						curData,
						Start,
						End,
						StereoMode);
}

static	DialogPtr	theDialogControl;

pascal void actionProcSample(ControlHandle theControl, short ctlPart)
{
long			lRefCon;
short			CurWin, maxValue, minValue, curVal, sVal;
//CGrafPort		aNewPort;
GDHandle		oldGDeviceH;
CGrafPtr		oldPort;
GWorldPtr		theGWorld = 0L;
PixMapHandle	tempPM;

if( ctlPart <= 0) return;

lRefCon = GetCRefCon( theControl);
maxValue = GetCtlMax( theControl);
minValue = GetCtlMin( theControl);
curVal = sVal = GetCtlValue( theControl);
CurWin = FindSample( theDialogControl);

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
			curVal -= SAMPLEDEF;
			if( curVal < minValue) curVal = minValue;
		break;
		
		case kControlPageDownPart:
			curVal += SAMPLEDEF;
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
		/************/
		GetGWorld( &oldPort, &oldGDeviceH);
	/*	NewGWorld( &theGWorld, 8, &SampleRect, nil, nil, (GWorldFlags) 0);
		if( theGWorld)
		{
			LockPixels( GetPortPixMap( theGWorld));
			
			tempPM = GetPortPixMap( theGWorld);
			theGWorld->portPixMap = BitMapSample[ CurWin];
	*/
			SetGWorld( BitMapSample[ CurWin], 0L);
		
			DrawSample( -1, -1, CurWin, SampleDlog[ CurWin]);
		
		//	theGWorld->portPixMap = tempPM;
		
			SetGWorld( oldPort, oldGDeviceH);
		 /*	UnlockPixels( GetPortPixMap( theGWorld));
			DisposeGWorld( theGWorld);
		}*/
		/************/
		
		if( BitMapSample[ CurWin]) InvalWindowRect( GetDialogWindow( SampleDlog[ CurWin]), &(*GetPortPixMap( BitMapSample[ CurWin]))->bounds);
		
		UpdateSampleWindow( theDialogControl);
		
		UpdateDisplay( CurWin);
	}
}

void MoveLoopSelection( short InstruNo)
{
	Boolean		firstTime = true;
	Point		aPt, prevPt;
	Boolean		StartPoint;
	long		start;
	long		end;
	long		byte, i;
	Boolean		TokTakCopy;
	
	GetMouse( &aPt);
	byte = PosToByte( aPt.h, InstruNo);
	
	if( byte < SampleDataD( InstruNo)->loopBeg + SampleDataD( InstruNo)->loopSize/2) StartPoint = true;
	else StartPoint = false;
	
	TokTak[ InstruNo] = false;
	
	SetCursor( &CHandCrsr);
	
	SaveUndo( USample, InstruNo, "\pUndo 'Loop Edition'");
	
	gNowMovingLoop = true;
	
	do
	{
		GetMouse( &aPt);
		
		if( aPt.h < SampleRect.left) aPt.h = SampleRect.left;
		else if( aPt.h > SampleRect.right) aPt.h = SampleRect.right;

		if( prevPt.h != aPt.h || firstTime == true)
		{
			firstTime = false;
			
			UpdateDisplayPosition( InstruNo);
			UpdateDisplayLoop( InstruNo);
			
			//// *************
			
			if( StartPoint)
			{
				if( PosToByte( aPt.h, InstruNo) < SampleDataD( InstruNo)->loopBeg + SampleDataD( InstruNo)->loopSize) StartPoint = true;
				else StartPoint = false;
			}
			else
			{
				if( PosToByte( aPt.h, InstruNo) < SampleDataD( InstruNo)->loopBeg) StartPoint = true;
				else StartPoint = false;
			}
			
			if( StartPoint)
			{
				end = SampleDataD( InstruNo)->loopBeg + SampleDataD( InstruNo)->loopSize;
				SampleDataD( InstruNo)->loopBeg = PosToByte( aPt.h, InstruNo);
				SampleDataD( InstruNo)->loopSize = end - SampleDataD( InstruNo)->loopBeg;
			}
			else
			{
				SampleDataD( InstruNo)->loopSize = PosToByte( aPt.h, InstruNo) - SampleDataD( InstruNo)->loopBeg;
			}
			
			if( SampleDataD( InstruNo)->stereo)
			{
				if( SampleDataD( InstruNo)->amp == 8)
				{
					SampleDataD( InstruNo)->loopBeg 	/= 2;		SampleDataD( InstruNo)->loopBeg 	*= 2;
					SampleDataD( InstruNo)->loopSize	/= 2;		SampleDataD( InstruNo)->loopSize	*= 2;
				}
				else
				{
					SampleDataD( InstruNo)->loopBeg 	/= 4;		SampleDataD( InstruNo)->loopBeg 	*= 4;
					SampleDataD( InstruNo)->loopSize	/= 4;		SampleDataD( InstruNo)->loopSize	*= 4;
				}
			}
			
			//// *************
			
			InternalUpdate( InstruNo);
			
			InvalWindowRect( GetDialogWindow( SampleDlog[ InstruNo]), &SampleRect);
			UpdateSampleWindow( SampleDlog[ InstruNo]);
			
			// Check to see if this instrument is currently playing, and update it!
			
			for( i=0; i< MADDriver->DriverSettings.numChn; i++)
			{
				Channel *curVoice = &MADDriver->chan[i];
				 
				if( curVoice->ins == InstruNo && curVoice->samp == curSample[ InstruNo])
				{
					if( curVoice->begPtr == SampleDataD( InstruNo)->data && curVoice->curPtr != curVoice->maxPtr)
					{
						if( SampleDataD( InstruNo)->loopSize > 2)
						{
							curVoice->maxPtr = (Ptr) (	(long) SampleDataD( InstruNo)->data +
														(long) SampleDataD( InstruNo)->loopBeg +
														(long) SampleDataD( InstruNo)->loopSize);
						}
						curVoice->loopBeg 		= SampleDataD( InstruNo)->loopBeg;
						curVoice->loopSize 		= SampleDataD( InstruNo)->loopSize;
					}
				}
			}
			
			prevPt = aPt;
		}
		
		DoGlobalNull();
		
		WaitNextEvent( everyEvent, &theEvent, 1, 0L);
	
	//	if (QDIsPortBuffered( GetDialogPort( SampleDlog[ InstruNo])))
    //					QDFlushPortBuffer( GetDialogPort( SampleDlog[ InstruNo]), NULL);
		
	}while( Button());
	
	gNowMovingLoop = false;
	
	TokTak[ InstruNo] = false;
}

void DoItemPressSample( short whichItem, DialogPtr whichDialog)
{
		Cell				theCell;
 		short				ctlPart, bogus, CurWin, temp, itemType, InstruNo;
		Point				myPt, Pt2, aPt;
		long				val, sampleSize, start, beginPoint, otherPoint, tL, TickEnd;
		ControlHandle		theControl;
		Handle				itemHandle;
		RgnHandle			tempRgn;
		Boolean				InvertSwitch;
		GrafPtr				SavePort;
		Rect				caRect, tempRect, oldSelect, itemRect;
		ControlActionUPP	MyControlUPP;
		
	GetPort( &SavePort);
	
	InstruNo = FindSample( whichDialog);
	NSelectInstruList( InstruNo, curSample[ InstruNo]);
	
	SetPortDialogPort( whichDialog);
	GetSampleRect( whichDialog);
	TextFont( 4);	TextSize( 9);

	if (theEvent.what == mouseDown)
	{
		myPt = theEvent.where;
		GlobalToLocal( &myPt);
		
		//ctlPart = FindControl( myPt, GetDialogWindow( whichDialog), &theControl);
		
		theControl = 0L;
		if( TestControl( xScroll[ InstruNo], myPt)) theControl = xScroll[ InstruNo];
		
		if( theControl == xScroll[ InstruNo])
		{
		/*	if( ctlPart == kControlIndicatorPart && gUseControlSize == false)
			{
				bogus = TrackControl( theControl, myPt, 0L);
				if( bogus != 0)
				{
					CurWin = FindSample( whichDialog);
					InternalUpdate( CurWin);
				}
			}
			else if( ctlPart > 0)*/
			{
				theDialogControl = whichDialog;
				MyControlUPP = NewControlActionUPP( actionProcSample);
				gThumbPrev = GetCtlValue( theControl);
				TrackControl(theControl, myPt, MyControlUPP);
				DisposeControlActionUPP( MyControlUPP);
			}
		}
		
		if( PtInRect( myPt, &SampleRect))
		{
			switch( curSample[ InstruNo])
			{
				case panningEnv:
				
				break;
				
				case volumeEnv:
				
				break;
				
				default:
				{
					long	start, end;
					
					if( SampleDataD( InstruNo)->size <= 0) goto END3;
					
					if( SelectMode[ InstruNo] == eSelect)
					{
						start = ByteToPos( SampleDataD( InstruNo)->loopBeg, InstruNo);
						end = ByteToPos( SampleDataD( InstruNo)->loopBeg + SampleDataD( InstruNo)->loopSize, InstruNo);
									
						if( myPt.h >= start-1 && myPt.h <= start+1)
						{
							MoveLoopSelection( InstruNo);
							goto END3;
						}
						
						if( myPt.h >= end-1 && myPt.h <= end+1)
						{
							MoveLoopSelection( InstruNo);
							goto END3;
						}
					}
				}
				break;
			}
			
			GetKeys( km);
			if( IsPressed( 0x003A))
			{
				while( Button()){};
				CurWin = FindSample( GetDialogFromWindow( oldWindow));
				if( ZoomLevel[ CurWin] > 1)
				{
					ZoomLevel[ CurWin] /= 2;
				
					SetCtlValue( 	xScroll[ CurWin],
									GetCtlValue( xScroll[ CurWin]) / 2);
					
					SetControlH( CurWin);
					
					InternalUpdate( CurWin);
				}
			}
			else if( IsPressed( 0x0037) || SelectMode[ InstruNo] == eZoom)
			{
				while( Button()){};
				CurWin = FindSample( GetDialogFromWindow( oldWindow));
				if( ZoomLevel[ CurWin] < 1024)
				{
					ZoomLevel[ CurWin] *= 2;
					SetControlH( CurWin);
					
					GetPortBounds( GetDialogPort( whichDialog), &caRect);
					
					bogus = (myPt.h * SAMPLEDEF) / caRect.right;
					bogus *= 2;
					bogus -= SAMPLEDEF/2;
					
					SetCtlValue( 	xScroll[ CurWin],
									GetCtlValue( xScroll[ CurWin])*2 + bogus);
					
					InternalUpdate( CurWin);
				}
			}
			else if( SelectMode[ InstruNo] == eSelect)		/** Selection mode **/
			{
				if( Clignote[ InstruNo])
				{
					if( TokTak[ InstruNo])
					{
						TokTak[ InstruNo] = false;
						
						tempRect.left = ByteToPos( SelecRect[ InstruNo].start, InstruNo);
						tempRect.right = tempRect.left +1;
						tempRect.top = SampleRect.top;
						tempRect.bottom = SampleRect.bottom;
						
						InvertRect( &tempRect);
					}
				}
				else
				{
					tempRect.left = ByteToPos( SelecRect[ InstruNo].start, InstruNo);
					tempRect.right = ByteToPos( SelecRect[ InstruNo].end, InstruNo);
					tempRect.top = SampleRect.top;
					tempRect.bottom = SampleRect.bottom;
					
					if( PtInRect( myPt, &tempRect))
					{
						GetMouse( &aPt);
					
						TickEnd = TickCount() + 20;
						while( Button() && TickCount() < TickEnd){};
						
						if( TickCount() >= TickEnd && curSample[ InstruNo] != volumeEnv && curSample[ InstruNo] != panningEnv)
						{
							SetCursor( &HandCrsr);
						
							tempRgn = NewRgn();
							RectRgn( tempRgn, &tempRect);
							
							LocalToGlobal( &aPt);
							theEvent.where = aPt;
							
							DragSample( tempRgn, InstruNo, &theEvent);
							
							DisposeRgn( tempRgn);
							
							SetCursor( GetQDGlobalsArrow( &qdarrow));
							
							goto EndSample;
						}
					}
					InvertRect( &tempRect);
				}
				
				Clignote[ InstruNo] = false;
				
				if( TickCount() - doubleClick <= GetDblTime() && lastPosition.h == myPt.h && lastPosition.v == myPt.v)
				{
					switch( curSample[ InstruNo])
					{
						case volumeEnv:
						case panningEnv:
							SelecRect[ InstruNo].end = ENVSIZE;
							SelecRect[ InstruNo].start = 0;
						break;
						
						default:
							SetSelection( InstruNo, 0, SampleDataD( InstruNo)->size);
						break;
					}
					
					InvalWindowRect( GetDialogWindow( SampleDlog[ InstruNo]), &SampleRect);
					UpdateDisplaySelec( InstruNo);
				}
				else
				{
					if( IsPressed( 0x0038))
					{
						if( PosToByte( myPt.h, InstruNo) < SelecRect[ InstruNo].end)
							beginPoint = SelecRect[ InstruNo].end;
						else beginPoint = SelecRect[ InstruNo].start;
					}
					else
					{
						lastPosition.h = myPt.h;
						lastPosition.v = myPt.v;
					
						beginPoint = PosToByte( myPt.h, InstruNo);
					}
				//	SelecRect[ InstruNo].end = beginPoint;
				//	SelecRect[ InstruNo].start = beginPoint;
					
					SetSelection( InstruNo, beginPoint, beginPoint);
					
					doubleClick = TickCount();
				
					tempRect.top = SampleRect.top;
					tempRect.bottom = SampleRect.bottom;
				
					/** AU Cas où on tire dans les bords **/
				
					theDialogControl = SampleDlog[ InstruNo];
					
					switch( curSample[ InstruNo])
					{
						case panningEnv:
						case volumeEnv:
							val = 0;
							start = 0;
							sampleSize = ENVSIZE;
						break;
						
						default:
							val = GetCtlValue( xScroll[ InstruNo]);
							start = GetStartSampleValue( val, InstruNo);	//(val * (SampleDataD( InstruNo)->size / SAMPLEDEF)) / ZoomLevel[ InstruNo];
							sampleSize = SampleDataD( InstruNo)->size / ZoomLevel[ InstruNo];
						break;
					}
					
					/****/
					
					do
					{
						GetMouse( &Pt2);
					
						otherPoint = PosToByte( Pt2.h, InstruNo);
						
						if( tL != otherPoint)
						{
							tL = otherPoint;
							
							oldSelect.left = ByteToPos( SelecRect[ InstruNo].start ,InstruNo);
							oldSelect.right = ByteToPos( SelecRect[ InstruNo].end, InstruNo);
							
							oldSelect.top = SampleRect.top;
							oldSelect.bottom = SampleRect.bottom;
							
							if( otherPoint >= beginPoint)
							{
								if( !InvertSwitch) InvertRect( &oldSelect);
								
								if( oldSelect.right >= ByteToPos( otherPoint, InstruNo))
								{
									tempRect.left = ByteToPos( otherPoint, InstruNo);
									tempRect.right = oldSelect.right;
								}
								else
								{
									tempRect.right = ByteToPos( otherPoint, InstruNo);
									tempRect.left = oldSelect.right;
									
								//	else tempRect.left = oldSelect.left;
								}
								
								InvertSwitch = true;
								
						//		SelecRect[ InstruNo].end = otherPoint;
						//		SelecRect[ InstruNo].start = beginPoint;
								
								SetSelection( InstruNo, beginPoint, otherPoint);
							}
							else
							{
								if( InvertSwitch) InvertRect( &oldSelect);
							
								if( oldSelect.left <= ByteToPos( otherPoint, InstruNo))
								{
									tempRect.right = ByteToPos( otherPoint, InstruNo);
									tempRect.left = oldSelect.left;
								}
								else
								{
									tempRect.left = ByteToPos( otherPoint, InstruNo);
								//	if( InvertSwitch) tempRect.right = oldSelect.right;
								
									tempRect.right = oldSelect.left;
								}
								
								InvertSwitch = false;
								
							//	SelecRect[ InstruNo].start = otherPoint;
							//	SelecRect[ InstruNo].end = beginPoint;
								
								SetSelection( InstruNo, otherPoint, beginPoint);
							}
							
							InvertRect( &tempRect);
							
							switch( curSample[ InstruNo])
							{
								case volumeEnv:
								case panningEnv:
									if( Pt2.h < 0)
									{
										if( SelecRect[ InstruNo].start < 0) SelecRect[ InstruNo].start = 0;
									}
									
									GetPortBounds( GetDialogPort( whichDialog), &caRect);
									
									if( Pt2.h > caRect.right)
									{
										if( SelecRect[ InstruNo].end > ENVSIZE) SelecRect[ InstruNo].end = ENVSIZE;
									}
								break;
								
								default:
									if( Pt2.h < 0)
									{
										if( SelecRect[ InstruNo].start < 0) SelecRect[ InstruNo].start = 0;	//xxx
										actionProcSample(  xScroll[ InstruNo],  kControlUpButtonPart);
										
										val = GetCtlValue( xScroll[ InstruNo]);
										start = GetStartSampleValue( val, InstruNo);	//(val * (SampleDataD( InstruNo)->size / SAMPLEDEF)) / ZoomLevel[ InstruNo];
										sampleSize = SampleDataD( InstruNo)->size / ZoomLevel[ InstruNo];
									}
									
									GetPortBounds( GetDialogPort( whichDialog), &caRect);
									
									if( Pt2.h > caRect.right)
									{
										if( SelecRect[ InstruNo].end > SampleDataD( InstruNo)->size) SelecRect[ InstruNo].end = SampleDataD( InstruNo)->size;
										actionProcSample(  xScroll[ InstruNo],  kControlDownButtonPart);
										
										val = GetCtlValue( xScroll[ InstruNo]);
										start = GetStartSampleValue( val, InstruNo);	//(val * (SampleDataD( InstruNo)->size / SAMPLEDEF)) / ZoomLevel[ InstruNo];
										sampleSize = SampleDataD( InstruNo)->size / ZoomLevel[ InstruNo];
									}
								break;
							}
							UpdateDisplaySelec( InstruNo);
							UpdateDisplayPosition( InstruNo);
						}
						
						WaitNextEvent( everyEvent, &theEvent, 1, 0L);
					
					//	if (QDIsPortBuffered( GetDialogPort( SampleDlog[ InstruNo])))
    				//	QDFlushPortBuffer( GetDialogPort( SampleDlog[ InstruNo]), NULL);
    				
					}
					while( Button());
					
					UpdateDisplaySelec( InstruNo);
				}
				
				if( SelecRect[ InstruNo].start == SelecRect[ InstruNo].end)
				{
					Clignote[ InstruNo] = true;
					TokTak[ InstruNo] = false;
				}
				else Clignote[ InstruNo] = false;
			}
			else if( SelectMode[ InstruNo] == ePencil) 	/** Pencil Mode **/
			{
				DrawPencil( SampleDlog[ InstruNo], InstruNo);
			}
			
			END3:;
		}
	}

	switch( whichItem)
	{
		case 40:
			if( curSample[ InstruNo] < 0 && MyTrackControl( FixedBut[ InstruNo], theEvent.where, 0L))
			{
				switch( curSample[ InstruNo])
				{
					case panningEnv:
						if( curMusic->fid[ InstruNo].pannType & EFNOTE) curMusic->fid[ InstruNo].pannType -= EFNOTE;
						else curMusic->fid[ InstruNo].pannType += EFNOTE;
					
						if( !(curMusic->fid[ InstruNo].pannType & EFNOTE))		HiliteControl( FixedBut[ InstruNo], kControlButtonPart);
						else HiliteControl( FixedBut[ InstruNo], 0);
					break;
					
					case volumeEnv:
						if( curMusic->fid[ InstruNo].volType & EFNOTE) curMusic->fid[ InstruNo].volType -= EFNOTE;
						else curMusic->fid[ InstruNo].volType += EFNOTE;
					
						if( !(curMusic->fid[ InstruNo].volType & EFNOTE))		HiliteControl( FixedBut[ InstruNo], kControlButtonPart);
						else HiliteControl( FixedBut[ InstruNo], 0);
					break;
				}
				InternalUpdate( InstruNo);
			}
		break;
		
		case 13:
			if(curSample[ InstruNo] < 0 && MyTrackControl( EnvBut[ InstruNo], theEvent.where, 0L))
			{
				switch( curSample[ InstruNo])
				{
					case panningEnv:
						if( curMusic->fid[ InstruNo].pannType & EFON) curMusic->fid[ InstruNo].pannType -= EFON;
						else curMusic->fid[ InstruNo].pannType += EFON;
					
						if( curMusic->fid[ InstruNo].pannType & EFON)		HiliteControl( EnvBut[ InstruNo], kControlButtonPart);
						else HiliteControl( EnvBut[ InstruNo], 0);
					break;
					
					case volumeEnv:
						if( curMusic->fid[ InstruNo].volType & EFON) curMusic->fid[ InstruNo].volType -= EFON;
						else curMusic->fid[ InstruNo].volType += EFON;
					
						if( curMusic->fid[ InstruNo].volType & EFON)		HiliteControl( EnvBut[ InstruNo], kControlButtonPart);
						else HiliteControl( EnvBut[ InstruNo], 0);
					break;
				}
				InternalUpdate( InstruNo);
			}
		break;
		
		case 36:
			if(curSample[ InstruNo] < 0 && MyTrackControl( SustainBut[ InstruNo], theEvent.where, 0L))
			{
				switch( curSample[ InstruNo])
				{
					case panningEnv:
						if( curMusic->fid[ InstruNo].pannType & EFSUSTAIN) curMusic->fid[ InstruNo].pannType -= EFSUSTAIN;
						else curMusic->fid[ InstruNo].pannType += EFSUSTAIN;
					
						if( curMusic->fid[ InstruNo].pannType & EFSUSTAIN)		HiliteControl( SustainBut[ InstruNo], kControlButtonPart);
						else HiliteControl( SustainBut[ InstruNo], 0);
					break;
					
					case volumeEnv:
						if( curMusic->fid[ InstruNo].volType & EFSUSTAIN) curMusic->fid[ InstruNo].volType -= EFSUSTAIN;
						else curMusic->fid[ InstruNo].volType += EFSUSTAIN;
					
						if( curMusic->fid[ InstruNo].volType & EFSUSTAIN)		HiliteControl( SustainBut[ InstruNo], kControlButtonPart);
						else HiliteControl( SustainBut[ InstruNo], 0);
					break;
				}
				InternalUpdate( InstruNo);
			}
		break;
		
		case 37:
			if(curSample[ InstruNo] < 0 && MyTrackControl( LoopBut[ InstruNo], theEvent.where, 0L))
			{
				switch( curSample[ InstruNo])
				{
					case panningEnv:
						if( curMusic->fid[ InstruNo].pannType & EFLOOP) curMusic->fid[ InstruNo].pannType -= EFLOOP;
						else curMusic->fid[ InstruNo].pannType += EFLOOP;
					
						if( curMusic->fid[ InstruNo].pannType & EFLOOP)		HiliteControl( LoopBut[ InstruNo], kControlButtonPart);
						else HiliteControl( LoopBut[ InstruNo], 0);
					break;
					
					case volumeEnv:
						if( curMusic->fid[ InstruNo].volType & EFLOOP) curMusic->fid[ InstruNo].volType -= EFLOOP;
						else curMusic->fid[ InstruNo].volType += EFLOOP;
					
						if( curMusic->fid[ InstruNo].volType & EFLOOP)		HiliteControl( LoopBut[ InstruNo], kControlButtonPart);
						else HiliteControl( LoopBut[ InstruNo], 0);
					break;
				}
				InternalUpdate( InstruNo);
			}
		break;
	
		case 26:
			if( GetControlHilite( PencilBut[ InstruNo]) != 255 && MyTrackControl( PencilBut[ InstruNo], theEvent.where, 0L))
			{
				EraseTikTak( InstruNo);
			
				SelectMode[ InstruNo] = ePencil;
				HiliteControl( PencilBut[ InstruNo], kControlButtonPart);
				HiliteControl( SelectBut[ InstruNo], 0);
				if( GetControlHilite( ZoomBut[ InstruNo]) != 255) HiliteControl( ZoomBut[ InstruNo], 0);
			}
		break;
		
		case 27:
			if( GetControlHilite( SelectBut[ InstruNo]) != 255 && MyTrackControl( SelectBut[ InstruNo], theEvent.where, 0L))
			{
				SelectMode[ InstruNo] = eSelect;
				HiliteControl( SelectBut[ InstruNo], kControlButtonPart);
				HiliteControl( PencilBut[ InstruNo], 0);
				if( GetControlHilite( ZoomBut[ InstruNo]) != 255) HiliteControl( ZoomBut[ InstruNo], 0);
			}
		break;

		case 18:
			if( GetControlHilite( ZoomBut[ InstruNo]) != 255 && MyTrackControl( ZoomBut[ InstruNo], theEvent.where, 0L))
			{
				SelectMode[ InstruNo] = eZoom;
				HiliteControl( SelectBut[ InstruNo], 0);
				HiliteControl( PencilBut[ InstruNo], 0);
				HiliteControl( ZoomBut[ InstruNo], kControlButtonPart);
				
				if( TickCount() - doubleClick <= GetDblTime())
				{
					ZoomLevel[ InstruNo] = 1;
				
					SetCtlValue( 	xScroll[ InstruNo],
									0);
					
					SetControlH( InstruNo);
					
					InternalUpdate( InstruNo);
				}
				doubleClick = TickCount();
			}
		break;
	
		case 25:
			if( MyTrackControl( InfoBut[ InstruNo], theEvent.where, 0L))
			{
				NEditInstruInfo( InstruNo, curSample[ InstruNo]);
			}
		break;
	
		case 28:
		{
			long		mresult;
			Point		Zone;
			short		i, kk;
			MenuHandle	sMenu;
			short		menuSampleId[ 255];
		
			sMenu = GetMenu( 156);
			
			InsertMenu( sMenu, hierMenu);
			
			for( i = 0, kk = 0; i < curMusic->fid[ InstruNo].numSamples; i++)
			{
				sData		*curData = curMusic->sample[ curMusic->fid[ InstruNo].firstSample +  i];
				Str255		str;
				
				strcpy( (Ptr) str, curData->name);
				
				MyC2PStr( (Ptr) str);
				
				if( str[ 0] == 0)
				{
					Str255	str2;
					
					pStrcpy( str, "\puntitled sample #");
					
					NumToString( i+1, str2);
					pStrcat( str, str2);
				}
				
				if( curData->stereo)
				{
					Str255 str2;
					
					menuSampleId[ kk++] = i;
					menuSampleId[ kk++] = i;
					
					pStrcpy( str2, str);	pStrcat( str2, "\p Left");		AppendMenu( sMenu, str2);
					pStrcpy( str2, str);	pStrcat( str2, "\p Right");		AppendMenu( sMenu, str2);
				}
				else
				{
					menuSampleId[ kk++] = i;
					AppendMenu( sMenu, str);
				}
			}
			
			GetDialogItem( whichDialog, 28, &itemType, &itemHandle, &itemRect);
			Zone.h = itemRect.left;
			Zone.v = itemRect.top;
			
			LocalToGlobal( &Zone);
			
			switch( curSample[ InstruNo])
			{
				case volumeEnv:
					i = 1;
				break;
				
				case panningEnv:
					i = 2;
				break;
				
				default:
					for( i = 0; i < 255; i++)
					{
						if( menuSampleId[ i] == curSample[ InstruNo]) break;
					}
					i = 4 + menuSampleId[ i];
					if( curMode[ InstruNo]) i++;
				break;
			}
			
			mresult = PopUpMenuSelect(	sMenu,
										Zone.v,
										Zone.h,
										i);
										
			DeleteMenu( GetMenuID( sMenu));
			DisposeMenu( sMenu);
			
			if ( HiWord( mresult ) != 0 )
			{
				short prevSample = curSample[ InstruNo];
				
				switch( LoWord( mresult))
				{
					case 1:		// Volume Envelope
						curSample[ InstruNo] = volumeEnv;
					break;
					
					case 2:
						curSample[ InstruNo] = panningEnv;
					break;
					
					default:	// Select new sample
						curSample[ InstruNo] = menuSampleId[ LoWord( mresult) - 4];
						
						if( LoWord( mresult) - 4 > 0)
						{
							if( menuSampleId[ LoWord( mresult) - 4] == menuSampleId[ LoWord( mresult) - 5]) curMode[ InstruNo] = 1;
							else curMode[ InstruNo] = 0;
						}
						else curMode[ InstruNo] = 0;
					break;
				}
				
				if( prevSample != curSample[ InstruNo])
				{
					ResetSelectionSample( InstruNo);
				
					ZoomLevel[ InstruNo] = 1;
				
					SetCtlValue( 	xScroll[ InstruNo], 0);
					SetControlH( InstruNo);
				}
				
				InternalUpdate( InstruNo);
				NSelectInstruList( InstruNo, curSample[ InstruNo]);
			}
			}
		break;
		
		case 14:
			switch( curSample[ InstruNo])
			{
				case volumeEnv:
				case panningEnv:
				{
					long		mresult;
					Point		Zone;
					short		i;
					MenuHandle	tMenu;
					
					GetDialogItem( whichDialog, 14, &itemType, &itemHandle, &itemRect);
					
					tMenu = GetMenu( 162);
					InsertMenu( tMenu, hierMenu);
					
					Zone.h = itemRect.left;
					Zone.v = itemRect.top;
					
					LocalToGlobal( &Zone);
					
					mresult = PopUpMenuSelect(	tMenu,
												Zone.v,
												Zone.h,
												0);
					
					DeleteMenu( GetMenuID( tMenu));
					DisposeMenu( tMenu);
					
					if ( HiWord( mresult ) != 0 )
					{
						temp = LoWord( mresult );
					
						SaveUndo( USample, InstruNo, "\pUndo 'Plug editing'");
					
						switch( temp)
						{
							case 1:	// Selection to loop
								Selection2Loop( InstruNo);
							break;
							
							case 2:	// Loop to selection
								Loop2Selection( InstruNo);
							break;
							
							case 3:	// set sustain point
								SetSustainPoint( InstruNo);
							break;
						}
						InternalUpdate( InstruNo);
						if( thePrefs.ClassicalProjection) UpdateMozartInfo();
						curMusic->hasChanged = true;
					}
				}
				break;
				
				default:
					GetDialogItem( whichDialog, 14, &itemType, &itemHandle, &itemRect);
					temp = PressSampleMenu( &itemRect);
					
					if( temp != -1)
					{
						SaveUndo( USample, InstruNo, "\pUndo 'Plug editing'");
					
						switch( temp)
						{
						case 1:
							Selection2Loop( InstruNo);
						break;
						
						case 2:
							Loop2Selection( InstruNo);
						break;
						
						case 3:
							DoKeyPressSample( whichDialog, 0x0008);
						break;
						
						case 5:	// Hz Filter
							FFTSampleFilter( curMusic->sample[ curMusic->fid[ InstruNo].firstSample +  curSample[ InstruNo]], InstruNo, false);
						break;
						
						case 6:	// Hz Shift
							FFTSampleFilter( curMusic->sample[ curMusic->fid[ InstruNo].firstSample +  curSample[ InstruNo]], InstruNo, true);
						break;
						
						default:
							if( temp - 8 >= GetMaxSoundFilterPlugs())
							{
								NAppelVSTPlug( InstruNo, curSample[ InstruNo], temp - 8 - GetMaxSoundFilterPlugs() - 1);
							} 
							else
							{
								NAppelPlug( InstruNo, curSample[ InstruNo], temp - 8);
							}
						break;
						}
						InternalUpdate( InstruNo);
						if( thePrefs.ClassicalProjection) UpdateMozartInfo();
						curMusic->hasChanged = true;
					}
				break;
			}
		break;
	}

	EndSample:

	SetPort( SavePort);
}

void DoKeyPressSample( DialogPtr	theDia, short theChar)
{
GrafPtr		SavePort;
short		CurWin, i;
Ptr			newSnd;
long		Start, End, sSize, newSize, reste;
Str255		str1;
Point		theCell = { 0, 0}, ppt = { 0,0};

	CurWin = FindSample( theDia);
		
	GetPort( &SavePort);
	SetPortDialogPort( theDia);
	GetSampleRect( theDia);
	
	if( theChar == selectAll)
	{
		Clignote[ CurWin] = false;
		
		switch( curSample[ CurWin])
		{
			case volumeEnv:
			case panningEnv:
				SelecRect[ CurWin].end = ENVSIZE;
				SelecRect[ CurWin].start = 0;
			break;
			
			default:
			//	SelecRect[ CurWin].end = SampleDataD( CurWin)->size;
			//	SelecRect[ CurWin].start = 0;
				
				SetSelection( CurWin, 0, SampleDataD( CurWin)->size);
			break;
		}
		
		InvalWindowRect( GetDialogWindow( SampleDlog[ CurWin]), &SampleRect);
		UpdateDisplaySelec( CurWin);
	}
	
	if( theChar == getinfo)
	{
		NEditInstruInfo( CurWin, curSample[ CurWin]);
	}
	else if( theChar == 0x1C)		// Left Arrow
	{
		if( curSample[ CurWin] != volumeEnv && curSample[ CurWin] != panningEnv)
		{
			if( SampleDataD( CurWin)->stereo) curMode[ CurWin] = 0;
		}
		
		ResetSelectionSample( CurWin);
		InternalUpdate( CurWin);
		NSelectInstruList( CurWin, curSample[ CurWin]);
	}
	else if( theChar == 0x1D)		// Right Arrow
	{
		if( curSample[ CurWin] != volumeEnv && curSample[ CurWin] != panningEnv)
		{
			if( SampleDataD( CurWin)->stereo) curMode[ CurWin] = 1;
		}
		
		ResetSelectionSample( CurWin);
		InternalUpdate( CurWin);
		NSelectInstruList( CurWin, curSample[ CurWin]);
	}
	else if( theChar == 0x1E)		// --
	{
		if( curSample[ CurWin] != volumeEnv && curSample[ CurWin] != panningEnv)
		{
			curSample[ CurWin]--;
			
			if( curSample[ CurWin] < 0) curSample[ CurWin] = panningEnv;
		}
		else if( curSample[ CurWin] == panningEnv)
		{
			curSample[ CurWin] = volumeEnv;
		}
		
		
		ResetSelectionSample( CurWin);
		ZoomLevel[ CurWin] = 1;
		SetCtlValue( 	xScroll[ CurWin], 0);
		SetControlH( CurWin);
		
		InternalUpdate( CurWin);
		NSelectInstruList( CurWin, curSample[ CurWin]);
	}
	else if( theChar == 0x1F)		// ++
	{
		if( curSample[ CurWin] == volumeEnv) curSample[ CurWin] = panningEnv;
		else if( curSample[ CurWin] == panningEnv)
		{
			if( curMusic->fid[ CurWin].numSamples > 0) curSample[ CurWin] = 0;
		}
		else if( curMusic->fid[ CurWin].numSamples > 0)
		{
			curSample[ CurWin]++;
			if( curSample[ CurWin] >= curMusic->fid[ CurWin].numSamples)
			{
				curSample[ CurWin] = curMusic->fid[ CurWin].numSamples-1;
			}
		}
		
		ResetSelectionSample( CurWin);
		ZoomLevel[ CurWin] = 1;
		SetCtlValue( 	xScroll[ CurWin], 0);
		SetControlH( CurWin);
		
		InternalUpdate( CurWin);
		NSelectInstruList( CurWin, curSample[ CurWin]);
	}
	else if( theChar == 0x08)		// Delete
	{
		Start = FindByteStart( CurWin);
		End = FindByteEnd( CurWin);
		
		if( Start != End)
		{
			SaveUndo( USample, CurWin, "\pUndo 'Delete instrument'");
			
			curMusic->hasChanged = true;
			
			switch( curSample[ CurWin])
			{
				case panningEnv:
				{
					InstrData	*curIns = &curMusic->fid[ CurWin];
					
					for( i = 0; i < curIns->pannSize; i++)
					{
						if( curIns->pannEnv[ i].pos >= Start && curIns->pannEnv[ i].pos <= End)
						{
							short ww;
							
							for( ww = i; ww < curIns->pannSize-1; ww++)
							{
								curIns->pannEnv[ ww] = curIns->pannEnv[ ww+1];
							}
							
							curIns->pannSize--;
							i--;
						}
					}
					
					if( curIns->pannSize <= 0)
					{
						curIns->pannSize = 0;
						if( curIns->pannType & EFON) curIns->pannType -= EFON;
					}
					
					InternalUpdate( CurWin);
				}
				break;
				
				case volumeEnv:
				{
					InstrData	*curIns = &curMusic->fid[ CurWin];
					
					for( i = 0; i < curIns->volSize; i++)
					{
						if( curIns->volEnv[ i].pos >= Start && curIns->volEnv[ i].pos <= End)
						{
							short ww;
							
							for( ww = i; ww < curIns->volSize-1; ww++)
							{
								curIns->volEnv[ ww] = curIns->volEnv[ ww+1];
							}
							
							curIns->volSize--;
							i--;
						}
					}
					
					if( curIns->volSize <= 0)
					{
						curIns->volSize = 0;
						if( curIns->volType & EFON) curIns->volType -= EFON;
					}
					
					InternalUpdate( CurWin);
				}
				break;
				
				default:
					sSize = (long) SampleDataD( CurWin)->size;
					
					if( End > sSize) End = sSize;
					if( Start > sSize) Start = sSize;
					
					reste = sSize - End;
					
					newSize = reste + Start;
					
					if( newSize == 0L)
					{
						SampleDataD( CurWin)->loopBeg	= 0L;
						SampleDataD( CurWin)->loopSize	= 0L;
						SampleDataD( CurWin)->size		= 0L;
						MyDisposePtr( (Ptr*) &SampleDataD( CurWin)->data);
						SampleDataD( CurWin)->data		= NewPtr( 0);
						
						InternalUpdate( CurWin);
						if( thePrefs.ClassicalProjection) UpdateMozartInfo();
					}
					else
					{
						newSnd = MyNewPtr( newSize);
						if( newSnd != 0L)
						{
							BlockMoveData( SampleDataD( CurWin)->data, newSnd, Start);
							
							if( reste < 0) MyDebugStr( __LINE__, __FILE__, "Error in Sample Delete");
							BlockMoveData( SampleDataD( CurWin)->data + End, newSnd + Start, reste);
							
							MyDisposePtr( & SampleDataD( CurWin)->data);
							SampleDataD( CurWin)->data = newSnd;
							
							SampleDataD( CurWin)->size = newSize;
							
							/*** Réajuste la loop ***/
							
							if( SampleDataD( CurWin)->loopBeg > Start)
							{
								SampleDataD( CurWin)->loopBeg -= End - Start;
							}
							
							if( SampleDataD( CurWin)->loopBeg < 0) SampleDataD( CurWin)->loopBeg = 0;
							
							if( SampleDataD( CurWin)->loopBeg > SampleDataD( CurWin)->size)
							{
								SampleDataD( CurWin)->loopBeg = 0;
								SampleDataD( CurWin)->loopSize = 0;
							}
							
							if(		SampleDataD( CurWin)->loopBeg +
									SampleDataD( CurWin)->loopSize > SampleDataD( CurWin)->size)
							{
								SampleDataD( CurWin)->loopSize = SampleDataD( CurWin)->size - SampleDataD( CurWin)->loopBeg;
							}
							
							/***********************/
							
							curMusic->hasChanged = true;
							
							if( SelecRect[ CurWin].start > SampleDataD( CurWin)->size)
								SelecRect[ CurWin].start = SampleDataD( CurWin)->size;	//xxx
							SelecRect[ CurWin].end = SelecRect[ CurWin].start;
						
							InternalUpdate( CurWin);
							if( thePrefs.ClassicalProjection) UpdateMozartInfo();
							
							// Réduction de la sélection à une barre de clignotement:
							
							Clignote[ CurWin] = true;
							TokTak[ CurWin] = false;
						}
						else Erreur( 5, -84);
					}
				break;
			}
		}
	}
	else if( theChar == '/' || theChar == '*')
	{
		short 	temp;
		
		if( curSample[ CurWin] != volumeEnv && curSample[ CurWin] != panningEnv)
		{
			sData	*curData;
			
			curData = curMusic->sample[ curMusic->fid[ CurWin].firstSample +  curSample[ CurWin]];
			
			if( theChar == '/')
			{
				if( curData->c2spd > 0)
					curData->c2spd --;
					
				curMusic->hasChanged = true;
			}
			else
			{
				if( curData->c2spd < 60000L)
					curData->c2spd ++;
					
				curMusic->hasChanged = true;
			}
			
			UpdateDisplay( CurWin);
		}
	}
	else if( theChar ==  9)
 	{
 		HiliteControl( SelectBut[ CurWin], 0);
		HiliteControl( PencilBut[ CurWin], 0);
		HiliteControl( ZoomBut[ CurWin], 0);
		
		SelectMode[ CurWin]++;
		if( SelectMode[ CurWin] > MODESIZE) SelectMode[ CurWin] = 0;
		switch( SelectMode[ CurWin])
		{
			case eSelect:	HiliteControl( SelectBut[ CurWin], kControlButtonPart);	break;
			case ePencil:	HiliteControl( PencilBut[ CurWin], kControlButtonPart);	break;
			case eZoom:		HiliteControl( ZoomBut[ CurWin], kControlButtonPart);		break;
		}
 	}
	else if( theEvent.what == keyDown)
	{
		str1[0] = 1;	str1[1] = theChar;
	//	UprString( str1, true);
		theChar = str1[1];
		
		if( (short) theChar >= 0 && (short) theChar < 300)
		{
		/*	for( i = 1; i < 61; i++)
			{
				if( thePrefs.PianoKey[ (short) theChar] == MADDriver->pitchTable[ i * MAXSAMPLE +  0] ) break;
			}*/
			
			i = thePrefs.PianoKey[ (short) theChar];
			
			if( i != 0xFF && i != -1) i += thePrefs.pianoOffset*12;
			
			if( i >= 0 && i < NUMBER_NOTES)
			{
				short track = GetWhichTrackPlay();
				
				SelectTouche( i, -1);
				SelectToucheMozart( i, 0);
				
				TouchIn++;
				if( TouchIn < 0 || TouchIn >= 10) TouchIn = 0;
				TouchMem[ TouchIn] = i;
				TrackMem[ TouchIn] = track;
				if( TrackMem[ TouchIn] < 0 || TrackMem[ TouchIn] >= MADDriver->DriverSettings.numChn) TrackMem[ TouchIn] = 0;
				
				if( curSample[ CurWin] < 0)
				{
					DoPlayInstruInt( i, CurWin, 0, 0, 0xFF, &MADDriver->chan[ track], 0, 0);
				}
				else
				{
					if( SelecRect[ CurWin].start == SelecRect[ CurWin].end) DoPlayInstruInt( i, CurWin, 0, 0, 0xFF, &MADDriver->chan[ track], 0, 0);
					else
					{
						DoPlayInstruInt( i, CurWin, 0, 0, 0xFF, &MADDriver->chan[ track], SelecRect[ CurWin].start, SelecRect[ CurWin].end);
					}
				}
			}
		}
	}
	SetPort( SavePort);
}

void PrintSample( DialogPtr theDia)
{
	PrintPixMap( GetPortPixMap( BitMapSample[ FindSample( theDia)]));
}

void ResetSelectionSample( short CurWin)
{
GrafPtr	SavePort;

	Clignote[ CurWin] = false;
	SelecRect[ CurWin].end = 0;
	SelecRect[ CurWin].start = 0;

	if( SampleDlog[ CurWin] == 0L) return;

	GetPort( &SavePort);
 	SetPortDialogPort( SampleDlog[ CurWin]);
 	GetSampleRect( SampleDlog[ CurWin]);
		
	InvalWindowRect( GetDialogWindow( SampleDlog[ CurWin]), &SampleRect);
	UpdateDisplaySelec( CurWin);
	
	SetPort( SavePort);
}

static	Boolean	canAcceptDrag;

pascal OSErr MyTrackingSample(short message, WindowPtr theWindow, void *handlerRefCon, DragReference theDrag)

{	short				result, offset, i;
	long				theTime = TickCount();
	unsigned short		count, index;
	unsigned long		flavorFlags, attributes;
	ItemReference		theItem;
	RgnHandle			theRgn;
	Point				theMouse, localMouse, theCell;
	Rect				tempRect;

	if( !mainSystemDrag) return noErr;

	if ((message != kDragTrackingEnterHandler) && (!canAcceptDrag)) return(noErr);

	SetPortWindowPort( theWindow);
	BackColor( whiteColor);
	
	GetDragAttributes(theDrag, &attributes);

	switch (message)
	{
		case kDragTrackingEnterHandler:
			canAcceptDrag = true;	//IsMyTypeAvailable( theDrag);
		break;

		case kDragTrackingEnterWindow:
		
		break;

		case kDragTrackingInWindow:
			GetDragMouse(theDrag, &theMouse, 0L);
			localMouse = theMouse;
			GlobalToLocal(&localMouse);


			GetSampleRect( GetDialogFromWindow( theWindow));
			if( PtInRect( localMouse, &SampleRect))
			{
				tempRect.top = SampleRect.top;
				tempRect.bottom = SampleRect.bottom;
				tempRect.left = localMouse.h - 1;
				tempRect.right = localMouse.h + 1;
			
				RectRgn(theRgn = NewRgn(), &tempRect);
				ShowDragHilite(theDrag, theRgn, true);
				DisposeRgn(theRgn);
			}
			else HideDragHilite( theDrag);
		break;

		case kDragTrackingLeaveWindow:
			HideDragHilite( theDrag);
		break;

		case kDragTrackingLeaveHandler:
		
		break;

	}
	RGBBackColor( &theColor);

	return(noErr);
}
/*

pascal OSErr MyReceiveSampleHandler(WindowPtr theWindow, unsigned long handlerRefCon, DragReference theDrag)
{
	FSSpec				pseudoFF;
	HFSFlavor			myFlavor;
	OSErr				result;
	Handle				mySnd;
	Rect				theRect, srcRect;
	unsigned short		items;
	ItemReference		theItem;
	DragAttributes		attributes;
	Ptr					textData;
	StScrpHandle		stylHandle;
	Size				textSize;
	short				offset, selStart, selEnd, mouseDownModifiers, mouseUpModifiers, moveInstru;
	Point				theCell;
	Str255				myStr;

	if( !mainSystemDrag) return dragNotAcceptedErr;

	SetPortWindowPort(theWindow);
	if( CurInstruDrag == -1 && selectedControl == 0L) return( dragNotAcceptedErr);
	if( !canAcceptDrag) return( dragNotAcceptedErr);

	GetDragAttributes(theDrag, &attributes);
	GetDragModifiers(theDrag, 0L, &mouseDownModifiers, &mouseUpModifiers);

	if( selectedControl != 0L)	// Drag sur un bouton !!!!
	{
		HiliteControl( selectedControl, 0);
		
		if( selectedControl == SaveBut)		HandleInstruChoice( 5);
		if( selectedControl == InfoBut)		HandleInstruChoice( 6);
		if( selectedControl == DelBut)		HandleInstruChoice( 7);
		if( selectedControl == RecBut)		HandleInstruChoice( 3);
		if( selectedControl == LoadBut)		HandleInstruChoice( 4);
		if( selectedControl == OpenBut)
		{
			theCell.v = 0;
			if( GetIns( &theCell.v, 0L))
			{
				short	ins, samp;
			
				ConvertIDtoInsSamp( theCell.v, &ins, &samp);
				
				NCreateSampleWindow( ins, samp);
			}
		}

		selectedControl = 0L;
		
		return noErr;
	}

	moveInstru = (attributes & kDragInsideSenderWindow) && (!((mouseDownModifiers & optionKey) | (mouseUpModifiers & optionKey)));
	if( moveInstru && DragInstruSource == CurInstruDrag) return noErr;

	BackColor( whiteColor);

	HideDragHilite(theDrag);
	
	theCell.v = 0;	theCell.h = 0;
//	while( LGetSelect( true, &theCell, InstruList)) LSetSelect( false, theCell, InstruList);
	theCell.v = CurInstruDrag;
//	LSetSelect( true, theCell, InstruList);
	
	RGBBackColor( &theColor);

	curMusic->hasChanged = true;
	
	//	Un fichier en provenance du Finder
	
	GetDragItemReferenceNumber(theDrag, 1, &theItem);
	result = GetFlavorDataSize(theDrag, theItem, flavorTypeHFS, &textSize);

	if (result == noErr)
	{
		SaveUndo( UAllSamples, 0, "\pUndo 'Drop instrument file'");
	
		GetFlavorData(theDrag, theItem, flavorTypeHFS, &myFlavor, &textSize, 0L);
		
		ResolveAliasFile( &myFlavor.fileSpec, true, &targetIsFolder, &wasAliased);
		
		OpenSampleInt( CurInstruDrag, myFlavor.fileSpec, 0L);
		
		TextFont( 4);	TextSize( 9);
		CreateInstruList();
		DrawInfoInstrument();
		UpdateSampleWindows();
		UpdateInstruMenu();
		TextFont( 0);	TextSize( 0);
		
		return(noErr);
	}

	//	Un drag de type 'snd '

	GetDragItemReferenceNumber(theDrag, 1, &theItem);
	result = GetFlavorDataSize(theDrag, theItem, 'snd ', &textSize);

	if (result == noErr)
	{
		SaveUndo( UAllSamples, 0, "\pUndo 'Move instrument'");
	
		mySnd = MyNewHandle( textSize);
		
		HLock( mySnd);
		
		GetFlavorData(theDrag, theItem, 'snd ', *mySnd, &textSize, 0L);
		
		OpenSampleInt( CurInstruDrag, pseudoFF, mySnd);
		
		if( moveInstru && DragInstruSource != CurInstruDrag)				// Delete the source, if moving without option key.
		{
			SetInstruName( DragInstruSource, "\p");

			SampleDataD( DragInstruSource)->size 		= 0;
			SampleDataD( DragInstruSource)->loopBeg		= 0;
			SampleDataD( DragInstruSource)->loopSize 	= 0;
			SampleDataD( DragInstruSource)->vol 		= 64;
		//	SampleDataD( DragInstruSource)->freq 		= 1;
			SampleDataD( DragInstruSource)->amp 		= 8;
	
			if( SampleDataD( DragInstruSource)->data != 0L) MyDisposePtr( &SampleDataD( DragInstruSource)->data);
			SampleDataD( DragInstruSource)->data = 0L;

			DragInstruSource = -1;
		}
		
		result = GetFlavorDataSize(theDrag, theItem, 'STR ', &textSize);
		if( result == noErr)
		{
			GetFlavorData(theDrag, theItem, 'STR ', &myStr, &textSize, 0L);
			
			SetInstruName( CurInstruDrag, myStr);
		}
		
		TextFont( 4);	TextSize( 9);
		CreateInstruList();
		DrawInfoInstrument();
		UpdateSampleWindows();
		UpdateInstruMenu();
		TextFont( 0);	TextSize( 0);
		
		return(noErr);
	}

	return( dragNotAcceptedErr);
}


pascal OSErr MySendSampleProc(FlavorType theFlavor,  void *refCon, ItemReference theItem,  DragReference theDrag)
{
	AEDesc			dropLoc;
	FSSpec			target;
	OSErr			err;
	Boolean			wasChanged;
	OSType			compressionType;
	short			SoundType, i;
	Handle			theSound;
	long			SoundSize;
	
	if (theFlavor == 'VCT1')
	{
		err = GetDropLocation( theDrag, &dropLoc);
		if (err) return (err);

		err = ResolveAlias(nil, (AliasHandle) dropLoc.dataHandle, &target, &wasChanged);
		if (err) return (err);
		
		theSound = SoundQualityExportSnd( DragInstruSource, &compressionType, &SoundType, &SoundSize);
		if( theSound == 0L) return noErr;

		strcpy( (Ptr) target.name, SampleDataD( DragInstruSource)->name);
		for( i=0; i<strlen( (Ptr) target.name); i++) if( target.name[i] == ':') target.name[i] = '-';
		MyC2PStr( (Ptr) target.name);

		HSetVol( 0L, target.vRefNum, target.parID);
		
		SaveSampleInt( theSound, DragInstruSource, SoundType, compressionType, &target);

		err = SetDragItemFlavorData( theDrag, theItem, theFlavor, &target, sizeof( target), 0L);
		if (err) return (err);
	}

	return (noErr);
}
*/

Boolean DragSample( RgnHandle myRgn, short theNo, EventRecord *theEvent)
{
	short				result;
	RgnHandle			dragRegion, tempRgn;
	Point				theLoc;
	DragReference		theDrag;
	StScrpHandle		theStyl;
	AEDesc				dropLocation;
	DragAttributes		attributes;
	short				mouseDownModifiers, mouseUpModifiers, copyText;
	long				inOutBytes, Start, End;
	short				temp;
	Handle				theSound;
	Ptr					Instru = SampleDataD( theNo)->data;
	Str255				theStr;
	PromiseHFSFlavor	myNewFile;
	FSSpec				mySpec;
	short				numChan;
	Rect				dragRegionRect;
	
	if( !DragManagerUse) return false;
	if( Instru == 0L) return false;
	if( SampleDataD( theNo)->size == 0L) return false;

	//******************************************
	// Prepare l'instrument pour etre exporte !!
	//******************************************

	DragInstruSource = theNo;

	Start = FindByteStart( theNo);
	End = FindByteEnd( theNo);
	
	inOutBytes = End - Start;

	if( SampleDataD( theNo)->stereo) numChan = 2;
	else numChan = 1;
	
	theSound = MyNewHandle( 10000L);
	SetupSndHeader(		(SndListHandle) theSound,
						numChan,
						((unsigned long) SampleDataD( theNo)->c2spd)<<16L,
						SampleDataD( theNo)->amp,
						'NONE',
						60 - SampleDataD( theNo)->relNote,
						inOutBytes,
						&temp);
	
	SetHandleSize( theSound, inOutBytes + temp);
	
/*	AddLoopToSndHandle(	theSound,
						curMusic->fid[ theNo].loopBeg,
						curMusic->fid[ theNo].loopBeg + curMusic->fid[ theNo].loopSize);*/

	HLock( theSound);
	BlockMoveData( Instru + Start, (Ptr) ((*theSound) + temp), inOutBytes);
	if( SampleDataD( theNo)->amp == 8) ConvertInstrumentIn( (Byte*) (*theSound + temp), inOutBytes);
	else ConvertInstrumentOut16( (short*) (*theSound + temp), inOutBytes);
	//******************************************
	//******************************************
	
	CopyRgn( myRgn, dragRegion = NewRgn());
	SetPt( &theLoc, 0, 0);
	LocalToGlobal( &theLoc);
	OffsetRgn( dragRegion, theLoc.h, theLoc.v);
	
	NewDrag( &theDrag);
	
	myNewFile.fileType			=	'snd ';
	myNewFile.fileCreator		=	'SNPL';
	myNewFile.fdFlags			=	0;
	myNewFile.promisedFlavor	=	0;	//'VCT1';
//	AddDragItemFlavor( theDrag, 1, flavorTypePromiseHFS, &myNewFile, sizeof( myNewFile), flavorNotSaved);
//	AddDragItemFlavor( theDrag, 1, 'VCT1', 0L, 0, 0);
	AddDragItemFlavor( theDrag, 1, 'snd ', *theSound, GetHandleSize( theSound), 0);

/*	strcpy( (Ptr) theStr, SampleDataD( theNo)->name);
	MyC2PStr( (Ptr) theStr);
	AddDragItemFlavor( theDrag, 1, 'STR ', &theStr, sizeof( theStr), 0);
*/
/*	
	result = SetDragSendProc( theDrag, mySendDataUPP, 0L);
*/
	SetDragItemBounds(theDrag, 1, GetRegionBounds( dragRegion, &dragRegionRect));

	tempRgn = NewRgn();
	CopyRgn(dragRegion, tempRgn);
	InsetRgn(tempRgn, 1, 1);
	DiffRgn(dragRegion, tempRgn, dragRegion);
	DisposeRgn(tempRgn);

	result = TrackDrag(theDrag, theEvent, dragRegion);

	if (result != noErr && result != userCanceledErr)
	{
		return(true);
	}

	//
	//	Check to see if the drop occurred in the Finder's Trash. If the drop occurred
	//	in the Finder's Trash and a copy operation wasn't specified, delete the
	//	source selection. Note that we can continute to get the attributes, drop location
	//	modifiers, etc. of the drag until we dispose of it using DisposeDrag.
	//

	GetDragAttributes(theDrag, &attributes);
	if (!(attributes & kDragInsideSenderApplication))
	{
		GetDropLocation(theDrag, &dropLocation);

		GetDragModifiers(theDrag, 0L, &mouseDownModifiers, &mouseUpModifiers);
		copyText = (mouseDownModifiers | mouseUpModifiers) & optionKey;

		if ((!copyText) && ( DropLocationIsFinderTrash( &dropLocation)))
		{
			DoKeyPressSample( SampleDlog[ theNo], 8);
			
			curMusic->hasChanged = true;
			
			CreateInstruList();
			UpdateSampleWindows();
		}
		else
		{
		}

		AEDisposeDesc(&dropLocation);
	}

	//
	//	Dispose of the drag.
	//

	DisposeDrag(theDrag);

	DisposeRgn(dragRegion);

	HUnlock( theSound);
	MyDisposHandle( & theSound);
	
	return(true);
}
