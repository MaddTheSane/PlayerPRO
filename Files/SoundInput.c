#include <Sound.h>
#include "shuddup.h"
#include "RDriver.h"
#include "stdio.h"

void RecordSnd(Handle);
OSErr NOpenMicroDevice( Str255 myDeviceName);
void ChangeDialogFont( DialogPtr aDia);
Ptr GetAudioInPut( Boolean LeftChannel, long Size);
void DrawFillFree( Rect *rect);

#define			SOUNDINPUTMUL	2

static	Ptr				SoundInputPtr;

static	long 			myInRefNum, maxSndSize;
static	SPB				mySPB;
static	ControlHandle	RecordBut, StopBut, PauseBut, PlayBut;

static	Boolean			RecordingVBL;
static	Ptr				RecordingPtr, StartRecordingPtr;

static	DialogPtr		deviceDialog;
static	ListHandle		myList;
static	Str255			deviceName[ 20], sourceNameList[ 20];
static	MenuHandle		bitsM, rateM;
static	short			curBits;
static	short			curRate;
static	RGBColor		redC = { 61166, 0, 0}, blueC = { 13107, 65535, 65535}, blackC = { 0, 0, 0};
static	Byte			SmallOsci[ 200];

static	unsigned long	LASTrate = -1;
static	short			LASTbits = -1, LASTvolume = -1;
static	short			LASTchannels = -1;

		unsigned long	SoundInputRate;
		short			SoundInputBits, MicroDeviceID, inputSourceID;
		Boolean			SoundInputStereo, stereoAvailable, monoAvailable;

		long			deviceBufferSize, gPeakAmplitude = 0, lastOSTypeInput;
		short			previousSoundVolume, baseChan;
		Ptr				outPutPtr = (Ptr) -1L;
		Boolean			MicroPhone;
		Str255			InPutName;
		
		Boolean			POKPOK;
		
#if defined(powerc) || defined (__powerc)
pascal void CompletionRoutine (SPBPtr inParamPtr)
{
	OSErr			err;
	
	outPutPtr = SoundInputPtr;
	
	if( RecordingVBL)
	{
		if( RecordingPtr - StartRecordingPtr + SOUNDINPUTMUL*deviceBufferSize < maxSndSize)
		{
			BlockMoveData( SoundInputPtr, RecordingPtr, SOUNDINPUTMUL*deviceBufferSize);
			
			if( SoundInputStereo == false && baseChan > 1)
			{
				long 		i;
				short	*RecordingShortPtr = (short*) RecordingPtr;
				
				switch( SoundInputBits)
				{
					case 8:
						for( i = 0; i < SOUNDINPUTMUL*deviceBufferSize; i++)
						{
							RecordingPtr[ i] = RecordingPtr[ i*baseChan];
						}
					break;
					
					case 16:
						for( i = 0; i < SOUNDINPUTMUL*deviceBufferSize; i++)
						{
							RecordingShortPtr[ i] = RecordingShortPtr[ i*baseChan];
						}
					break;
				}
				
				RecordingPtr += (SOUNDINPUTMUL*deviceBufferSize) / baseChan;
			}
			else RecordingPtr += SOUNDINPUTMUL*deviceBufferSize;
		}
		else RecordingVBL = false;
	}
	
	if (inParamPtr->error == noErr)
	{
		mySPB.error = noErr;
		mySPB.count	= SOUNDINPUTMUL*deviceBufferSize;
		err = SPBRecord (inParamPtr, true);
	}
}

/*
pascal void InterruptRoutine(SPBPtr inParamPtr, Ptr dataBuffer,short peakAmplitude, long sampleSize)
{
	outPutPtr = dataBuffer;
}
*/

#else

#endif

#pragma options align=mac68k
typedef struct
{
	short	no;
	short	**l;
} SPListBits;

typedef struct
{
	short		no;
	unsigned long	**l;
} SPListRate;

typedef struct
{
	short		no;
	unsigned long	**l;
} SPListChannels;
#pragma options align=reset

static	SPListBits		BitsList;
static	SPListRate		RateList;


void SetUpDeviceInfo()
{
OSErr			myErr;
short			i, infoData;
Str255			str;
float			tByte;
OSType			aa;
short			ChannelsList;
short			on = 1;

	myErr = SPBGetDeviceInfo( myInRefNum, siPlayThruOnOff, (Ptr) &infoData);
	
	myErr = SPBStopRecording( myInRefNum);
	
	/** RATE **/
	
	myErr = SPBGetDeviceInfo( myInRefNum, siSampleRateAvailable, (Ptr) &RateList);
	if( myErr == noErr)
	{	
		if( rateM != 0L) DisposeMenu( rateM);
		
		rateM = NewMenu( 344, "\pRate");
		
		for( i = 0; i < RateList.no; i++)
		{
			NumToString( (unsigned long) (*RateList.l)[ i]/65535UL, str);
			pStrcat( str, "\p Hz");
		
			AppendMenu( rateM, str);
		}
	}
	
	myErr = SPBGetDeviceInfo( myInRefNum, siSampleRate, (Ptr) &SoundInputRate);
	if( myErr == noErr)
	{
		NumToString( (unsigned long) SoundInputRate/65535UL, str);
		pStrcat( str, "\p Hz");
		SetDText( deviceDialog, 13, str);
		
		for ( i = 0; i < RateList.no; i++) if( SoundInputRate == (*RateList.l)[ i]) curRate = i;
	}
	
	/** MODE **/
	
	myErr = SPBGetDeviceInfo( myInRefNum, siChannelAvailable, (Ptr) &ChannelsList);
	if( myErr == noErr)
	{
		monoAvailable = true;
		stereoAvailable = false;
		
		if( ChannelsList >= 2) stereoAvailable = true;
	}
	
	if( !stereoAvailable) SoundInputStereo = false;
	
	if( SoundInputStereo) pStrcpy( str, "\pStereo");
	else pStrcpy( str, "\pMono");
	
	SetDText( deviceDialog, 44, str);
	
	/** BITS **/
	
	myErr = SPBGetDeviceInfo( myInRefNum, siSampleSizeAvailable, (Ptr) &BitsList);
	if( myErr == noErr)
	{	
		if( bitsM != 0L) DisposeMenu( bitsM);
		
		bitsM = NewMenu( 345, "\pBits");
	
		for( i = 0; i < BitsList.no; i++)
		{
			NumToString( (*BitsList.l)[ i], str);
			pStrcat( str, "\p Bits");
			
			AppendMenu( bitsM, str);
		}
	}
	myErr = SPBGetDeviceInfo( myInRefNum, siSampleSize, (Ptr) &SoundInputBits);
	if( myErr == noErr)
	{
		NumToString( SoundInputBits, str);
		pStrcat( str, "\p Bits");
		SetDText( deviceDialog, 11, str);
		
		for ( i = 0; i < BitsList.no; i++) if( SoundInputBits == (*BitsList.l)[ i]) curBits = i;
	}
	/** Bytes / Samples **/
	
	tByte = maxSndSize;
	sprintf( (Ptr) str, "%.1f",  (float) (tByte/1024.));		MyC2PStr( (Ptr) str); 		SetDText( deviceDialog, 34, str);
	
	if( SoundInputBits != 0) 		tByte /= SoundInputBits / 8;
	if( SoundInputStereo)			tByte /= 2;
	if( SoundInputRate != 0) 		sprintf( (Ptr) str, "%.1f", (tByte * 65535.)/ (float) SoundInputRate);
	
	MyC2PStr( (Ptr) str); 			SetDText( deviceDialog, 35, str);
	
//	SPBGetDeviceInfo( myInRefNum, siDeviceBufferInfo, (Ptr) &deviceBufferSize);
//	MyDebugStr( deviceBufferSize, __FILE__, "");
	
	myErr = SPBSetDeviceInfo (myInRefNum, siContinuous, (Ptr) &on);
	
	mySPB.error = noErr;
	mySPB.count	=	SOUNDINPUTMUL*deviceBufferSize;
	myErr = SPBRecord( &mySPB, true);
	
	myErr = SPBSetDeviceInfo( myInRefNum, siPlayThruOnOff, (Ptr) &infoData);
	
	myErr = SPBGetDeviceInfo( myInRefNum, siPlayThruOnOff, (Ptr) &infoData);
	if( myErr != noErr) pStrcpy( str, "\p-");
	else
	{
		if( infoData != 0) pStrcpy( str, "\pOn");
		else pStrcpy( str, "\pOff");
	}
	SetDText( deviceDialog, 3, str);
	
	myErr = SPBGetDeviceInfo( myInRefNum, siNumberChannels, (Ptr) &baseChan);
	
	
	// INPUT LIST 
	{
	Handle			sourceNames;
	long			offset, sourceNo;
	Point			theCell;

	SPBGetDeviceInfo ( myInRefNum, siInputSourceNames, &sourceNames);	
	
	offset = sizeof (short);    // skip over count field
	
	LDelRow( 0, 0, myList);
	
	sourceNo = 1;
	
    for (i = 0; i < (*(short**)sourceNames)[0]; i++)
	{
		pStrcpy( sourceNameList[ sourceNo],  &((unsigned char*)(*sourceNames))[offset]);
		offset += (*(char**)sourceNames)[offset] + 1;
		
		theCell.v = LAddRow( 1, 20000, myList);
		theCell.h = 0;
		LSetCell( sourceNameList[ sourceNo]+1 , sourceNameList[ sourceNo][ 0], theCell, myList);
		
		sourceNo++;
    }
	
	myErr = SPBGetDeviceInfo( myInRefNum, siInputSource, &inputSourceID);
	
	theCell.h = 0;	theCell.v = inputSourceID-1;
	LSetSelect( true, theCell, myList);
	}	
}

void DeviceFilterMouseDown( EventRecord *theEventI)
{
		char			xMyFilter;
		WindowPtr	whichWindow;
		short		thePart,i, theChar, remember, itemType, infoData;
		GrafPtr		oldPort;
		Point			aPoint, theCell;
		Rect			itemRect;
 		Str255		str1, str2;
 		Boolean		DrawAll;
 		Handle		deviceIconHandle, itemHandle;
 		OSErr		myErr;

			thePart = FindWindow( theEventI->where, &whichWindow);
		
			if( whichWindow == GetDialogWindow( deviceDialog))
			{
				Rect tempRect = (*myList)->rView;
				
				aPoint.v = theEventI->where.v;
				aPoint.h = theEventI->where.h;
				GlobalToLocal( &aPoint);
				
				tempRect.right += 16;
				if( PtInRect( aPoint, &tempRect) == true)
				{
					Boolean isDbl;
					
					theCell.v = theCell.h = 0;
					if(LGetSelect( true, &theCell, myList))
					{
						remember = theCell.v;
					}
					else remember = - 10;
				
					TextFont( kFontIDGeneva);	TextSize( 9);
					isDbl = LClick(aPoint, theEventI->modifiers, myList);
					
					theCell.v = theCell.h = 0;
					if(LGetSelect( true, &theCell, myList))
					{
						if( remember != theCell.v && theCell.v != inputSourceID-1)
						{
							short infoData;
							
							inputSourceID = theCell.v+1;
							
							myErr = SPBGetDeviceInfo( myInRefNum, siPlayThruOnOff, (Ptr) &infoData);
							
							myErr = SPBSetDeviceInfo (myInRefNum, siInputSource, &inputSourceID);
							
							myErr = SPBSetDeviceInfo( myInRefNum, siPlayThruOnOff, (Ptr) &infoData);
							
							SetUpDeviceInfo();
						}
					}
				}
			}
}
		
void DeviceFilterUpdate()
{
		char		xMyFilter;
		WindowPtr	whichWindow;
		short		thePart,i, theChar, remember, itemType, infoData;
		GrafPtr		oldPort;
		Point		aPoint, theCell;
		Rect		itemRect;
 		Str255		str1, str2;
 		Boolean		DrawAll;
 		Handle		deviceIconHandle, itemHandle;
 		OSErr		myErr;
		RgnHandle	visibleRegion;
		
    GetPort( &oldPort);
	SetPortDialogPort( deviceDialog);
	
	BeginUpdate( GetDialogWindow( deviceDialog));

	DrawDialog( deviceDialog);
	oldFrameButton( deviceDialog);
	
	TextFont( kFontIDGeneva);	TextSize( 9);
	
	itemRect = (*myList)->rView;
	
	InsetRect( &itemRect, -1, -1);
	FrameRect( &itemRect);
	
	visibleRegion = NewRgn();
	
	GetPortVisibleRegion( GetDialogPort( deviceDialog), visibleRegion);
	
	LUpdate( visibleRegion, myList);
	
	DisposeRgn( visibleRegion);
	
	GetDialogItem( deviceDialog, 8, &itemType, &itemHandle, &itemRect);
	
	myErr = SPBGetDeviceInfo( myInRefNum, 'icon', (Ptr) &deviceIconHandle);
	if( myErr == noErr)
	{
		PlotIcon( &itemRect, deviceIconHandle);
		DisposeHandle( deviceIconHandle);
	}
	
	GetDialogItem( deviceDialog, 27, &itemType, &itemHandle, &itemRect);
	PaintRect( &itemRect);
	FrameRectRelief( &itemRect);
	
	GetDialogItem( deviceDialog, 38, &itemType, &itemHandle, &itemRect);
	PaintRect( &itemRect);
	FrameRectRelief( &itemRect);
	
	GetDialogItem( deviceDialog, 40, &itemType, &itemHandle, &itemRect);
	FrameRectRelief( &itemRect);
	itemRect.top++;	itemRect.left++;
	DrawFillFree( &itemRect);
	
	EndUpdate( GetDialogWindow( deviceDialog));
	
	SetPort( oldPort);
}

void DrawLevelRect( short meterLevel)
{
short	itemType;
Rect		itemRect;
Handle	itemHandle;

	GetDialogItem( deviceDialog, 27, &itemType, &itemHandle, &itemRect);
	itemRect.left++;
	ScrollRect( &itemRect, -1, 0, 0L);
	
	if( RecordingVBL) RGBForeColor( &redC);
	else RGBForeColor( &blueC);
	
	MoveTo( itemRect.right - 1, itemRect.bottom - 1);
	if( meterLevel != 0) LineTo( itemRect.right - 1, itemRect.bottom - meterLevel);
	else MoveTo( itemRect.right - 1, itemRect.bottom - 1);
	
	ForeColor( blackColor);
	LineTo( itemRect.right - 1, itemRect.top + 1);
}

void DrawFillFree( Rect *rect)
{
	Rect	aRect;
	
	aRect = *rect;
	
	ForeColor( redColor);
	aRect.right = aRect.left + ((rect->right - rect->left) * (RecordingPtr - StartRecordingPtr))/ maxSndSize;
	PaintRect( &aRect);
	
	aRect.left = aRect.right;
	aRect.right = rect->right;
	ForeColor( blackColor);
	PaintRect( &aRect);
}

void DrawSmallOscillo()
{
	short		i, hSize;
	Byte			*audioPtr;
	short		itemType;
	Rect			itemRect;
	Handle		itemHandle;
	RGBColor		myC;
	
	if( RecordingVBL) myC = redC;
	else myC = blueC;

	GetDialogItem( deviceDialog, 38, &itemType, &itemHandle, &itemRect);

	itemRect.top++;
	itemRect.left++;
	hSize = itemRect.right - itemRect.left;

	audioPtr = (Byte*) GetAudioInPut( false, hSize*2);
	
	if( hSize >= 200) MyDebugStr( __LINE__, __FILE__, "");
	
	for( i = 0; i < hSize; i++)
	{
		SetCPixel( itemRect.left + i, itemRect.top + SmallOsci[ i], &blackC);
		SmallOsci[ i] = audioPtr[ i] / 16;
		SetCPixel( itemRect.left + i, itemRect.top + SmallOsci[ i], &myC);
	}
	ForeColor( blackColor);
}

void ConvertInstrument16LL( register	short	*tempPtr,	register long sSize)
{
	register	short			val = 0x8000;

	sSize /= 2;

	while( sSize > 0)
	{
		sSize--;
		*(tempPtr + sSize) += val;
	}
}

void CloseSoundInput()
{
	if( deviceDialog != 0L)
	{
		DisposDialog( deviceDialog);
		deviceDialog = 0L;
	}
}

OSErr ActiveSoundInput( Boolean RecordingMode, Handle *RecordedSound, Str255 name)
{
Str255			aStr;
short			infoData, itemHit, itemType, nameNo, sourceNo, i;
Handle			itemHandle, deviceIconHandle;
long			Result, offset;
MenuHandle		aMenu, soundDrivers;
Rect			caRect, itemRect, dataBounds;
OSErr			myErr;
Point			myPt = { 0, 0}, theCell;
Boolean			StopPressed = true;
MenuHandle		tempMenu;
GrafPtr			savePort;

RecordingVBL = false;
SoundInputStereo = true;

if( deviceDialog == 0L)
{
	deviceDialog = GetNewDialog( 165,0L, (WindowPtr) -1L);
	SetPortDialogPort( deviceDialog);
	TextFont( kFontIDGeneva);	TextSize( 9);
	ChangeDialogFont( deviceDialog);
	AutoPosition( deviceDialog);
	
	GetDialogItem( deviceDialog , 18, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	RecordBut = NewControl( GetDialogWindow( deviceDialog), &itemRect, "\p",
							true,
							0,
							kControlContentIconSuiteRes,
							170,
							kControlBevelButtonNormalBevelProc,
							0);
	
	GetDialogItem( deviceDialog , 19, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	StopBut = NewControl( GetDialogWindow( deviceDialog), &itemRect, "\p",
							true,
							0,
							kControlContentIconSuiteRes,
							137,
							kControlBevelButtonNormalBevelProc,
							0);
	
	GetDialogItem( deviceDialog , 20, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	PauseBut = NewControl( GetDialogWindow( deviceDialog), &itemRect, "\p",
							true,
							0,
							kControlContentIconSuiteRes,
							181,
							kControlBevelButtonNormalBevelProc,
							0);
	
	GetDialogItem( deviceDialog , 21, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	PlayBut = NewControl( GetDialogWindow( deviceDialog), &itemRect, "\p",
							true,
							0,
							kControlContentIconSuiteRes,
							136,
							kControlBevelButtonNormalBevelProc,
							0);
	
	GetDialogItem( deviceDialog, 6, &itemType, &itemHandle, &itemRect);
	
	SetRect( &dataBounds, 0, 0, 1, 0);
	itemRect.right -= 16;
	myList =  LNew( &itemRect, &dataBounds, myPt, 0, GetDialogWindow( deviceDialog), true, false, false, true);
	(*myList)->selFlags = lOnlyOne;
}
else
{
	SetPortDialogPort( deviceDialog);
	GetPortBounds( GetDialogPort( deviceDialog), &caRect);
	InvalWindowRect( GetDialogWindow( deviceDialog), &caRect);
}

if( !RecordingMode)
{
	GetPortBounds( GetDialogPort( deviceDialog), &caRect);
	MySizeWindow( deviceDialog, caRect.right, 122, true);
	maxSndSize = 1;
}
else
{
	long	totalSizeSys, totalSizeApp, contigSize;
	
	*RecordedSound = 0L;
	
	#if MACOS9VERSION
	
	totalSizeSys = MaxMemSys( &contigSize);
	totalSizeApp = MaxMem( &contigSize);
	
	if(  totalSizeSys > totalSizeApp )
	{
		maxSndSize = totalSizeSys - 20L*1024L;
		*RecordedSound = NewHandleSys( maxSndSize);
	}
	else
	{
		maxSndSize = totalSizeApp/2L;
		*RecordedSound = NewHandle( maxSndSize);
	}
	
	#else
	
	totalSizeApp = MaxMem( &contigSize);
	
	maxSndSize = totalSizeApp/2L;
	*RecordedSound = NewHandle( maxSndSize);
	#endif
	
	if( *RecordedSound == 0L) return -1;
	HLock( *RecordedSound);
	RecordingPtr = StartRecordingPtr = **RecordedSound;
}

ShowWindow( GetDialogWindow( deviceDialog));

	/** Create Device List **/
	
	nameNo = 1;
	
	soundDrivers = NewMenu( 455, "\pSoundDrivers Menu");
	
	do
	{
		myErr = SPBGetIndexedDevice( nameNo, deviceName[ nameNo], &deviceIconHandle);
		if( myErr == noErr)
		{
			DisposeHandle( deviceIconHandle);
			
			AppendMenu( soundDrivers, deviceName[ nameNo]);
		}
		nameNo++;
	}while( myErr == noErr);
	
	if( MicroPhone == false)
	{
		MicroDeviceID = 1;
		if( NOpenMicroDevice( deviceName[ MicroDeviceID]) != noErr)
		{
			DisposDialog( deviceDialog);
			deviceDialog = 0L;
			Erreur( 74, -22);
			return -1;
		}
	}
	
/*****************/
myErr = SPBGetDeviceInfo( myInRefNum, siPlayThruOnOff, (Ptr) &infoData);
if( myErr != noErr) pStrcpy( aStr, "\p-");
else
{
	if( infoData != 0) pStrcpy( aStr, "\pOn");
	else pStrcpy( aStr, "\pOff");
}
SetDText( deviceDialog, 3, aStr);
SetDText( deviceDialog, 39, name);
SelIText( deviceDialog, 39, 0, 5000);

aMenu = GetMenu( 144);

bitsM = 0L;	rateM = 0L;
SetUpDeviceInfo();

for( i = 0; i < 200; i++) SmallOsci[ i] = 0;

SetDText( deviceDialog, 36, "\p0");
SetDText( deviceDialog, 37, "\p0");


// Set STOP state

GetDialogItem( deviceDialog, 46, &itemType, &itemHandle, &itemRect);		// ADD IT button
HiliteControl( (ControlHandle) itemHandle, 255);

HiliteControl( PlayBut, 255);
HiliteControl( PauseBut, 255);
HiliteControl( RecordBut, 0);

HiliteControl( StopBut, kControlButtonPart);

StopPressed = true;
RecordingVBL = false;

/////////////////////////////////////////////////

if( LASTchannels != -1)
{
	SoundInputStereo = LASTchannels;
}

if( LASTvolume != -1)
{
	myErr = SPBStopRecording( myInRefNum);
	
	myErr = SPBSetDeviceInfo( myInRefNum, siPlayThruOnOff, (Ptr) &LASTvolume);
	
	{ short a;
	a = 0;
	myErr = SPBSetDeviceInfo( myInRefNum, siTwosComplementOnOff, &a);
	}
	
	mySPB.error = noErr;
	mySPB.count 			= 	SOUNDINPUTMUL*deviceBufferSize;
	myErr = SPBRecord( &mySPB, true);

	SetUpDeviceInfo();
}

if( LASTbits != -1)
{
	myErr = SPBStopRecording( myInRefNum);
	
	myErr = SPBSetDeviceInfo( myInRefNum, siSampleSize, (Ptr) &LASTbits);
	
	{ short a;
	a = 0;
	myErr = SPBSetDeviceInfo( myInRefNum, siTwosComplementOnOff, &a);
	}
	
	mySPB.error = noErr;
	mySPB.count 			= 	SOUNDINPUTMUL*deviceBufferSize;
	myErr = SPBRecord( &mySPB, true);
	
	SetUpDeviceInfo();
}

if( LASTrate != -1)
{
	myErr = SPBStopRecording( myInRefNum);
	
	myErr = SPBSetDeviceInfo( myInRefNum, siSampleRate, (Ptr) &LASTrate);
	
	{ short a;
	a = 0;
	myErr = SPBSetDeviceInfo( myInRefNum, siTwosComplementOnOff, &a);
	}
	mySPB.error = noErr;
	mySPB.count 			= 	SOUNDINPUTMUL*deviceBufferSize;
	myErr = SPBRecord( &mySPB, true);
	
	SetUpDeviceInfo();
}

onrecommence:
do
{
	//ModalDialog( MyPrefFilterDesc, &itemHit);
	MyModalDialog( deviceDialog, &itemHit);
    //ModalDialog( 0L, &itemHit);
    
    GetPort( &savePort);
	SetPortDialogPort( deviceDialog);
	
	GetMouse( &theEvent.where);
	LocalToGlobal( &theEvent.where);
	
	switch( itemHit)
	{
	case -5:
	{
		Str255			str;
		short			recordingStatus, meterLevel;
		float			tByte;
		unsigned long	totalSamplesToRecord, numberOfSamplesRecorded, totalMsecsToRecord, numberOfMsecsRecorded;
		
		myErr = SPBGetRecordingStatus(	myInRefNum,
										&recordingStatus,
										&meterLevel,
										&totalSamplesToRecord,
										&numberOfSamplesRecorded,
										&totalMsecsToRecord,
										&numberOfMsecsRecorded);
		if( myErr == noErr)
		{
			if( RecordingVBL)
			{
				tByte = RecordingPtr - **RecordedSound;
				sprintf( (Ptr) str, "%.1f", tByte/1024.);		MyC2PStr( (Ptr) str); 		SetDText( deviceDialog, 36, str);
				
				tByte /= SoundInputBits / 8;
				if( SoundInputStereo) tByte /= 2;
				sprintf( (Ptr) str, "%.1f", (tByte * 65535.)/ (float) SoundInputRate);		MyC2PStr( (Ptr) str); 		SetDText( deviceDialog, 37, str);
				
				GetDialogItem( deviceDialog, 40, &itemType, &itemHandle, &itemRect);
				itemRect.top++;	itemRect.left++;
				DrawFillFree( &itemRect);
			}
			
			DrawLevelRect( meterLevel / 16);
		}
		
		DrawSmallOscillo();
	}
	break;
	
	case 45:
		tempMenu = GetMenu( 172);
		
		InsertMenu( tempMenu, hierMenu);
		
		GetDialogItem( deviceDialog, itemHit, &itemType, &itemHandle, &itemRect);
		myPt.v = itemRect.top;	myPt.h = itemRect.left;
		LocalToGlobal( &myPt);
		
		SetItemMark( tempMenu, SoundInputStereo + 1, 0xa5);
		
		if( !monoAvailable) DisableMenuItem( tempMenu, 1);
		if( !stereoAvailable) DisableMenuItem( tempMenu, 2);
		DisableMenuItem( tempMenu, 4);
		DisableMenuItem( tempMenu, 5);
		DisableMenuItem( tempMenu, 6);
		Result = PopUpMenuSelect(	tempMenu,
									myPt.v,
									myPt.h,
									SoundInputStereo + 1);
		
		SetItemMark( tempMenu, SoundInputStereo + 1, 0);
		
		if ( HiWord( Result ) != 0 )
		{
			if( LoWord( Result) != SoundInputStereo + 1)
			{
				SoundInputStereo = LoWord( Result) - 1;
				
				LASTchannels = SoundInputStereo;
				
				if( SoundInputStereo) pStrcpy( aStr, "\pStereo");
				else pStrcpy( aStr, "\pMono");
				
				SetDText( deviceDialog, 44, aStr);
				
				SetUpDeviceInfo();
			}
		}
		
		DeleteMenu( GetMenuID( tempMenu));
		DisposeMenu( tempMenu);
	break;
	
	case 18:
		if( GetControlHilite( RecordBut) == 0 && MyTrackControl( RecordBut, theEvent.where, 0L))
		{
			GetDialogItem( deviceDialog, 46, &itemType, &itemHandle, &itemRect);		// ADD IT button
			HiliteControl( (ControlHandle) itemHandle, 0);
			
			HiliteControl( PlayBut, 255);
			HiliteControl( PauseBut, 0);
			HiliteControl( StopBut, 0);
			
			HiliteControl( RecordBut, kControlButtonPart);
			if( StopPressed)
			{
				StopPressed = false;
				RecordingPtr = **RecordedSound;
			}
			else
			{
				if( SoundInputBits == 8) ConvertInstrument( (Byte*) **RecordedSound, RecordingPtr - **RecordedSound);
			}
			
			RecordingVBL = true;
		}
	break;
	
	case 19:
		if( GetControlHilite( StopBut) == 0 && MyTrackControl( StopBut, theEvent.where, 0L))
		{
			HiliteControl( PlayBut, 0);
			HiliteControl( PauseBut, 255);
			HiliteControl( RecordBut, 0);
			
			HiliteControl( StopBut, kControlButtonPart);
			
			StopPressed = true;
			RecordingVBL = false;
			
			if( SoundInputBits == 8) ConvertInstrumentIn( (Byte*) **RecordedSound, RecordingPtr - **RecordedSound);
		}
		
		MADCleanDriver( MADDriver);
	break;
	
	case 20:
		if( GetControlHilite( PauseBut) == 0 && MyTrackControl( PauseBut, theEvent.where, 0L))
		{
			HiliteControl( StopBut, 0);
			HiliteControl( PlayBut, 255);
			HiliteControl( RecordBut, kControlButtonPart);
		
			HiliteControl( PauseBut, kControlButtonPart);
			
			RecordingVBL = false;
			
			if( SoundInputBits == 8) ConvertInstrumentIn( (Byte*) **RecordedSound, RecordingPtr - **RecordedSound);
		}
		else if( GetControlHilite( PauseBut) == kControlButtonPart && MyTrackControl( PauseBut, theEvent.where, 0L))
		{
			HiliteControl( StopBut, 0);
			HiliteControl( PlayBut, 255);
			HiliteControl( RecordBut, kControlButtonPart);
			
			HiliteControl( PauseBut, 0);
			
			if( SoundInputBits == 8) ConvertInstrument( (Byte*) **RecordedSound, RecordingPtr - **RecordedSound);
			RecordingVBL = true;
		}
	break;
	
	case 21:
		if( GetControlHilite( PlayBut) == 0 && MyTrackControl( PlayBut, theEvent.where, 0L))
		{
			MADPlaySoundData( MADDriver, **RecordedSound, RecordingPtr - **RecordedSound, 0, 0xFF, SoundInputBits, 0, 0, SoundInputRate, SoundInputStereo);
			MADPlaySoundData( MADDriver, **RecordedSound, RecordingPtr - **RecordedSound, 1, 0xFF, SoundInputBits, 0, 0, SoundInputRate, SoundInputStereo);
		}
	break;
	
	case 42:
		InsertMenu( soundDrivers, hierMenu);
		
		GetDialogItem( deviceDialog, 42, &itemType, &itemHandle, &itemRect);
		myPt.v = itemRect.top;	myPt.h = itemRect.left;
		LocalToGlobal( &myPt);
		
		SetItemMark( soundDrivers, MicroDeviceID, 0xa5);
		
		Result = PopUpMenuSelect(	soundDrivers,
									myPt.v,
									myPt.h,
									MicroDeviceID);
		
		SetItemMark( soundDrivers, MicroDeviceID, 0);
		
		if ( HiWord( Result ) != 0 )
		{
			MicroDeviceID = LoWord( Result);
			SetDText( deviceDialog, 41, deviceName[ MicroDeviceID]);
			
			MicroOff();
			
			NOpenMicroDevice( deviceName[ MicroDeviceID]);
			
			SetUpDeviceInfo();
		}
		
		DeleteMenu( GetMenuID( soundDrivers));
	break;
	
	case 4:
		InsertMenu( aMenu, hierMenu);
			
		GetDialogItem( deviceDialog, 4, &itemType, &itemHandle, &itemRect);
		myPt.v = itemRect.top;	myPt.h = itemRect.left;
		LocalToGlobal( &myPt);
		
		myErr = SPBGetDeviceInfo( myInRefNum, siPlayThruOnOff, (Ptr) &infoData);
		if( infoData > 0) infoData = 1;
		
		SetItemMark( aMenu, infoData + 1, 0xa5);
		
		Result = PopUpMenuSelect(	aMenu,
									myPt.v,
									myPt.h,
									infoData + 1);
		
		SetItemMark( aMenu, infoData + 1, 0);
		
		if ( HiWord( Result ) != 0 )
		{
			infoData = LoWord( Result) - 1;
			
			if( infoData != 0) pStrcpy( aStr, "\pOn");
			else pStrcpy( aStr, "\pOff");
			
			SetDText( deviceDialog, 3, aStr);
			
			myErr = SPBStopRecording( myInRefNum);
			
			LASTvolume = infoData;
			
			myErr = SPBSetDeviceInfo( myInRefNum, siPlayThruOnOff, (Ptr) &infoData);
			
			{ short a;
			a = 0;
			myErr = SPBSetDeviceInfo( myInRefNum, siTwosComplementOnOff, &a);
			}
			mySPB.error = noErr;
			mySPB.count 			= 	SOUNDINPUTMUL*deviceBufferSize;
			myErr = SPBRecord( &mySPB, true);
		}
		
		DeleteMenu( GetMenuID( aMenu));
	break;
	
	case 7:
	/*	myErr = SPBGetDeviceInfo( myInRefNum, siOptionsDialog, (Ptr) &infoData);
		if( infoData == 1)
		{
			myErr = SPBSetDeviceInfo( myInRefNum, siOptionsDialog, 0L);
			
			SPBGetDeviceInfo( myInRefNum, siPlayThruOnOff, (Ptr) &infoData);
			if( infoData != 0) NumToString( (long) infoData, aStr);
			else pStrcpy( aStr, "\pOff");
			SetDText( deviceDialog, 3, aStr);
			
			UpdateALLWindow();
		}
		else Erreur( 61, -56);	*/
	break;
	
	case 14:
		InsertMenu( rateM, hierMenu );
		GetDialogItem( deviceDialog, 14, &itemType, &itemHandle, &itemRect);
		
		myPt.v = itemRect.top;	myPt.h = itemRect.left;
		LocalToGlobal( &myPt);
		
		SetItemMark( rateM, curRate + 1, 0xa5);
		
		Result = PopUpMenuSelect(	rateM,
								myPt.v,
								myPt.h,
								curRate + 1);
		
		SetItemMark( rateM, curRate + 1, 0);
		
		if ( HiWord(Result ) != 0 )
		{
			unsigned long		rate;
			
			curRate = LoWord( Result) - 1;
			
			myErr = SPBStopRecording( myInRefNum);
			
			LASTrate = rate = (*RateList.l)[ curRate];
			myErr = SPBSetDeviceInfo( myInRefNum, siSampleRate, (Ptr) &rate);
			
			{ short a;
			a = 0;
			myErr = SPBSetDeviceInfo( myInRefNum, siTwosComplementOnOff, &a);
			}
			mySPB.error = noErr;
			mySPB.count 			= 	SOUNDINPUTMUL*deviceBufferSize;
			myErr = SPBRecord( &mySPB, true);
			
			SetUpDeviceInfo();
		}
		DeleteMenu( GetMenuID( rateM));
	break;
	
	case 12:
		InsertMenu( bitsM, hierMenu );
		GetDialogItem( deviceDialog, 12, &itemType, &itemHandle, &itemRect);
		
		myPt.v = itemRect.top;	myPt.h = itemRect.left;
		LocalToGlobal( &myPt);
		
		SetItemMark( bitsM, curBits + 1, 0xa5);
		
		Result = PopUpMenuSelect(	bitsM,
								myPt.v,
								myPt.h,
								curBits + 1);
		
		SetItemMark( bitsM, curBits + 1, 0);
		
		if ( HiWord(Result ) != 0 )
		{
			short	bits, a;
			OSType	aa;
			
			curBits = LoWord( Result) - 1;
			
			myErr = SPBStopRecording( myInRefNum);
			
			LASTbits = bits = (*BitsList.l)[ curBits];
			myErr = SPBSetDeviceInfo( myInRefNum, siSampleSize, (Ptr) &bits);
			
			if( bits == 8)
			{
			//	aa = 'best';
			//	myErr = SPBSetDeviceInfo( myInRefNum, 'qual', (Ptr) &aa);
			}
			
			a = 0;
			myErr = SPBSetDeviceInfo( myInRefNum, siTwosComplementOnOff, &a);
			
			if( RecordingMode)
			{
				RecordingPtr = **RecordedSound;
			}
			
			mySPB.error				= noErr;
			mySPB.count 			= 	SOUNDINPUTMUL*deviceBufferSize;
			myErr = SPBRecord( &mySPB, true);
			
			SetUpDeviceInfo();
		}
		DeleteMenu( GetMenuID( bitsM));
	break;
	}
	
	SetPort( savePort);
	
}while( itemHit != 1 && itemHit != 16 && itemHit != 46);

if( RecordingVBL)
{
	RecordingVBL = false;
	if( SoundInputBits == 8) ConvertInstrumentIn( (Byte*) **RecordedSound, RecordingPtr - **RecordedSound);
}

GetDText( deviceDialog, 39, name);

if( rateM != 0L) DisposeMenu( rateM);
if( bitsM != 0L) DisposeMenu( bitsM);

DisposeMenu( soundDrivers);
DisposeMenu( aMenu);
UpdateALLWindow();

DoPause();

if( itemHit == 1 || itemHit == 46)
{
	if( RecordingMode)
	{
		long 		sndSize;
		short		headerLen;
		Handle		tHandle;
		short		channels;
		
		if( SoundInputBits == 8) ConvertInstrument( (Byte*) **RecordedSound, RecordingPtr - **RecordedSound);
		
		sndSize = RecordingPtr - **RecordedSound;
		
		if( sndSize <= 0L)
		{
			HUnlock( *RecordedSound);
			DisposeHandle( *RecordedSound);
			return -2;
		}
		
		tHandle = NewHandle( 1024L);
		
		if( SoundInputStereo) channels = 2;
		else channels = 1;
		
		SetupSndHeader(	(SndListHandle) tHandle,
						channels,
						SoundInputRate,
						SoundInputBits,
						'NONE',
						60,
						sndSize,
						&headerLen);
						
		HUnlock( *RecordedSound);
		SetHandleSize( *RecordedSound, sndSize + headerLen);
		
		HLock( *RecordedSound);
		HLock( tHandle);
		BlockMoveData( **RecordedSound, (**RecordedSound) + headerLen, sndSize);
		if( SoundInputBits == 16) ConvertInstrument16LL( (short*) ((**RecordedSound) + headerLen), sndSize);
		BlockMoveData( *tHandle, **RecordedSound, headerLen);
		HUnlock( *RecordedSound);
		HUnlock( tHandle);
		DisposeHandle( tHandle);
	}
	
	if( itemHit == 46) return -999;
	else return noErr;
}
else
{
	if( RecordingVBL)
	{
		HUnlock( *RecordedSound);
		DisposeHandle( *RecordedSound);
		*RecordedSound = 0L;
	}
	MicroOff();
	return -1;
}
}

OSErr NOpenMicroDevice( Str255 myDeviceName)
{
	OSErr			myErr;
	Handle			aHdl;
	
//	MyDebugStr( myErr, __FILE__, (char*) myDeviceName);
	
	myInRefNum = 0L;
	myErr = SPBOpenDevice( myDeviceName, siWritePermission, &myInRefNum);
//	if( myErr) MyDebugStr( myErr, __FILE__, (char*) myDeviceName);
	
	if( myErr == noErr)
	{
		myErr = SPBGetDeviceInfo( myInRefNum, siDeviceName, (Ptr) &InPutName);
	//	if( myErr) MyDebugStr( __LINE__, __FILE__, "2");
		
		myErr = SPBGetDeviceInfo( myInRefNum, siDeviceBufferInfo, (Ptr) &deviceBufferSize);
	//	if( myErr) MyDebugStr( __LINE__, __FILE__, "3");
		
		myErr = SPBGetDeviceInfo( myInRefNum, siPlayThruOnOff, (Ptr) &previousSoundVolume);
	//	if( myErr) MyDebugStr( __LINE__, __FILE__, "4");
		myErr = noErr;
		
		myErr = SPBSetDeviceInfo( myInRefNum, siOSTypeInputSource, &lastOSTypeInput);
	//	if( myErr) MyDebugStr( __LINE__, __FILE__, "5");
		myErr = noErr;
		
		mySPB.interruptRoutine 	= 0L;	//NewSIInterruptUPP( InterruptRoutine);
		mySPB.completionRoutine = NewSICompletionUPP (CompletionRoutine);
		
		RecordingVBL = false;
		
		if ( myErr == noErr)
		{
			unsigned long	temp;
			
			mySPB.inRefNum 			= 	myInRefNum;
			mySPB.count 			= 	SOUNDINPUTMUL*deviceBufferSize;				//SOUNDINPUTSIZE;
			mySPB.milliseconds		= 	0L;
			mySPB.bufferLength		= 	SOUNDINPUTMUL*deviceBufferSize;
			mySPB.bufferPtr 		=	SoundInputPtr;
			
			mySPB.userLong 			=	noErr;
			mySPB.error 			=	noErr;
			mySPB.unused1 			=	0;
			
			{ short a;
			a = 0;
			myErr = SPBSetDeviceInfo( myInRefNum, siTwosComplementOnOff, &a);
		//	if( myErr) MyDebugStr( __LINE__, __FILE__, "6");
			myErr = noErr;
			
			a = 1;
			myErr = SPBSetDeviceInfo (myInRefNum, siContinuous, (Ptr) &a);
			}
			mySPB.error = noErr;
			mySPB.count 			= 	SOUNDINPUTMUL*deviceBufferSize;
			myErr = SPBRecord( &mySPB, true);
			
			MicroPhone = true;
			SetWindowEnviron();
			SetWindowSpectrum();
			RecomputeEQ();
		}
	}
	
	return myErr;
}

void MicroOff(void)
{
	OSErr 			myErr;
	short			infoData;
	long			fourchar;
	unsigned long	maxRate;
	
	outPutPtr = (Ptr) -1L;
	
	if( MicroPhone == false) return;
	MicroPhone = false;
	
//	MyDebugStr( myErr, __FILE__, (char*) "off");
	
	myErr = SPBSetDeviceInfo( myInRefNum, siPlayThruOnOff, (Ptr) &previousSoundVolume);
//	if( myErr) MyDebugStr( myErr, __FILE__, "1");
	
	myErr = SPBStopRecording( myInRefNum);
//	if( myErr) MyDebugStr( myErr, __FILE__, "2");
	
	myErr = SPBGetDeviceInfo( myInRefNum, siOSTypeInputSource, &lastOSTypeInput);
//	if( myErr) MyDebugStr( myErr, __FILE__, "3");
	
	fourchar = kNoSource;
	myErr = SPBSetDeviceInfo( myInRefNum, siOSTypeInputSource, &fourchar);
	
	// Set Max Rate !!!
	maxRate = (*RateList.l)[ RateList.no-1];
	myErr = SPBSetDeviceInfo( myInRefNum, siSampleRate, &maxRate);
//	if( myErr) MyDebugStr( myErr, __FILE__, "5");
	
	myErr = SPBCloseDevice( myInRefNum);
//	if( myErr) MyDebugStr( myErr, __FILE__, "6");
	
	DisposeSICompletionUPP( mySPB.completionRoutine);
	mySPB.completionRoutine = 0L;
	
//	DisposeSIInterruptUPP( mySPB.interruptRoutine);
}

void InitSoundInput()
{
	outPutPtr = (Ptr) -1L;
	
	SoundInputPtr = NewPtrClear( 40L*2048L);
	lastOSTypeInput = kNoSource;
	MicroPhone = false;
	RecordingVBL = false;
	deviceDialog = 0L;
}