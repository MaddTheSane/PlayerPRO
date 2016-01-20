/*
	File:		RequestVideo.h

	Contains:	

	Written by: Eric Anderson	

	Copyright:	Copyright © 1995-1999 by Apple Computer, Inc., All Rights Reserved.

				You may incorporate this Apple sample source code into your program(s) without
				restriction. This Apple sample source code has been provided "AS IS" and the
				responsibility for its operation is yours. You are not permitted to redistribute
				this Apple sample source code as "Apple sample source code" after having made
				changes. If you're going to re-distribute the source, we require that you make
				it clear in the source that the code was descended from Apple sample source
				code, but that you've made changes.

	Change History (most recent first):
				7/15/1999	Karl Groethe	Updated for Metrowerks Codewarror Pro 2.1
				5/31/95		ewa				Added RVGetCurrentVideoSetting and RVConfirmVideoRequest routines
											to make it easy to revert back to where you came from and to give
											the user a chance to confirm the new setting if the new mode was
											valid (ie: the card supports it) but not safe (the monitor may not).
				5/24/95		ewa				Give the kAllValidModesBit requestFlags option for safe only or all
											valid resolution timings.
	
				

*/
 
#include <Carbon/Carbon.h>

// requestFlags bit values in VideoRequestRec (example use: 1<<kAbsoluteRequestBit)
enum {
	kBitDepthPriorityBit		= 0,	// Bit depth setting has priority over resolution
	kAbsoluteRequestBit			= 1,	// Available setting must match request
	kShallowDepthBit			= 2,	// Match bit depth less than or equal to request
	kMaximizeResBit				= 3,	// Match screen resolution greater than or equal to request
	kAllValidModesBit			= 4		// Match display with valid timing modes (may include modes which are not marked as safe)
};

// availFlags bit values in VideoRequestRec (example use: 1<<kModeValidNotSafeBit)
enum {
	kModeValidNotSafeBit		= 0		//  Available timing mode is valid but not safe (requires user confirmation of switch)
};

// video request structure
struct VideoRequestRec	{
	GDHandle		screenDevice;		// <in/out>	nil will force search of best device, otherwise search this device only
	short			reqBitDepth;		// <in>		requested bit depth
	short			availBitDepth;		// <out>	available bit depth
	unsigned long	reqHorizontal;		// <in>		requested horizontal resolution
	unsigned long	reqVertical;		// <in>		requested vertical resolution
	unsigned long	availHorizontal;	// <out>	available horizontal resolution
	unsigned long	availVertical;		// <out>	available vertical resolution
	unsigned long	requestFlags;		// <in>		request flags
	unsigned long	availFlags;			// <out>	available mode flags
	unsigned long	displayMode;		// <out>	mode used to set the screen resolution
	unsigned long	depthMode;			// <out>	mode used to set the depth
	Fixed 			refreshRate;		// <out>	Vertical Refresh Rate in Hz
	VDSwitchInfoRec	switchInfo;			// <out>	DM2.0 uses this rather than displayMode/depthMode combo
};
typedef struct VideoRequestRec VideoRequestRec;
typedef struct VideoRequestRec *VideoRequestRecPtr;

// Routine defines
OSErr RVRequestVideoSetting(VideoRequestRecPtr requestRecPtr);
OSErr RVGetCurrentVideoSetting(VideoRequestRecPtr requestRecPtr);
OSErr RVSetVideoRequest (VideoRequestRecPtr requestRecPtr);
OSErr RVConfirmVideoRequest (VideoRequestRecPtr requestRecPtr);
OSErr RVSetVideoAsScreenPrefs (void);

