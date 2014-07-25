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
extern NSString * const PPMLDCUTI;

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

#if !TARGET_OS_IPHONE
//Color preference keys
extern NSString * const PPCColor1;
extern NSString * const PPCColor2;
extern NSString * const PPCColor3;
extern NSString * const PPCColor4;
extern NSString * const PPCColor5;
extern NSString * const PPCColor6;
extern NSString * const PPCColor7;
extern NSString * const PPCColor8;
extern NSString * const PPCColor9;
extern NSString * const PPCColor10;
extern NSString * const PPCColor11;
extern NSString * const PPCColor12;
extern NSString * const PPCColor13;
extern NSString * const PPCColor14;
extern NSString * const PPCColor15;
extern NSString * const PPCColor16;
extern NSString * const PPCColor17;
extern NSString * const PPCColor18;
extern NSString * const PPCColor19;
extern NSString * const PPCColor20;
extern NSString * const PPCColor21;
extern NSString * const PPCColor22;
extern NSString * const PPCColor23;
extern NSString * const PPCColor24;
extern NSString * const PPCColor25;
extern NSString * const PPCColor26;
extern NSString * const PPCColor27;
extern NSString * const PPCColor28;
extern NSString * const PPCColor29;
extern NSString * const PPCColor30;
extern NSString * const PPCColor31;
extern NSString * const PPCColor32;
extern NSString * const PPCColor33;
extern NSString * const PPCColor34;
extern NSString * const PPCColor35;
extern NSString * const PPCColor36;
extern NSString * const PPCColor37;
extern NSString * const PPCColor38;
extern NSString * const PPCColor39;
extern NSString * const PPCColor40;
extern NSString * const PPCColor41;
extern NSString * const PPCColor42;
extern NSString * const PPCColor43;
extern NSString * const PPCColor44;
extern NSString * const PPCColor45;
extern NSString * const PPCColor46;
extern NSString * const PPCColor47;
extern NSString * const PPCColor48;
extern NSString * const PPCColor49;
extern NSString * const PPCColor50;
extern NSString * const PPCColor51;
extern NSString * const PPCColor52;
extern NSString * const PPCColor53;
extern NSString * const PPCColor54;
extern NSString * const PPCColor55;
extern NSString * const PPCColor56;
extern NSString * const PPCColor57;
extern NSString * const PPCColor58;
extern NSString * const PPCColor59;
extern NSString * const PPCColor60;
extern NSString * const PPCColor61;
extern NSString * const PPCColor62;
extern NSString * const PPCColor63;
extern NSString * const PPCColor64;
extern NSString * const PPCColor65;
extern NSString * const PPCColor66;
extern NSString * const PPCColor67;
extern NSString * const PPCColor68;
extern NSString * const PPCColor69;
extern NSString * const PPCColor70;
extern NSString * const PPCColor71;
extern NSString * const PPCColor72;
extern NSString * const PPCColor73;
extern NSString * const PPCColor74;
extern NSString * const PPCColor75;
extern NSString * const PPCColor76;
extern NSString * const PPCColor77;
extern NSString * const PPCColor78;
extern NSString * const PPCColor79;
extern NSString * const PPCColor80;
extern NSString * const PPCColor81;
extern NSString * const PPCColor82;
extern NSString * const PPCColor83;
extern NSString * const PPCColor84;
extern NSString * const PPCColor85;
extern NSString * const PPCColor86;
extern NSString * const PPCColor87;
extern NSString * const PPCColor88;
extern NSString * const PPCColor89;
extern NSString * const PPCColor90;
extern NSString * const PPCColor91;
extern NSString * const PPCColor92;
extern NSString * const PPCColor93;
extern NSString * const PPCColor94;
extern NSString * const PPCColor95;
extern NSString * const PPCColor96;
#endif

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
extern NSString * const PPDoubleDash;
extern NSString * const kMusicListKVO;
