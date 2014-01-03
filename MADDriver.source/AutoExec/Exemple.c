#include <Carbon/Carbon.h>
#include <PlayerPROCore/PlayerPROCore.h>
#include "Exemple.h"

extern	Cursor	qdarrow;

#define	VERSION			0x0531
#define	PLAYERPREF	"\pPlayerPRO Prefs"

void DoPreferences2();
void SetDText (DialogPtr dlog, short item, Str255 str);

MADDriverRec	*MADDriver;
MADMusic		*music;
MADLibrary		*MADLib;

//#if defined(powerc) || defined (__powerc)
#pragma options align = mac68k
//#endif

typedef struct
{
	short			Version;
	Boolean			Loop;
	Boolean			Stereo;
	Boolean			PPCMachine;
	Boolean 		xxxxxxxx;
	Fixed			Frequence;
	Boolean			LargeWindow;
	Boolean			AutoCreator;
	Boolean			MicroRecording;
	short			oldShort;
	Boolean			AffichageDIGI;
	short			NoStart;
	Point			WinPos[ 30];
	short			WinEtat[ 30];
	short			PianoKey[ 300];
	Boolean			oldBool;
	Boolean			InstruListBig;
	Boolean			PatListBig;
	Boolean			AutoScroll;
	short			LoopType;
	short			WinHi[30];
	Boolean			Registred;
	long			SNumber;
	Byte			Decode;
	Str32			AutorName;
	long			checkSum;
	long			Mz;
	unsigned long	firstStart;
	short			WinLarg[30];
	RGBColor		tracksColor[ MAXTRACK];
	short			unused[ MAXTRACK];
	short			PianoPos;
	short			volumeLevel;
	Boolean			MADCompression;
	Boolean			SSText;
	Boolean			SSStars;
	Boolean			SSJumping;
	Boolean			FText;
	Boolean			FStars;
	Boolean			FBalls;
	short			TextS;
	Boolean			ThreadUse;
	Boolean			FSinScroll;
	Boolean			RememberMusicList;
	Boolean			OscilloLine;
	long			WinID[ 30];
	Boolean			DigitalInstru;
	Boolean			DigitalNote;
	Boolean			DigitalEffect;
	Boolean			DigitalArgu;
	Boolean			DigitalVol;
	Boolean			GoToStop;
	Boolean			DriverEffects[ 20];
	Boolean			MADC;
	short			OscilloSize;
	short			OscilloType;
	short			fileTypeExportSND;
	OSType			CompressionExportSND;
	Boolean			ActiveHelp;
	short			SpectrumSize;
	short			SpectrumType;
	
	/** Driver Settings **/
	
	short			numChn;								// Active tracks from 2 to 32, automatically setup when a new music is loaded
	short			outPutBits;							// 8 or 16 Bits
	unsigned long	outPutRate;							// Fixed number, by example : rate44khz, rate22050hz, rate22khz, rate11khz, rate11025hz
	short			outPutMode;							// MonoOutPut, StereoOutPut or DeluxeStereoOutPut ?
	short			driverMode;							// ASCSoundDriver, AWACSoundDriver, MIDISoundDriver or SoundManagerDriver
	Boolean			antiAliasing;						// Use AntiAliasing filter ?
	
	/** Mozart Prefs **/
	
	Boolean			UseOctaveMarkers;
	Boolean			UseMarkers;
	short			MarkersSize;
	short			MarkersOffSet;
	short			MozartX;
	
	/** News 4.5	**/
	
	Boolean			SpectrumScale;
	Boolean			ClassicalProjection;
	Boolean			PianoOctaveMarkers;
	Boolean			SmallPiano;
	Boolean			FastMusicList;
	long			FastDigitalEdition;
	
	Boolean			MacKeyBoard;
	Boolean			MidiKeyBoard;
	Boolean			QKMidiKeyBoard;
	
	Boolean			MIDIVelocity;
	Boolean			MIDIChanInsTrack;
	
	short			KeyUpMode;
	
	/** News 4.5.1	**/
	
	Boolean			Interpolation;
	Boolean			MicroDelay;
	long			MicroDelaySize;
	Boolean			surround;
	
	/** News 4.5.2	**/
	
	Boolean			OCArrow[ MAXINSTRU];
	short			MozartC1h, MozartC2h;
	OSType			SoundTypeSamp;
	OSType			SoundTypeIns;
	short			LinesHeight;
	Boolean			SaveMusicList;
	short			softVolumeLevel;
	
	Boolean			oADAPremember;
	Boolean			oADAPsave;
	Boolean			oADAPuse;
	
	Boolean			osciTile;
	
	Boolean			addExtension;
	
	Boolean			AutoPlayWhenOpen;
	
	Boolean			Reverb;
	long			ReverbSize;
	long			ReverbStrength;
	
	Boolean			oldDirectVideo;
	
	/** News 4.5.3 **/
	
	Boolean			TickRemover;
	Boolean			AutomaticOpen;
	Boolean			FinePositioning;
	
	short			ChannelType;
	short			amplitude;
	OSType			Compressor;
	Fixed			FrequenceSpeed;
	
	Byte			RecordAllTrack;			// 0 : a track, 1 : all tracks, 2 : current track
	
	/** News 4.5.4	**/
	
	Boolean			StaffShowAllNotes;
	Boolean			StaffShowLength;
	short			TempsNum;
	short			TempsUnit;
	short			TrackHeight;
	
	short			FKeyActive[ 20];
	short			FKeyItem[ 20];
	short			FKeyWind[ 20];
	
	/** News 4.5.7 **/
	
	Boolean			NewPrefSystem;
	unsigned char	NewPrefsCode[ 30];	
	
	/** News 4.5.8 **/
	
	Boolean			keyMapNote;
	
	/** News 5.0.0 **/
	
	RGBColor		yellC;
	short			whichEditorPatterns;
	Boolean			MusicTrace;
	long			oversampling;
	
	/** News 5.0.3 **/
	
	short			RAWBits;
	long			RAWRate;
	Boolean			RAWStereo;
	Boolean			RAWSigned;
	Boolean			RAWLittleEndian;
	Boolean			RAWEOF;
	long			RAWLength;
	long			RAWHeader;
	
	Byte			SelectedTracks[ MAXTRACK];
	
	Point			WinPosO[ 3][ 30];
	short			WinEtatO[ 3][ 30];
	short			WinLargO[ 3][ 30];
	short			WinHiO[ 3][ 30];
	long			WinIDO[ 3][ 30];
	
	Str32			WinNames[ 3];
	
	// 5.2
	Boolean			clickSound;
	Boolean			patternWrapping;
	Boolean			SendMIDIClockData;
	short			pianoOffset;
	
}	Prefs;

//#if defined(powerc) || defined (__powerc)
#pragma options align = reset
//#endif

static 	Boolean			Stereo, StereoMixing, NewSoundManager, has16Bit;
static	int				VolumeH, SpeedH;
static	PixPatHandle	workPixPat;
Prefs			thePrefs;
DialogPtr		myDialog;

Boolean			OnTourne;


void MegaLoop( void);
void ReadAndPlayMusic( Str255	myFile, OSType	fdType);

//#ifndef THINK_C
#if 0
static	DragTrackingHandlerUPP	MyTrackingUPP;
static	DragReceiveHandlerUPP	MyReceiveUPP;
#endif

static	Boolean					canAcceptDrag;

void pStrcat(register unsigned char *s1, register unsigned char *s2)
{
	register unsigned char *p;
	register short len, i;
	
	if (*s1+*s2<=255) 
	{
		p = *s1 + s1 + 1;
		*s1 += (len = *s2++);
	}
	else 
	{
		*s1 = 255;
		p = s1 + 256 - (len = *s2++);
	}
	for (i=len; i; --i) *p++ = *s2++;
}

void InitDriverString()
{
	Str255	str;
	
	if (MADDriver->DriverSettings.Reverb)
	{
		NumToString( MADDriver->DriverSettings.ReverbSize, str);
		pStrcat( str, "\pms");
		SetDText( myDialog, 27, str);
		
		NumToString( MADDriver->DriverSettings.ReverbStrength, str);
		pStrcat( str, "\p%");
		SetDText( myDialog, 29, str);
	}
	else
	{
		SetDText( myDialog, 27, "\pOFF");
		
		NumToString( MADDriver->DriverSettings.ReverbStrength, str);
		pStrcat( str, "\p%");
		SetDText( myDialog, 29, str);
	}
	
	if (MADDriver->DriverSettings.MicroDelaySize)
	{
		NumToString( MADDriver->DriverSettings.MicroDelaySize, str);
		pStrcat( str, "\pms");
		SetDText( myDialog, 7, str);
	}
	else SetDText( myDialog, 7, "\pDelay OFF");
	
	if (MADDriver->DriverSettings.surround) SetDText( myDialog, 24, "\pSurround ON");
	else SetDText( myDialog, 24, "\pSurround OFF");
	
	switch( MADDriver->DriverSettings.outPutRate)
	{
		case rate11khz:	SetDText( myDialog, 10, "\p11 Khz");					break;
		case rate22khz:	SetDText( myDialog, 10, "\p22 Khz");					break;
		case rate44khz:	SetDText( myDialog, 10, "\p44 Khz");					break;
	}
	
	switch( MADDriver->DriverSettings.outPutMode)
	{
			//	case MonoOutPut:			SetDText( myDialog, 13, "\pMono");			break;
			//	case StereoOutPut:			SetDText( myDialog, 13, "\pStereo");		break;
		case DeluxeStereoOutPut:	SetDText( myDialog, 13, "\pTrue Stereo");	break;
	}
	switch( MADDriver->DriverSettings.outPutBits)
	{
		case 8:		SetDText( myDialog, 16, "\p8 bits");						break;
		case 16:	SetDText( myDialog, 16, "\p16 bits");						break;
	}
	
}

void FrameRectRelief( Rect *theRect)
{
	RGBColor	theColor;
	
#define GrisClair	0xFFFF
#define GrisFonce	0x4000
	
	MoveTo( theRect->left, theRect->top);
	
	theColor.red = GrisFonce;	theColor.blue = GrisFonce;	theColor.green = GrisFonce;
	RGBForeColor( &theColor);
	LineTo( theRect->right, theRect->top);
	
	theColor.red = GrisClair;	theColor.blue = GrisClair;	theColor.green = GrisClair;
	RGBForeColor( &theColor);
	LineTo( theRect->right, theRect->bottom);
	
	theColor.red = GrisClair;	theColor.blue = GrisClair;	theColor.green = GrisClair;
	RGBForeColor( &theColor);
	LineTo( theRect->left, theRect->bottom);
	
	theColor.red = GrisFonce;	theColor.blue = GrisFonce;	theColor.green = GrisFonce;
	RGBForeColor( &theColor);
	LineTo( theRect->left, theRect->top);
	
	
	ForeColor( blackColor);
}

pascal OSErr MyTrackingTools(short message, WindowPtr theWindow, void *handlerRefCon, DragReference theDrag)
{	short				result;
	unsigned long		attributes;
	ItemReference		theItem;
	RgnHandle			theRgn;
	Point				theMouse, localMouse;
	FlavorFlags     	theFlags;
	long				textSize;
	HFSFlavor			myFlavor;
	FInfo				fndrInfo;
	char				str[ 5];
	
	if ((message != kDragTrackingEnterHandler) && (!canAcceptDrag)) return( noErr);
	
	SetPort( GetWindowPort(theWindow));
	
	GetDragAttributes(theDrag, &attributes);
	
	switch (message) {
			
		case kDragTrackingEnterHandler:
			canAcceptDrag = false;
			
			GetDragItemReferenceNumber(theDrag, 1, &theItem);
			result = GetFlavorFlags(theDrag, theItem, flavorTypeHFS, &theFlags);
			if (result == noErr)
			{
				GetFlavorDataSize( theDrag, theItem, flavorTypeHFS, &textSize);
				
				GetFlavorData( theDrag, theItem, flavorTypeHFS, &myFlavor, &textSize, 0);
				
				HSetVol( NULL, myFlavor.fileSpec.vRefNum, myFlavor.fileSpec.parID);
				HGetFInfo(myFlavor.fileSpec.vRefNum, myFlavor.fileSpec.parID, myFlavor.fileSpec.name, &fndrInfo);
				
				OSType2Ptr( fndrInfo.fdType, str);
				
				if (MADPlugAvailable( MADLib, str)) canAcceptDrag = true;
			}
			break;
			
		case kDragTrackingEnterWindow:
			
			break;
			
		case kDragTrackingInWindow:
			//
			//	We receive InWindow messages as long as the mouse is in one of our windows
			//	during a drag. We draw the window highlighting and blink the insertion caret
			//	when we get these messages.
			//
			
			GetDragMouse(theDrag, &theMouse, 0L);
			localMouse = theMouse;
			GlobalToLocal(&localMouse);
			
			//
			//	Show or hide the window highlighting when the mouse enters or leaves the
			//	TextEdit field in our window (we don't want to show the highlighting when
			//	the mouse is over the window title bar or over the scroll bars).
			//
			Rect dRect;
			GetDialogItem(myDialog, 0, 0, NULL, &dRect);
			
			if (PtInRect( localMouse, &dRect))
			{
				RectRgn(theRgn = NewRgn(), &dRect);
				
				ShowDragHilite(theDrag, theRgn, true);
				DisposeRgn(theRgn);
			}
			break;
			
		case kDragTrackingLeaveWindow:
			HideDragHilite( theDrag);
			break;
			
		case kDragTrackingLeaveHandler:
			
			break;
	}
	
	return(noErr);
}

pascal OSErr MyReceiveTools(WindowPtr theWindow, unsigned long handlerRefCon, DragReference theDrag)
{
	HFSFlavor			myFlavor;
	OSErr				result;
	unsigned short		items;
	ItemReference		theItem;
	DragAttributes		attributes;
	Size				textSize;
	short				index, mouseDownModifiers, mouseUpModifiers, moveInstru;
	FInfo				fndrInfo;
	OSErr				err;
	
	SetPort(GetWindowPort(theWindow));
	
	GetDragAttributes(theDrag, &attributes);
	GetDragModifiers(theDrag, 0L, &mouseDownModifiers, &mouseUpModifiers);
	
	moveInstru = (attributes & kDragInsideSenderWindow) &&  (!((mouseDownModifiers & optionKey) | (mouseUpModifiers & optionKey)));
	
	HideDragHilite( theDrag);
	
	//
	//	A file is coming...
	//
	
	CountDragItems(theDrag, &items);
	
	index = 1;		// Take only 1 file
	
	GetDragItemReferenceNumber(theDrag, index, &theItem);
	
	result = GetFlavorDataSize(theDrag, theItem, flavorTypeHFS, &textSize);
	
	if (result == noErr)
	{		
		GetFlavorData(theDrag, theItem, flavorTypeHFS, &myFlavor, &textSize, 0L);
		
		err = HGetFInfo( myFlavor.fileSpec.vRefNum, myFlavor.fileSpec.parID, myFlavor.fileSpec.name, &fndrInfo);
		
		if (err == noErr)
		{
			ReadAndPlayMusic( myFlavor.fileSpec.name, fndrInfo.fdType);
			
			return(noErr);
		}
	}
	
	return(dragNotAcceptedErr);
}

void SetDText (DialogPtr dlog, short item, Str255 str)
{
	Handle	itemHandle;
	short	itemType;
	Rect	itemRect;
	Str255	myStr;
	
	GetDialogItem (dlog, item, &itemType, &itemHandle, &itemRect);
	if (itemHandle == 0L) MyDebugStr( __LINE__, __FILE__, "Error in SetDText");
	
	GetDialogItemText (itemHandle, myStr);
	if (RelString( myStr, str, true, true) != 0) SetDialogItemText (itemHandle, str);
}

void SetUpSize()
{
#if 0
	Handle	hRsrc;
	long	fileSize;
	
	hRsrc = GetResource( 'SIZE', -1);
	if (hRsrc == 0L) Debugger();
	
	HNoPurge( hRsrc);
	HLock( hRsrc);
	
	fileSize = *((long*)((*hRsrc)+2));
	
	fileSize -= FreeMem();
	fileSize += 20000L;
	
	*((long*)((*hRsrc)+2)) = (long) fileSize;
	*((long*)((*hRsrc)+6)) = (long) fileSize;
	
	ChangedResource( hRsrc);
	WriteResource( hRsrc);
	
	HUnlock( hRsrc);
	HPurge( hRsrc);
	
	ReleaseResource( (Handle) hRsrc);
#endif
}

void	SKVolume( short vol)
{
	Point			tempL;
	long			*tL;
	
	if(vol>8) vol = 8;
	if(vol<0) vol = 0;
	
	if (vol > 0)
	{
		if (NewSoundManager)
		{
			tempL.v = vol * 32L;
			tempL.h = vol * 32L;
			
			tL = (long*) &tempL;
			SetDefaultOutputVolume( *tL);
		}
		//else SetSoundVol( vol - 1);
	}
	
}

short PressTypeMenu( short whichMenu, short item)
{
	long				mresult, r;
	Point				Zone;
	short				i, itemType, startitem;
	Handle				itemHandle;
	Rect				itemRect;
	MenuHandle			MenuDriver;
	Str255				str;
	MADDriverSettings	Init;
	
	MenuDriver = GetMenu( whichMenu);
	
	for (i = 0 ; i < CountMenuItems( MenuDriver); i++) EnableMenuItem( MenuDriver, i+1);
	
	InsertMenu( MenuDriver, hierMenu);
	
	GetDialogItem( myDialog, item, &itemType, &itemHandle, &itemRect);
	Zone.h = itemRect.left;	Zone.v = itemRect.top;
	
	LocalToGlobal( &Zone);
	
	//Uhh... what?
#define ApFontID	*((short*) 0x984)
	
	i = ApFontID;	ApFontID = 4;
	
	switch( whichMenu)
	{
		case 128:
			if (!Stereo) DisableMenuItem( MenuDriver, 2);
			
			
			if (MADDriver->DriverSettings.surround) startitem = 2;
			else startitem = 1;
			break;
			
		case 165:
			if (!Stereo)
			{
				for (i = 1 ; i < CountMenuItems( MenuDriver); i++) DisableMenuItem( MenuDriver, i+1);
			}
			
			if (MADDriver->DriverSettings.MicroDelaySize)
			{
				startitem = 2;
				for (i = 1 ; i < CountMenuItems( MenuDriver); i++)
				{
					Str255	str;
					long	r;
					
					GetMenuItemText( MenuDriver, i+1, str);
					
					str[ 0] -= 3;	StringToNum( str, &r);	str[ 0] += 3;
					
					if (r == MADDriver->DriverSettings.MicroDelaySize) startitem = i+1;
				}
			}
			else startitem = 1;
			break;
			
		case 170:
			if (!Stereo)
			{
				for (i = 1 ; i < CountMenuItems( MenuDriver); i++) DisableMenuItem( MenuDriver, i+1);
			}
			
			if (MADDriver->DriverSettings.Reverb)
			{
				startitem = 2;
				for (i = 1 ; i < CountMenuItems( MenuDriver); i++)
				{
					Str255	str;
					long	r;
					
					GetMenuItemText( MenuDriver, i+1, str);
					
					str[ 0] -= 3;	StringToNum( str, &r);	str[ 0] += 3;
					
					if (r == MADDriver->DriverSettings.ReverbSize) startitem = i+1;
				}
			}
			else startitem = 1;
			break;
			
		case 169:
			startitem = 2;
			for (i = 0 ; i < CountMenuItems( MenuDriver); i++)
			{
				Str255	str;
				long	r;
				
				GetMenuItemText( MenuDriver, i+1, str);
				
				str[ 0] -= 2;	StringToNum( str, &r);	str[ 0] += 2;
				
				if (r == MADDriver->DriverSettings.ReverbStrength) startitem = i+1;
			}
			break;
			
		case 131:
			if (!has16Bit) { DisableMenuItem( MenuDriver, 3);}
			
			switch( MADDriver->DriverSettings.outPutRate)
		{
			default:
			case rate11khz:		startitem = 1;	break;
			case rate22khz:		startitem = 2;	break;
			case rate44khz:		startitem = 3;	break;
		}
			break;
			
		case 132:
			if (!Stereo) { DisableMenuItem( MenuDriver, 2);		DisableMenuItem( MenuDriver, 3);}
			
			switch( MADDriver->DriverSettings.outPutMode)
		{
			default:
				//	case MonoOutPut:			startitem = 1;	break;
				//	case StereoOutPut:			startitem = 2;	break;
			case DeluxeStereoOutPut:	startitem = 3;	break;
		}
			break;
			
		case 133:
			if (!has16Bit) { DisableMenuItem( MenuDriver, 2); }
			
			switch( MADDriver->DriverSettings.outPutBits)
		{
			default:
			case 8:			startitem = 1;			break;
			case 16:		startitem = 2;			break;
		}
			break;
	}
	mresult = PopUpMenuSelect(	MenuDriver,
							  Zone.v,
							  Zone.h,
							  startitem);
	ApFontID = i;
	MenuID TheMenu = GetMenuID(MenuDriver);
	
	if (HiWord( mresult) != 0)
	{
		Boolean	IsPlaying;
		long	fulltime, curTime;
		
		Init = MADDriver->DriverSettings;
		
		IsPlaying = MADDriver->Reading;
		MADDriver->Reading = false;
		
		MADGetMusicStatus( MADDriver, &fulltime, &curTime);
		
		if (MADStopDriver( MADDriver) != noErr) 			ExitToShell();
		if (MADDisposeDriver( MADDriver) != noErr) 	ExitToShell();
		
		MADDriver = NULL;
		
		switch( whichMenu)
		{
			case 131:
				switch( LoWord( mresult))
			{
				default:
				case 1:	Init.outPutRate = rate11khz;	break;
				case 2:	Init.outPutRate = rate22khz;	break;
				case 3:	Init.outPutRate = rate44khz;	break;
			}
				break;
				
			case 132:
				switch( LoWord( mresult))
			{
				default:
					//	case 1:	Init.outPutMode = MonoOutPut;			break;
					//	case 2: Init.outPutMode = StereoOutPut;		break;
				case 3: Init.outPutMode = DeluxeStereoOutPut;	break;
			}
				break;
				
			case 133:
				switch( LoWord( mresult))
			{
				default:
				case 1:		Init.outPutBits = 8;	break;
				case 2:		Init.outPutBits = 16;	break;
			}
				break;
				
			case 128:
				switch( LoWord( mresult))
			{
				default:
				case 1:		Init.surround = false;	break;
				case 2:		Init.surround = true;	break;
			}
				break;
				
			case 165:
				if (LoWord( mresult) == 1)
				{
					Init.MicroDelaySize = 0;
				}
				else
				{
					GetMenuItemText( MenuDriver, LoWord( mresult), str);
					str[ 0] -= 3;
					StringToNum( str, &r);
					str[ 0] += 3;
					
					if (r >= 0 && r <= 1000) Init.MicroDelaySize = r;
				}
				break;
				
			case 170:
				if (LoWord( mresult) == 1)
				{
					Init.Reverb = false;
				}
				else
				{
					Init.Reverb = true;
					
					GetMenuItemText( MenuDriver, LoWord( mresult), str);
					str[ 0] -= 3;
					StringToNum( str, &r);
					str[ 0] += 3;
					
					if (r >= 0 && r <= 1000) Init.ReverbSize = r;
				}
				break;
				
			case 169:
				GetMenuItemText( MenuDriver, LoWord( mresult), str);
				str[ 0] -= 2;
				StringToNum( str, &r);
				str[ 0] += 2;
				
				if (r >= 0 && r <= 1000) Init.ReverbStrength = r;
				break;
				
				
		}
		if (MADCreateDriver( &Init, MADLib, &MADDriver)  != noErr) ExitToShell();
		
		InitDriverString();
		
		MADAttachDriverToMusic( MADDriver, music, NULL);
		
		MADStartDriver( MADDriver);
		
		MADSetMusicStatus( MADDriver, 0, fulltime, curTime);
		
		if (IsPlaying) MADDriver->Reading = true;
		DeleteMenu( TheMenu);
		return 0;
	}
	else
	{
		DeleteMenu( TheMenu);
		return -1;
	}
}

long	oldTempL;

void DrawTimeBar()
{
	Rect   		itemRect;
	long		tempL;
	long		fullTime, curTime;
	short		itemType, i;
	Handle		itemHandle;
	Str255		aStr;
	
	GetDialogItem( myDialog, 4, &itemType, &itemHandle, &itemRect);
	
	MADGetMusicStatus( MADDriver, &fullTime, &curTime);
	
	tempL = ((long) (itemRect.right - itemRect.left) * curTime) / fullTime;
	
	if (tempL != oldTempL)
	{
		oldTempL = tempL;
		
		
		i = itemRect.right;
		itemRect.right = itemRect.left + tempL;
		if (itemRect.right > i) itemRect.right = i;
		
		ForeColor( blackColor);
		PaintRect( &itemRect);
		
		itemRect.left = itemRect.right;
		itemRect.right = i;
		
		ForeColor( whiteColor);							
		PaintRect( &itemRect);
		ForeColor( blackColor);
		
		/**/
		
		GetDialogItem (myDialog, 4, &itemType, &itemHandle, &itemRect);
		MoveTo( itemRect.left + 2, itemRect.bottom - 1);
		TextMode( srcXor);
		if (tempL > 100) tempL = 100;
		NumToString( tempL, aStr);	pStrcat( aStr, "\p %");
		DrawString( aStr);
		TextMode( srcCopy);
		
		/**/
		
		ForeColor( blackColor);
	}
}

void MegaLoop()
{
	char			theChar;
	EventRecord		theEvent;
	short			thePart, whichItem, oldH = 0;
	WindowPtr		whichWindow;
	short			itemType, temp;
	Rect			itemRect;
	Handle			itemHandle;
	DialogPtr		whichDialog;
	Point			Location, myPt;
	GrafPtr			savePort;
	Str255			aStr;
	
	do
	{
		WaitNextEvent( everyEvent, &theEvent, 60L, 0L);
		
		if (theEvent.what == nullEvent)
		{
			GetPort( &savePort);
			SetPort( GetDialogPort(myDialog));
			
			DrawTimeBar();
			
			//SetCursor( &qd.arrow); TODO:FIXME?
			
			SetPort( savePort);
		}
		else if (theEvent.what == mouseDown)
		{
			switch( thePart = FindWindow( theEvent.where, &whichWindow))
			{
				case inContent:
					if (DialogSelect( &theEvent, &whichDialog, &whichItem ))
					{
						GetPort( &savePort);
						SetPort( GetDialogPort(whichDialog));
						
						switch( whichItem)
						{
							case 2:
								do
								{
									GetDialogItem (whichDialog, 2, &itemType, &itemHandle, &itemRect);
									GetMouse( &Location);
									
									if (oldH != Location.h)
									{
										oldH = Location.h;
										
										if (Location.h < itemRect.left) 		Location.h = itemRect.left;
										else if ( Location.h > itemRect.right)	Location.h = itemRect.right;
										
										VolumeH = Location.h - itemRect.left;
										itemRect.right = Location.h;
										PaintRect( &itemRect);
										GetDialogItem (whichDialog, 2, &itemType, &itemHandle, &itemRect);
										itemRect.left = Location.h;
										
										ForeColor( whiteColor);							
										PaintRect( &itemRect);
										ForeColor( blackColor);
										
										/**/
										
										GetDialogItem (whichDialog, 2, &itemType, &itemHandle, &itemRect);
										MoveTo( itemRect.left + 2, itemRect.bottom - 1);
										TextMode( srcXor);
										NumToString( VolumeH, aStr);	pStrcat( aStr, "\p %");
										DrawString( aStr);
										TextMode( srcCopy);
										
										/**/
										
										GetDialogItem (whichDialog, 2, &itemType, &itemHandle, &itemRect);
										
										SKVolume( 1 + (8 * (Location.h - itemRect.left) / (itemRect.right - itemRect.left)));
									}
								}while (Button());
								break;
								
							case 18:
								do
								{
									GetDialogItem (whichDialog, 18, &itemType, &itemHandle, &itemRect);
									GetMouse( &Location);
									
									if (oldH != Location.h)
									{
										oldH = Location.h;
										
										if (Location.h < itemRect.left) 		Location.h = itemRect.left;
										else if ( Location.h > itemRect.right)	Location.h = itemRect.right;
										
										SpeedH = Location.h - itemRect.left;
										itemRect.right = Location.h;
										PaintRect( &itemRect);
										GetDialogItem (whichDialog, 18, &itemType, &itemHandle, &itemRect);
										itemRect.left = Location.h;
										
										ForeColor( whiteColor);							
										PaintRect( &itemRect);
										ForeColor( blackColor);
										
										/**/
										
										GetDialogItem (whichDialog, 18, &itemType, &itemHandle, &itemRect);
										MoveTo( itemRect.left + 2, itemRect.bottom - 1);
										TextMode( srcXor);
										NumToString( SpeedH*2, aStr);		pStrcat( aStr, "\p %");
										DrawString( aStr);
										TextMode( srcCopy);
										
										/**/
										
										GetDialogItem (whichDialog, 2, &itemType, &itemHandle, &itemRect);
										
										MADDriver->VExt = (160 * (Location.h - itemRect.left)) / (itemRect.right - itemRect.left);
										if (MADDriver->VExt <= 0) MADDriver->VExt = 1;
									}
								}while (Button());
								break;
								
								
							case 4:
								temp = -1;
								do
								{
									GetDialogItem( whichDialog , 4, &itemType, &itemHandle, &itemRect);
									GetMouse( &myPt);
									
									if (temp != myPt.h)
									{
										if (myPt.h < itemRect.left) 		myPt.h = itemRect.left;
										else if ( myPt.h > itemRect.right)	myPt.h = itemRect.right;
										
										MADSetMusicStatus( MADDriver, itemRect.left, itemRect.right, myPt.h);
										
										DrawTimeBar();
										
										temp = myPt.h;
									}
								}while (Button());
								break;
								
							case 21:	PressTypeMenu( 129, whichItem);	break;
							case 23:	PressTypeMenu( 128, whichItem);	break;
							case 30:	PressTypeMenu( 130, whichItem);	break;
							case 8:		PressTypeMenu( 165, whichItem);	break;
							case 25:	PressTypeMenu( 170, whichItem);	break;
							case 28:	PressTypeMenu( 169, whichItem);	break;
							case 11:	PressTypeMenu( 131, whichItem);	break;
							case 14:	PressTypeMenu( 132, whichItem);	break;
							case 17:	PressTypeMenu( 133, whichItem);	break;
						}
						
						SetPort( savePort);
					}
					break;
					
				case inGoAway:
					if (TrackGoAway( whichWindow, theEvent.where)) OnTourne = false;
					break;
					
				case inDrag:
					DragWindow( whichWindow, theEvent.where, NULL);
					break;
					
				case inSysWindow:
					//SystemClick( &theEvent, whichWindow);
					break;
					
				case inZoomIn:
				case inZoomOut:
					if (TrackBox( whichWindow, theEvent.where, thePart))
					{
						Rect theDiagBounds;
						GetWindowBounds(whichWindow, kWindowContentRgn, &theDiagBounds);
						
						if (theDiagBounds.bottom > 29) SizeWindow( whichWindow, theDiagBounds.right, 29, true);
						else SizeWindow( whichWindow, theDiagBounds.right, 100, true); 
					}
					break;
			}
		}
		else if (theEvent.what == updateEvt)
		{
			BeginUpdate( GetDialogWindow(myDialog));
			DrawDialog( myDialog);
			SetPort( GetDialogPort(myDialog));
			
			/*** Volume ***/
			GetDialogItem (myDialog, 2, &itemType, &itemHandle, &itemRect);
			itemRect.left-=1;		//itemRect.right+=1;
			itemRect.top-=1;		//itemRect.bottom+=1;
			FrameRectRelief( &itemRect);
			
			GetDialogItem (myDialog, 2, &itemType, &itemHandle, &itemRect);
			Location.h = VolumeH + itemRect.left;
			itemRect.right = Location.h;
			PaintRect( &itemRect);
			GetDialogItem (myDialog, 2, &itemType, &itemHandle, &itemRect);
			itemRect.left = Location.h;
			
			ForeColor( whiteColor);							
			PaintRect( &itemRect);
			ForeColor( blackColor);
			
			/**/
			
			GetDialogItem (myDialog, 2, &itemType, &itemHandle, &itemRect);
			MoveTo( itemRect.left + 2, itemRect.bottom - 1);
			TextMode( srcXor);
			NumToString( VolumeH, aStr);	pStrcat( aStr, "\p %");
			DrawString( aStr);
			TextMode( srcCopy);
			
			/**/
			/*****/
			
			/*** Speed **/
			
			GetDialogItem (myDialog, 18, &itemType, &itemHandle, &itemRect);
			itemRect.left-=1;		//itemRect.right+=1;
			itemRect.top-=1;		//itemRect.bottom+=1;
			FrameRectRelief( &itemRect);
			
			GetDialogItem (myDialog, 18, &itemType, &itemHandle, &itemRect);
			Location.h = SpeedH + itemRect.left;
			itemRect.right = Location.h;
			PaintRect( &itemRect);
			GetDialogItem (myDialog, 18, &itemType, &itemHandle, &itemRect);
			itemRect.left = Location.h;
			
			ForeColor( whiteColor);							
			PaintRect( &itemRect);
			ForeColor( blackColor);
			
			/**/
			
			GetDialogItem (myDialog, 18, &itemType, &itemHandle, &itemRect);
			MoveTo( itemRect.left + 2, itemRect.bottom - 1);
			TextMode( srcXor);
			NumToString( SpeedH*2, aStr);		pStrcat( aStr, "\p %");
			DrawString( aStr);
			TextMode( srcCopy);
			
			/**/
			
			/*** Position ***/
			GetDialogItem (myDialog, 4, &itemType, &itemHandle, &itemRect);
			itemRect.left-=1;		//itemRect.right+=1;
			itemRect.top-=1;		//itemRect.bottom+=1;
			FrameRectRelief( &itemRect);
			
			DrawTimeBar();
			/*****/
			
			
			EndUpdate( GetDialogWindow(myDialog));
		}
		else if (theEvent.what == keyDown)
		{
			theChar=theEvent.message & charCodeMask;
			
			if ((theEvent.modifiers & cmdKey) !=0)
			{
				if (theChar =='Q' ||
				   theChar =='q' ||
				   theChar == 'w' ||
				   theChar == 'W' ||
				   theChar =='f' ||
				   theChar == 'F')
					
					OnTourne = false;
			}
		}
		else if (theEvent.what == kHighLevelEvent) AEProcessAppleEvent (&theEvent);
	}while (OnTourne == true);
}

extern	Boolean		Reading;

void MusiqueDriverInit(void)
{
	OSErr				iErr;
	MADDriverSettings	init;
	
	init.numChn			= 4;
	init.outPutBits		= thePrefs.outPutBits;
	init.outPutRate		= thePrefs.outPutRate;
	init.outPutMode		= thePrefs.outPutMode;
	init.driverMode		= thePrefs.driverMode;
	init.sysMemory		= true;
	init.repeatMusic 	= true;
	init.surround		= thePrefs.surround;
	init.MicroDelaySize	= thePrefs.MicroDelaySize;
	init.Reverb			= thePrefs.Reverb;
	init.ReverbSize		= thePrefs.ReverbSize;
	init.ReverbStrength	= thePrefs.ReverbStrength;
	init.TickRemover	= thePrefs.TickRemover;
	init.oversampling	= thePrefs.oversampling;
	
	iErr = MADCreateDriver( &init, MADLib, &MADDriver);
	if (iErr) MyDebugStr( __LINE__, __FILE__, "MusicDriver ERROR !");
}

void DoPreferences2()
{
	OSErr			iErr;
	short			vRefNum, fRefNum, SvRefNum;
	long			DirID, inOutBytes, SdirID;
	Boolean			initOK;
	
	HGetVol( 0L, &SvRefNum,&SdirID);
	
	initOK = false;
	
	iErr = FindFolder( kOnSystemDisk, kPreferencesFolderType, kCreateFolder, &vRefNum, &DirID);
	
	iErr = HSetVol( NULL, vRefNum, DirID);
	
	FSSpec PrefsSpec;
	
	FSMakeFSSpec(vRefNum, DirID, PLAYERPREF, &PrefsSpec);
	
	iErr = FSpOpenDF(&PrefsSpec, fsCurPerm, &fRefNum);
	if (iErr == noErr)
	{
		inOutBytes = sizeof( Prefs);
		iErr = FSRead( fRefNum, &inOutBytes, &thePrefs);
		iErr = FSClose( fRefNum);
		
		if (thePrefs.Version >= 0x0500)
		{
			MusiqueDriverInit();
			initOK = true;
		}
	}
	
	if (initOK == false)
	{
		OSErr				iErr;
		MADDriverSettings	init;
		
		init.numChn			= 4;
		init.outPutBits		= 16;
		init.outPutRate		= rate44khz;
		init.outPutMode		= DeluxeStereoOutPut;
		init.driverMode		= SoundManagerDriver;
		init.sysMemory		= true;
		init.repeatMusic 	= true;
		init.surround		= true;
		init.MicroDelaySize	= 0;
		init.Reverb			= false;
		init.ReverbSize		= 0;
		init.ReverbStrength	= 0;
		init.TickRemover	= true;
		init.oversampling	= 1;
		
		iErr = MADCreateDriver( &init, MADLib, &MADDriver);
		if (iErr) MyDebugStr( __LINE__, __FILE__, "MusicDriver ERROR !");
	}
	
	HSetVol( 0L, SvRefNum, SdirID);
}

int main()
{
	long			gestaltAnswer;
	int				myBit, OldVol;
	short			itemType;
	Rect			itemRect;
	Handle			itemHandle;
	Point			Svolume;
	
	FlushEvents(everyEvent, 0);
	InitCursor();	
	
	oldTempL = -100;
	OnTourne = true;
	MADInitLibrary(NULL, true, &MADLib);
	
	workPixPat = GetPixPat( 130);
	
	InstallAE();
	
	myDialog = GetNewDialog( diaNum, NULL, (WindowPtr)  -1L);
	Rect theDiagBounds;
	GetWindowBounds(GetDialogWindow(myDialog), kWindowContentRgn, &theDiagBounds);
	SizeWindow( GetDialogWindow(myDialog), theDiagBounds.right, 29, true);
	ShowWindow( GetDialogWindow(myDialog));
	SetPort( GetDialogPort(myDialog));
	TextFont( kFontIDGeneva);
	TextSize( 9);
	
	GetDialogItem ( myDialog, 1, &itemType, &itemHandle, &itemRect);
	
	Gestalt( gestaltSoundAttr, &gestaltAnswer);
	myBit = gestaltStereoCapability;
	Stereo = BitTst( &gestaltAnswer, 31-myBit);
	
	myBit = gestalt16BitSoundIO;
	has16Bit = BitTst( &gestaltAnswer, 31-myBit);
	
	NewSoundManager = true;
	
	//	if (NewSoundManager)
	{
		GetDefaultOutputVolume( (long*) &Svolume);
		Svolume.h /= 32;
		OldVol = Svolume.h - 1;
	}
	//	else
	//	{
	//		GetSoundVol((void *)&OldVol);
	//	}
	GetDialogItem (myDialog, 2, &itemType, &itemHandle, &itemRect);
	VolumeH 	= (OldVol + 1)*( itemRect.right - itemRect.left) / 8;
	SpeedH 		= 50;
	
	SetWTitle( GetDialogWindow(myDialog), LMGetCurApName());
	
	DoPreferences2();
	
	InitDriverString();
	
	if (MADLoadMusicRsrc( &music, 'MADI', 3214) != noErr)
	{
		ExitToShell();
	}
	
	//#ifndef THINK_C
#if 0
	MyTrackingUPP = NewDragTrackingHandlerProc( MyTrackingTools);
	MyReceiveUPP = NewDragReceiveHandlerProc( MyReceiveTools);
	
	InstallTrackingHandler( (DragTrackingHandlerUPP) MyTrackingUPP, myDialog, (void *) 0L);
	InstallReceiveHandler( (DragReceiveHandlerUPP) MyReceiveUPP, myDialog, (void *) 0L);
#endif
	
	
	/* Music is turn ON !! */
	
	MADAttachDriverToMusic( MADDriver, music, NULL);
	
	MADStartDriver( MADDriver);
	MADDriver->Reading = true;
	
	MegaLoop();
	
	SetUpSize();
	
	MADStopDriver( MADDriver);
	MADDisposeMusic( &music, MADDriver);
	MADDisposeDriver( MADDriver);
	MADDisposeLibrary( MADLib);
	
	FlushEvents( everyEvent, 0);
	return EXIT_SUCCESS;
}

