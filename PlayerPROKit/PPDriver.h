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
@property (nonatomic, strong, nullable) PPMusicObject *currentMusic;
@property (readonly, strong, nonnull) PPLibrary *theLibrary;
@property (readonly) MADDriverSettings driverSettings;
@property NSTimeInterval musicPosition;
@property (getter = isExporting) BOOL exporting;
@property (readonly) NSTimeInterval totalMusicPlaybackTime;

- (instancetype)init UNAVAILABLE_REASON("PPDriver cannot be inited without a library");
- (instancetype)initWithLibrary:(__nonnull PPLibrary *)theLib error:(out __nullable NSError* __autoreleasing* __nullable)theErr;
- (instancetype)initWithLibrary:(__nonnull PPLibrary *)theLib settings:(inout __nullable MADDriverSettings *)theSettings error:(out __nullable NSError* __autoreleasing* __nullable)theErr NS_DESIGNATED_INITIALIZER;

- (MADErr)changeDriverSettingsToSettings:(MADDriverSettings)theSett;

- (void)beginExport;
- (void)endExport;

- (void)cleanDriver;
- (MADErr)stopDriver;

- (BOOL)directSaveToPointer:(__nonnull void*)thePtr settings:(__nonnull MADDriverSettings*)theSett;
- (__nullable NSData*)directSave;
@property (readonly) NSInteger audioDataLength;

- (MADErr)getMusicStatusWithCurrentTime:(__nonnull long*)curTime totalTime:(__nonnull long*)totTime;
- (MADErr)setMusicStatusWithCurrentTime:(long)curTime maximumTime:(long)maxV minimumTime:(long)minV;

//This is the main one that gets called
- (MADErr)playSoundDataFromPointer:(__nonnull const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote withLoopStartingAt:(NSUInteger)loopStart andLoopLength:(NSUInteger)loopLen;

- (MADErr)playSoundDataFromPointer:(__nonnull const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo;
- (MADErr)playSoundDataFromPointer:(__nonnull const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote;
- (MADErr)playSoundDataFromPointer:(__nonnull const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withLoopInRange:(NSRange)loopRange;
- (MADErr)playSoundDataFromPointer:(__nonnull const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote withLoopInRange:(NSRange)loopRange;
- (MADErr)playSoundDataFromPointer:(__nonnull const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withLoopStartingAt:(NSUInteger)loopStart andLoopLength:(NSUInteger)loopLen;

- (MADErr)playSoundDataFromData:(__nonnull NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo;
- (MADErr)playSoundDataFromData:(__nonnull NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote;
- (MADErr)playSoundDataFromData:(__nonnull NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withLoopInRange:(NSRange)loopRange;
- (MADErr)playSoundDataFromData:(__nonnull NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote withLoopInRange:(NSRange)loopRange;
- (MADErr)playSoundDataFromData:(__nonnull NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withLoopStartingAt:(NSUInteger)loopStart andLoopLength:(NSUInteger)loopLen;
- (MADErr)playSoundDataFromData:(__nonnull NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote withLoopStartingAt:(NSUInteger)loopStart andLoopLength:(NSUInteger)loopLen;

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

- (__nullable PPMusicObject *)loadMusicFile:(__nonnull NSString*)path NS_RETURNS_RETAINED;
- (__nullable PPMusicObject *)loadMusicURL:(__nonnull NSURL*)url NS_RETURNS_RETAINED;

// More in-depth modification of the driver:

- (MADChannel)channelAtIndex:(NSInteger)idx; //Read-only
@property short patternPosition;
@property short patternIdentifier;
@property short partitionPosition;
///0 to 64
@property short volume;
@property BOOL useEqualizer;
@property (readonly, nullable) void *oscilloscopePointer NS_RETURNS_INNER_POINTER;
@property (readonly) size_t oscilloscopeSize;
- (BOOL)isChannelActiveAtIndex:(NSInteger)idx;
- (void)setChannelAtIndex:(NSInteger)idx to:(BOOL)enabled;

@end

@interface PPDriver (deprecated)
- (BOOL)isDonePlaying DEPRECATED_ATTRIBUTE;

- (NSInteger)audioLength DEPRECATED_ATTRIBUTE;
@end
