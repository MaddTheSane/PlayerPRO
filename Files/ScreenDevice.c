#define USE_DRAW_SPROCKETS 1
//#define USE_DISP_MGR 1
#define EG_MAC 1

#include "VisualPlugin.h"


#if EG_MAC
#include <QuickDraw.h>
#include <QDOffscreen.h>
#include <LowMem.h>

typedef ColorSpec PixPalEntry;

#if USE_DISP_MGR
	#undef USE_DRAW_SPROCKETS
	#include "RequestVideo.h"
#elif USE_DRAW_SPROCKETS
	#include <DrawSprocket.h>
#else
	typedef long DSpContextReference;
	struct DSpContextAttributes	{ };
#endif
#else
#endif

#if EG_MAC
#include <Displays.h>
#endif


long		sOSDepth				= 32;
long		sMinDepth 			= 16;

		long					mDispID;
		long					mBitDepth;			 
		GrafPtr					mFS_DC;
		
#if USE_DRAW_SPROCKETS
		DSpContextReference		mContextRef = 0;
		DSpContextAttributes	mContext;
#elif USE_DISP_MGR
		GrafPtr					mContextRef;
		RgnHandle				mMenuBarRgn;
		long					mMenuBarHeight;
#elif EG_WIN
		LPDIRECTDRAWPALETTE FAR	mFS_Palette;
		LPDIRECTDRAWSURFACE		mContextRef;
		LPDIRECTDRAW			mDDObj;
		HWND					mFS_Win;
#else			
		long					mContextRef;
#endif


#ifdef __cplusplus
extern "C" {
#endif

void DoVisualNull();
void CallVisualFonction( short PlugNo, OSType msg, CGrafPtr port);
short GetCurrentID( void);

#ifdef __cplusplus
}
#endif

void ExitFullscreen();
void EndFrame();

//#if MACOS9VERSION
#if 1
Boolean IsFullscreen()									{ return mContextRef != 0; }

void ScreenDevice() {
	

#if USE_DIRECTX
	HDC hdc = GetDC( NULL );
	sOSDepth = GetDeviceCaps( hdc, BITSPIXEL );
	ReleaseDC( NULL, hdc );
	if ( sOSDepth == 24 )
		sOSDepth = 32;	
#endif
	
#if EG_WIN
	mDDObj		= NULL;
	mFS_DC		= NULL;
#endif
	
//TODO: is THIS needed?
#if 0	
	GDHandle gDevice = GetMainDevice();
	if ( gDevice ) {
		PixMapHandle pixMap = (**gDevice).gdPMap;
		sOSDepth = (**pixMap).pixelSize;
		if ( sOSDepth == 8 ) {
			BlockMove( (**(**pixMap).pmTable).ctTable, sOSPalette, 256 * sizeof( ColorSpec ) ); 
		} }
	else
		sOSDepth = 16;
#endif	

}

Boolean EnterFullscreen( long inDispID, Point ioSize, int inBitDepth, WindowPtr inWin, long inFreq )
{
	Boolean ok = false;
	
	if ((Ptr) DSpStartup == (Ptr) kUnresolvedCFragSymbolAddress) return false;
		
	// Check inBitDepth
	if ( inBitDepth != 8 && inBitDepth != 16 && inBitDepth != 32 )
		inBitDepth = sOSDepth;
	if ( inBitDepth < sMinDepth )
		inBitDepth = sMinDepth;
		
		
	ExitFullscreen();
	mDispID		= inDispID;
	mBitDepth	= inBitDepth;


#if USE_DISP_MGR
#pragma unused( inFreq )
	
	Rect		r;
	GDHandle	theGDevice;

	HideCursor();
	HideWindow( inWin );


	// Hide that pesky menubar...
	RgnHandle grayRgn;
	grayRgn = LMGetGrayRgn();
	mMenuBarHeight	= LMGetMBarHeight();
	LMSetMBarHeight( 0 );
	r = qd.screenBits.bounds;
	r.bottom = r.top + mMenuBarHeight;
	mMenuBarRgn	= NewRgn();
	RectRgn( mMenuBarRgn, &r );
	UnionRgn( grayRgn, mMenuBarRgn, grayRgn );

	// Fetch a ptr to the device given by inDispNum
	if ( DMGetGDeviceByDisplayID( inDispNum, &theGDevice, false ) != noErr )
		theGDevice = NULL;
	if ( ! theGDevice )
		theGDevice = GetMainDevice();
	
	// Use RequestVideo.c to get the Disp Mgr to do what we want
	VideoRequestRec requestRec;
	requestRec.screenDevice		=	theGDevice;
	requestRec.reqBitDepth		=	inBitDepth;	
	requestRec.reqHorizontal	=	ioSize.h;
	requestRec.reqVertical		=	ioSize.v;
	requestRec.displayMode		=	nil;					// must init to nil
	requestRec.depthMode		=	nil;					// must init to nil
	requestRec.requestFlags		=	0;						
	if ( RVRequestVideoSetting( &requestRec ) == noErr ) {
		if ( RVSetVideoRequest( &requestRec ) == noErr ) {
			outSize.h = requestRec.availHorizontal;
			outSize.v = requestRec.availVertical;
			ok = true;
		}
	}
	
	if ( ok ) {

		// Make the window cover the device
		MoveWindow( inWin, 0, 0, true );
		SizeWindow( inWin, outSize.h, outSize.v, true ); 
		ShowWindow( inWin );

		// Setup the window as the main grafport
		mContextRef = inWin;
		mX			= outSize.h;
		mY			= outSize.v;
		SetRect( &r, 0, 0, mX, mY+2 );
		NewGWorld( &mWorld, inBitDepth, &r, NULL, NULL, useTempMem );
		mBM = GetGWorldPixMap( mWorld );
		mBytesPerRow	= (**mBM).rowBytes & 0xFFF;
		mBytesPerPix	= (**mBM).pixelSize / 8;
	}



#elif USE_DRAW_SPROCKETS
	{
	DSpContextReference		ref;
	OSStatus				err;
	DSpContextAttributes	context;
	long					bestWidth = 0x7FFFFFFF;
	Boolean 					isInitted = false;

	err = DSpStartup();

	
	if ( ! err ) {
		err = DSpGetFirstContext( inDispID, &ref );
		
		// Look for smallest size w/ for given depth
		while ( ! err && ref ) {
			err = DSpContext_GetAttributes( ref, &context );
			if ( ! err && ref ) {
				if ( context.displayBestDepth == inBitDepth ) {
					if ( context.displayWidth == ioSize.h && context.displayHeight == ioSize.v ) {
						mContextRef = ref;
						isInitted = true; 
						break; }
					else if ( context.displayWidth <= bestWidth && context.displayWidth >= 640 ) {
						mContextRef = ref;
						isInitted = true;
						bestWidth = context.displayWidth;
					}
				}
				
				// Try the next context for this display
				err = DSpGetNextContext( ref, &ref );
			}
		}
		
		if ( ! isInitted ) {
			mContextRef = 0;
			DSpShutdown();
			return false;
		}
			
		DSpContext_GetAttributes( mContextRef, &context );
		ioSize.h = context.displayWidth;
		ioSize.v = context.displayHeight;
		
		context.contextOptions 			= kDSpContextOption_DontSyncVBL;
		context.frequency					= inFreq;
		context.reserved1					= 0;
		context.reserved2					= 0;
		context.gameMustConfirmSwitch		= false;
		context.reserved3[0]	= 0;
		context.reserved3[1]	= 0;
		context.reserved3[2]	= 0;
		context.reserved3[3]	= 0;
		context.colorTable		= 0;
		context.pageCount		= 1;
		context.colorNeeds		= kDSpColorNeeds_Require;
					
		RGBColor back = { 0, 0, 0 };
		DSpSetBlankingColor( &back );
	
		// Try to reserve the device
		err = DSpContext_Reserve( mContextRef, &context );
		if ( ! err ) {

			// If no errors, 'activate' the device into fullscreen
			DSpContext_FadeGammaOut( mContextRef, NULL );
			HideCursor();

			err = DSpContext_SetState( mContextRef, kDSpContextState_Active );
			DSpContext_FadeGamma( mContextRef, 100, NULL );
			
			if ( err && err != kDSpConfirmSwitchWarning ) {
				DSpContext_Release( mContextRef );
				DSpShutdown(); }
			else {
				ok = true;
				
#pragma unused( inWin )
				/*
				// Make the window cover the device
				MoveWindow( inWin, 0, 0, true );
				SizeWindow( inWin, ioSize.h, ioSize.v, true ); 
				ShowWindow( inWin );

				// Setup the window as the main grafport
				mFS_DC = inWin; */
				mFS_DC = NULL;
			}
		}
	}
	}
	
#elif USE_DIRECTX
#pragma unused( inFreq )
	
	if ( inWin ) {
		HRESULT err = DirectDrawCreate( NULL, &mDDObj, NULL );
		if ( err == DD_OK ) {
			LPDIRECTDRAWSURFACE context;
			
			err = mDDObj -> SetCooperativeLevel( inWin, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
			if ( err == DD_OK ) {
				err = mDDObj -> SetDisplayMode( ioSize.h, ioSize.v, inBitDepth );
				if ( err == DD_OK ) {
					mDDObj -> Compact();
					DDSURFACEDESC ddsd;
					ddsd.dwSize = sizeof(ddsd);
					ddsd.dwFlags = DDSD_CAPS;
					ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
					err = mDDObj -> CreateSurface( &ddsd, &context, NULL);
				}
			}
			
			if ( err == DD_OK ) {
				mContextRef = context;
				SetWindowPos( inWin, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
				SetForegroundWindow( inWin );
				SetCapture( inWin );
				mFS_Win = inWin;
				
				PALETTEENTRY pal[ 256 ];
				for ( int j = 0; j < 256; j++ ) {
					pal[ j ].peRed = j;
					pal[ j ].peGreen = j;
					pal[ j ].peBlue = j;
					pal[ j ].peFlags = 0;
				}
				mDDObj -> CreatePalette( DDPCAPS_8BIT, pal, &mFS_Palette, NULL );
				mContextRef -> SetPalette( mFS_Palette );
				ok = true; }
			else {
				 mDDObj -> Release();
				 mDDObj = NULL;
			}
		}
	}
	
	
#else
#pragma unused( ioSize, inWin, inFreq )
#endif
	
	if ( ok ) {
#if EG_MAC
		HideCursor();
#elif EG_WIN
		SetCursor( LoadCursor( NULL, IDC_ARROW ) );
		while ( ShowCursor( false ) >= 0 ) { }
#endif
		}
	else
		mContextRef = 0;
	
	
	return ok;
}
	


	
void ExitFullscreen() {


	EndFrame();
	if ( ! IsFullscreen() ) 
		return;
		
#if USE_DRAW_SPROCKETS
	DSpContext_FadeGamma( mContextRef, 0, NULL );
	DSpContext_SetState( mContextRef, kDSpContextState_Inactive );
	DSpContext_FadeGamma( mContextRef, 100, NULL );	
	DSpContext_Release( mContextRef );
	DSpShutdown();
	InitCursor();
#endif
	
	
#if USE_DISP_MGR
	RVSetVideoAsScreenPrefs();

	// Make the menu bar visible again
	RgnHandle grayRgn;
	grayRgn = LMGetGrayRgn();
	LMSetMBarHeight( mMenuBarHeight );
	DiffRgn( grayRgn, mMenuBarRgn, grayRgn );	// remove the menu bar from the desktop
	PaintOne( NULL, mMenuBarRgn );			// redraw the menubar 
	DisposeRgn( mMenuBarRgn );

	// Restore the original color table for the main device
	if ( sOSDepth == 8 && mBytesPerPix == 1 )
		SetEntries( 0, 255, sOSPalette );
	InitCursor();
#endif
	
	
#if USE_DIRECTX
	if ( mFS_DC ) {
		mContextRef -> ReleaseDC( mFS_DC );
		mFS_DC = NULL;
	}	
	if ( mContextRef ) {
		mContextRef -> Release();
		mContextRef = 0;
	}
	if ( mDDObj ) {
		mDDObj -> SetCooperativeLevel( mFS_Win, DDSCL_NORMAL );
		mDDObj -> Release();
		mDDObj = NULL;
	}
	ReleaseCapture();
	while ( ShowCursor( true ) < 0 ) { }
#endif
	
	mContextRef = NULL;
	mFS_DC = NULL;
}






	/*
void SetPalette( PixPalEntry inPal[ 256 ] ) {

	if ( mBitDepth != 8 || ! IsFullscreen() )
		return;

	#if USE_DIRECTX
	PALETTEENTRY pal[ 256 ];
	for ( int i = 0; i < 256; i++ ) {
		* ( (long*) &pal[ i ] ) = inPal[ i ].rgbRed | ( inPal[ i ].rgbGreen << 8 )| ( inPal[ i ].rgbBlue << 16 ) | ( PC_RESERVED << 24 );
	}
	mFS_Palette -> SetEntries( 0, 0, 256, pal );
	#endif
	
	
	#if EG_MAC
	SetEntries( 0, 255, inPal );
	

}
*/
	
GrafPtr BeginFrame() {
	
	if ( IsFullscreen() ) {

#if USE_DRAW_SPROCKETS
		OSErr err;
		err = DSpContext_GetBackBuffer( mContextRef, kDSpBufferKind_Normal, (CGrafPtr*) &mFS_DC );
		if ( ! mFS_DC ) {
			err = DSpContext_GetFrontBuffer( mContextRef, (CGrafPtr*) &mFS_DC );
			if ( mFS_DC )
				SetPort( mFS_DC );
			else
				ExitFullscreen();
		}
#endif
		
		
#if USE_DISP_MGR
		mBM	= GetGWorldPixMap( mWorld );
#error fix me!
#endif
		

#if USE_DIRECTX
		if ( mContextRef -> GetDC( &mFS_DC ) != DD_OK )
			mFS_DC = NULL;
#endif
		
	}
	
	return mFS_DC;
}



void EndFrame() {

	if ( IsFullscreen() ) {

#if USE_DRAW_SPROCKETS
		DSpContext_SwapBuffers( mContextRef, NULL, 0 );
		mFS_DC = NULL;
#endif 
		
#if USE_DIRECTX
		if ( mFS_DC ) {
			mContextRef -> ReleaseDC( mFS_DC );
			mFS_DC = NULL;
		}
#endif
	}
}


#if 0
long GetDisplayID( long inDeviceNum ) {


#if EG_MAC
	OSStatus			err;
	DisplayIDType		id = 0;
	GDHandle theGDevice = DMGetFirstScreenDevice( false );
	while ( theGDevice && inDeviceNum ) {
		inDeviceNum--;
		
		theGDevice = DMGetNextScreenDevice( theGDevice, false );
	}
	
	if ( ! theGDevice )
		theGDevice = DMGetFirstScreenDevice( false );
		
	err = DMGetDisplayIDByGDevice( theGDevice, &id, false );

	return ( err ) ? 0 : id;
#endif
	
#if EG_WIN
	//#pragma unused( inX, inY )
	return 0;
#endif
}
#endif
	

long GetDisplayID( long inX, long inY ) {
	
#if EG_MAC
	OSStatus			err;
	DisplayIDType		id = 0;
	Point				inPt;
	
	inPt.h = inX;
	inPt.v = inY;

	GDHandle theGDevice;

	/*
	** Walk the list of display devices in the system.  DrawSprocket is
	** centered around the DisplayIDType, which is used by the Display
	** Manager.  The GDevice records are going to be in flux with future
	** versions of the system software, so it is best to make the change
	** now and make your software DisplayManager-centric.
	*/
	theGDevice = DMGetFirstScreenDevice( false );
	while( theGDevice && ! id ) {
		
		if ( PtInRect( inPt, &(**theGDevice).gdRect ) ) {
		
			/* get the display ID */
			err = DMGetDisplayIDByGDevice( theGDevice, &id, false );
			if ( err )
				id = 0;
		}
		
		/* next device */
		theGDevice = DMGetNextScreenDevice( theGDevice, false );
	}	

	//TODO: is this needed?
//	err = DSpFindContextFromPoint( inPt, &ref );	
//	if ( ! err )
//		err = DSpContext_GetDisplayID( ref, &id );

	return ( err ) ? 0 : id;
#endif
	
#if EG_WIN
#pragma unused( inX, inY )
	return 0;
#endif
}
#else
long GetDisplayID( long inX, long inY ) {};
//long GetDisplayID( long inDeviceNum ) {};
void EndFrame() {};
GrafPtr BeginFrame() {};
void ExitFullscreen() {};
Boolean EnterFullscreen( long inDispID, Point ioSize, int inBitDepth, WindowPtr inWin, long inFreq )
{};
void ScreenDevice() {};
Boolean IsFullscreen() {};

#endif
