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

NSString * const PPCColor1 = @"PPColor 1";
NSString * const PPCColor2 = @"PPColor 2";
NSString * const PPCColor3 = @"PPColor 3";
NSString * const PPCColor4 = @"PPColor 4";
NSString * const PPCColor5 = @"PPColor 5";
NSString * const PPCColor6 = @"PPColor 6";
NSString * const PPCColor7 = @"PPColor 7";
NSString * const PPCColor8 = @"PPColor 8";

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
