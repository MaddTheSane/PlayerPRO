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
#define PPCOLOR(num) extern NSString * const PPCColor##num
PPCOLOR(1);
PPCOLOR(2);
PPCOLOR(3);
PPCOLOR(4);
PPCOLOR(5);
PPCOLOR(6);
PPCOLOR(7);
PPCOLOR(8);
PPCOLOR(9);
PPCOLOR(10);
PPCOLOR(11);
PPCOLOR(12);
PPCOLOR(13);
PPCOLOR(14);
PPCOLOR(15);
PPCOLOR(16);
PPCOLOR(17);
PPCOLOR(18);
PPCOLOR(19);
PPCOLOR(20);
PPCOLOR(21);
PPCOLOR(22);
PPCOLOR(23);
PPCOLOR(24);
PPCOLOR(25);
PPCOLOR(26);
PPCOLOR(27);
PPCOLOR(28);
PPCOLOR(29);
PPCOLOR(30);
PPCOLOR(31);
PPCOLOR(32);
PPCOLOR(33);
PPCOLOR(34);
PPCOLOR(35);
PPCOLOR(36);
PPCOLOR(37);
PPCOLOR(38);
PPCOLOR(39);
PPCOLOR(40);
PPCOLOR(41);
PPCOLOR(42);
PPCOLOR(43);
PPCOLOR(44);
PPCOLOR(45);
PPCOLOR(46);
PPCOLOR(47);
PPCOLOR(48);
PPCOLOR(49);
PPCOLOR(50);
PPCOLOR(51);
PPCOLOR(52);
PPCOLOR(53);
PPCOLOR(54);
PPCOLOR(55);
PPCOLOR(56);
PPCOLOR(57);
PPCOLOR(58);
PPCOLOR(59);
PPCOLOR(60);
PPCOLOR(61);
PPCOLOR(62);
PPCOLOR(63);
PPCOLOR(64);
PPCOLOR(65);
PPCOLOR(66);
PPCOLOR(67);
PPCOLOR(68);
PPCOLOR(69);
PPCOLOR(70);
PPCOLOR(71);
PPCOLOR(72);
PPCOLOR(73);
PPCOLOR(74);
PPCOLOR(75);
PPCOLOR(76);
PPCOLOR(77);
PPCOLOR(78);
PPCOLOR(79);
PPCOLOR(80);
PPCOLOR(81);
PPCOLOR(82);
PPCOLOR(83);
PPCOLOR(84);
PPCOLOR(85);
PPCOLOR(86);
PPCOLOR(87);
PPCOLOR(88);
PPCOLOR(89);
PPCOLOR(90);
PPCOLOR(91);
PPCOLOR(92);
PPCOLOR(93);
PPCOLOR(94);
PPCOLOR(95);
PPCOLOR(96);
#undef PPCOLOR

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
