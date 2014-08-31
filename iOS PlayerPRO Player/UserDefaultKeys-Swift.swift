//
//  UserDefaultKeys-Swift.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/31/14.
//
//

import Foundation

internal let PPListPreferencesDidChange = "Music List Preferences Changed";
internal let PPSoundPreferencesDidChange = "Sound Preferences changed";
internal let PPDigitalEditorPrefrencesDidChange = "Digital Editor Preferences changed";
internal let PPBoxEditorPreferencesDidChange = "Box Editor Preferences changed";
internal let PPClassicalEditorPreferencesDidChange = "Classical Preferences changed";
internal let PPMiscPreferencesDidChange = "Misc. Preferences Changed";
internal let PPColorsDidChange = "PP Color Preferences changed";
internal let PPMusicDidChange = "PP Music changed";
internal let PPDriverDidChange = "PlayerPRO Driver did change";

internal let PPDoubleDash = "--";
internal let kMusicListKVO = "musicList";

internal let MADNativeUTI = "com.quadmation.playerpro.madk";
internal let MADGenericUTI = "com.quadmation.playerpro.mad";
internal let PPMusicListUTI = "net.sourceforge.playerpro.musiclist";
internal let PPOldMusicListUTI = "com.quadmation.playerpro.stcfmusiclist";
internal let PPPCMDUTI = "com.quadmation.playerpro.pcmd";
internal let PPGenericTrackerUTI = "net.sourceforge.playerpro.tracker";
internal let PPGenericInstrumentUTI = "net.sourceforge.playerpro.instrumentfile";
internal let PPInstrumentListUTI = "com.quadmation.playerpro.list";
internal let PPMLDCUTI = "net.sourceforge.playerpro.PlayerPRO-Player.playerlistdragtype";

internal let PPRememberMusicList = "Remember Music List";
internal let PPLoadMusicAtListLoad = "Load music when loading list";
internal let PPAfterPlayingMusic = "After playing music";
internal let PPGotoStartupAfterPlaying = "Go to startup pos. after playing";
internal let PPSaveModList = "Ask to save modified Mod list";
internal let PPLoadMusicAtMusicLoad = "Play music on Music load";
internal let PPLoopMusicWhenDone = "Loop music";

internal let PPSoundOutBits = "Sound output bits";
internal let PPSoundOutRate = "Sound output rate";
internal let PPSoundDriver = "Sound Driver";
internal let PPStereoDelayToggle = "Stereo delay?";
internal let PPReverbToggle = "Reverb?";
internal let PPSurroundToggle = "Surround?";
internal let PPOversamplingToggle = "Oversampling?";
internal let PPStereoDelayAmount = "Stereo Delay amount";
internal let PPReverbAmount = "Reverb Amount";
internal let PPReverbStrength = "Reverb Strength";
internal let PPOversamplingAmount = "Oversampling Amount";

internal let PPDEShowInstrument = "Digital Editor Show Instrument";
internal let PPDEShowNote = "Digital Editor Show Note";
internal let PPDEShowEffect = "Digital Editor Show Effect";
internal let PPDEShowArgument = "Digital Editor Show Argument";
internal let PPDEShowVolume = "Digital Editor Show Volume";
internal let PPDEShowMarkers = "Digital Editor Show Markers";
internal let PPDEMarkerOffsetPref = "Digital Editor Marker Offset";
internal let PPDEMarkerLoopPref = "Digital Editor Marker Loop";
internal let PPDEMarkerColorPref = "Digital Editor Marker Color";
internal let PPDEMouseClickControlPref = "Digital Editor Mouse Click+Control";
internal let PPDEMouseClickShiftPref = "Digital Editor Mouse Click+Shift";
internal let PPDEMouseClickCommandPref = "Digital Editor Mouse Click+Command";
internal let PPDEMouseClickOptionPref = "Digital EditorMouse Click+Option";
internal let PPDELineHeightNormal = "Digital Editor Line Height Normal?";
internal let PPDEMusicTraceOn = "Digital Editor Trace On?";
internal let PPDEPatternWrappingPartition = "Digital Editor Pattern Wrapping Partition?";
internal let PPDEDragAsPcmd = "Digital Editor Drag as Pcmd?";

internal let PPBEMarkersEnabled = "Box Editor Markers On?";
internal let PPBEMarkersOffset = "Box Editor Markers Offset";
internal let PPBEMarkersLoop = "Box Editor Markers Loop";
internal let PPBEOctaveMarkers = "Box Editor Octave Markers?";
internal let PPBENotesProjection = "Box Editor Notes Projection?";

internal let PPMAddExtension = "Add Extension";
internal let PPMMadCompression = "Compress MAD?";
internal let PPMNoLoadMixerFromFiles = "No load Mixer?";
internal let PPMOscilloscopeDrawLines = "Oscilloscope Draw Lines";

internal let PPCEShowNotesLen = "Classical Editor Show Notes Length";
internal let PPCEShowMarkers = "Classical Editor Show Markers?";
internal let PPCEMarkerOffset = "Classical Editor Marker Offset";
internal let PPCEMarkerLoop = "Classical Editor Marker Loop";
internal let PPCETempoNum = "Classical Editor Temo Num.";
internal let PPCETempoUnit = "Classical Editor Tempo Unit";
internal let PPCETrackHeight = "Classical Editor Track Height";
