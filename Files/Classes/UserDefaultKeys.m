//
//  UserDefaultKeys.m
//  PPMacho
//
//  Created by C.W. Betts on 9/21/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#include "UserDefaultKeys.h"
#import "NSColor+PPPreferences.h"

NSString *const PPPreferencesSet = @"PlayerPRO 5 CoreFoundation Preferences set?";
NSString *const PPAutoCreator = @"Auto Creator";
NSString *const PPStartNumber = @"No. Start";
NSString *const PPPianoKeys = @"Piano Keys";
NSString *const PPLoopType = @"Loop Type";
NSString *const PPVolumeLevel = @"Volume Level";

NSString *const MADNativeUTI = @"com.quadmation.playerpro.madk";
NSString *const MADGenericUTI = @"com.quadmation.playerpro.mad";
NSString *const MADPackageUTI = @"net.sourceforge.playerpro.mad-bundle";
NSString *const PPMusicListUTI = @"net.sourceforge.playerpro.musiclist";
NSString *const PPOldMusicListUTI = @"net.sourceforge.playerpro.stcfmusiclist";
NSString *const PPPCMDUTI = @"com.quadmation.playerpro.pcmd";
NSString *const PPGenericTrackerUTI = @"net.sourceforge.playerpro.tracker";
NSString *const PPGenericInstrumentUTI = @"net.sourceforge.playerpro.instrumentfile";
NSString *const PPInstrumentListUTI = @"com.quadmation.playerpro.list";

NSString *const PPRememberMusicList = @"Remember Music List";
NSString *const PPLoadMusicAtListLoad = @"Load music when loading list";
NSString *const PPAfterPlayingMusic = @"After playing music";
NSString *const PPGotoStartupAfterPlaying = @"Go to startup pos. after playing";
NSString *const PPSaveModList = @"Ask to save modified Mod list";
NSString *const PPLoadMusicAtMusicLoad = @"Play music on Music load";
NSString *const PPLoopMusicWhenDone = @"Loop music";

NSString *const PPSoundOutBits = @"Sound output bits";
NSString *const PPSoundOutRate = @"Sound output rate";
NSString *const PPSoundDriver = @"Sound Driver";
NSString *const PPStereoDelayToggle = @"Stereo delay?";
NSString *const PPReverbToggle = @"Reverb?";
NSString *const PPSurroundToggle = @"Surround?";
NSString *const PPOversamplingToggle = @"Oversampling?";
NSString *const PPStereoDelayAmount = @"Stereo Delay amount";
NSString *const PPReverbAmount = @"Reverb Amount";
NSString *const PPReverbStrength = @"Reverb Strength";
NSString *const PPOversamplingAmount = @"Oversampling Amount";

NSString *const PPDEShowInstrument = @"Digital Editor Show Instrument";
NSString *const PPDEShowNote = @"Digital Editor Show Note";
NSString *const PPDEShowEffect = @"Digital Editor Show Effect";
NSString *const PPDEShowArgument = @"Digital Editor Show Argument";
NSString *const PPDEShowVolume = @"Digital Editor Show Volume";
NSString *const PPDEShowMarkers = @"Digital Editor Show Markers";
NSString *const PPDEMarkerOffsetPref = @"Digital Editor Marker Offset";
NSString *const PPDEMarkerLoopPref = @"Digital Editor Marker Loop";
NSString *const PPDEMarkerColorPref = @"Digital Editor Marker Color";
NSString *const PPDEMouseClickPref = @"Digital Editor Mouse Click";
NSString *const PPDELineHeightNormal = @"Digital Editor Line Height Normal?";
NSString *const PPDEMusicTraceOn = @"Digital Editor Trace On?";
NSString *const PPDEPatternWrappingPartition = @"Digital Editor Pattern Wrapping Partition?";
NSString *const PPDEDragAsPcmd = @"Digital Editor Drag as Pcmd?";

#define PPCOLOR(num) NSString *const PPCColor##num = @"PPColor " @#num
PPCOLORPOPULATE();
#undef PPCOLOR

NSString *const PPBEMarkersEnabled = @"Box Editor Markers On?";
NSString *const PPBEMarkersOffset = @"Box Editor Markers Offset";
NSString *const PPBEMarkersLoop = @"Box Editor Markers Loop";
NSString *const PPBEOctaveMarkers = @"Box Editor Octave Markers?";
NSString *const PPBENotesProjection = @"Box Editor Notes Projection?";

NSString *const PPMAddExtension = @"Add Extension";
NSString *const PPMMadCompression = @"Compress MAD?";
NSString *const PPMNoLoadMixerFromFiles = @"No load Mixer?";
NSString *const PPMOscilloscopeDrawLines = @"Oscilloscope Draw Lines";

NSString *const PPCEShowNotesLen = @"Classical Editor Show Notes Length";
NSString *const PPCEShowMarkers = @"Classical Editor Show Markers?";
NSString *const PPCEMarkerOffset = @"Classical Editor Marker Offset";
NSString *const PPCEMarkerLoop = @"Classical Editor Marker Loop";
NSString *const PPCETempoNum = @"Classical Editor Temo Num.";
NSString *const PPCETempoUnit = @"Classical Editor Tempo Unit";
NSString *const PPCETrackHeight = @"Classical Editor Track Height";

NSString *const PPWindowName1 = @"PPWindowName1";
NSString *const PPWindowName2 = @"PPWindowName2";
NSString *const PPWindowName3 = @"PPWindowName3";
