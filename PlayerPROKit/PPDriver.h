//
//  PPDriver.h
//  PPMacho
//
//  Created by C.W. Betts on 11/28/12.
//
//

#ifndef __PLAYERPROKIT_PPDRIVER_H__
#define __PLAYERPROKIT_PPDRIVER_H__

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

@class PPLibrary;
@class PPMusicObject;

#define UNAVAILABLE_REASON(theReason) __attribute__((unavailable(theReason)))

NS_ASSUME_NONNULL_BEGIN

@interface PPDriver : NSObject
@property (nonatomic, strong, nullable) PPMusicObject *currentMusic;
@property (readonly, strong) PPLibrary *theLibrary;
@property (readonly) MADDriverSettings driverSettings;
@property NSTimeInterval musicPosition;
@property (getter = isExporting) BOOL exporting;
@property (readonly) NSTimeInterval totalMusicPlaybackTime;

- (nonnull instancetype)init UNAVAILABLE_REASON("PPDriver cannot be inited without a library");
- (nullable instancetype)initWithLibrary:(PPLibrary *)theLib error:(out NSError* __nullable __autoreleasing* __nullable)theErr;
- (nullable instancetype)initWithLibrary:(PPLibrary *)theLib settings:(inout nullable MADDriverSettings *)theSettings error:(out NSError* __nullable __autoreleasing* __nullable)theErr NS_DESIGNATED_INITIALIZER;

/*!
 *	@method changeDriverSettingsToSettings:
 *	@abstract changes the sound format used by the driver.
 *	@param theSett
 *		The new sound settings to use.
 *	@param error
 *		A pointer to an \c NSError object. On failure, is populated with an error in the \c PPMADErrorDomain
 *	@return \c YES if successful, otherwise <code>NO</code>.
 */
- (BOOL)changeDriverSettingsToSettings:(MADDriverSettings*)theSett error:(out NSError* __nullable __autoreleasing* __nullable)error NS_SWIFT_NAME(changeDriverSettings(to:));

- (void)beginExport;
- (void)endExport;

- (void)cleanDriver NS_SWIFT_NAME(cleanDriver());
- (MADErr)stopDriver NS_SWIFT_NAME(stopDriver());

- (BOOL)directSaveToPointer:(void*)thePtr settings:(nullable MADDriverSettings*)theSett;
- (nullable NSData*)directSave;
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

/*!
 *	@method play
 *	@abstract Plays the song loaded in <code>currentMusic</code>.
 *	@return An error type on failure, or \c MADNoErr on success.
 */
- (MADErr)play;

/*!
 *	@method pause
 *	@abstract Pauses the currently playing song.
 *	@return An error type on failure, or \c MADNoErr on success.
 */
- (MADErr)pause;

/*!
 *	@method stop
 *	@abstract Pauses the currently playing song and sets the position at
 *	the beginning of the song
 *	@return An error type on failure, or \c MADNoErr on success.
 */
- (MADErr)stop;

@property (readonly, getter=isPlayingMusic)		BOOL playingMusic;
@property (readonly, getter=isDonePlayingMusic) BOOL donePlayingMusic;
@property (getter=isPaused)						BOOL paused;

- (nullable PPMusicObject *)loadMusicFile:(NSString*)path NS_RETURNS_RETAINED;
- (nullable PPMusicObject *)loadMusicURL:(NSURL*)url NS_RETURNS_RETAINED;

#pragma mark - More in-depth modification of the driver:

- (MADChannel)channelAtIndex:(NSInteger)idx; //Read-only
@property short patternPosition;
@property short patternIdentifier;
@property short partitionPosition;
///0 to 64
@property short volume;
@property BOOL usesEqualizer;
@property (readonly, nullable) void *oscilloscopePointer NS_RETURNS_INNER_POINTER;
@property (readonly) size_t oscilloscopeSize;
- (BOOL)isChannelActiveAtIndex:(NSInteger)idx;
- (void)setChannelAtIndex:(NSInteger)idx toActive:(BOOL)enabled;

@end

@interface PPDriver (deprecated)
- (BOOL)isDonePlaying DEPRECATED_ATTRIBUTE;

- (NSInteger)audioLength DEPRECATED_ATTRIBUTE;
@end

NS_ASSUME_NONNULL_END

#endif
