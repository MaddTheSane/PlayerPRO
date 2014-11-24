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

#import <Foundation/Foundation.h>
#import <PlayerPROKit/PlayerPROKit.h>

__private_extern extern PPMusicObject *MIDIReadFromData(NSData *fileData) NS_RETURNS_RETAINED;
__private_extern extern NSInteger GetTracksNumber(NSURL *theURL);

#endif
