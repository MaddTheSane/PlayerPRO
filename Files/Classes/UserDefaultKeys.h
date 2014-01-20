//
//  UserDefaultKeys.h
//  PPMacho
//
//  Created by C.W. Betts on 9/21/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#ifndef __PPUSERDEFAULTKEYS__
#define __PPUSERDEFAULTKEYS__

#include <CoreFoundation/CFBase.h>

extern CFStringRef const PPPreferencesSet;
extern CFStringRef const PPPreferencesVersion;
extern CFStringRef const PPLoop;

//UTIs
extern CFStringRef const MADNativeUTI;
extern CFStringRef const MADGenericUTI;
extern CFStringRef const MADPackageUTI;
extern CFStringRef const PPMusicListUTI;
extern CFStringRef const PPOldMusicListUTI;
extern CFStringRef const PPPCMDUTI;
extern CFStringRef const PPGenericTrackerUTI;
extern CFStringRef const PPGenericInstrumentUTI;
extern CFStringRef const PPInstrumentListUTI;

//Music list preference keys
extern CFStringRef const PPRememberMusicList;
extern CFStringRef const PPLoadMusicAtListLoad;
extern CFStringRef const PPAfterPlayingMusic;
extern CFStringRef const PPGotoStartupAfterPlaying;
extern CFStringRef const PPSaveModList;
extern CFStringRef const PPLoadMusicAtMusicLoad;
extern CFStringRef const PPLoopMusicWhenDone;

//Sound driver Preference keys
extern CFStringRef const PPSoundOutBits;
extern CFStringRef const PPSoundOutRate;
extern CFStringRef const PPSoundDriver;
extern CFStringRef const PPStereoDelayToggle;
extern CFStringRef const PPReverbToggle;
extern CFStringRef const PPSurroundToggle;
extern CFStringRef const PPOversamplingToggle;
extern CFStringRef const PPStereoDelayAmount;
extern CFStringRef const PPReverbAmount;
extern CFStringRef const PPReverbStrength;
extern CFStringRef const PPOversamplingAmount;

//Digital Editor preference keys
extern CFStringRef const PPDEShowInstrument;
extern CFStringRef const PPDEShowNote;
extern CFStringRef const PPDEShowEffect;
extern CFStringRef const PPDEShowArgument;
extern CFStringRef const PPDEShowVolume;
extern CFStringRef const PPDEShowMarkers;
extern CFStringRef const PPDEMarkerOffsetPref;
extern CFStringRef const PPDEMarkerLoopPref;
extern CFStringRef const PPDEMarkerColorPref;
extern CFStringRef const PPDEMouseClickControlPref;
extern CFStringRef const PPDEMouseClickShiftPref;
extern CFStringRef const PPDEMouseClickCommandPref;
extern CFStringRef const PPDEMouseClickOptionPref;
extern CFStringRef const PPDELineHeightNormal;
extern CFStringRef const PPDEMusicTraceOn;
extern CFStringRef const PPDEPatternWrappingPartition;
extern CFStringRef const PPDEDragAsPcmd;

//Color preference keys
extern CFStringRef const PPCColor1;
extern CFStringRef const PPCColor2;
extern CFStringRef const PPCColor3;
extern CFStringRef const PPCColor4;
extern CFStringRef const PPCColor5;
extern CFStringRef const PPCColor6;
extern CFStringRef const PPCColor7;
extern CFStringRef const PPCColor8;
extern CFStringRef const PPCColor9;
extern CFStringRef const PPCColor10;
extern CFStringRef const PPCColor11;
extern CFStringRef const PPCColor12;
extern CFStringRef const PPCColor13;
extern CFStringRef const PPCColor14;
extern CFStringRef const PPCColor15;
extern CFStringRef const PPCColor16;
extern CFStringRef const PPCColor17;
extern CFStringRef const PPCColor18;
extern CFStringRef const PPCColor19;
extern CFStringRef const PPCColor20;
extern CFStringRef const PPCColor21;
extern CFStringRef const PPCColor22;
extern CFStringRef const PPCColor23;
extern CFStringRef const PPCColor24;
extern CFStringRef const PPCColor25;
extern CFStringRef const PPCColor26;
extern CFStringRef const PPCColor27;
extern CFStringRef const PPCColor28;
extern CFStringRef const PPCColor29;
extern CFStringRef const PPCColor30;
extern CFStringRef const PPCColor31;
extern CFStringRef const PPCColor32;
extern CFStringRef const PPCColor33;
extern CFStringRef const PPCColor34;
extern CFStringRef const PPCColor35;
extern CFStringRef const PPCColor36;
extern CFStringRef const PPCColor37;
extern CFStringRef const PPCColor38;
extern CFStringRef const PPCColor39;
extern CFStringRef const PPCColor40;
extern CFStringRef const PPCColor41;
extern CFStringRef const PPCColor42;
extern CFStringRef const PPCColor43;
extern CFStringRef const PPCColor44;
extern CFStringRef const PPCColor45;
extern CFStringRef const PPCColor46;
extern CFStringRef const PPCColor47;
extern CFStringRef const PPCColor48;
extern CFStringRef const PPCColor49;
extern CFStringRef const PPCColor50;
extern CFStringRef const PPCColor51;
extern CFStringRef const PPCColor52;
extern CFStringRef const PPCColor53;
extern CFStringRef const PPCColor54;
extern CFStringRef const PPCColor55;
extern CFStringRef const PPCColor56;
extern CFStringRef const PPCColor57;
extern CFStringRef const PPCColor58;
extern CFStringRef const PPCColor59;
extern CFStringRef const PPCColor60;
extern CFStringRef const PPCColor61;
extern CFStringRef const PPCColor62;
extern CFStringRef const PPCColor63;
extern CFStringRef const PPCColor64;
extern CFStringRef const PPCColor65;
extern CFStringRef const PPCColor66;
extern CFStringRef const PPCColor67;
extern CFStringRef const PPCColor68;
extern CFStringRef const PPCColor69;
extern CFStringRef const PPCColor70;
extern CFStringRef const PPCColor71;
extern CFStringRef const PPCColor72;
extern CFStringRef const PPCColor73;
extern CFStringRef const PPCColor74;
extern CFStringRef const PPCColor75;
extern CFStringRef const PPCColor76;
extern CFStringRef const PPCColor77;
extern CFStringRef const PPCColor78;
extern CFStringRef const PPCColor79;
extern CFStringRef const PPCColor80;
extern CFStringRef const PPCColor81;
extern CFStringRef const PPCColor82;
extern CFStringRef const PPCColor83;
extern CFStringRef const PPCColor84;
extern CFStringRef const PPCColor85;
extern CFStringRef const PPCColor86;
extern CFStringRef const PPCColor87;
extern CFStringRef const PPCColor88;
extern CFStringRef const PPCColor89;
extern CFStringRef const PPCColor90;
extern CFStringRef const PPCColor91;
extern CFStringRef const PPCColor92;
extern CFStringRef const PPCColor93;
extern CFStringRef const PPCColor94;
extern CFStringRef const PPCColor95;
extern CFStringRef const PPCColor96;

//Box editor preference keys
extern CFStringRef const PPBEMarkersEnabled;
extern CFStringRef const PPBEMarkersOffset;
extern CFStringRef const PPBEMarkersLoop;
extern CFStringRef const PPBEOctaveMarkers;
extern CFStringRef const PPBENotesProjection;

//miscelaneous preference keys
extern CFStringRef const PPMAddExtension;
extern CFStringRef const PPMMadCompression;
extern CFStringRef const PPMNoLoadMixerFromFiles;
extern CFStringRef const PPMOscilloscopeDrawLines;

//Classic editor preference keys
extern CFStringRef const PPCEShowNotesLen;
extern CFStringRef const PPCEShowMarkers;
extern CFStringRef const PPCEMarkerOffset;
extern CFStringRef const PPCEMarkerLoop;
extern CFStringRef const PPCETempoNum;
extern CFStringRef const PPCETempoUnit;
extern CFStringRef const PPCETrackHeight;

#endif
