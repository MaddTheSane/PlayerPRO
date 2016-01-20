//
//  DisplayManagerHidden.h
//  PPMacho
//
//  Created by C.W. Betts on 1/20/16.
//
//

#ifndef DisplayManagerHidden_h
#define DisplayManagerHidden_h

#include <Carbon/Carbon.h>

#ifdef QD_HEADERS_ARE_PRIVATE

#pragma pack (push, 2)

enum {
	cscGetMode                    = 2,
	cscGetEntries                 = 3,
	cscGetPageCnt                 = 4,
	cscGetPages                   = 4,
	cscGetPageBase                = 5,
	cscGetBaseAddr                = 5,
	cscGetGray                    = 6,
	cscGetInterrupt               = 7,
	cscGetGamma                   = 8,
	cscGetDefaultMode             = 9,
	cscGetCurMode                 = 10,
	cscGetSync                    = 11,
	cscGetConnection              = 12,
	cscGetModeTiming              = 13,
	cscGetModeBaseAddress         = 14,
	cscGetScanProc                = 15,
	cscGetPreferredConfiguration  = 16,
	cscGetNextResolution          = 17,
	cscGetVideoParameters         = 18,
	cscGetGammaInfoList           = 20,
	cscRetrieveGammaTable         = 21,
	cscSupportsHardwareCursor     = 22,
	cscGetHardwareCursorDrawState = 23,
	cscGetConvolution             = 24,
	cscGetPowerState              = 25,
	cscPrivateStatusCall          = 26,
	cscGetDDCBlock                = 27,
	cscGetMultiConnect            = 28,
	cscGetClutBehavior            = 29,
	cscGetTimingRanges            = 30,
	cscGetDetailedTiming          = 31,
	cscGetCommunicationInfo       = 32
};

#ifdef CF_ENUM
typedef CF_ENUM(unsigned short, DepthMode) {
	kDepthMode1 = 128,
	kDepthMode2 = 129,
	kDepthMode3 = 130,
	kDepthMode4 = 131,
	kDepthMode5 = 132,
	kDepthMode6 = 133
};
#else
typedef unsigned short DepthMode;
enum {
	kDepthMode1 = 128,
	kDepthMode2 = 129,
	kDepthMode3 = 130,
	kDepthMode4 = 131,
	kDepthMode5 = 132,
	kDepthMode6 = 133
};
#endif

enum {
	sixteenBitMode		= 132,
	thirtyTwoBitMode	= 133,
	firstVidMode		= 128,
	secondVidMode		= 129,
	thirdVidMode		= 130,
	fourthVidMode		= 131,
	fifthVidMode		= 132,
	sixthVidMode		= 133,
	spGammaDir			= 64,
	spVidNamesDir		= 65
};

struct VPBlock {
	long	vpBaseOffset;
	short	vpRowBytes;
	Rect	vpBounds;
	short	vpVersion;
	short	vpPackType;
	long	vpPackSize;
	long	vpHRes;
	long	vpVRes;
	short	vpPixelType;
	short	vpPixelSize;
	short	vpCmpCount;
	short	vpCmpSize;
	long	vpPlaneBytes;
};
typedef struct VPBlock	VPBlock;
typedef VPBlock *		VPBlockPtr;

struct VDTimingInfoRec {
	unsigned long csTimingMode;
	unsigned long csTimingReserved;
	unsigned long csTimingFormat;
	unsigned long csTimingData;
	unsigned long csTimingFlags;
};
typedef struct VDTimingInfoRec	VDTimingInfoRec;
typedef VDTimingInfoRec *		VDTimingInfoPtr;


struct VDPowerStateRec {
	unsigned long powerState;
	unsigned long powerFlags;
	
	unsigned long powerReserved1;
	unsigned long powerReserved2;
};
typedef struct VDPowerStateRec	VDPowerStateRec;
typedef VDPowerStateRec *		VDPowerStatePtr;

struct VDResolutionInfoRec {
	DisplayModeID	csPreviousDisplayModeID;
	DisplayModeID	csDisplayModeID;
	unsigned long	csHorizontalPixels;
	unsigned long	csVerticalLines;
	Fixed			csRefreshRate;
	DepthMode		csMaxDepthMode;
	unsigned long	csResolutionFlags;
	unsigned long	csReserved;
};
typedef struct VDResolutionInfoRec	VDResolutionInfoRec;
typedef VDResolutionInfoRec *		VDResolutionInfoPtr;

typedef ProcessSerialNumberPtr	DMProcessInfoPtr;
typedef ModalFilterUPP			DMModalFilterUPP;

enum {
	kModeValid			= 0,
	kModeSafe			= 1,
	kModeDefault		= 2,
	kModeShowNow		= 3,
	kModeNotResize		= 4,
	kModeRequiresPan	= 5,
	kModeInterlaced		= 6,
	kModeShowNever		= 7,
	kModeSimulscan		= 8,
	kModeNotPreset		= 9,
	kModeBuiltIn		= 10,
	kModeStretched		= 11
};

enum {
	kAESystemConfigNotice	= FOUR_CHAR_CODE('cnfg'),
	kAEDisplayNotice		= FOUR_CHAR_CODE('dspl'),
	kAEDisplaySummary		= FOUR_CHAR_CODE('dsum'),
	keyDMConfigVersion		= FOUR_CHAR_CODE('dmcv'),
	keyDMConfigFlags		= FOUR_CHAR_CODE('dmcf'),
	keyDMConfigReserved		= FOUR_CHAR_CODE('dmcr'),
	keyDisplayID			= FOUR_CHAR_CODE('dmid'),
	keyDisplayComponent		= FOUR_CHAR_CODE('dmdc'),
	keyDisplayDevice		= FOUR_CHAR_CODE('dmdd'),
	keyDisplayFlags			= FOUR_CHAR_CODE('dmdf'),
	keyDisplayMode			= FOUR_CHAR_CODE('dmdm'),
	keyDisplayModeReserved	= FOUR_CHAR_CODE('dmmr'),
	keyDisplayReserved		= FOUR_CHAR_CODE('dmdr'),
	keyDisplayMirroredId	= FOUR_CHAR_CODE('dmmi'),
	keyDeviceFlags			= FOUR_CHAR_CODE('dddf'),
	keyDeviceDepthMode		= FOUR_CHAR_CODE('dddm'),
	keyDeviceRect			= FOUR_CHAR_CODE('dddr'),
	keyPixMapRect			= FOUR_CHAR_CODE('dpdr'),
	keyPixMapHResolution	= FOUR_CHAR_CODE('dphr'),
	keyPixMapVResolution	= FOUR_CHAR_CODE('dpvr'),
	keyPixMapPixelType		= FOUR_CHAR_CODE('dppt'),
	keyPixMapPixelSize		= FOUR_CHAR_CODE('dpps'),
	keyPixMapCmpCount		= FOUR_CHAR_CODE('dpcc'),
	keyPixMapCmpSize		= FOUR_CHAR_CODE('dpcs'),
	keyPixMapAlignment		= FOUR_CHAR_CODE('dppa'),
	keyPixMapResReserved	= FOUR_CHAR_CODE('dprr'),
	keyPixMapReserved		= FOUR_CHAR_CODE('dppr'),
	keyPixMapColorTableSeed	= FOUR_CHAR_CODE('dpct'),
	keySummaryMenubar		= FOUR_CHAR_CODE('dsmb'),
	keySummaryChanges		= FOUR_CHAR_CODE('dsch'),
	keyDisplayOldConfig		= FOUR_CHAR_CODE('dold'),
	keyDisplayNewConfig		= FOUR_CHAR_CODE('dnew')
};

enum {
	dmOnlyActiveDisplays	= true,
	dmAllDisplays			= false
};


enum {
	kDependentNotifyClassShowCursor = FOUR_CHAR_CODE('shcr'),
	kDependentNotifyClassDriverOverride = FOUR_CHAR_CODE('ndrv'),
	kDependentNotifyClassDisplayMgrOverride = FOUR_CHAR_CODE('dmgr'),
	kDependentNotifyClassProfileChanged = FOUR_CHAR_CODE('prof')
};


enum {
	kNoSwitchConfirmBit		= 0,
	kDepthNotAvailableBit	= 1,
	kShowModeBit			= 3,
	kModeNotResizeBit		= 4,
	kNeverShowModeBit		= 5
};

enum {
	kBeginEndConfigureBit	= 0,
	kMovedDisplayBit		= 1,
	kSetMainDisplayBit		= 2,
	kSetDisplayModeBit		= 3,
	kAddDisplayBit			= 4,
	kRemoveDisplayBit		= 5,
	kNewDisplayBit			= 6,
	kDisposeDisplayBit		= 7,
	kEnabledDisplayBit		= 8,
	kDisabledDisplayBit		= 9,
	kMirrorDisplayBit		= 10,
	kUnMirrorDisplayBit		= 11
};


enum {
	kDMNotifyRequestConnectionProbe	= 0,
	kDMNotifyInstalled				= 1,
	kDMNotifyEvent					= 2,
	kDMNotifyRemoved				= 3,
	kDMNotifyPrep					= 4,
	kDMNotifyExtendEvent			= 5,
	kDMNotifyDependents				= 6,
	kDMNotifySuspendConfigure		= 7,
	kDMNotifyResumeConfigure		= 8,
	kDMNotifyRequestDisplayProbe	= 9,
	kExtendedNotificationProc		= (1L << 16)
};


enum {
	kFullNotify				= 0,
	kFullDependencyNotify	= 1
};

enum {
	kDummyDeviceID		= 0x00FF,
	kInvalidDisplayID	= 0x0000,
	kFirstDisplayID		= 0x0100
};

enum {
	kAllowDuplicatesBit           = 0
};

enum {
	kSuppressNumberBit	= 0,
	kSuppressNumberMask	= 1,
	kForceNumberBit		= 1,
	kForceNumberMask	= 2,
	kSuppressNameBit	= 2,
	kSuppressNameMask	= 4
};

enum {
	kDMSupressNumbersMask	= (1 << 0),
	kDMForceNumbersMask		= (1 << 1),
	kDMSupressNameMask		= (1 << 2)
};

enum {
	kNoFidelity						= 0,
	kMinimumFidelity				= 1,
	kDefaultFidelity				= 500,
	kDefaultManufacturerFidelity	= 1000
};

enum {
	kAnyPanelType	= 0,
	kAnyEngineType	= 0,
	kAnyDeviceType	= 0,
	kAnyPortType	= 0
};

enum {
	kPLIncludeOfflineDevicesBit = 0
};


enum {
	kForceConfirmBit	= 0,
	kForceConfirmMask	= (1 << kForceConfirmBit)
};


enum {
	kDisplayModeListNotPreferredBit		= 0,
	kDisplayModeListNotPreferredMask	= (1 << kDisplayModeListNotPreferredBit)
};


/* Flags for itemFlags */
enum {
	kComponentListNotPreferredBit = 0,
	kComponentListNotPreferredMask = (1 << kComponentListNotPreferredBit)
};

enum {
	kDisplayTimingInfoVersionZero	= 1,
	kDisplayTimingInfoReservedCountVersionZero = 16,
	kDisplayModeEntryVersionZero	= 0,
	kDisplayModeEntryVersionOne		= 1
};


enum {
	kMakeAndModelReservedCount = 4
};


enum {
	kDisplayGestaltDisplayCommunicationAttr = FOUR_CHAR_CODE('comm'),
	kDisplayGestaltForbidI2CMask  = (1 << 0),
	kDisplayGestaltUseI2CPowerMask = (1 << 1),
	kDisplayGestaltCalibratorAttr = FOUR_CHAR_CODE('cali'),
	kDisplayGestaltBrightnessAffectsGammaMask = (1 << 0),
	kDisplayGestaltViewAngleAffectsGammaMask = (1 << 1)
};


typedef UInt32                          DMFidelityType;
typedef void *                          DMListType;
typedef unsigned long                   DMListIndexType;
typedef VDPowerStateRec                 AVPowerStateRec;
typedef VDPowerStateRec *               AVPowerStatePtr;
struct DMDisplayTimingInfoRec {
	UInt32 timingInfoVersion;
	UInt32 timingInfoAttributes;
	SInt32 timingInfoRelativeQuality;
	SInt32 timingInfoRelativeDefault;
	
	UInt32 timingInfoReserved[16];
};
typedef struct DMDisplayTimingInfoRec	DMDisplayTimingInfoRec;
typedef DMDisplayTimingInfoRec *		DMDisplayTimingInfoPtr;

struct DMComponentListEntryRec {
	DisplayIDType		itemID;
	Component			itemComponent;
	ComponentDescription  itemDescription;
	
	ResType				itemClass;
	DMFidelityType		itemFidelity;
	ResType				itemSubClass;
	Point				itemSort;
	
	unsigned long		itemFlags;
	ResType				itemReserved;
	unsigned long		itemFuture1;
	unsigned long		itemFuture2;
	unsigned long		itemFuture3;
	unsigned long		itemFuture4;
};
typedef struct DMComponentListEntryRec	DMComponentListEntryRec;
typedef DMComponentListEntryRec *		DMComponentListEntryPtr;
struct AVLocationRec {
	unsigned long locationConstant;
};
typedef struct AVLocationRec	AVLocationRec;
typedef AVLocationRec *			AVLocationPtr;
struct DMDepthInfoRec {
	VDSwitchInfoPtr		depthSwitchInfo;
	VPBlockPtr			depthVPBlock;
	UInt32				depthFlags;
	UInt32				depthReserved1;
	UInt32				depthReserved2;
};
typedef struct DMDepthInfoRec	DMDepthInfoRec;
typedef DMDepthInfoRec *		DMDepthInfoPtr;
struct DMDepthInfoBlockRec {
	unsigned long	depthBlockCount;
	DMDepthInfoPtr	depthVPBlock;
	unsigned long	depthBlockFlags;
	unsigned long	depthBlockReserved1;
	unsigned long	depthBlockReserved2;
};
typedef struct DMDepthInfoBlockRec	DMDepthInfoBlockRec;
typedef DMDepthInfoBlockRec *		DMDepthInfoBlockPtr;
struct DMDisplayModeListEntryRec {
	UInt32              displayModeFlags;
	VDSwitchInfoPtr     displayModeSwitchInfo;
	VDResolutionInfoPtr  displayModeResolutionInfo;
	VDTimingInfoPtr     displayModeTimingInfo;
	DMDepthInfoBlockPtr  displayModeDepthBlockInfo;
	UInt32              displayModeVersion;
	StringPtr           displayModeName;
	DMDisplayTimingInfoPtr  displayModeDisplayInfo;
};
typedef struct DMDisplayModeListEntryRec	DMDisplayModeListEntryRec;
typedef DMDisplayModeListEntryRec *			DMDisplayModeListEntryPtr;

struct DependentNotifyRec {
	ResType				notifyType;
	ResType				notifyClass;
	DisplayIDType		notifyPortID;
	ComponentInstance	notifyComponent;
	
	unsigned long		notifyVersion;
	unsigned long		notifyFlags;
	unsigned long		notifyReserved;
	unsigned long		notifyFuture;
};
typedef struct DependentNotifyRec	DependentNotifyRec;
typedef DependentNotifyRec *		DependentNotifyPtr;

struct DMMakeAndModelRec {
	ResType	manufacturer;
	UInt32	model;
	UInt32	serialNumber;
	UInt32	manufactureDate;
	
	UInt32	makeReserved[4];
};
typedef struct DMMakeAndModelRec	DMMakeAndModelRec;
typedef DMMakeAndModelRec *			DMMakeAndModelPtr;

enum {
	kIncludeOnlineActiveDisplaysMask = (1 << 0),
	kIncludeOnlineDisabledDisplaysMask = (1 << 1),
	kIncludeOfflineDisplaysMask   = (1 << 2),
	kIncludeOfflineDummyDisplaysMask = (1 << 3),
	kIncludeHardwareMirroredDisplaysMask = (1 << 4)
};


enum {
	kDMModeListIncludeAllModesMask = (1 << 0),
	kDMModeListIncludeOfflineModesMask = (1 << 1),
	kDMModeListExcludeDriverModesMask = (1 << 2),
	kDMModeListExcludeDisplayModesMask = (1 << 3),
	kDMModeListExcludeCustomModesMask = (1 << 4),
	kDMModeListPreferStretchedModesMask = (1 << 5),
	kDMModeListPreferSafeModesMask = (1 << 6)
};


struct DisplayListEntryRec {
	GDHandle		displayListEntryGDevice;
	DisplayIDType	displayListEntryDisplayID;
	UInt32			displayListEntryIncludeFlags;
	UInt32			displayListEntryReserved1;
	
	UInt32	displayListEntryReserved2;
	UInt32	displayListEntryReserved3;
	UInt32	displayListEntryReserved4;
	UInt32	displayListEntryReserved5;
};
typedef struct DisplayListEntryRec      DisplayListEntryRec;
typedef DisplayListEntryRec *           DisplayListEntryPtr;
struct DMProfileListEntryRec {
	CMProfileRef	profileRef;
	Ptr				profileReserved1;
	Ptr				profileReserved2;
	Ptr				profileReserved3;
};
typedef struct DMProfileListEntryRec	DMProfileListEntryRec;
typedef DMProfileListEntryRec *			DMProfileListEntryPtr;
typedef CALLBACK_API( void , DMNotificationProcPtr )(AppleEvent * theEvent);
typedef CALLBACK_API( void , DMExtendedNotificationProcPtr )(void *userData, short theMessage, void *notifyData);
typedef CALLBACK_API( void , DMComponentListIteratorProcPtr )(void *userData, DMListIndexType itemIndex, DMComponentListEntryPtr componentInfo);
typedef CALLBACK_API( void , DMDisplayModeListIteratorProcPtr )(void *userData, DMListIndexType itemIndex, DMDisplayModeListEntryPtr displaymodeInfo);
typedef CALLBACK_API( void , DMProfileListIteratorProcPtr )(void *userData, DMListIndexType itemIndex, DMProfileListEntryPtr profileInfo);
typedef CALLBACK_API( void , DMDisplayListIteratorProcPtr )(void *userData, DMListIndexType itemIndex, DisplayListEntryPtr displaymodeInfo);
typedef STACK_UPP_TYPE(DMNotificationProcPtr)                   DMNotificationUPP;
typedef STACK_UPP_TYPE(DMExtendedNotificationProcPtr)           DMExtendedNotificationUPP;
typedef STACK_UPP_TYPE(DMComponentListIteratorProcPtr)          DMComponentListIteratorUPP;
typedef STACK_UPP_TYPE(DMDisplayModeListIteratorProcPtr)        DMDisplayModeListIteratorUPP;
typedef STACK_UPP_TYPE(DMProfileListIteratorProcPtr)            DMProfileListIteratorUPP;
typedef STACK_UPP_TYPE(DMDisplayListIteratorProcPtr)            DMDisplayListIteratorUPP;

#ifdef __cplusplus
extern "C" {
#endif

extern DMNotificationUPP NewDMNotificationUPP(DMNotificationProcPtr userRoutine);
extern DMExtendedNotificationUPP NewDMExtendedNotificationUPP(DMExtendedNotificationProcPtr userRoutine);
extern DMComponentListIteratorUPP NewDMComponentListIteratorUPP(DMComponentListIteratorProcPtr userRoutine);
extern DMDisplayModeListIteratorUPP NewDMDisplayModeListIteratorUPP(DMDisplayModeListIteratorProcPtr userRoutine);
extern DMProfileListIteratorUPP NewDMProfileListIteratorUPP(DMProfileListIteratorProcPtr userRoutine);
extern DMDisplayListIteratorUPP NewDMDisplayListIteratorUPP(DMDisplayListIteratorProcPtr userRoutine);
extern void DisposeDMNotificationUPP(DMNotificationUPP userUPP);
extern void DisposeDMExtendedNotificationUPP(DMExtendedNotificationUPP userUPP);
extern void DisposeDMComponentListIteratorUPP(DMComponentListIteratorUPP userUPP);
extern void DisposeDMDisplayModeListIteratorUPP(DMDisplayModeListIteratorUPP userUPP);
extern void DisposeDMProfileListIteratorUPP(DMProfileListIteratorUPP userUPP);
extern void DisposeDMDisplayListIteratorUPP(DMDisplayListIteratorUPP userUPP);
extern void InvokeDMNotificationUPP(AppleEvent *, DMNotificationUPP);
extern void InvokeDMExtendedNotificationUPP(void *, short, void *, DMExtendedNotificationUPP);
extern void InvokeDMComponentListIteratorUPP(void *, DMListIndexType, DMComponentListEntryPtr, DMComponentListIteratorUPP);
extern void InvokeDMDisplayModeListIteratorUPP(void *, DMListIndexType, DMDisplayModeListEntryPtr, DMDisplayModeListIteratorUPP);
extern void InvokeDMProfileListIteratorUPP(void *, DMListIndexType, DMProfileListEntryPtr, DMProfileListIteratorUPP);
extern void InvokeDMDisplayListIteratorUPP(void *, DMListIndexType, DisplayListEntryPtr, DMDisplayListIteratorUPP);


extern OSErr DMGetDisplayMode(GDHandle, VDSwitchInfoPtr);
extern OSErr DMCheckDisplayMode(GDHandle, unsigned long, unsigned long, unsigned long*, unsigned long, Boolean*);
extern OSErr DMGetIndexedDisplayModeFromList(DMListType, DMListIndexType, unsigned long, DMDisplayModeListIteratorUPP, void*);
extern OSErr DMBeginConfigureDisplays(Handle*);
extern OSErr DMEndConfigureDisplays(Handle);
extern OSErr DMSetDisplayMode(GDHandle, unsigned long, unsigned long*, unsigned long, Handle);
extern OSErr DMNewDisplayModeList(DisplayIDType, unsigned long, unsigned long, DMListIndexType*, DMListType*);
extern OSErr DMDisposeList(DMListType);
#ifdef __cplusplus
}
#endif

#pragma pack (pop)

#endif
#endif /* DisplayManagerHidden_h */
