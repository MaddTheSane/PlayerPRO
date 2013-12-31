//
//  PPDriver.h
//  PPMacho
//
//  Created by C.W. Betts on 11/28/12.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

@class PPLibrary;
@class PPMusicObject;

@interface PPDriver : NSObject
@property (nonatomic, strong) PPMusicObject *currentMusic;
@property (readonly) PPLibrary *theLibrary;
@property (readonly) MADDriverSettings driverSettings;
@property NSTimeInterval musicPosition;

- (id)initWithLibrary:(PPLibrary *)theLib;
- (id)initWithLibrary:(PPLibrary *)theLib settings:(MADDriverSettings *)theSettings;
- (id)initWithLibrary:(PPLibrary *)theLib settings:(MADDriverSettings *)theSettings error:(out OSErr*)theErr;

- (OSErr)changeDriverSettingsToSettings:(MADDriverSettings)theSett;

- (void)beginExport;
- (void)endExport;
@property BOOL isExporting;

- (void)cleanDriver;

- (BOOL)directSaveToPointer:(void*)thePtr settings:(MADDriverSettings*)theSett;
- (NSInteger)audioLength;

- (OSErr)getMusicStatusWithCurrentTime:(long*)curTime totalTime:(long*)totTime;

- (OSErr)play;
- (OSErr)pause;
- (OSErr)stop;
- (BOOL)isPlayingMusic;
- (BOOL)isDonePlaying DEPRECATED_ATTRIBUTE;
- (BOOL)isDonePlayingMusic;
- (BOOL)isPaused;

- (PPMusicObject *)loadMusicFile:(NSString*)path;
- (PPMusicObject *)loadMusicURL:(NSURL*)url;

@end
