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

#define UNAVAILABLE_REASON(theReason) __attribute__((unavailable(theReason)))

#ifndef NS_DESIGNATED_INITIALIZER
#define NS_DESIGNATED_INITIALIZER
#endif

@interface PPDriver : NSObject
@property (nonatomic, strong) PPMusicObject *currentMusic;
@property (readonly) PPLibrary *theLibrary;
@property (readonly) MADDriverSettings driverSettings;
@property NSTimeInterval musicPosition;
@property (getter = isExporting) BOOL exporting;
@property (readonly) NSTimeInterval totalMusicPlaybackTime;
@property (readonly) NSTimeInterval currentMusicPlaybackTime;

- (instancetype)init UNAVAILABLE_REASON("PPDriver cannot be inited without a library");
- (instancetype)initWithLibrary:(PPLibrary *)theLib error:(out NSError* __autoreleasing*)theErr;
- (instancetype)initWithLibrary:(PPLibrary *)theLib settings:(inout MADDriverSettings *)theSettings error:(out NSError* __autoreleasing*)theErr NS_DESIGNATED_INITIALIZER;

- (MADErr)changeDriverSettingsToSettings:(MADDriverSettings)theSett;

- (void)beginExport;
- (void)endExport;

- (void)cleanDriver;
- (MADErr)stopDriver;

- (BOOL)directSaveToPointer:(void*)thePtr settings:(MADDriverSettings*)theSett;
- (NSData*)directSave;
@property (readonly) NSInteger audioDataLength;

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

@property (readonly) short availableChannel;

- (MADErr)play;
- (MADErr)pause;
- (MADErr)stop;

@property (readonly, getter=isPlayingMusic)		BOOL playingMusic;
@property (readonly, getter=isDonePlayingMusic) BOOL donePlayingMusic;
@property (getter=isPaused)						BOOL paused;

- (PPMusicObject *)loadMusicFile:(NSString*)path NS_RETURNS_RETAINED;
- (PPMusicObject *)loadMusicURL:(NSURL*)url NS_RETURNS_RETAINED;

@end

@interface PPDriver (deprecated)
- (BOOL)isDonePlaying DEPRECATED_ATTRIBUTE;

- (NSInteger)audioLength DEPRECATED_ATTRIBUTE;
@end
