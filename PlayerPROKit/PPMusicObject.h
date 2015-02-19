//
//  PPMusicObject.h
//  PPMacho
//
//  Created by C.W. Betts on 12/1/12.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

#ifndef NS_DESIGNATED_INITIALIZER
#define NS_DESIGNATED_INITIALIZER
#endif

@class PPDriver;
@class PPLibrary;
@class PPInstrumentObject;

@interface PPMusicObject : NSObject <NSCopying>
@property (readonly) int totalPatterns;
@property (readonly) int totalPartitions;
@property (readonly) int partitionLength;
@property (readonly) short totalTracks;
@property (readonly) short totalInstruments;
@property (readonly, strong, nonatomic, nonnull) NSArray *sDatas;
@property (readonly, strong, nonatomic, nonnull) NSArray *instruments;
@property (readonly, strong, nonatomic, nonnull) NSMutableArray *patterns;
@property (readonly, strong, nonatomic, nonnull) NSMutableArray *buses;
@property (readwrite, copy, null_resettable) NSString *title;
@property (readwrite, copy, null_resettable) NSString *information;
@property (readonly, weak, nullable) PPDriver *attachedDriver;
@property (readonly, strong, nullable) NSURL *filePath;

@property BOOL usesLinearPitchTable;
@property BOOL limitPitchToMODTable;
@property BOOL showsCopyright;
@property int newPitch;
@property int newSpeed;
@property MADByte generalPitch;
@property MADByte generalSpeed;
@property MADByte generalVolume;

- (instancetype)init;
// Creates a music object from the supplied MADK file ONLY
- (instancetype)initWithURL:(__nonnull NSURL *)url error:(out __nullable NSError* __autoreleasing*)error NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithPath:(__nonnull NSString *)url error:(out __nullable NSError* __autoreleasing*)error;

// Creates a music object from any supported tracker type.
- (instancetype)initWithURL:(__nonnull NSURL *)url library:(__nonnull PPLibrary *)theLib error:(out __nullable NSError* __autoreleasing*)error;
- (instancetype)initWithPath:(__nonnull NSString *)url library:(__nonnull PPLibrary *)theLib error:(out __nullable NSError* __autoreleasing* __nullable)error;

// Creates a music object from the specified music type.
// If the type isn't available, it returns nil.
- (instancetype)initWithURL:(__nonnull NSURL *)url type:(in __nonnull const char*)type library:(__nonnull PPLibrary *)theLib error:(out __nullable NSError* __autoreleasing* __nullable)error NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithPath:(__nonnull NSString *)path type:(in __nonnull const char*)type library:(__nonnull PPLibrary *)theLib error:(out __nullable NSError* __autoreleasing* __nullable)error;
- (instancetype)initWithURL:(__nonnull NSURL *)url stringType:(__nonnull NSString*)type library:(__nonnull PPLibrary *)theLib error:(out __nullable NSError* __autoreleasing* __nullable)error;
- (instancetype)initWithPath:(__nonnull NSString *)path stringType:(__nonnull NSString*)type library:(__nonnull PPLibrary *)theLib error:(out __nullable NSError* __autoreleasing* __nullable)error;

// Creates a music object from any supported tracker type, also attaching a driver to the music.
- (instancetype)initWithURL:(__nonnull NSURL *)url driver:(__nonnull PPDriver *)theLib error:(out __nullable NSError* __autoreleasing* __nullable)error;
- (instancetype)initWithPath:(__nonnull NSString *)path driver:(__nonnull PPDriver *)theLib error:(out __nullable NSError* __autoreleasing* __nullable)error;
- (instancetype)initWithURL:(__nonnull NSURL *)url type:(in const char*)type driver:(__nonnull PPDriver *)theLib error:(out __nullable NSError* __autoreleasing* __nullable)error;
- (instancetype)initWithPath:(__nonnull NSString *)path type:(in const char*)type driver:(__nonnull PPDriver *)theLib error:(out __nullable NSError* __autoreleasing* __nullable)error;
- (instancetype)initWithURL:(__nonnull NSURL *)url stringType:(NSString*)type driver:(__nonnull PPDriver *)theDriv error:(out __nullable NSError* __autoreleasing* __nullable)error;
- (instancetype)initWithPath:(__nonnull NSString *)path stringType:(NSString*)type driver:(__nonnull PPDriver *)theDriv error:(out __nullable NSError* __autoreleasing* __nullable)error;

/*!
 *	Initializes a music object based on a music struct, copying if specified.
 */
- (instancetype)initWithMusicStruct:(__nonnull MADMusic*)theStruct copy:(BOOL)copyData NS_DESIGNATED_INITIALIZER;

/*!
 *	Initializes a music object based on a music struct, copying it.
 */
- (instancetype)initWithMusicStruct:(__nonnull MADMusic*)theStruct;

+ (MADErr)info:(__nonnull MADInfoRec*)theInfo fromTrackerAtURL:(__nonnull NSURL*)thURL usingLibrary:(__nonnull PPLibrary*)theLib;

// Save music to a URL in MADK format.
- (MADErr)saveMusicToURL:(__nonnull NSURL *)tosave;
- (MADErr)saveMusicToURL:(__nonnull NSURL *)tosave compress:(BOOL)mad1Comp;
- (MADErr)exportMusicToURL:(__nonnull NSURL *)tosave format:(__nonnull NSString*)form library:(__nonnull PPLibrary*)otherLib;

//This method sets the music object as the playback music
- (void)attachToDriver:(__nonnull PPDriver *)theDriv;

- (void)addInstrumentObject:(__nonnull PPInstrumentObject *)object;
- (void)replaceObjectInInstrumentsAtIndex:(NSInteger)index withObject:(__nonnull PPInstrumentObject *)object;
@property (readonly) NSInteger countOfInstruments;
- (__nonnull PPInstrumentObject*)instrumentObjectAtIndex:(NSInteger)idx;
- (void)clearInstrumentsAtIndexes:(__nonnull NSIndexSet *)indexes;
- (void)clearInstrumentObjectAtIndex:(NSInteger)index;

@property (readonly, nonnull) MADMusic *internalMadMusicStruct NS_RETURNS_INNER_POINTER;

- (MADErr)exportInstrumentListToURL:(__nonnull NSURL*)outURL;
- (BOOL)addInstrument:(__nonnull PPInstrumentObject*)theIns;
- (BOOL)importInstrumentListFromURL:(__nonnull NSURL *)insURL error:(out __nullable NSError *__autoreleasing*__nullable)theErr;

@end
