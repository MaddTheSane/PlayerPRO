//
//  UserDefaultKeys.m
//  PPMacho
//
//  Created by C.W. Betts on 9/21/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#include "UserDefaultKeys.h"
#import "NSColor+PPPreferences.h"

CFStringRef const PPPreferencesSet = CFSTR("PlayerPRO 5 CoreFoundation Preferences set?");

CFStringRef const MADNativeUTI = CFSTR("com.quadmation.playerpro.madk");
CFStringRef const MADGenericUTI = CFSTR("com.quadmation.playerpro.mad");
CFStringRef const MADPackageUTI = CFSTR("net.sourceforge.playerpro.mad-bundle");
CFStringRef const PPMusicListUTI = CFSTR("net.sourceforge.playerpro.musiclist");
CFStringRef const PPOldMusicListUTI = CFSTR("net.sourceforge.playerpro.stcfmusiclist");
CFStringRef const PPPCMDUTI = CFSTR("com.quadmation.playerpro.pcmd");
CFStringRef const PPGenericTrackerUTI = CFSTR("net.sourceforge.playerpro.tracker");
CFStringRef const PPGenericInstrumentUTI = CFSTR("net.sourceforge.playerpro.instrumentfile");
CFStringRef const PPInstrumentListUTI = CFSTR("com.quadmation.playerpro.list");

CFStringRef const PPRememberMusicList = CFSTR("Remember Music List");
CFStringRef const PPLoadMusicAtListLoad = CFSTR("Load music when loading list");
CFStringRef const PPAfterPlayingMusic = CFSTR("After playing music");
CFStringRef const PPGotoStartupAfterPlaying = CFSTR("Go to startup pos. after playing");
CFStringRef const PPSaveModList = CFSTR("Ask to save modified Mod list");
CFStringRef const PPLoadMusicAtMusicLoad = CFSTR("Play music on Music load");
CFStringRef const PPLoopMusicWhenDone = CFSTR("Loop music");

CFStringRef const PPSoundOutBits = CFSTR("Sound output bits");
CFStringRef const PPSoundOutRate = CFSTR("Sound output rate");
CFStringRef const PPSoundDriver = CFSTR("Sound Driver");
CFStringRef const PPStereoDelayToggle = CFSTR("Stereo delay?");
CFStringRef const PPReverbToggle = CFSTR("Reverb?");
CFStringRef const PPSurroundToggle = CFSTR("Surround?");
CFStringRef const PPOversamplingToggle = CFSTR("Oversampling?");
CFStringRef const PPStereoDelayAmount = CFSTR("Stereo Delay amount");
CFStringRef const PPReverbAmount = CFSTR("Reverb Amount");
CFStringRef const PPReverbStrength = CFSTR("Reverb Strength");
CFStringRef const PPOversamplingAmount = CFSTR("Oversampling Amount");

CFStringRef const PPDEShowInstrument = CFSTR("Digital Editor Show Instrument");
CFStringRef const PPDEShowNote = CFSTR("Digital Editor Show Note");
CFStringRef const PPDEShowEffect = CFSTR("Digital Editor Show Effect");
CFStringRef const PPDEShowArgument = CFSTR("Digital Editor Show Argument");
CFStringRef const PPDEShowVolume = CFSTR("Digital Editor Show Volume");
CFStringRef const PPDEShowMarkers = CFSTR("Digital Editor Show Markers");
CFStringRef const PPDEMarkerOffsetPref = CFSTR("Digital Editor Marker Offset");
CFStringRef const PPDEMarkerLoopPref = CFSTR("Digital Editor Marker Loop");
CFStringRef const PPDEMarkerColorPref = CFSTR("Digital Editor Marker Color");
CFStringRef const PPDEMouseClickControlPref = CFSTR("Digital Editor Mouse Click+Control");
CFStringRef const PPDEMouseClickShiftPref = CFSTR("Digital Editor Mouse Click+Shift");
CFStringRef const PPDEMouseClickCommandPref = CFSTR("Digital Editor Mouse Click+Command");
CFStringRef const PPDEMouseClickOptionPref = CFSTR("Digital EditorMouse Click+Option");
CFStringRef const PPDELineHeightNormal = CFSTR("Digital Editor Line Height Normal?");
CFStringRef const PPDEMusicTraceOn = CFSTR("Digital Editor Trace On?");
CFStringRef const PPDEPatternWrappingPartition = CFSTR("Digital Editor Pattern Wrapping Partition?");
CFStringRef const PPDEDragAsPcmd = CFSTR("Digital Editor Drag as Pcmd?");

#define PPCOLOR(num) CFStringRef const PPCColor##num = CFSTR("PPColor " #num)
PPCOLORPOPULATE();
#undef PPCOLOR

CFStringRef const PPBEMarkersEnabled = CFSTR("Box Editor Markers On?");
CFStringRef const PPBEMarkersOffset = CFSTR("Box Editor Markers Offset");
CFStringRef const PPBEMarkersLoop = CFSTR("Box Editor Markers Loop");
CFStringRef const PPBEOctaveMarkers = CFSTR("Box Editor Octave Markers?");
CFStringRef const PPBENotesProjection = CFSTR("Box Editor Notes Projection?");

CFStringRef const PPMAddExtension = CFSTR("Add Extension");
CFStringRef const PPMMadCompression = CFSTR("Compress MAD?");
CFStringRef const PPMNoLoadMixerFromFiles = CFSTR("No load Mixer?");
CFStringRef const PPMOscilloscopeDrawLines = CFSTR("Oscilloscope Draw Lines");

CFStringRef const PPCEShowNotesLen = CFSTR("Classical Editor Show Notes Length");
CFStringRef const PPCEShowMarkers = CFSTR("Classical Editor Show Markers?");
CFStringRef const PPCEMarkerOffset = CFSTR("Classical Editor Marker Offset");
CFStringRef const PPCEMarkerLoop = CFSTR("Classical Editor Marker Loop");
CFStringRef const PPCETempoNum = CFSTR("Classical Editor Temo Num.");
CFStringRef const PPCETempoUnit = CFSTR("Classical Editor Tempo Unit");
CFStringRef const PPCETrackHeight = CFSTR("Classical Editor Track Height");
