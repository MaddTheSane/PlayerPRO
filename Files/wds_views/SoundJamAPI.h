/*
	File:		SoundJamAPI.h

	Contains:	

	Written by:	Jeff Robbin

	Copyright:	Copyright ©1998 Jeff Robbin

*/


#pragma once

#include <Carbon/Carbon.h>

#if PRAGMA_STRUCT_ALIGN
#pragma options align=power
#endif

struct PlayerMessageInfo;
struct PlayerPluginMessageInfo;
struct VisualPluginMessageInfo;
struct EncoderPluginMessageInfo;
struct SkinPluginMessageInfo;
struct DevicePluginMessageInfo;

#ifdef __cplusplus
extern "C" {
#endif

enum {
	kCurrentPluginMessageVersion = 8
};

enum {
	/* Track Kind */
	
	kFileKind		= 'FILE',
	kCDKind			= 'CD  ',
	kDeviceKind		= 'DEV ',
	kHTTPKind		= 'HTTP',
	kSoundInputKind	= 'SINP'
};

#define	kMaxURLSize	512

enum {
	// TrackHeader flags
	
	kTrackSupportsID3Tags	= (1L << 0)			/* Can have ID3 information tags. */
};

typedef struct SJTrackHeader {
	Str63			name;
	Str63			artist;
	Str63			album;
	
	Str63			notice;
	
	Str63			kind;
	
	Str63			genre;
	
	Str63			url;
	
	UInt32			trackNumber;
	UInt32			numTracks;
	
	UInt32			validFlags;
	UInt32			flags;
	
	SInt16			year;
	
	UInt8			reserved[62];
}SJTrackHeader;
//typedef struct TrackHeader TrackHeader;

struct FileLocation {
	FSSpec				spec;
	FInfo				fInfo;
};
typedef struct FileLocation FileLocation;

struct HTTPLocation {
	UInt32				length;
	UInt8				url[kMaxURLSize];
};
typedef struct HTTPLocation HTTPLocation;

struct CDPositioning {
	UInt8				minute;
	UInt8				second;
	UInt8				frame;
};
typedef struct CDPositioning CDPositioning;

struct CDLocation {
	SInt16				vRefNum;
	SInt16				dRefNum;
	SInt16				trackNumber;
	CDPositioning		baseTimePosition;
	CDPositioning		endTimePosition;
	UInt32				baseTimeInMS;
	UInt32				numFrames;
};
typedef struct CDLocation CDLocation;

struct SoundInputLocation {
	Str255				inputDevice;
};
typedef struct SoundInputLocation SoundInputLocation;

union TrackLocation {
	FileLocation		file;
	HTTPLocation		http;
	CDLocation			cd;
	SoundInputLocation	soundIn;
};
typedef union TrackLocation TrackLocation;

enum {
	/* playOptions */
	
	kTrackCanBeLiveSeeked		= (1L << 0),		/* Can handle rapid position messages */
	kTrackCanBeExtracted		= (1L << 1),		/* Can support extraction messages. */
	kTrackCanBeDecoded			= (1L << 2)			/* Can support decode messages. */
};

struct TrackSpec {
	Str255				name;
	Str255				artist;
	Str255				album;
	
	Str255				genre;
	
	UInt32				trackKind;
	TrackLocation		location;
	
	UInt32				trackID;
	UInt32				trackNumber;

	UInt32				totalTimeInMS;
	OptionBits			playOptions;
};
typedef struct TrackSpec TrackSpec;

enum {
	/* Plugin (main) messages */
	
	kPluginNullMessage					= 'null',

	kPluginInitializeMessage			= 'init',
	kPluginCleanupMessage				= 'clr ',
	
	kPluginIdleMessage					= 'idle'
};


enum {
	/* SoundJamAPI messages */
	
	kPlayerIdleMessage					= 'idle',	/* Give the player some time */
	kPlayerUnregisterPluginMessage		= 'unrg',	/* Unregister the plugin this comes from */
	
	kPlayerSetPreferenceMessage			= 'sprf',	/* Set a preference */
	kPlayerGetPreferenceMessage			= 'gprf',	/* Get a preference */
	
	kPlayerSetFullScreenMessage			= 'sful',	/* Set full screen mode */
	kPlayerSetFullScreenOptionsMessage	= 'sfop',	/* Set full screen options */
	
	kPlayerSetDefaultWindowSizeMessage	= 'sdef',	/* Set default window size */
	
	kPlayerDoneTrackMessage				= 'done',	/* Currently playing track finished. */
	kPlayerPlayTrackMessage				= 'play',	/* Clicked the play button */
	kPlayerStopTrackMessage				= 'stop',	/* Clicked the stop button */
	kPlayerPauseTrackMessage			= 'paus',	/* Clicked the pause button */
	kPlayerNextTrackMessage				= 'next',	/* Go to the next track */
	kPlayerPrevTrackMessage				= 'prev',	/* Go to the prev track */
	kPlayerLoadTrackMessage				= 'load',	/* Load a track with Standard File */
	
	kPlayerShowHideEQMessage			= 'eq  ',	/* Show or hide the eq window */
	kPlayerShowHidePlaylistMessage		= 'pl  ',	/* Show or hide the current playlist */
	
	kPlayerOpenPrefsMessage				= 'oprf',	/* Open the preferences dialog */
	kPlayerCloseWindowMessage			= 'clos',	/* Close the window */
	kPlayerMinimizeWindowMessage		= 'minw',	/* Minimize the window */
	
	kPlayerSetTrackPositionMessage		= 'spos',	/* Set the position */

	kPlayerSetVolumeBalanceMessage		= 'svol',	/* Set the volume and balance */

	kPlayerSetTrebleBassMessage			= 'stb ',	/* Set the treble and bass */

	kPlayerSetEQLevelMessage			= 'seql',	/* Set the eq level for a band. */
	kPlayerSetEQOptionsMessage			= 'seqo',	/* Set the eq options. */
	
	kPlayerPopupEQPresetsMenuMessage	= 'eqpr',	/* Popup the EQ presets menu. */
	
	kPlayerShowAboutMessage				= 'abou',	/* Show the about box. */
	kPlayerOpenURLMessage				= 'url ',	/* Open a URL */
	
	kPlayerDragWindowMessage			= 'drag',	/* Drag a window */
	
	kPlayerDecodeDoneMessage			= 'ddcd',	/* Play some data (INTERRUPT-SAFE from Player plugin). */
	kPlayerInstallDeferredTaskMessage	= 'indt',	/* Install a deferredTask (INTERRUPT-SAFE from Player plugin). */
	
	kPlayerExtractProgressMessage		= 'extp',	/* Report on the progress of the extract */
	
	kPlayerRegisterVisualPluginMessage	= 'rvis',	/* Register a visual plugin */
	kPlayerRegisterPlayerPluginMessage	= 'rply',	/* Register a player plugin */
	kPlayerRegisterEncoderPluginMessage	= 'renc',	/* Register an encoder plugin */
	kPlayerRegisterSkinPluginMessage	= 'rskn',	/* Register a skin plugin */
	kPlayerRegisterDevicePluginMessage	= 'rdev',	/* Register a device plugin */
		
	kPlayerSetPluginDataMessage			= 'sprf',	/* Set plugin preferences */
	kPlayerGetPluginDataMessage			= 'gprf'	/* Get plugin preferences */
};


enum {
	/* Plugin (main) options */
	
	kKeepPluginResourceForkOpen = (1L << 0),	/* Keep the plugin resource fork open */
												/* beyond the initialization message. */
	kPluginWantsIdleMessages	= (1L << 1),
	kPluginWantsToBeLeftOpen	= (1L << 2)		/* Don't close this plugin just because */
												/* he didn't register anyone */
};


typedef OSStatus (*SoundJamProcPtr)(void *appCookie, OSType message, struct PlayerMessageInfo *messageInfo);


/*
	Plugin main Messages
*/

struct PluginInitMessage {
	UInt32							version;				/* Input */

	void *							appCookie;				/* Input */
	SoundJamProcPtr					playerProc;				/* Input */
	
	OptionBits						options;				/* Output */
	void *							refcon;					/* Output */
};
typedef struct PluginInitMessage PluginInitMessage;

struct PluginMessageInfo {
	union {
		PluginInitMessage			initMessage;
	} u;
};
typedef struct PluginMessageInfo PluginMessageInfo;



/* Plugin main. */
typedef OSStatus (*PluginProcPtr)(OSType message, PluginMessageInfo *messageInfo, void *refcon);

typedef OSStatus (*PlayerPluginProcPtr)(OSType message, struct PlayerPluginMessageInfo *messageInfo, void *refcon);
typedef OSStatus (*VisualPluginProcPtr)(OSType message, struct VisualPluginMessageInfo *messageInfo, void *refcon);
typedef OSStatus (*EncoderPluginProcPtr)(OSType message, struct EncoderPluginMessageInfo *messageInfo, void *refcon);
typedef OSStatus (*SkinPluginProcPtr)(OSType message, struct SkinPluginMessageInfo *messageInfo, void *refcon);
typedef OSStatus (*DevicePluginProcPtr)(OSType message, struct DevicePluginMessageInfo *messageInfo, void *refcon);



/*
	Exported interfaces from the Player
*/

enum {
	kRepeatOff	= 0,
	kRepeatAllTracks,
	kRepeatOneTrack
};

enum {
	/* Preferences */
	
	kPrefSetTimeDisplay					= 'stim',	/* Set the time display mode */
	kPrefShuffleTracks					= 'shuf',	/* Shuffle tracks Off/On (0 or 1) */
	kPrefRepeatTracks					= 'rept',	/* Repeat tracks (see enum). */
	kPrefDisplayTrackNumber				= 'dtrk'	/* Display the track number */
};

enum {
	/* EQ Options */
	
	kEQEnabled		= (1L << 0),
	kEQAuto			= (1L << 1)
};

enum {
	/* Tone Options */
	
	kToneEnabled	= (1L << 0)
};

enum {
	/* RegisterVisualMessage.options */
	
	kVisualRequiresFullScreen			= (1L << 0),
	kVisualForBuiltIn					= (1L << 1),
	kVisualIsResizeable					= (1L << 2),
	kVisualWantsIdleMessages			= (1L << 3),
	kVisualWantsToProcessSamples		= (1L << 4),
	kVisualWantsConfigure				= (1L << 5),
	kVisualWantsAbout					= (1L << 6),
	kVisualEnabledByDefault				= (1L << 7)
};

enum {
	/* RegisterPlayerMessage.options */
	kPlayerWantsIdleMessages			= (1L << 0)
};

enum {
	/* RegisterEncoderMessage.options */
	kEncoderWantsIdleMessages			= (1L << 0)
};

enum {
	/* RegisterSkinMessage.options */
	kSkinWantsIdleMessages				= (1L << 0)
};

enum {
	/* RegisterDeviceMessage.options */
	kDeviceWantsIdleMessages			= (1L << 0)
};

struct PlayerPreferenceMessage {
	OSType							preference;
	UInt32							value;
};
typedef struct PlayerPreferenceMessage PlayerPreferenceMessage;

struct PlayerSetFullScreenMessage {
	Boolean							fullScreen;
};
typedef struct PlayerSetFullScreenMessage PlayerSetFullScreenMessage;

struct PlayerSetFullScreenOptionsMessage {
	SInt16							minBitDepth;
	SInt16							maxBitDepth;
	SInt16							preferredBitDepth;
	
	SInt16							desiredWidth;
	SInt16							desiredHeight;
};
typedef struct PlayerSetFullScreenOptionsMessage PlayerSetFullScreenOptionsMessage;

struct PlayerSetDefaultWindowSizeMessage {
	SInt16							defaultWidth;
	SInt16							defaultHeight;
	
	SInt16							minWidth;
	SInt16							minHeight;

	SInt16							maxWidth;
	SInt16							maxHeight;	
};
typedef struct PlayerSetDefaultWindowSizeMessage PlayerSetDefaultWindowSizeMessage;

struct PlayerShowHideMessage {
	Boolean							showWindow;
};
typedef struct PlayerShowHideMessage PlayerShowHideMessage;

struct PlayerDecodeDoneMessage {
	void *							playerCookie;
	OSStatus						readStatus;
	OSStatus						decodeStatus;
	UInt32							numSamples;
};
typedef struct PlayerDecodeDoneMessage PlayerDecodeDoneMessage;

struct PlayerExtractProgressMessage {
	void *							extractCookie;
	UInt32							progressInMS;
};
typedef struct PlayerExtractProgressMessage PlayerExtractProgressMessage;

struct PlayerSetTrackPositionMessage {
	UInt32							positionInMS;
};
typedef struct PlayerSetTrackPositionMessage PlayerSetTrackPositionMessage;

struct PlayerSetVolumeBalanceMessage {
	SInt32							volume;		/* 0-0x100 |  0-0x100 ((Right << 16) | Left) */
	SInt32							balance;	/* 0-100 (Left to Right) */
};
typedef struct PlayerSetVolumeBalanceMessage PlayerSetVolumeBalanceMessage;

struct PlayerSetTrebleBassMessage {
	SInt32							treble;		/* -100 - 100 */
	SInt32							bass;		/* -100 - 100 */
};
typedef struct PlayerSetTrebleBassMessage PlayerSetTrebleBassMessage;

struct PlayerSetEQLevelMessage {
	UInt32							band;		/* 0-9 */
	UInt32							level;		/* 0-2400 */
};
typedef struct PlayerSetEQLevelMessage PlayerSetEQLevelMessage;

struct PlayerSetEQOptionsMessage {
	OptionBits						options;
};
typedef struct PlayerSetEQOptionsMessage PlayerSetEQOptionsMessage;

struct PlayerPopupEQPresetsMenuMessage {
	Point							localPt;
};
typedef struct PlayerPopupEQPresetsMenuMessage PlayerPopupEQPresetsMenuMessage;

struct PlayerOpenURLMessage {
	SInt8 *							url;
	UInt32							length;
};
typedef struct PlayerOpenURLMessage PlayerOpenURLMessage;

struct PlayerCloseWindowMessage {
	WindowPtr						window;
};
typedef struct PlayerCloseWindowMessage PlayerCloseWindowMessage;

struct PlayerMinimizeWindowMessage {
	WindowPtr						window;
	Boolean							minimized;
};
typedef struct PlayerMinimizeWindowMessage PlayerMinimizeWindowMessage;

struct PlayerDragWindowMessage {
	WindowPtr						window;
	Point							globalWhere;
};
typedef struct PlayerDragWindowMessage PlayerDragWindowMessage;

struct PlayerRegisterPlayerPluginMessage {
	/* Input from plugin */
	
	Str63							name;
	OptionBits						options;
	
	UInt32							messageVersion;
	PlayerPluginProcPtr				handler;
	void *							registerRefcon;
};
typedef struct PlayerRegisterPlayerPluginMessage PlayerRegisterPlayerPluginMessage;

struct PlayerRegisterVisualPluginMessage {
	/* Input from plugin */
	
	Str63							name;
	OptionBits						options;
	
	UInt32							messageVersion;
	VisualPluginProcPtr				handler;
	void *							registerRefcon;

	UInt32							timeBetweenDataInMS;
	UInt32							numWaveformChannels;
	UInt32							numSpectrumChannels;
	
	SInt16							minWidth;					/* For backward compatibility with 1.0 only (default window size) */
	SInt16							minHeight;					/* For backward compatibility with 1.0 only (default window size) */
	
	SInt16							maxWidth;					/* For backward compatibility with 1.0 only (should not be really large) */
	SInt16							maxHeight;					/* For backward compatibility with 1.0 only (should not be really large) */
	
																/* Call PlayerSetDefaultWindowSize from */
																/* the initializeMessage in the visual plugin */
																/* handler to set the resizable min/max width & height. */
	
	UInt16							minFullScreenBitDepth;		/* 0 = Any */
	UInt16							maxFullScreenBitDepth;		/* 0 = Any */
	
	UInt16							windowAlignmentInBytes;		/* 0 = Any */
};
typedef struct PlayerRegisterVisualPluginMessage PlayerRegisterVisualPluginMessage;

struct PlayerRegisterEncoderPluginMessage {
	/* Input from plugin */
	
	Str63							name;
	OptionBits						options;
	
	UInt32							messageVersion;
	EncoderPluginProcPtr			handler;
	void *							registerRefcon;
};
typedef struct PlayerRegisterEncoderPluginMessage PlayerRegisterEncoderPluginMessage;

struct PlayerRegisterSkinPluginMessage {
	/* Input from plugin */
	
	Str63							name;
	OptionBits						options;
	
	UInt32							messageVersion;
	SkinPluginProcPtr				handler;
	void *							registerRefcon;
};
typedef struct PlayerRegisterSkinPluginMessage PlayerRegisterSkinPluginMessage;

struct PlayerRegisterDevicePluginMessage {
	/* Input from plugin */
	
	Str63							name;
	OptionBits						options;
	
	UInt32							messageVersion;
	DevicePluginProcPtr				handler;
	void *							registerRefcon;
	
	PicHandle						logoPicture;
	UInt8 *							logoURL;
};
typedef struct PlayerRegisterDevicePluginMessage PlayerRegisterDevicePluginMessage;

struct PlayerCreateDeviceWindowMessage {
	void *							handlerData;	// DeviceHandlerData
};
typedef struct PlayerCreateDeviceWindowMessage PlayerCreateDeviceWindowMessage;

struct PlayerInstallDeferredTaskMessage {
	DeferredTask *					deferredTask;
};
typedef struct PlayerInstallDeferredTaskMessage PlayerInstallDeferredTaskMessage;

struct PlayerSetPluginDataMessage {
	void *							dataPtr;
	UInt32							dataSize;
};
typedef struct PlayerSetPluginDataMessage PlayerSetPluginDataMessage;

struct PlayerGetPluginDataMessage {
	void *							dataPtr;		/* Input */
	UInt32							dataBufferSize;	/* Input */
	
	UInt32							dataSize;		/* Output */
};
typedef struct PlayerGetPluginDataMessage PlayerGetPluginDataMessage;

struct PlayerMessageInfo {
	UInt32									version;
	union {
		PlayerPreferenceMessage				preferenceMessage;
		PlayerSetFullScreenMessage			setFullScreenMessage;
		PlayerSetFullScreenOptionsMessage	setFullScreenOptionsMessage;
		PlayerSetDefaultWindowSizeMessage	setDefaultWindowSizeMessage;
		PlayerShowHideMessage				showHideMessage;
		PlayerDecodeDoneMessage				decodeDoneMessage;
		PlayerExtractProgressMessage		extractProgressMessage;
		PlayerInstallDeferredTaskMessage	installDeferredTaskMessage;
		PlayerSetTrackPositionMessage		setTrackPositionMessage;
		PlayerSetVolumeBalanceMessage		setVolumeBalanceMessage;
		PlayerSetTrebleBassMessage			setTrebleBassMessage;
		PlayerSetEQLevelMessage				setEQLevelMessage;
		PlayerSetEQOptionsMessage			setEQOptionsMessage;
		PlayerPopupEQPresetsMenuMessage		popupEQPresetsMenuMessage;
		PlayerOpenURLMessage				openURLMessage;
		PlayerCloseWindowMessage			closeWindowMessage;
		PlayerMinimizeWindowMessage			minimizeWindowMessage;
		PlayerDragWindowMessage				dragWindowMessage;
		PlayerGetPluginDataMessage			getPluginDataMessage;
		PlayerSetPluginDataMessage			setPluginDataMessage;
		PlayerRegisterVisualPluginMessage	registerVisualPluginMessage;
		PlayerRegisterPlayerPluginMessage	registerPlayerPluginMessage;
		PlayerRegisterEncoderPluginMessage	registerEncoderPluginMessage;
		PlayerRegisterSkinPluginMessage		registerSkinPluginMessage;
		PlayerRegisterDevicePluginMessage	registerDevicePluginMessage;
	} u;
};
typedef struct PlayerMessageInfo PlayerMessageInfo;

extern OSStatus PlayerIdle (void *appCookie, SoundJamProcPtr playerHandler);
extern OSStatus PlayerSetPreference (void *appCookie, SoundJamProcPtr playerHandler, OSType preference, UInt32 value);
extern OSStatus PlayerGetPreference (void *appCookie, SoundJamProcPtr playerHandler, OSType preference, UInt32 *value);
extern OSStatus PlayerSetFullScreen (void *appCookie, SoundJamProcPtr playerHandler, Boolean fullScreen);
extern OSStatus PlayerSetFullScreenOptions (void *appCookie, SoundJamProcPtr playerHandler, SInt16 minBitDepth, SInt16 maxBitDepth, SInt16 preferredBitDepth, SInt16 desiredWidth, SInt16 desiredHeight);
extern OSStatus PlayerSetDefaultWindowSize (	void *			appCookie,
												SoundJamProcPtr	playerHandler,
												SInt16			defaultWidth,
												SInt16			defaultHeight,
												SInt16			minWidth,
												SInt16			minHeight,
												SInt16			maxWidth,
												SInt16			maxHeight);
extern void		PlayerShowHideEQWindow (void *appCookie, SoundJamProcPtr playerHandler, Boolean showWindow);
extern void		PlayerShowHidePlaylistWindow (void *appCookie, SoundJamProcPtr playerHandler, Boolean showWindow);
extern OSStatus PlayerDoneTrack (void *appCookie, SoundJamProcPtr playerHandler);
extern OSStatus PlayerNextTrack (void *appCookie, SoundJamProcPtr playerHandler);
extern OSStatus PlayerPrevTrack (void *appCookie, SoundJamProcPtr playerHandler);
extern OSStatus PlayerPlayTrack (void *appCookie, SoundJamProcPtr playerHandler);
extern OSStatus PlayerStopTrack (void *appCookie, SoundJamProcPtr playerHandler);
extern OSStatus PlayerPauseTrack (void *appCookie, SoundJamProcPtr playerHandler);
extern OSStatus PlayerLoadTrack (void *appCookie, SoundJamProcPtr playerHandler);
extern OSStatus	PlayerSetTrackPosition (void *appCookie, SoundJamProcPtr playerHandler, UInt32 positionInMS);
extern OSStatus PlayerSetVolumeBalance (void *appCookie, SoundJamProcPtr playerHandler, SInt32 volume, SInt32 balance);
extern OSStatus PlayerSetTrebleBass (void *appCookie, SoundJamProcPtr playerHandler, SInt32 treble, SInt32 bass);
extern void		PlayerShowAbout (void *appCookie, SoundJamProcPtr playerHandler);
extern void		PlayerOpenURL (void *appCookie, SoundJamProcPtr playerHandler, SInt8 *string, UInt32 length);
extern OSStatus PlayerSetEQLevel (void *appCookie, SoundJamProcPtr playerHandler, UInt32 band, SInt32 level);
extern OSStatus PlayerSetEQOptions (void *appCookie, SoundJamProcPtr playerHandler, OptionBits options);
extern OSStatus PlayerPopupEQPresetsMenu (void *appCookie, SoundJamProcPtr playerHandler, Point localPt);
extern void		PlayerDragWindow (void *appCookie, SoundJamProcPtr playerHandler, WindowPtr window, Point globalWhere);
extern OSStatus	PlayerGetPluginData (void *appCookie, SoundJamProcPtr playerHandler, void *dataPtr, UInt32 dataBufferSize, UInt32 *dataSize);
extern OSStatus	PlayerSetPluginData (void *appCookie, SoundJamProcPtr playerHandler, void *dataPtr, UInt32 dataSize);
extern void		PlayerOpenPrefs (void *appCookie, SoundJamProcPtr playerHandler);
extern void		PlayerCloseWindow (void *appCookie, SoundJamProcPtr playerHandler, WindowPtr window);
extern void		PlayerMinimizeWindow (void *appCookie, SoundJamProcPtr playerHandler, WindowPtr window, Boolean isMinimized);
extern void		PlayerDecodeDone (void *appCookie, SoundJamProcPtr playerHandler, void *playerCookie, OSStatus readStatus, OSStatus decodeStatus, UInt32 numSamples);
extern OSStatus PlayerExtractProgress (void *appCookie, SoundJamProcPtr playerHandler, void *extractCookie, UInt32 progressInMS);
extern OSStatus PlayerRegisterVisualPlugin (void *appCookie, SoundJamProcPtr playerHandler, PlayerMessageInfo *messageInfo);
extern OSStatus PlayerRegisterPlayerPlugin (void *appCookie, SoundJamProcPtr playerHandler, PlayerMessageInfo *messageInfo);
extern OSStatus PlayerRegisterEncoderPlugin (void *appCookie, SoundJamProcPtr playerHandler, PlayerMessageInfo *messageInfo);
extern OSStatus PlayerRegisterSkinPlugin (void *appCookie, SoundJamProcPtr playerHandler, PlayerMessageInfo *messageInfo);
extern OSStatus PlayerRegisterDevicePlugin (void *appCookie, SoundJamProcPtr playerHandler, PlayerMessageInfo *messageInfo);
extern OSStatus	PlayerUnregisterPlugin (void *appCookie, SoundJamProcPtr playerHandler, PlayerMessageInfo *messageInfo);
extern OSStatus PlayerInstallDeferredTask (void *appCookie, SoundJamProcPtr playerHandler, DeferredTask *dTask);

#if PRAGMA_STRUCT_ALIGN
#pragma options align=reset
#endif

#ifdef __cplusplus
}
#endif

