//
//  UserDefaultKeys.m
//  PPMacho
//
//  Created by C.W. Betts on 9/21/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "UserDefaultKeys.h"
#if !TARGET_OS_IPHONE
#import "NSColor+PPPreferences.h"
#endif

NSString * const PPListPreferencesDidChange = @"Music List Preferences Changed";
NSString * const PPDigitalEditorPrefrencesDidChange = @"Digital Editor Preferences changed";
NSString * const PPBoxEditorPreferencesDidChange = @"Box Editor Preferences changed";
NSString * const PPClassicalEditorPreferencesDidChange = @"Classical Preferences changed";
NSString * const PPMiscPreferencesDidChange = @"Misc. Preferences Changed";
NSString * const PPColorsDidChange = @"PPColor Preferences changed";
NSString * const PPColorChangedColor = @"PPColor Preferences Changed Color";
NSString * const PPColorChangedValue = @"PPColor Preferences Changed Color Value";

NSString * const PPRememberMusicList = @"Remember Music List";
NSString * const PPLoadMusicAtListLoad = @"Load music when loading list";
NSString * const PPAfterPlayingMusic = @"After playing music";
NSString * const PPGotoStartupAfterPlaying = @"Go to startup pos. after playing";
NSString * const PPSaveModList = @"Ask to save modified Mod list";
NSString * const PPLoadMusicAtMusicLoad = @"Play music on Music load";
NSString * const PPLoopMusicWhenDone = @"Loop music";

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

#if !TARGET_OS_IPHONE
//#define PPCOLOR(num) NSString * const PPCColor##num = @"PPColor " @#num
//PPCOLORPOPULATE();
//#undef PPCOLOR
#endif

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
