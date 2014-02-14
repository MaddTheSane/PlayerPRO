#include <CoreFoundation/CFPlugInCOM.h>
#include <CoreFoundation/CoreFoundation.h>
#include "OldCarbHeaders.h"
#include <QuickTime/QuickTime.h>
#include "MAD.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include "PPPlug.h"
#include "PPPrivate.h"

#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#endif

typedef OSType SFTypeList[4];

#ifdef __cplusplus
#include <string>
#include <iostream>
#include <vector>

extern "C" {
#endif	

#include "3D.proto.h"
#include "About.proto.h"
#include "Adaptators.proto.h"
#include "AEvent.proto.h"
#include "ClassicalP.proto.h"
#include "DataFiles.proto.h"
#include "Editor.proto.h"
#include "Fourier.proto.h"
#include "Help.proto.h"
#include "InstruList.proto.h"
#include "Liste.proto.h"
#include "Main.proto.h"
#include "MIDI.proto.h"
#include "MIDI-GM.proto.h"
#include "MODList.proto.h"
#include "Pattern.proto.h"
#include "Piano.proto.h"
#include "Record.proto.h"
#include "TextScroll.proto.h"
#include "VideoToolbox.h"
#include "Timer.proto.h"
#include "Tools.proto.h"
#include "Utils.proto.h"
#include "WaveEditor.proto.h"
#include "Command.proto.h"
#include "Mozart.proto.h"
#include "StaffEditor.proto.h"
#include "PubWindow.proto.h"
#include "Partition.proto.h"
#include "Undo.proto.h"
#include "Check.proto.h"
#include "DragManager.proto.h"
#include "SoundInput.proto.h"
#include "PlugsInit.proto.h"
#include "QuicktimeWindow.proto.h"
#include "ActiveHelp.proto.h"
#include "Spectrum.proto.h"
#include "Print.proto.h"
#include "InstruView.proto.h"
#include "TrackView.proto.h"
#include "DigitalInfo.proto.h"
#include "HelpOnline.proto.h"
#include "Oscillo.proto.h"
#include "PlugsPPDGInit.proto.h"
#include "ProgressBar.proto.h"
#include "Equalizer.proto.h"
#include "MIDI-Hardware.proto.h"
#include "Functions.proto.h"
#ifdef __cplusplus
}
#endif
