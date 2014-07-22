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

- (instancetype)initWithLibrary:(PPLibrary *)theLib;
- (instancetype)initWithLibrary:(PPLibrary *)theLib settings:(MADDriverSettings *)theSettings;
- (instancetype)initWithLibrary:(PPLibrary *)theLib settings:(MADDriverSettings *)theSettings error:(out MADErr*)theErr;

- (MADErr)changeDriverSettingsToSettings:(MADDriverSettings)theSett;

- (void)beginExport;
- (void)endExport;

- (void)cleanDriver;
- (MADErr)stopDriver;

- (BOOL)directSaveToPointer:(void*)thePtr settings:(MADDriverSettings*)theSett;
- (NSInteger)audioLength DEPRECATED_ATTRIBUTE;
- (NSInteger)audioDataLength;

- (MADErr)getMusicStatusWithCurrentTime:(long*)curTime totalTime:(long*)totTime;
- (MADErr)setMusicStatusWithCurrentTime:(long)curTime maximumTime:(long)maxV minimumTime:(long)minV;

//This is the main one that gets called
- (MADErr)playSoundDataFromPointer:(const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote withLoopStartingAt:(NSUInteger)loopStart andLoopLength:(NSUInteger)loopLen;

- (MADErr)playSoundDataFromPointer:(const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo;
- (MADErr)playSoundDataFromPointer:(const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote;
- (MADErr)playSoundDataFromPointer:(const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withLoopInRange:(NSRange)loopRange;
- (MADErr)playSoundDataFromPointer:(const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote withLoopInRange:(NSRange)loopRange;
- (MADErr)playSoundDataFromPointer:(const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withLoopStartingAt:(NSUInteger)loopStart andLoopLength:(NSUInteger)loopLen;

- (MADErr)playSoundDataFromData:(NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo;
- (MADErr)playSoundDataFromData:(NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote;
- (MADErr)playSoundDataFromData:(NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withLoopInRange:(NSRange)loopRange;
- (MADErr)playSoundDataFromData:(NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote withLoopInRange:(NSRange)loopRange;
- (MADErr)playSoundDataFromData:(NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withLoopStartingAt:(NSUInteger)loopStart andLoopLength:(NSUInteger)loopLen;
- (MADErr)playSoundDataFromData:(NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote withLoopStartingAt:(NSUInteger)loopStart andLoopLength:(NSUInteger)loopLen;

#if 0
- (MADErr)playSoundDataFromData:(NSData*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo;
#endif

- (int)availableChannel;

- (MADErr)play;
- (MADErr)pause;
- (MADErr)stop;
- (BOOL)isPlayingMusic;
- (BOOL)isDonePlaying DEPRECATED_ATTRIBUTE;
- (BOOL)isDonePlayingMusic;
- (BOOL)isPaused;

- (PPMusicObject *)loadMusicFile:(NSString*)path;
- (PPMusicObject *)loadMusicURL:(NSURL*)url;

@end
