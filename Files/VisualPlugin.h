/*
	File:		VisualPlugin.h

	Contains:	

	Written by:	Jeff Robbin

	Copyright:	Copyright ©1999 Jeff Robbin

*/


#pragma once

#include "SoundJamAPI.h"

#if PRAGMA_STRUCT_ALIGN
	#pragma options align=power
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum {
	/* VisualPlugin messages */
	
	kVisualPluginIdleMessage			= 'null',

	kVisualPluginInitializeMessage		= 'init',
	kVisualPluginCleanupMessage			= 'clr ',
	
	kVisualPluginConfigureMessage		= 'cnfg',	/* Configure the plugin (may not be enabled) */
	kVisualPluginAboutMessage			= 'abou',	/* Display any about information. (may not be enabled) */
	
	kVisualPluginProcessSamplesMessage	= 'mods',	/* Process samples (INTERRUPT-SAFE). */
	kVisualPluginFlushSamplesMessage	= 'flus',	/* Flush any samples being processed. */
													/* - Probably from a stream reposition. */

	kVisualPluginEnableMessage			= 'von ',	/* Turn on the module */
	kVisualPluginDisableMessage			= 'voff',	/* Turn off the module */
	
	kVisualPluginShowWindowMessage		= 'show',	/* Show the plugin window */
	kVisualPluginHideWindowMessage		= 'hide',	/* Hide the plugin window */
	
	kVisualPluginResizeMessage			= 'resz',	/* Resizing request */
	kVisualPluginSetWindowMessage		= 'swin',	/* Change the window parameters */
	
	kVisualPluginRenderMessage			= 'vrnd',	/* Render some samples */
	kVisualPluginUpdateMessage			= 'vupd',	/* Update the window */
	kVisualPluginPlayMessage			= 'vply',	/* Playing a track */
	kVisualPluginStopMessage			= 'vstp',	/* Stopping a track */
	kVisualPluginPauseMessage			= 'vpau',	/* Pausing a track */
	kVisualPluginUnpauseMessage			= 'vunp',	/* Unpausing a track */
	kVisualPluginEventMessage			= 'vevt'	/* Mac-event. */
};

/*
	VisualPlugin messages
*/

enum {
	kVisualMaxDataChannels		= 2,

	kVisualNumWaveformEntries	= 512,
	kVisualNumSpectrumEntries	= 512
};

enum {
	/* ShowWindow options */
	
	kWindowIsFullScreen = (1L << 0)
};

struct RenderVisualData {
	UInt8							numWaveformChannels;
	UInt8							waveformData[kVisualMaxDataChannels][kVisualNumWaveformEntries];
	
	UInt8							numSpectrumChannels;
	UInt8							spectrumData[kVisualMaxDataChannels][kVisualNumSpectrumEntries];
};
typedef struct RenderVisualData RenderVisualData;

struct VisualPluginInitMessage {
	UInt32							version;				/* Input */

	void *							appCookie;				/* Input */
	SoundJamProcPtr					playerProc;				/* Input */

	OptionBits						options;				/* Output */
	void *							refcon;					/* Output */
};
typedef struct VisualPluginInitMessage VisualPluginInitMessage;

struct VisualPluginShowWindowMessage {
	CGrafPtr						port;					/* Input - May be nil! */
	Rect							drawRect;				/* Input */
	OptionBits						options;				/* Input */
};
typedef struct VisualPluginShowWindowMessage VisualPluginShowWindowMessage;

struct VisualPluginSetWindowMessage {
	CGrafPtr						port;					/* Input - May be nil! */
	Rect							drawRect;				/* Input */
	OptionBits						options;				/* Input */
};
typedef struct VisualPluginSetWindowMessage VisualPluginSetWindowMessage;

struct VisualPluginResizeMessage {
	SInt16							desiredWidth;			/* Input */
	SInt16							desiredHeight;			/* Input */
	
	SInt16							approvedWidth;			/* Output */
	SInt16							approvedHeight;			/* Output */
};
typedef struct VisualPluginResizeMessage VisualPluginResizeMessage;

struct VisualPluginPlayMessage {
	TrackSpec *						trackSpec;				/* Input */
	SInt32							volume;					/* Input */
	
	UInt32							bitRate;				/* Input */
	
	SoundComponentData				soundFormat;			/* Input */
};
typedef struct VisualPluginPlayMessage VisualPluginPlayMessage;

struct VisualPluginRenderMessage {
	RenderVisualData *				renderData;
	UInt32							timeStampID;
};
typedef struct VisualPluginRenderMessage VisualPluginRenderMessage;

struct VisualPluginEventMessage {
	EventRecord *					event;
};
typedef struct VisualPluginEventMessage VisualPluginEventMessage;

struct VisualPluginProcessSamplesMessage {

	UInt32							timeStampID;			/* Input */

	SInt16 *						sampleBuffer;			/* Input */
		
	UInt32							numSamples;				/* Input */
	UInt32							maxSamples;				/* Input */
	
	UInt32							numOutputSamples;		/* Output (<= maxSamples) */
};
typedef struct VisualPluginProcessSamplesMessage VisualPluginProcessSamplesMessage;

struct VisualPluginMessageInfo {
	union {
		VisualPluginInitMessage				initMessage;
		VisualPluginShowWindowMessage		showWindowMessage;
		VisualPluginSetWindowMessage		setWindowMessage;
		VisualPluginResizeMessage			resizeMessage;
		VisualPluginPlayMessage				playMessage;
		VisualPluginRenderMessage			renderMessage;
		VisualPluginEventMessage			eventMessage;

		VisualPluginProcessSamplesMessage	processSamplesMessage;
	} u;
};
typedef struct VisualPluginMessageInfo VisualPluginMessageInfo;

#ifdef __cplusplus
}
#endif

#if PRAGMA_STRUCT_ALIGN
	#pragma options align=reset
#endif

