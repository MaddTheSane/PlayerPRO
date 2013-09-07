//
//  PPMusicObject_PPKPrivate.h
//  PPMacho
//
//  Created by C.W. Betts on 12/1/12.
//
//

#import "PPMusicObject.h"

@interface PPMusicObject ()
@property (readonly) MADMusic *_currentMusic;
@property (readwrite, weak) PPDriver *attachedDriver;
@property (strong) PPLibrary *currentLibrary;
@end
