//#define USE_DRAW_SPROCKETS 1
//#define USE_DISP_MGR 1
#define EG_MAC 1

#include "VisualPlugin.h"
#include "ScreenDevice.h"

#include <Carbon/Carbon.h>
#ifdef USE_DISP_MGR
extern "C" {
#include "RequestVideo.h"
}
#endif

static ScreenDevice *mScreenDevice = NULL;

long ScreenDevice::sOSDepth = 32;
long ScreenDevice::sMinDepth = 16;

ScreenDevice::ScreenDevice()
{
	mContextRef = 0;
	
#if 0
	GDHandle gDevice = ::GetMainDevice();
	if (gDevice) {
		PixMapHandle pixMap = (**gDevice).gdPMap;
		sOSDepth = (**pixMap).pixelSize;
		if (sOSDepth == 8) {
			::BlockMove((**(**pixMap).pmTable).ctTable, sOSPalette, 256 * sizeof(ColorSpec)); 
		} }
	else
		sOSDepth = 16;
#endif
}


ScreenDevice::~ScreenDevice()
{
	//EndFrame();
	
	ExitFullscreen();
}

bool ScreenDevice::EnterFullscreen(long inDispID, Point& ioSize, int inBitDepth, WindowPtr inWin, long inFreq)
{
	bool ok = false;
	
	// Check inBitDepth
	if (inBitDepth != 16 && inBitDepth != 32)
		inBitDepth = sOSDepth;
	if (inBitDepth < sMinDepth)
		inBitDepth = sMinDepth;
	
	
	ExitFullscreen();
	mDispID		= inDispID;
	mBitDepth	= inBitDepth;
	
	
#if USE_DISP_MGR
#pragma unused(inFreq)
	
	Rect		r;
	GDHandle	theGDevice;
	
	::HideCursor();
	::HideWindow(inWin);
	
	
	// Hide that pesky menubar...
	RgnHandle grayRgn;
	grayRgn = ::GetGrayRgn();
	mMenuBarHeight	= ::GetMBarHeight();
	HideMenuBar();
	r = ::GetQDGlobalsScreenBits(NULL)->bounds;
	r.bottom = r.top + mMenuBarHeight;
	mMenuBarRgn	= ::NewRgn();
	::RectRgn(mMenuBarRgn, &r);
	::UnionRgn(grayRgn, mMenuBarRgn, grayRgn);
	
	// Fetch a ptr to the device given by inDispNum
	if (::DMGetGDeviceByDisplayID(inDispID, &theGDevice, false) != noErr)
		theGDevice = NULL;
	if (!theGDevice)
		theGDevice = ::GetMainDevice();
	
	// Use RequestVideo.c to get the Disp Mgr to do what we want
	VideoRequestRec requestRec;
	requestRec.screenDevice		=	theGDevice;
	requestRec.reqBitDepth		=	inBitDepth;	
	requestRec.reqHorizontal	=	ioSize.h;
	requestRec.reqVertical		=	ioSize.v;
	requestRec.displayMode		=	nil;					// must init to nil
	requestRec.depthMode		=	nil;					// must init to nil
	requestRec.requestFlags		=	0;						
	if (RVRequestVideoSetting(&requestRec) == noErr) {
		if (RVSetVideoRequest(&requestRec) == noErr) {
			ioSize.h = requestRec.availHorizontal;
			ioSize.v = requestRec.availVertical;
			ok = true;
		}
	}
	
	if (ok) {
		
		// Make the window cover the device
		::MoveWindow(inWin, 0, 0, true);
		::SizeWindow(inWin, ioSize.h, ioSize.v, true);
		::ShowWindow(inWin);
		
		// Setup the window as the main grafport
		mContextRef = GetWindowPort(inWin);
		int mX			= ioSize.h;
		int mY			= ioSize.v;
		::SetRect(&r, 0, 0, mX, mY + 2);
		::NewGWorld(&mWorld, inBitDepth, &r, NULL, NULL, false);
		PixMapHandle mBM = ::GetGWorldPixMap(mWorld);
		mBytesPerRow	= (**mBM).rowBytes & 0xFFF;
		mBytesPerPix	= (**mBM).pixelSize / 8;
	}
	
#elif USE_DRAW_SPROCKETS
	DSpContextReference		ref;
	OSStatus				err;
	DSpContextAttributes	context;
	long					bestWidth = 0x7FFFFFFF;
	bool 					isInitted = false;
	
	err = ::DSpStartup();
	
	if (!err) {
		err = ::DSpGetFirstContext(inDispID, &ref);
		
		// Look for smallest size w/ for given depth
		while (! err && ref) {
			err = DSpContext_GetAttributes(ref, &context);
			if (!err && ref) {
				if (context.displayBestDepth == inBitDepth) {
					if (context.displayWidth == ioSize.h && context.displayHeight == ioSize.v) {
						mContextRef = ref;
						isInitted = true; 
						break; 
					} else if (context.displayWidth <= bestWidth && context.displayWidth >= 640) {
						mContextRef = ref;
						isInitted = true;
						bestWidth = context.displayWidth;
					}
				}
				
				// Try the next context for this display
				err = ::DSpGetNextContext(ref, &ref);
			}
		}
		
		if (!isInitted) {
			mContextRef = 0;
			::DSpShutdown();
			return false;
		}
		
		::DSpContext_GetAttributes(mContextRef, &context);
		ioSize.h = context.displayWidth;
		ioSize.v = context.displayHeight;
		
		context.contextOptions			= kDSpContextOption_DontSyncVBL;
		context.frequency				= inFreq;
		context.reserved1				= 0;
		context.reserved2				= 0;
		context.gameMustConfirmSwitch	= false;
		context.reserved3[0]	= 0;
		context.reserved3[1]	= 0;
		context.reserved3[2]	= 0;
		context.reserved3[3]	= 0;
		context.colorTable		= 0;
		context.pageCount		= 1;
		context.colorNeeds		= kDSpColorNeeds_Require;
		
		RGBColor back = {0, 0, 0};
		::DSpSetBlankingColor(&back);
		
		// Try to reserve the device
		err = ::DSpContext_Reserve(mContextRef, &context);
		if (!err) {
			
			// If no errors, 'activate' the device into fullscreen
			::DSpContext_FadeGammaOut(mContextRef, NULL);
			::HideCursor();
			
			err = ::DSpContext_SetState(mContextRef, kDSpContextState_Active);
			::DSpContext_FadeGamma(mContextRef, 100, NULL);
			
			if (err && err != kDSpConfirmSwitchWarning) {
				::DSpContext_Release(mContextRef);
				::DSpShutdown(); 
			} else {
				ok = true;
				
				mFS_DC = NULL;
			}
		}
	}
	
#endif
	
	if (ok)
		::HideCursor();
	else
		mContextRef = 0;
	
	return ok;
}
	
void ScreenDevice::ExitFullscreen()
{
	GDHandle    prevDev;
	GWorldPtr   prevPort;
	::GetGWorld(&prevPort, &prevDev);
	
	EndFrame();
	if (!IsFullscreen())
		return;
	
#if USE_DRAW_SPROCKETS
	::DSpContext_FadeGamma(mContextRef, 0, NULL);
	::DSpContext_SetState(mContextRef, kDSpContextState_Inactive);
	::DSpContext_FadeGamma(mContextRef, 100, NULL);	
	::DSpContext_Release(mContextRef);
	::DSpShutdown();
	::InitCursor();
#endif
	
	
#if USE_DISP_MGR
	RVSetVideoAsScreenPrefs();
	
	// Make the menu bar visible again
	RgnHandle grayRgn;
	grayRgn = ::GetGrayRgn();
	ShowMenuBar();
	::DiffRgn(grayRgn, mMenuBarRgn, grayRgn);	// remove the menu bar from the desktop
	::PaintOne(NULL, mMenuBarRgn);				// redraw the menubar 
	::DisposeRgn(mMenuBarRgn);
	
	// Restore the original color table for the main device
	//if (sOSDepth == 8 && mBytesPerPix == 1)
	//	::SetEntries(0, 255, sOSPalette);
	::InitCursor();
#endif
	
	mContextRef = NULL;
	mFS_DC = NULL;
}

#if 0
void ScreenDevice::SetPalette(PixPalEntry inPal[256])
{
	if (mBitDepth != 8 || !IsFullscreen())
		return;
	
#if EG_MAC
	::SetEntries(0, 255, inPal);
#endif
}
#endif
	
GrafPtr ScreenDevice::BeginFrame()
{
	if (IsFullscreen()) {
#if USE_DRAW_SPROCKETS
		OSErr err;
		//	err = ::DSpContext_GetBackBuffer(mContextRef, kDSpBufferKind_Normal, (CGrafPtr*) &mFS_DC );
		//if (! mFS_DC ) {
		err = ::DSpContext_GetFrontBuffer(mContextRef, (CGrafPtr*) &mFS_DC);
		if (mFS_DC)
			::SetPort(mFS_DC);
		else
			ExitFullscreen();
		//}
#endif
		
#if USE_DISP_MGR
		PixMapHandle mBM	= ::GetGWorldPixMap(mWorld);
		//TODO: fix me!
#endif
	}
	
	return mFS_DC;
}

void ScreenDevice::EndFrame()
{
	if (IsFullscreen()) {
#if USE_DRAW_SPROCKETS
		//::DSpContext_SwapBuffers(mContextRef, NULL, 0 );
		mFS_DC = NULL;
#endif
	}
}

long ScreenDevice::GetDisplayID(long inDeviceNum)
{
	OSStatus			err;
	DisplayIDType		id = 0;
	GDHandle theGDevice = ::DMGetFirstScreenDevice(false);
	while (theGDevice && inDeviceNum) {
		inDeviceNum--;
		theGDevice = ::DMGetNextScreenDevice(theGDevice, false);
	}
	
	if (!theGDevice)
		theGDevice = ::DMGetFirstScreenDevice(false);
	
	err = ::DMGetDisplayIDByGDevice(theGDevice, &id, false);
	
	return (err) ? 0 : id;
}

long ScreenDevice::GetDisplayID(long inX, long inY)
{
	OSStatus			err = noErr;
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
	theGDevice = ::DMGetFirstScreenDevice(false);
	while (theGDevice && ! id) {
		
		if (::PtInRect(inPt, &(**theGDevice).gdRect)) {
			
			/* get the display ID */
			err = ::DMGetDisplayIDByGDevice(theGDevice, &id, false);
			if (err)
				id = 0;
		}
		
		/* next device */
		theGDevice = ::DMGetNextScreenDevice(theGDevice, false);
	}	
	/*
	 err = ::DSpFindContextFromPoint(inPt, &ref );	
	 if (! err )
	 err = ::DSpContext_GetDisplayID(ref, &id );
	 */
	return err ? 0 : id;
}

#pragma mark C-based functions

Boolean EnterFullscreen(long inDispID, Point *ioSize, int inBitDepth, WindowPtr inWin, long inFreq)
{
	if ((void*)mScreenDevice == NULL)
		mScreenDevice = new ScreenDevice();
	return mScreenDevice->EnterFullscreen(inDispID, ioSize, inBitDepth, inWin, inFreq);
}

GrafPtr BeginFrame()
{
	return mScreenDevice->BeginFrame();
}

void EndFrame()
{
	mScreenDevice->EndFrame();
}

void ExitFullscreen()
{
	delete mScreenDevice;
	mScreenDevice = NULL;
}

long GetDisplayID(long inDeviceNum)
{
	return ScreenDevice::GetDisplayID(inDeviceNum);
}

long GetDisplayIDWithXandY(long inX, long inY)
{
	return ScreenDevice::GetDisplayID(inX, inY);
}

