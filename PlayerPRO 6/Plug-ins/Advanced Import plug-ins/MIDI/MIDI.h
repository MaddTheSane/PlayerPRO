//
//  MIDI.h
//  PPMacho
//
//  Created by C.W. Betts on 8/28/14.
//
//

#ifndef PPMacho_MIDI_h
#define PPMacho_MIDI_h

#define __private_extern __attribute__((visibility("hidden")))

@import Foundation;
@import PlayerPROKit;
__private_extern extern PPMusicObject *MIDIReadFromData(NSData *fileData);

#endif
