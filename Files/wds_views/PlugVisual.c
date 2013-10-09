//TODO: rewrite this plug-in architecture!
#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include <stdio.h>

#include "VisualPlugin.h"
#include "SoundJamAPI.h"
#include "ScreenDevice.h"

	extern	EventRecord				theEvent;
	extern	Cursor					HandCrsr;
	extern	MenuHandle				ViewsMenu;
	extern	DialogPtr				AHelpDlog;
	extern	WindowPtr				oldWindow;
	
			DialogPtr				VisualDlog;
			RenderVisualData		currentData;

//	static	short*					sampleBuffer;
//	static	long					numSamples;
	static	Boolean					ReadyToProcess = false;


long GetDisplayID( long inDeviceNum );

typedef struct
{
	CFragConnectionID				connID;
	Str63						MenuName;
	FSSpec						file;
	PluginMessageInfo			msgInfo;
	
//	VisualPluginMessageInfo		msgVisual;
	PlayerMessageInfo			msgPlayer;
	struct	PlayerSetDefaultWindowSizeMessage winSize;
	void						*refCon;
	
}	VisualInfo;

static	VisualInfo 	*VisualPlug;
static	short		tPlug, wantedHeight, wantedWidth;
static	short		currentID;

void CallVisualFonction( MADDriverRec*, short PlugNo, OSType msg, CGrafPtr port, short*, long);
short GetCurrentID( void);
void CloseVisual(void);
void MySizeWindow( DialogPtr dlg, short right, short bottom, Boolean v);
Ptr GetAudioSourceSpectrum( short item);
Ptr GetAudioChannel( Boolean LeftChannel, long Size);
Ptr MakeCalculusSpectrum( Ptr srcPtr, Boolean logScale);
void ProcessVisualPlug( MADDriverRec *intDriver, short* in, long inNum);
void DoFullScreenNow( WindowPtr mWind);
Boolean EnterFullscreen( long inDispID, Point ioSize, int inBitDepth, WindowPtr inWin, long inFreq );
GrafPtr BeginFrame();
void EndFrame();
void ExitFullscreen();
OSErr GetApplicationPackageFSSpecFromBundle(FSSpecPtr theFSSpecPtr);

//#if MACOS9VERSION

void DoFullScreenNow( WindowPtr mWind)
{
	Point			mFullscreenSize;
	long			mFullscreenDepth, mFullscreenFreq, mFullscreenDevice;
	GrafPtr			currentPort;
	Boolean			success;
	
	mFullscreenSize.h	= 640;
	mFullscreenSize.v	= 480;
	mFullscreenDepth	= 32;
	mFullscreenDevice	= 0;
	mFullscreenFreq		= 0;
	
	success = EnterFullscreen( GetDisplayID( mFullscreenDevice ), mFullscreenSize, mFullscreenDepth, mWind, mFullscreenFreq );
	
	if( success)
	{
		currentPort = BeginFrame();
		
		HideCursor();
		
		CallVisualFonction( MADDriver, GetCurrentID(), kVisualPluginSetWindowMessage, (CGrafPtr) currentPort, 0, 0);
		CallVisualFonction( MADDriver, GetCurrentID(), kVisualPluginUpdateMessage, (CGrafPtr) currentPort, 0, 0);
		CallVisualFonction( MADDriver, GetCurrentID(), kVisualPluginEnableMessage, (CGrafPtr) currentPort, 0, 0);
		
		while( !Button()) 
		{
			CallVisualFonction( MADDriver, GetCurrentID(), 'vrnd', (CGrafPtr) currentPort, 0, 0);
			
			WaitNextEvent( everyEvent, &theEvent, 1, NULL);
			if( theEvent.what == keyDown) CallVisualFonction( MADDriver, GetCurrentID(), 'vevt', (CGrafPtr) currentPort, 0, 0);
			
			if( MADDriver->musicEnd == true) DoLoadOtherMusic( true);
		}
		EndFrame();
		
		ExitFullscreen();
		
		ShowCursor();
	}
	
	FlushEvents( 0xFFFF, 0 );
}


short GetCurrentID()
{
	return currentID;
}

void VisualFullScreen( void)
{
	Boolean	ActiveHelpActive;
	
	if( AHelpDlog != NULL) ActiveHelpActive = true;
	else ActiveHelpActive = false;
	
	CloseAHelp();
	
	HideWindow( GetDialogWindow( VisualDlog));
	
	SelectWindow( GetDialogWindow( ToolsDlog));
	CheckShowWindow();
			
	DoFullScreenNow( GetDialogWindow( VisualDlog));
	
	CallVisualFonction( MADDriver, currentID, kVisualPluginSetWindowMessage, 0, NULL, 0);
	
	if( ActiveHelpActive) CreateAHelpWindow();
	
	ShowWindowPref( -1);
	
	ShowWindow( GetDialogWindow( ToolsDlog));
	SelectWindow( GetDialogWindow( ToolsDlog));
	SelectWindow2( NextWindowVisible( GetDialogWindow( ToolsDlog)));
	
	ShowWindow( GetDialogWindow( VisualDlog));
	SelectWindow2( GetDialogWindow( VisualDlog));
	
	UpdateALLWindow();
}

void DoGrowVisual(void)
{
	long		lSizeVH;
	GrafPtr		SavePort;
	Rect		temp, cellRect;
	Point		theCell = { 0, 0}, aPt = { 0, 0};
	short		tempA, tempB;
	Handle		itemHandle;
	short		itemType;
	Rect		caRect;
	BitMap		screenBits;
	
	GetPort( &SavePort);
 	SetPortDialogPort( VisualDlog);

	LocalToGlobal( &aPt);
	
	GetQDGlobalsScreenBits( &screenBits);
	
	temp.left = VisualPlug[ currentID].winSize.minWidth;
	temp.top = VisualPlug[ currentID].winSize.minHeight;
	temp.right = VisualPlug[ currentID].winSize.maxWidth;
	temp.bottom = VisualPlug[ currentID].winSize.maxHeight;
	
	if( VisualPlug[ currentID].winSize.minWidth == VisualPlug[ currentID].winSize.maxWidth &&
		VisualPlug[ currentID].winSize.minHeight == VisualPlug[ currentID].winSize.maxHeight)
		{
			return;
		}
	
	lSizeVH = 0;
	if( theEvent.what == mouseDown) lSizeVH = GrowWindow( GetDialogWindow( VisualDlog), theEvent.where, &temp);
	
	if( lSizeVH != 0)
	{
		tempA = LoWord( lSizeVH);
		tempB = HiWord( lSizeVH);
		
		wantedHeight = tempB;
		wantedWidth = tempA;
	
		CallVisualFonction( MADDriver, currentID, kVisualPluginResizeMessage, 0, NULL, 0);
		CallVisualFonction( MADDriver, currentID, kVisualPluginSetWindowMessage, 0, NULL, 0);
	
		GetPortBounds( GetDialogPort( VisualDlog), &caRect);
	
		EraseRect( &caRect);
		InvalWindowRect( GetDialogWindow( VisualDlog), &caRect);
	}
	else
	{
	}
	
	
	SetPort( SavePort);
}

OSStatus PlayerPROProc(void *appCookie, OSType message, struct PlayerMessageInfo *messageInfo)
{
	switch( message)
	{
		case kPlayerRegisterVisualPluginMessage:
			VisualPlug[ currentID].msgPlayer = *messageInfo;
			
			VisualPlug[ currentID].winSize.defaultWidth		= VisualPlug[ currentID].winSize.minWidth 	= 	messageInfo->u.registerVisualPluginMessage.minWidth;
			VisualPlug[ currentID].winSize.defaultHeight	= VisualPlug[ currentID].winSize.minHeight = 	messageInfo->u.registerVisualPluginMessage.minHeight;
			
			VisualPlug[ currentID].winSize.maxWidth = 	messageInfo->u.registerVisualPluginMessage.maxWidth;
			VisualPlug[ currentID].winSize.maxHeight = messageInfo->u.registerVisualPluginMessage.maxHeight;
		break;
		
		case kPlayerGetPluginDataMessage:
			messageInfo->u.getPluginDataMessage.dataSize = 0;
		break;
		
		case kPlayerSetDefaultWindowSizeMessage:
			VisualPlug[ currentID].winSize = messageInfo->u.setDefaultWindowSizeMessage;
		break;
		
		default:
			return -1;
		break;
	}
	
	return noErr;
}

void CallVisualMain( long PlugNo, OSType msg)
{
	PluginProcPtr		mainPLUG;
	OSStatus			myErr;
	Str255				errName;
	short				fileID;

	fileID = FSpOpenResFile( &VisualPlug[ PlugNo].file, fsCurPerm);
	
	myErr = GetDiskFragment( &VisualPlug[ PlugNo].file, 0, kCFragGoesToEOF, VisualPlug[ PlugNo].file.name, kLoadCFrag, &VisualPlug[ PlugNo].connID, (Ptr *) &mainPLUG, errName);

	if( myErr == noErr)
	{
		
		VisualPlug[ PlugNo].msgInfo.u.initMessage.playerProc = PlayerPROProc;
		VisualPlug[ PlugNo].msgInfo.u.initMessage.appCookie = (void*) PlugNo;
		
	//	VisualPlug[ PlugNo].msgVisual.u.initMessage.playerProc = PlayerPROProc;
	//	VisualPlug[ PlugNo].msgVisual.u.initMessage.appCookie = (void*) PlugNo;
		
		myErr = mainPLUG ( 	msg,
							&VisualPlug[ PlugNo].msgInfo,
							&VisualPlug[ PlugNo].msgInfo.u.initMessage.refcon);		
		
		DisposePtr( (Ptr) mainPLUG);
	}
	
	CloseResFile( fileID);
}

void CallVisualFonction( MADDriverRec *intDriver, short PlugNo, OSType msg, CGrafPtr port, short* sampleBuffer, long numSamples)
{
	short 						fileID;
	OSStatus					Err;
	long						options = 0, i;
	VisualPluginMessageInfo		msgVisual;
	Rect						caRect;
	
	if( msg != kVisualPluginProcessSamplesMessage)
	{
		if( port)
		{
			//port = NULL;
			options = kWindowIsFullScreen;
		}
		else port = GetDialogPort( VisualDlog);
		
		fileID = FSpOpenResFile( &VisualPlug[ PlugNo].file, fsCurPerm);
		UseResFile( fileID);
	}
	
	switch( msg)
	{
		case kVisualPluginInitializeMessage:
		//	msgVisual.u.initMessage.appCookie = VisualDlog;
			msgVisual.u.initMessage.playerProc = PlayerPROProc;
		break;
		
		case kVisualPluginPlayMessage:
			msgVisual.u.playMessage.volume = 1;
			msgVisual.u.playMessage.bitRate = rate44khz;
			msgVisual.u.playMessage.soundFormat.flags = 0;
			msgVisual.u.playMessage.soundFormat.format = 'NONE';
			msgVisual.u.playMessage.soundFormat.numChannels = 2;
			msgVisual.u.playMessage.soundFormat.sampleSize = 16;
			msgVisual.u.playMessage.soundFormat.sampleRate = rate44khz;
			msgVisual.u.playMessage.soundFormat.sampleCount = 0;
			msgVisual.u.playMessage.soundFormat.buffer = 0;
			msgVisual.u.playMessage.soundFormat.reserved = 0;
		break;
		
		case kVisualPluginShowWindowMessage:
			GetPortBounds( port, &caRect);
			
			msgVisual.u.showWindowMessage.port = port;
			msgVisual.u.showWindowMessage.drawRect = caRect;
			msgVisual.u.showWindowMessage.options = options;
		break;
		
		case kVisualPluginSetWindowMessage:
			GetPortBounds( port, &caRect);
			
			msgVisual.u.setWindowMessage.port = port;
			msgVisual.u.setWindowMessage.drawRect = caRect;
			msgVisual.u.setWindowMessage.options = options;
		break;
		
		case kVisualPluginEventMessage:
			msgVisual.u.eventMessage.event = &theEvent;
		break;
		
		case kVisualPluginRenderMessage:
		{
			Byte		*Wave, *Spectre;
			long 		i;
		
			currentData.numWaveformChannels = 2;
			currentData.numSpectrumChannels = 2;
			
			Wave = (Byte*) GetAudioChannel( true, intDriver->ASCBUFFERReal);
			Spectre = (Byte*) MakeCalculusSpectrum( GetAudioSourceSpectrum( 1), false);
			
			for( i = 0; i < kVisualNumWaveformEntries; i++)
			{
				currentData.waveformData[ 0][ i] = Wave[ i];
				currentData.spectrumData[ 0][ i] = Spectre[ (i * 256) / kVisualNumSpectrumEntries];
			}
			
			Wave = (Byte*) GetAudioChannel( false, intDriver->ASCBUFFERReal);
			Spectre = (Byte*) MakeCalculusSpectrum( GetAudioSourceSpectrum( 0), false);

			for( i = 0; i < kVisualNumWaveformEntries; i++)
			{
				currentData.waveformData[ 1][ i] = Wave[ i];
				currentData.spectrumData[ 1][ i] = Spectre[ (i * 256) / kVisualNumSpectrumEntries];
			}
		
		}
			msgVisual.u.renderMessage.renderData = &currentData;
			msgVisual.u.renderMessage.timeStampID = TickCount();
		break;
		
		case kVisualPluginUpdateMessage:
		
		break;
		
		case kVisualPluginResizeMessage:
			msgVisual.u.resizeMessage.approvedWidth = msgVisual.u.resizeMessage.desiredWidth = wantedWidth;
			msgVisual.u.resizeMessage.approvedHeight = msgVisual.u.resizeMessage.desiredHeight = wantedHeight;
		break;
		
		case kVisualPluginProcessSamplesMessage:
			msgVisual.u.processSamplesMessage.timeStampID		=	TickCount();
			msgVisual.u.processSamplesMessage.sampleBuffer		=	sampleBuffer;
			msgVisual.u.processSamplesMessage.numSamples		=	numSamples;
			msgVisual.u.processSamplesMessage.maxSamples		=	numSamples;
			msgVisual.u.processSamplesMessage.numOutputSamples	= 	numSamples;
			
		/*	for( i = 0; i < numSamples; i++)
			{
				sampleBuffer[ i] -= 0x8000;
			}*/
		break;
	}

	Err = VisualPlug[ PlugNo].msgPlayer.u.registerVisualPluginMessage.handler(msg, &msgVisual, VisualPlug[ PlugNo].refCon);
	
	if( Err )
	{
//		Debugger();//Erreur( Err, Err);
	}
	else
	{
		switch( msg)		// OutPut
		{
			case kVisualPluginProcessSamplesMessage:
			{
			//	long temp = msgVisual.u.processSamplesMessage.numOutputSamples;
				
			/*	for( i = 0; i < numSamples; i++)
				{
					sampleBuffer[ i] += 0x8000;
				}*/
			
			//	if( temp != msgVisual.u.processSamplesMessage.maxSamples) Debugger();
			}
			//	Erreur( 0, msgVisual.u.processSamplesMessage.numOutputSamples);
			break;
		
			case kVisualPluginInitializeMessage:
				VisualPlug[ PlugNo].refCon = msgVisual.u.initMessage.refcon;
			break;
			
			case kVisualPluginResizeMessage:
				MySizeWindow( VisualDlog, msgVisual.u.resizeMessage.approvedWidth, msgVisual.u.resizeMessage.approvedHeight , true);
			break;

		}
	}
	
	if( msg != kVisualPluginProcessSamplesMessage)
	{
		CloseResFile( fileID);
	}
}

void DoVisualNull()
{
	if( VisualDlog == NULL) return;
	
	CallVisualFonction( MADDriver, currentID, kVisualPluginRenderMessage, 0, NULL, 0);
	
//	CallVisualFonction( MADDriver, currentID, kVisualPluginEventMessage, 0, NULL, 0);
}

void  UpdateVisualWindow(DialogPtr GetSelection)
{
	GrafPtr		SavePort;
	
	GetPort( &SavePort);
	SetPortDialogPort( VisualDlog);

	BeginUpdate( GetDialogWindow( VisualDlog));

	DrawDialog( VisualDlog);
	
	CallVisualFonction( MADDriver, currentID, kVisualPluginUpdateMessage, 0, NULL, 0);

	EndUpdate( GetDialogWindow( VisualDlog));

	SetPort( SavePort);
} 

void LoadVisualPLUG( short No, StringPtr theName)
{
	Handle		theRes;
	short		fileID, i, temp;
	Str255		tStr;
	char		aStr[ 256];

	/***********************/
	
	HGetVol( NULL, &VisualPlug[ No].file.vRefNum, &VisualPlug[ No].file.parID);
	pStrcpy( VisualPlug[ No].file.name, theName);
	
	{
		Boolean		targetIsFolder, wasAliased;
		
	ResolveAliasFile( &VisualPlug[ No].file, true, &targetIsFolder, &wasAliased);
	}
	
/*	fileID = FSpOpenResFile( &ThePPINPlug[ No].file, fsCurPerm);
	
	GetIndString( tStr, 1000, 1);
	BlockMoveData( tStr + 1, &ThePPINPlug[ No].type, 4);
	
	GetIndString( tStr, 1000, 2);
	BlockMoveData( tStr + 1, &ThePPINPlug[ No].mode, 4);
	
	GetIndString( ThePPINPlug[ No].MenuName, 1000, 3);
	GetIndString( ThePPINPlug[ No].AuthorString, 1000, 4);
	
	GetIndString( tStr, 1000, 5);
	BlockMoveData( tStr + 1, &ThePPINPlug[ No].InsSamp, 4);
	if( ThePPINPlug[ No].InsSamp != 'SAMP' && ThePPINPlug[ No].InsSamp != 'INST') MyDebugStr( __LINE__, __FILE__, "Plug-Ins SAMP/INST Error");
	
	CloseResFile( fileID);*/
	/*************************/
}

static long PlugsFolderOK;

void ScanDirVisualPlug( long dirID, short VRefNum)
{
	CInfoPBRec		info;
	Str255			tempStr, volName;
	long			dirIDCopy;
	short			i, vRefNum;
	OSErr			iErr;

	info.hFileInfo.ioNamePtr = tempStr;
	info.hFileInfo.ioVRefNum = VRefNum;
	
	for (i = 1; true; i ++)
	{
		info.hFileInfo.ioDirID = dirID;
		info.hFileInfo.ioFDirIndex = i;
		
		if (PBGetCatInfoSync(&info) != noErr) break;
		
		if( info.hFileInfo.ioFlFndrInfo.fdType == 'PLUG')
		{	
			HGetVol( NULL, &vRefNum, &dirIDCopy);
			
			iErr = HSetVol( NULL, info.hFileInfo.ioVRefNum, dirID);
			
			if( tPlug > 50) MyDebugStr( __LINE__, __FILE__, "Too many plugs");
			
			LoadVisualPLUG( tPlug, info.hFileInfo.ioNamePtr);
			
			tPlug++;
			
			iErr = HSetVol( NULL, vRefNum, dirIDCopy);
			if( iErr != noErr) MyDebugStr( __LINE__, __FILE__, "HSetVol error...");
		}
		else if((info.hFileInfo.ioFlAttrib & 16))
		{
			if( EqualString( info.hFileInfo.ioNamePtr, "\pPlugs", false, false) || PlugsFolderOK > 0)
			{
				PlugsFolderOK++;
				ScanDirVisualPlug(info.dirInfo.ioDrDirID, VRefNum);
				PlugsFolderOK--;
			}
		}
	}
}

short maxVisualPlug( void)
{
	return tPlug;
}

void CreateVisualWindow( short ID)
{
	Rect		itemRect, tempRect, dataBounds;
	Handle		itemHandle;
	short		itemType, itemHit, temp, i;
	Point		cSize;
	FontInfo	ThisFontInfo;
	Str255		String;
	GrafPtr		savePort;

	if( ID >= tPlug)
	{
		VisualFullScreen();
		return;
	}

	if( VisualDlog != NULL)
	{
		if( currentID == ID)
		{
			SelectWindow2( GetDialogWindow( VisualDlog));
			return;
		}
		else
		{
			
			CloseVisual();
		}
	}
	
	SetItemMark( ViewsMenu, mVisual + ID, checkMark);
	
	VisualDlog = GetNewDialog( 185, NULL, GetDialogWindow( ToolsDlog));
	
	SetWindEtat( GetDialogWindow( VisualDlog));
	SetPortDialogPort( VisualDlog);
	
	currentID = ID;
	
	SetWTitle( GetDialogWindow( VisualDlog), VisualPlug[ currentID].file.name);
	
	CallVisualMain( currentID, kPluginInitializeMessage);
	
	CallVisualFonction( MADDriver, currentID, kVisualPluginInitializeMessage, 0, NULL, 0);
	CallVisualFonction( MADDriver, currentID, kVisualPluginEnableMessage, 0, NULL, 0);
	
	MySizeWindow(			VisualDlog,
							VisualPlug[ currentID].winSize.defaultWidth,
							VisualPlug[ currentID].winSize.defaultHeight,
							true);

	
//	wantedHeight = thePrefs.WinHi[ GetWRefCon( VisualDlog)];
//	wantedWidth = thePrefs.WinLarg[ GetWRefCon( VisualDlog)];
	
//	CallVisualFonction( MADDriver, currentID, kVisualPluginResizeMessage, 0, NULL, 0);
	CallVisualFonction( MADDriver, currentID, kVisualPluginSetWindowMessage, 0, NULL, 0);
	CallVisualFonction( MADDriver, currentID, kVisualPluginShowWindowMessage, 0, NULL, 0);
	CallVisualFonction( MADDriver, currentID, kVisualPluginPlayMessage, 0, NULL, 0);
	
	ShowWindow( GetDialogWindow( VisualDlog));
	SelectWindow2( GetDialogWindow( VisualDlog));
	
	EnableMenuItem( ViewsMenu, tPlug + 1 + mVisual);
	
	ReadyToProcess = true;
	
	thePrefs.lastVisualPlugin = currentID;
}

void CloseVisual(void)
{
	Rect	caRect;
	Point	Start;
	GrafPtr	savedPort;
	
	if( VisualDlog != NULL)
	{
		GetPort( &savedPort);
		SetPortDialogPort( VisualDlog);
		
		ReadyToProcess = false;
		
		GetPortBounds( GetDialogPort( VisualDlog), &caRect);
		
		thePrefs.WinHi[ GetWRefCon( GetDialogWindow( VisualDlog))] = caRect.bottom;
		thePrefs.WinLarg[ GetWRefCon( GetDialogWindow( VisualDlog))] = caRect.right;
		thePrefs.WinEtat[ GetWRefCon( GetDialogWindow( VisualDlog))] = 1;
		
		Start.h = Start.v = 0;
		LocalToGlobal( &Start);
		thePrefs.WinPos[ GetWRefCon( GetDialogWindow( VisualDlog))].v = Start.v;
		thePrefs.WinPos[ GetWRefCon( GetDialogWindow( VisualDlog))].h = Start.h;
		
		CallVisualFonction( MADDriver, currentID, kVisualPluginDisableMessage, 0, NULL, 0);
		CallVisualFonction( MADDriver, currentID, kVisualPluginHideWindowMessage, 0, NULL, 0);
		CallVisualFonction( MADDriver, currentID, kVisualPluginCleanupMessage, 0, NULL, 0);
		
		CallVisualMain( currentID, kPluginCleanupMessage);
		
		DisposeDialog( VisualDlog);
		
		SetPort( savedPort);
		
		CloseConnection( &VisualPlug[ currentID].connID);
		
		SetItemMark( ViewsMenu, mVisual + currentID, noMark);
	}
	VisualDlog = NULL;
	
	if( tPlug > 0) DisableMenuItem( ViewsMenu, tPlug + 1 + mVisual);
	
//	SetItemMark( ViewsMenu, m3D, noMark);
}

void DoItemPressVisual( short whichItem, DialogPtr whichDialog)
{
	Point	Location;
	Rect	caRect;

	GetMouse( &Location);
	
	GetPortBounds( GetDialogPort( whichDialog), &caRect);
	
	if( Location.h > caRect.right-16 &&
		Location.h > caRect.right-16)
		{
			DoGrowVisual();
		}
	else CallVisualFonction( MADDriver, currentID, kVisualPluginEventMessage, 0, NULL, 0);
}

void DoKeyVisual( DialogPtr whichDialog)
{
	if( whichDialog == NULL) return;
	
	CallVisualFonction( MADDriver, currentID, kVisualPluginEventMessage, 0, NULL, 0);
}

static	Boolean actuallyUse;

void ProcessVisualPlug( MADDriverRec *intDriver, short* in, long inNum)
{
	if( actuallyUse) return;
	
	actuallyUse = true;

	if( VisualDlog != NULL)
	{
		if( ReadyToProcess)
		{
			CallVisualFonction( intDriver, currentID, kVisualPluginProcessSamplesMessage, 0, in, inNum);
		}
	}
	
	actuallyUse = false;
}

void FlushPlugin( void)
{
	if( VisualDlog != NULL)
	{
		if( ReadyToProcess)
		{
			CallVisualFonction( MADDriver, currentID, kVisualPluginFlushSamplesMessage, 0, 0, 0);
		}
	}
}

void InitVisual(void)
{
	short		vRefNum;
	long		i, dirID;
	FSSpec		spec;
	
	actuallyUse = false;
	
	VisualPlug = (VisualInfo*) MyNewPtr( 100 * sizeof( VisualInfo));
	
	HGetVol( NULL, &vRefNum, &dirID);
	
	tPlug			= 0;
	PlugsFolderOK	= 0;
	GetApplicationPackageFSSpecFromBundle( &spec);
	ScanDirVisualPlug( spec.parID, spec.vRefNum);
	
//	InitPPINMenu();

	HSetVol( NULL, vRefNum, dirID);
	
	if( MacOSXSystem) tPlug = 0;
	
	if( tPlug > 0)
	{
		AppendMenu( ViewsMenu, "\p-");
		
		for( i = 0; i < tPlug; i++)	AppendMenu( ViewsMenu, VisualPlug[ i].file.name);
		
		AppendMenu( ViewsMenu, "\p-");
		
		AppendMenu( ViewsMenu, "\pUse Full Screen");
		
		DisableMenuItem( ViewsMenu, tPlug + 1 + mVisual);
	}
}
/*#else
short GetCurrentID()
{
return 0;
}

void VisualFullScreen( void)
{
}

void DoGrowVisual(void)
{
}

OSStatus PlayerPROProc(void *appCookie, OSType message, struct PlayerMessageInfo *messageInfo)
{
return noErr;
}

void CallVisualMain( short PlugNo, OSType msg)
{
}

void CallVisualFonction( MADDriverRec*, short PlugNo, OSType msg, CGrafPtr port, short* sampleBuffer, long numSamples)
{
}

void DoVisualNull()
{
}

void  UpdateVisualWindow(DialogPtr GetSelection)
{
} 

void LoadVisualPLUG(	short	No, StringPtr	theName)
{
}

void ScanDirVisualPlug( long dirID, short VRefNum)
{
}

short maxVisualPlug( void)
{
return 0;
}

void CreateVisualWindow( short ID)
{
}

void CloseVisual(void)
{
}

void DoItemPressVisual( short whichItem, DialogPtr whichDialog)
{
}

void DoKeyVisual( DialogPtr whichDialog)
{
}

void ProcessVisualPlug( short* in, long inNum)
{
}

void FlushPlugin( void)
{
}

void InitVisual(void)
{
}

#endif*/