//
//  OldCarbHeaders.h
//  PPMacho
//
//  Created by C.W. Betts on 9/4/13.
//
//

#ifndef PPMacho_OldCarbHeaders_h
#define PPMacho_OldCarbHeaders_h
#include <Carbon/Carbon.h>

#ifdef QD_HEADERS_ARE_PRIVATE
enum {
	invalColReq                   = -1    /*invalid color table request*/
};

enum {
	/* transfer modes */
	srcOr                         = 1,
	srcXor                        = 2,
	srcBic                        = 3,
	notSrcCopy                    = 4,
	notSrcOr                      = 5,
	notSrcXor                     = 6,
	notSrcBic                     = 7,
	patCopy                       = 8,
	patOr                         = 9,
	patXor                        = 10,
	patBic                        = 11,
	notPatCopy                    = 12,
	notPatOr                      = 13,
	notPatXor                     = 14,
	notPatBic                     = 15,   /* Special Text Transfer Mode */
	grayishTextOr                 = 49,
	hilitetransfermode            = 50,
	hilite                        = 50,   /* Arithmetic transfer modes */
	blend                         = 32,
	addPin                        = 33,
	addOver                       = 34,
	subPin                        = 35,
	addMax                        = 37,
	adMax                         = 37,
	subOver                       = 38,
	adMin                         = 39,
	ditherCopy                    = 64,   /* Transparent mode constant */
	transparent                   = 36
};

enum {
	italicBit                     = 1,
	ulineBit                      = 2,
	outlineBit                    = 3,
	shadowBit                     = 4,
	condenseBit                   = 5,
	extendBit                     = 6
};

enum {
	/* QuickDraw color separation constants */
	normalBit                     = 0,    /*normal screen mapping*/
	inverseBit                    = 1,    /*inverse screen mapping*/
	redBit                        = 4,    /*RGB additive mapping*/
	greenBit                      = 3,
	blueBit                       = 2,
	cyanBit                       = 8,    /*CMYBk subtractive mapping*/
	magentaBit                    = 7,
	yellowBit                     = 6,
	blackBit                      = 5
};

enum {
	blackColor                    = 33,   /*colors expressed in these mappings*/
	whiteColor                    = 30,
	redColor                      = 205,
	greenColor                    = 341,
	blueColor                     = 409,
	cyanColor                     = 273,
	magentaColor                  = 137,
	yellowColor                   = 69
};

enum {
	picLParen                     = 0,    /*standard picture comments*/
	picRParen                     = 1,
	clutType                      = 0,    /*0 if lookup table*/
	fixedType                     = 1,    /*1 if fixed table*/
	directType                    = 2,    /*2 if direct values*/
	gdDevType                     = 0     /*0 = monochrome 1 = color*/
};

enum {
	interlacedDevice              = 2,    /* 1 if single pixel lines look bad */
	hwMirroredDevice              = 4,    /* 1 if device is HW mirrored */
	roundedDevice                 = 5,    /* 1 if device has been “rounded” into the GrayRgn */
	hasAuxMenuBar                 = 6,    /* 1 if device has an aux menu bar on it */
	burstDevice                   = 7,
	ext32Device                   = 8,
	ramInit                       = 10,   /*1 if initialized from 'scrn' resource*/
	mainScreen                    = 11,   /* 1 if main screen */
	allInit                       = 12,   /* 1 if all devices initialized */
	screenDevice                  = 13,   /*1 if screen device [not used]*/
	noDriver                      = 14,   /* 1 if no driver for this GDevice */
	screenActive                  = 15,   /*1 if in use*/
	hiliteBit                     = 7,    /*flag bit in LMGet/SetHiliteMode*/
	pHiliteBit                    = 0,    /*flag bit in LMGet/SetHiliteMode when used with BitClr*/
	defQDColors                   = 127,  /*resource ID of clut for default QDColors*/
	/* pixel type */
	RGBDirect                     = 16,   /* 16 & 32 bits/pixel pixelType value */
	/* pmVersion values */
	baseAddr32                    = 4     /*pixmap base address is 32-bit address*/
};


enum {
	sysPatListID                  = 0,
	iBeamCursor                   = 1,
	crossCursor                   = 2,
	plusCursor                    = 3,
	watchCursor                   = 4
};

enum {
	kQDGrafVerbFrame              = 0,
	kQDGrafVerbPaint              = 1,
	kQDGrafVerbErase              = 2,
	kQDGrafVerbInvert             = 3,
	kQDGrafVerbFill               = 4
};

#if TARGET_OS_MAC
#define MacInvertRect InvertRect
#define MacOffsetRect OffsetRect
#define MacSetRect SetRect
#define MacPtInRect PtInRect
#endif

extern void GetPort(GrafPtr * port);
extern short GetPortTextFont(CGrafPtr port);
extern short GetPortTextSize(CGrafPtr port);
extern void PaintRect(const Rect * r);
extern void ForeColor(long color);
extern void MacInvertRect(const Rect * r);
extern void MacSetRect(Rect *r, short left, short top, short right, short bottom);
extern GDHandle GetDeviceList(void);
extern void MacOffsetRect(Rect * r, short dh, short dv);
extern GDHandle GetNextDevice(GDHandle curDevice);
extern BitMap *GetQDGlobalsScreenBits(BitMap * screenBits);
extern void LocalToGlobal(Point * pt);
extern Rect * GetPortBounds(CGrafPtr port, Rect *rect);
extern void pStrcpy( unsigned char *, const unsigned char *);
extern Boolean MacPtInRect( Point pt, const Rect *r);

//Text funcs
extern void TextFont(short font);
extern void TextSize(short size);
extern short StringWidth(ConstStr255Param s);

#endif

#endif
