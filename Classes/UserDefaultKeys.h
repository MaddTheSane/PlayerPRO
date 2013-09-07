//
//  UserDefaultKeys.h
//  PPMacho
//
//  Created by C.W. Betts on 9/21/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//
#import <Foundation/NSString.h>

//Notifications
extern NSString * const PPListPreferencesDidChange;
extern NSString * const PPSoundPreferencesDidChange;
extern NSString * const PPDigitalEditorPrefrencesDidChange;
extern NSString * const PPBoxEditorPreferencesDidChange;
extern NSString * const PPClassicalEditorPreferencesDidChange;
extern NSString * const PPMiscPreferencesDidChange;
extern NSString * const PPColorsDidChange;
extern NSString * const PPMusicDidChange;
extern NSString * const PPDriverDidChange;

//UTIs
extern NSString * const MADNativeUTI;
extern NSString * const MADGenericUTI;
extern NSString * const MADPackageUTI;
extern NSString * const PPMusicListUTI;
extern NSString * const PPOldMusicListUTI;
extern NSString * const PPPCMDUTI;
extern NSString * const PPGenericTrackerUTI;
extern NSString * const PPGenericInstrumentUTI;
extern NSString * const PPInstrumentListUTI;

//Music list preference keys
extern NSString * const PPRememberMusicList;
extern NSString * const PPLoadMusicAtListLoad;
extern NSString * const PPAfterPlayingMusic;
extern NSString * const PPGotoStartupAfterPlaying;
extern NSString * const PPSaveModList;
extern NSString * const PPLoadMusicAtMusicLoad;
extern NSString * const PPLoopMusicWhenDone;

//Sound driver Preference keys
extern NSString * const PPSoundOutBits;
extern NSString * const PPSoundOutRate;
extern NSString * const PPSoundDriver;
extern NSString * const PPStereoDelayToggle;
extern NSString * const PPReverbToggle;
extern NSString * const PPSurroundToggle;
extern NSString * const PPOversamplingToggle;
extern NSString * const PPStereoDelayAmount;
extern NSString * const PPReverbAmount;
extern NSString * const PPReverbStrength;
extern NSString * const PPOversamplingAmount;

//Digital Editor preference keys
extern NSString * const PPDEShowInstrument;
extern NSString * const PPDEShowNote;
extern NSString * const PPDEShowEffect;
extern NSString * const PPDEShowArgument;
extern NSString * const PPDEShowVolume;
extern NSString * const PPDEShowMarkers;
extern NSString * const PPDEMarkerOffsetPref;
extern NSString * const PPDEMarkerLoopPref;
extern NSString * const PPDEMarkerColorPref;
extern NSString * const PPDEMouseClickControlPref;
extern NSString * const PPDEMouseClickShiftPref;
extern NSString * const PPDEMouseClickCommandPref;
extern NSString * const PPDEMouseClickOptionPref;
extern NSString * const PPDELineHeightNormal;
extern NSString * const PPDEMusicTraceOn;
extern NSString * const PPDEPatternWrappingPartition;
extern NSString * const PPDEDragAsPcmd;

//Color preference keys
extern NSString * const PPCColor1;
extern NSString * const PPCColor2;
extern NSString * const PPCColor3;
extern NSString * const PPCColor4;
extern NSString * const PPCColor5;
extern NSString * const PPCColor6;
extern NSString * const PPCColor7;
extern NSString * const PPCColor8;

//Box editor preference keys
extern NSString * const PPBEMarkersEnabled;
extern NSString * const PPBEMarkersOffset;
extern NSString * const PPBEMarkersLoop;
extern NSString * const PPBEOctaveMarkers;
extern NSString * const PPBENotesProjection;

//miscelaneous preference keys
extern NSString * const PPMAddExtension;
extern NSString * const PPMMadCompression;
extern NSString * const PPMNoLoadMixerFromFiles;
extern NSString * const PPMOscilloscopeDrawLines;

//Classic editor preference keys
extern NSString * const PPCEShowNotesLen;
extern NSString * const PPCEShowMarkers;
extern NSString * const PPCEMarkerOffset;
extern NSString * const PPCEMarkerLoop;
extern NSString * const PPCETempoNum;
extern NSString * const PPCETempoUnit;
extern NSString * const PPCETrackHeight;

//Misc./other
extern NSString * const PPMMusicList;
extern NSString * const PPDoubleDash;
extern NSString * const kMusicListKVO;
