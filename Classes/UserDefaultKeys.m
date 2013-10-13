//
//  UserDefaultKeys.m
//  PPMacho
//
//  Created by C.W. Betts on 9/21/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "UserDefaultKeys.h"

NSString * const PPListPreferencesDidChange = @"Music List Preferences Changed";
NSString * const PPSoundPreferencesDidChange = @"Sound Preferences changed";
NSString * const PPDigitalEditorPrefrencesDidChange = @"Digital Editor Preferences changed";
NSString * const PPBoxEditorPreferencesDidChange = @"Box Editor Preferences changed";
NSString * const PPClassicalEditorPreferencesDidChange = @"Classical Preferences changed";
NSString * const PPMiscPreferencesDidChange = @"Misc. Preferences Changed";
NSString * const PPColorsDidChange = @"PP Color Preferences changed";
NSString * const PPMusicDidChange = @"PP Music changed";
NSString * const PPDriverDidChange = @"PlayerPRO Driver did change";

NSString * const PPDoubleDash = @"--";
NSString * const kMusicListKVO = @"musicList";

NSString * const MADNativeUTI = @"com.quadmation.playerpro.madk";
NSString * const MADGenericUTI = @"com.quadmation.playerpro.mad";
NSString * const MADPackageUTI = @"net.sourceforge.playerpro.mad-bundle";
NSString * const PPMusicListUTI = @"net.sourceforge.playerpro.musiclist";
NSString * const PPOldMusicListUTI = @"net.sourceforge.playerpro.stcfmusiclist";
NSString * const PPPCMDUTI = @"com.quadmation.playerpro.pcmd";
NSString * const PPGenericTrackerUTI = @"net.sourceforge.playerpro.tracker";
NSString * const PPGenericInstrumentUTI = @"net.sourceforge.playerpro.instrumentfile";
NSString * const PPInstrumentListUTI = @"com.quadmation.playerpro.list";

NSString * const PPRememberMusicList = @"Remember Music List";
NSString * const PPLoadMusicAtListLoad = @"Load music when loading list";
NSString * const PPAfterPlayingMusic = @"After playing music";
NSString * const PPGotoStartupAfterPlaying = @"Go to startup pos. after playing";
NSString * const PPSaveModList = @"Ask to save modified Mod list";
NSString * const PPLoadMusicAtMusicLoad = @"Play music on Music load";
NSString * const PPLoopMusicWhenDone = @"Loop music";

NSString * const PPSoundOutBits = @"Sound output bits";
NSString * const PPSoundOutRate = @"Sound output rate";
NSString * const PPSoundDriver = @"Sound Driver";
NSString * const PPStereoDelayToggle = @"Stereo delay?";
NSString * const PPReverbToggle = @"Reverb?";
NSString * const PPSurroundToggle = @"Surround?";
NSString * const PPOversamplingToggle = @"Oversampling?";
NSString * const PPStereoDelayAmount = @"Stereo Delay amount";
NSString * const PPReverbAmount = @"Reverb Amount";
NSString * const PPReverbStrength = @"Reverb Strength";
NSString * const PPOversamplingAmount = @"Oversampling Amount";

NSString * const PPDEShowInstrument = @"Digital Editor Show Instrument";
NSString * const PPDEShowNote = @"Digital Editor Show Note";
NSString * const PPDEShowEffect = @"Digital Editor Show Effect";
NSString * const PPDEShowArgument = @"Digital Editor Show Argument";
NSString * const PPDEShowVolume = @"Digital Editor Show Volume";
NSString * const PPDEShowMarkers = @"Digital Editor Show Markers";
NSString * const PPDEMarkerOffsetPref = @"Digital Editor Marker Offset";
NSString * const PPDEMarkerLoopPref = @"Digital Editor Marker Loop";
NSString * const PPDEMarkerColorPref = @"Digital Editor Marker Color";
NSString * const PPDEMouseClickControlPref = @"Digital Editor Mouse Click+Control";
NSString * const PPDEMouseClickShiftPref = @"Digital Editor Mouse Click+Shift";
NSString * const PPDEMouseClickCommandPref = @"Digital Editor Mouse Click+Command";
NSString * const PPDEMouseClickOptionPref = @"Digital EditorMouse Click+Option";
NSString * const PPDELineHeightNormal = @"Digital Editor Line Height Normal?";
NSString * const PPDEMusicTraceOn = @"Digital Editor Trace On?";
NSString * const PPDEPatternWrappingPartition = @"Digital Editor Pattern Wrapping Partition?";
NSString * const PPDEDragAsPcmd = @"Digital Editor Drag as Pcmd?";

#define PPCOLOR(num) NSString * const PPCColor##num = @"PPColor " @#num

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

NSString * const PPBEMarkersEnabled = @"Box Editor Markers On?";
NSString * const PPBEMarkersOffset = @"Box Editor Markers Offset";
NSString * const PPBEMarkersLoop = @"Box Editor Markers Loop";
NSString * const PPBEOctaveMarkers = @"Box Editor Octave Markers?";
NSString * const PPBENotesProjection = @"Box Editor Notes Projection?";

NSString * const PPMAddExtension = @"Add Extension";
NSString * const PPMMadCompression = @"Compress MAD?";
NSString * const PPMNoLoadMixerFromFiles = @"No load Mixer?";
NSString * const PPMOscilloscopeDrawLines = @"Oscilloscope Draw Lines";

NSString * const PPCEShowNotesLen = @"Classical Editor Show Notes Length";
NSString * const PPCEShowMarkers = @"Classical Editor Show Markers?";
NSString * const PPCEMarkerOffset = @"Classical Editor Marker Offset";
NSString * const PPCEMarkerLoop = @"Classical Editor Marker Loop";
NSString * const PPCETempoNum = @"Classical Editor Temo Num.";
NSString * const PPCETempoUnit = @"Classical Editor Tempo Unit";
NSString * const PPCETrackHeight = @"Classical Editor Track Height";

NSString * const PPMMusicList = @"PlayerPRO Music List";
