//
//  OldCarbHeaders.h
//  PPMacho
//
//  Created by C.W. Betts on 9/4/13.
//
//

#ifndef PPMacho_OldCarbHeaders_h
#define PPMacho_OldCarbHeaders_h
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <CoreServices/CoreServices.h>
#include <CoreFoundation/CFPlugInCOM.h>
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>

#ifdef QD_HEADERS_ARE_PRIVATE

#pragma pack (push, 2)

enum {
	invalColReq = -1    /*invalid color table request*/
};

typedef UInt32		AVIDType;
typedef AVIDType	DisplayIDType;
typedef UInt32		DisplayModeID;
typedef UInt32		VideoDeviceType;
typedef UInt32		GammaTableID;

enum {
	kFontIDNewYork = 2,
	kFontIDGeneva,
	kFontIDMonaco,
	kFontIDVenice,
	kFontIDLondon,
	kFontIDAthens,
	kFontIDSanFrancisco,
	kFontIDToronto,
	kFontIDCairo = 11,
	kFontIDLosAngeles,
	kFontIDTimes = 20,
	kFontIDHelvetica,
	kFontIDCourier,
	kFontIDSymbol,
	kFontIDMobile
};

enum {
	systemFont = 0,
	applFont
};

enum {
	commandMark	= 17,
	checkMark,
	diamondMark,
	appleMark
};

enum {
	/* transfer modes */
	srcOr			= 1,
	srcXor,
	srcBic,
	notSrcCopy,
	notSrcOr,
	notSrcXor,
	notSrcBic,
	patCopy,
	patOr,
	patXor,
	patBic,
	notPatCopy,
	notPatOr,
	notPatXor,
	notPatBic,							/* Special Text Transfer Mode */
	grayishTextOr                 = 49,
	hilitetransfermode            = 50,
	hilite                        = 50,	/* Arithmetic transfer modes */
	blend                         = 32,
	addPin,
	addOver,
	subPin,
	addMax                        = 37,
	adMax                         = 37,
	subOver,
	adMin,
	ditherCopy                    = 64,	/* Transparent mode constant */
	transparent                   = 36
};

enum {
	italicBit = 1,
	ulineBit,
	outlineBit,
	shadowBit,
	condenseBit,
	extendBit
};

enum {
	normalBit = 0,
	inverseBit,
	blueBit,
	greenBit,
	redBit,
	blackBit,
	yellowBit,
	magentaBit,
	cyanBit
};

enum {
	blackColor		= 33,   /*colors expressed in these mappings*/
	whiteColor		= 30,
	redColor		= 205,
	greenColor		= 341,
	blueColor		= 409,
	cyanColor		= 273,
	magentaColor	= 137,
	yellowColor		= 69
};

enum {
	picLParen	= 0,	/*standard picture comments*/
	picRParen	= 1,
	clutType	= 0,	/*0 if lookup table*/
	fixedType	= 1,	/*1 if fixed table*/
	directType	= 2,	/*2 if direct values*/
	gdDevType	= 0		/*0 = monochrome 1 = color*/
};

enum {
	interlacedDevice	= 2,
	hwMirroredDevice	= 4,
	roundedDevice,
	hasAuxMenuBar,
	burstDevice,
	ext32Device,
	ramInit				= 10,
	mainScreen,
	allInit,
	screenDevice,
	noDriver,
	screenActive,
	hiliteBit			= 7,
	pHiliteBit			= 0,
	defQDColors			= 127,
	RGBDirect			= 16,
	baseAddr32			= 4
};

enum {
	sysPatListID = 0,
	iBeamCursor,
	crossCursor,
	plusCursor,
	watchCursor
};

enum {
	kQDGrafVerbFrame = 0,
	kQDGrafVerbPaint,
	kQDGrafVerbErase,
	kQDGrafVerbInvert,
	kQDGrafVerbFill
};

typedef SInt8 PixelType;
typedef short Bits16[16];

typedef struct Cursor {
	Bits16	data;
	Bits16	mask;
	Point	hotSpot;
} Cursor, *CursPtr, **CursHandle;

typedef struct PenState {
	Point	pnLoc;
	Point	pnSize;
	short	pnMode;
	Pattern	pnPat;
} PenState;

typedef struct GammaTbl {
	short	gVersion;
	short	gType;
	short	gFormulaSize;
	short	gChanCnt;
	short	gDataCnt;
	short	gDataWidth;
	short	gFormulaData[1];
} GammaTbl, *GammaTblPtr, **GammaTblHandle;

enum {
	pixPurgeBit = 0,
	noNewDeviceBit,
	useTempMemBit,
	keepLocalBit,
	useDistantHdwrMemBit,
	useLocalHdwrMemBit,
	pixelsPurgeableBit,
	pixelsLockedBit,
	nativeEndianPixMapBit,
	mapPixBit = 16,
	newDepthBit,
	alignPixBit,
	newRowBytesBit,
	reallocPixBit,
	clipPixBit = 28,
	stretchPixBit,
	ditherPixBit,
	gwFlagErrBit
};

enum {
	pixPurge				= 1L << pixPurgeBit,
	noNewDevice				= 1L << noNewDeviceBit,
	useTempMem				= 1L << useTempMemBit,
	keepLocal				= 1L << keepLocalBit,
	useDistantHdwrMem		= 1L << useDistantHdwrMemBit,
	useLocalHdwrMem			= 1L << useLocalHdwrMemBit,
	pixelsPurgeable			= 1L << pixelsPurgeableBit,
	pixelsLocked			= 1L << pixelsLockedBit,
	kNativeEndianPixMap		= 1L << nativeEndianPixMapBit,
	kAllocDirectDrawSurface	= 1L << 14,
	mapPix					= 1L << mapPixBit,
	newDepth				= 1L << newDepthBit,
	alignPix				= 1L << alignPixBit,
	newRowBytes				= 1L << newRowBytesBit,
	reallocPix				= 1L << reallocPixBit,
	clipPix					= 1L << clipPixBit,
	stretchPix				= 1L << stretchPixBit,
	ditherPix				= 1L << ditherPixBit,
	gwFlagErr				= 1L << gwFlagErrBit
};

struct VDSwitchInfoRec {
	unsigned short      csMode;                 /*(word) mode depth*/
	unsigned long       csData;                 /*(long) functional sResource of mode*/
	unsigned short      csPage;                 /*(word) page to switch in*/
	Ptr                 csBaseAddr;             /*(long) base address of page (return value)*/
	unsigned long       csReserved;             /*(long) Reserved (set to 0) */
};
typedef struct VDSwitchInfoRec          VDSwitchInfoRec;
typedef VDSwitchInfoRec *               VDSwitchInfoPtr;

#if TARGET_OS_MAC
#define MacInvertRect InvertRect
#define MacOffsetRect OffsetRect
#define MacSetRect SetRect
#define MacPtInRect PtInRect
#define MacLineTo LineTo
#define MacFillRect FillRect
#define MacSetPort SetPort
#define MacGetCursor GetCursor
#define MacSetCursor SetCursor
#define MacFrameRect FrameRect
#define MacUnionRgn UnionRgn
#define MacShowCursor ShowCursor
#define MacCopyRgn CopyRgn
#define MacInsetRect InsetRect
#define MacEqualRect EqualRect
#define MacOffsetRgn OffsetRgn
#define MacInvertRgn InvertRgn
#endif

__BEGIN_DECLS
extern void GetPort(GrafPtr *);
extern short GetPortTextFont(CGrafPtr);
extern short GetPortTextSize(CGrafPtr);
extern void PaintRect(const Rect *);
extern void ForeColor(long);
extern void MacInvertRect(const Rect *);
extern void MacSetRect(Rect *, short, short, short, short);
extern GDHandle GetDeviceList(void);
extern void MacOffsetRect(Rect * r, short, short);
extern GDHandle GetNextDevice(GDHandle);
extern BitMap *GetQDGlobalsScreenBits(BitMap *);
extern void LocalToGlobal(Point *);
extern Rect *GetPortBounds(CGrafPtr, Rect *);
extern void pStrcpy(register unsigned char *, register const unsigned char *);
extern Boolean MacPtInRect(Point, const Rect *);
extern GDHandle GetMainDevice();
extern RgnHandle NewRgn();
extern void OpenRgn();
extern void CloseRgn(RgnHandle);
extern PixMapHandle GetPortPixMap(CGrafPtr);
extern GDHandle GetGDevice();
extern void MoveTo(short, short);
extern void MacLineTo(short, short);
extern void EraseRect(const Rect *);
extern void MacFillRect(const Rect *,const Pattern *);
extern short TextWidth(const void *, short, short);
extern void DrawString(ConstStr255Param);
extern long DeltaPoint(Point, Point);
extern void MacSetPort(GrafPtr);
extern CursHandle MacGetCursor(short);
extern void MacSetCursor(const Cursor *);
extern Boolean PtInRgn(Point, RgnHandle);
extern void RGBForeColor(const RGBColor *);
extern void BackColor(long);
extern void CopyBits(const BitMap *, const BitMap *, const Rect *, const Rect *, short, RgnHandle);
extern void RGBBackColor(const RGBColor *);
extern RgnHandle GetPortVisibleRegion(CGrafPtr, RgnHandle);
extern void DisposeRgn(RgnHandle);
extern void GlobalToLocal(Point *);
extern void LMSetHiliteMode(UInt8);
extern UInt8 LMGetHiliteMode();
extern void GetClip(RgnHandle);
extern void SetClip(RgnHandle);
extern void ClipRect(const Rect *);
extern Cursor *GetQDGlobalsArrow(Cursor *);
extern PicHandle GetPicture(short);
extern void HideCursor(void);
extern void InitCursor(void);
extern void MacFrameRect(const Rect *);
extern void RectRgn(RgnHandle, const Rect *);
extern void DiffRgn(RgnHandle, RgnHandle, RgnHandle);
extern void MacUnionRgn(RgnHandle, RgnHandle, RgnHandle);
extern void MacShowCursor();
extern void DrawPicture(PicHandle, const Rect *);
extern Boolean SectRect(const Rect *, const Rect *, Rect *);
extern void PenSize(short, short);
extern void FrameArc(const Rect *, short, short);
extern void HiliteColor(const RGBColor *);
extern void LMGetHiliteRGB(RGBColor *);
extern Boolean QDIsPortBuffered(CGrafPtr);
extern void QDFlushPortBuffer(CGrafPtr, RgnHandle);
extern void ScrollRect(const Rect *, short, short, RgnHandle);
extern void MacCopyRgn(RgnHandle, RgnHandle);
extern void SetPt(Point *, short, short);
extern Rect *GetRegionBounds(RgnHandle, Rect *);
extern void InsetRgn(RgnHandle, short, short);
extern void DiffRgn(RgnHandle, RgnHandle, RgnHandle);
extern void MacInsetRect(Rect *, short, short);
extern void SetCPixel(short, short, const RGBColor *);
extern Boolean MacEqualRect(const Rect *, const Rect *);
extern void MacOffsetRgn(RgnHandle, short, short);
extern void EraseRgn(RgnHandle);
extern CTabHandle GetCTable(short);
extern void GetBackColor(RGBColor *);
extern long Color2Index(const RGBColor *);
extern void FrameRoundRect(const Rect *, short, short);
extern short Random();
extern void PenMode(short);
extern Boolean IsRegionRectangular(RgnHandle);
extern void GetPenState(PenState *);
extern void SetPenState(const PenState *);
extern void PenPixPat(PixPatHandle);
extern PixPatHandle GetPixPat(short);
extern Boolean EmptyRect(const Rect *);
extern PixMapHandle NewPixMap();
extern void DisposePixMap(PixMapHandle);
extern Boolean RectInRgn(const Rect *, RgnHandle);
extern Boolean EmptyRgn(RgnHandle);
extern void Pt2Rect(Point, Point, Rect *);
extern Rect *GetPixBounds(PixMapHandle, Rect *);
extern void SetOrigin(short, short);
extern void MacInvertRgn(RgnHandle);
extern void PenNormal();
extern const BitMap *GetPortBitMapForCopyBits(CGrafPtr);
extern void GetForeColor(RGBColor *);
extern void Index2Color(long, RGBColor *);
extern void CopyMask(const BitMap *, const BitMap *, const BitMap *, const Rect *, const Rect *, const Rect *);
extern void ObscureCursor();
extern CGrafPtr GetQDGlobalsThePort();

//Text funcs
extern void TextFont(short);
extern void TextSize(short);
extern short StringWidth(ConstStr255Param);
extern void TextFace(StyleParameter);
extern void GetFontInfo(FontInfo *);
extern void DrawChar(CharParameter);
extern short CharWidth(CharParameter);

//Offscreen
extern PixMapHandle GetGWorldPixMap(GWorldPtr);
extern void GetGWorld(CGrafPtr *, GDHandle *);
extern QDErr NewGWorld(GWorldPtr *, short, const Rect *, CTabHandle, GDHandle, GWorldFlags);
extern Boolean LockPixels(PixMapHandle pm);
extern void SetGWorld(CGrafPtr, GDHandle);
extern void UnlockPixels(PixMapHandle);
extern void DisposeGWorld(GWorldPtr);
extern void SetGDevice(GDHandle);

#pragma mark - Pen functions
extern void PenPat(const Pattern *);

//misc.
extern GDHandle DMGetFirstScreenDevice(Boolean);
extern GDHandle DMGetNextScreenDevice(GDHandle, Boolean);
extern OSErr DMGetDisplayIDByGDevice(GDHandle, DisplayIDType *, Boolean);
extern OSErr DMGetGDeviceByDisplayID(DisplayIDType, GDHandle *, Boolean);
extern OSErr SetDepth(GDHandle, short, short, short);
extern Pattern *GetQDGlobalsGray(Pattern *);
extern void SetEntries(short, short, CSpecArray);

__END_DECLS

#pragma pack (pop)

#endif


#endif
