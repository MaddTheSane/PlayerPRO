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

- (nonnull instancetype)init;
// Creates a music object from the supplied MADK file ONLY
- (nullable instancetype)initWithURL:(nonnull NSURL *)url error:(out NSError* __nullable __autoreleasing* __nullable)error NS_DESIGNATED_INITIALIZER;
- (nullable instancetype)initWithPath:(nonnull NSString *)url error:(out  NSError* __nullable  __autoreleasing* __nullable)error;

// Creates a music object from any supported tracker type.
- (nullable instancetype)initWithURL:(nonnull NSURL *)url library:(nonnull PPLibrary *)theLib error:(out  NSError* __nullable  __autoreleasing* __nullable)error;
- (nullable instancetype)initWithPath:(nonnull NSString *)url library:(nonnull PPLibrary *)theLib error:(out  NSError* __nullable  __autoreleasing* __nullable)error;

// Creates a music object from the specified music type.
// If the type isn't available, it returns nil.
- (nullable instancetype)initWithURL:(nonnull NSURL *)url type:(in nonnull const char*)type library:(nonnull PPLibrary *)theLib error:(out NSError* __nullable __autoreleasing* __nullable)error NS_DESIGNATED_INITIALIZER;
- (nullable instancetype)initWithPath:(nonnull NSString *)path type:(in nonnull const char*)type library:(nonnull PPLibrary *)theLib error:(out  NSError* __nullable  __autoreleasing* __nullable)error;
- (nullable instancetype)initWithURL:(nonnull NSURL *)url stringType:(nonnull NSString*)type library:(nonnull PPLibrary *)theLib error:(out  NSError* __nullable  __autoreleasing* __nullable)error;
- (nullable instancetype)initWithPath:(nonnull NSString *)path stringType:(nonnull NSString*)type library:(nonnull PPLibrary *)theLib error:(out  NSError* __nullable  __autoreleasing* __nullable)error;

// Creates a music object from any supported tracker type, also attaching a driver to the music.
- (nullable instancetype)initWithURL:(nonnull NSURL *)url driver:(nonnull PPDriver *)theLib error:(out  NSError* __nullable  __autoreleasing* __nullable)error;
- (nullable instancetype)initWithPath:(nonnull NSString *)path driver:(nonnull PPDriver *)theLib error:(out  NSError* __nullable  __autoreleasing* __nullable)error;
- (nullable instancetype)initWithURL:(nonnull NSURL *)url type:(in const char* __nonnull)type driver:(nonnull PPDriver *)theLib error:(out NSError* __nullable __autoreleasing* __nullable)error;
- (nullable instancetype)initWithPath:(nonnull NSString *)path type:(in const char* __nonnull)type driver:(nonnull PPDriver *)theLib error:(out NSError* __nullable __autoreleasing* __nullable)error;
- (nullable instancetype)initWithURL:(nonnull NSURL *)url stringType:(nonnull NSString *)type driver:(nonnull PPDriver *)theDriv error:(out  NSError* __nullable  __autoreleasing* __nullable)error;
- (nullable instancetype)initWithPath:(nonnull NSString *)path stringType:(nonnull NSString*)type driver:(nonnull PPDriver *)theDriv error:(out  NSError* __nullable  __autoreleasing* __nullable)error;

/*!
 *	Initializes a music object based on a music struct, copying if specified.
 */
- (nonnull instancetype)initWithMusicStruct:(nonnull MADMusic*)theStruct copy:(BOOL)copyData NS_DESIGNATED_INITIALIZER;

/*!
 *	Initializes a music object based on a music struct, copying it.
 */
- (nonnull instancetype)initWithMusicStruct:(nonnull MADMusic*)theStruct;

+ (MADErr)info:(nonnull MADInfoRec*)theInfo fromTrackerAtURL:(nonnull NSURL*)thURL usingLibrary:(nonnull PPLibrary*)theLib;

// Save music to a URL in MADK format.
- (MADErr)saveMusicToURL:(nonnull NSURL *)tosave;
- (MADErr)saveMusicToURL:(nonnull NSURL *)tosave compress:(BOOL)mad1Comp;
- (MADErr)exportMusicToURL:(nonnull NSURL *)tosave format:(nonnull NSString*)form library:(nonnull PPLibrary*)otherLib;

//This method sets the music object as the playback music
- (void)attachToDriver:(nonnull PPDriver *)theDriv;

- (void)addInstrumentObject:(nonnull PPInstrumentObject *)object;
- (void)replaceObjectInInstrumentsAtIndex:(NSInteger)index withObject:(nonnull PPInstrumentObject *)object;
@property (readonly) NSInteger countOfInstruments;
- (nonnull PPInstrumentObject*)instrumentObjectAtIndex:(NSInteger)idx;
- (void)clearInstrumentsAtIndexes:(nonnull NSIndexSet *)indexes;
- (void)clearInstrumentObjectAtIndex:(NSInteger)index;

@property (readonly, nonnull) MADMusic *internalMadMusicStruct NS_RETURNS_INNER_POINTER;

- (MADErr)exportInstrumentListToURL:(nonnull NSURL*)outURL;
- (BOOL)addInstrument:(nonnull PPInstrumentObject*)theIns;
- (BOOL)importInstrumentListFromURL:(nonnull NSURL *)insURL error:(out NSError * __nullable __autoreleasing*__nullable)theErr;

@end
