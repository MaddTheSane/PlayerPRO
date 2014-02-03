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
@property BOOL isExporting;

- (id)initWithLibrary:(PPLibrary *)theLib;
- (id)initWithLibrary:(PPLibrary *)theLib settings:(MADDriverSettings *)theSettings;
- (id)initWithLibrary:(PPLibrary *)theLib settings:(MADDriverSettings *)theSettings error:(out OSErr*)theErr;

- (OSErr)changeDriverSettingsToSettings:(MADDriverSettings)theSett;

- (void)beginExport;
- (void)endExport;

- (void)cleanDriver;

- (BOOL)directSaveToPointer:(void*)thePtr settings:(MADDriverSettings*)theSett;
- (NSInteger)audioLength DEPRECATED_ATTRIBUTE;
- (NSInteger)audioDataLength;

- (OSErr)getMusicStatusWithCurrentTime:(long*)curTime totalTime:(long*)totTime;

//This is the main one that gets called
- (OSErr)playSoundDataFromPointer:(const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote withLoopStartingAt:(NSUInteger)loopStart andLoopLength:(NSUInteger)loopLen;

- (OSErr)playSoundDataFromPointer:(const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo;
- (OSErr)playSoundDataFromPointer:(const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote;
- (OSErr)playSoundDataFromPointer:(const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withLoopInRange:(NSRange)loopRange;
- (OSErr)playSoundDataFromPointer:(const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote withLoopInRange:(NSRange)loopRange;
- (OSErr)playSoundDataFromPointer:(const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withLoopStartingAt:(NSUInteger)loopStart andLoopLength:(NSUInteger)loopLen;

- (OSErr)playSoundDataFromData:(NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo;
- (OSErr)playSoundDataFromData:(NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote;
- (OSErr)playSoundDataFromData:(NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withLoopInRange:(NSRange)loopRange;
- (OSErr)playSoundDataFromData:(NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote withLoopInRange:(NSRange)loopRange;
- (OSErr)playSoundDataFromData:(NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withLoopStartingAt:(NSUInteger)loopStart andLoopLength:(NSUInteger)loopLen;
- (OSErr)playSoundDataFromData:(NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote withLoopStartingAt:(NSUInteger)loopStart andLoopLength:(NSUInteger)loopLen;

#if 0
- (OSErr)playSoundDataFromData:(NSData*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo;
#endif

- (int)availableChannel;

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
