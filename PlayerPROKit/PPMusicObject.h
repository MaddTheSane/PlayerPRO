//
//  PPMusicObject.h
//  PPMacho
//
//  Created by C.W. Betts on 12/1/12.
//
//

#ifndef __PLAYERPROKIT_PPMUSICOBJECT_H__
#define __PLAYERPROKIT_PPMUSICOBJECT_H__

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

NS_ASSUME_NONNULL_BEGIN

@class PPDriver;
@class PPLibrary;
@class PPInstrumentObject;
@class PPSampleObject;
@class PPPatternObject;
@class PPFXBusObject;

@interface PPMusicObject : NSObject <NSCopying>
@property (readonly) NSInteger countOfPatterns;
@property (readonly) NSInteger lengthOfPartitions;
@property (readonly) NSInteger totalTracks;
@property (readonly, strong, nonatomic) NSArray<PPSampleObject*> *sDatas;
@property (readonly, strong, nonatomic) NSArray<PPInstrumentObject*> *instruments;
@property (readonly, strong, nonatomic) NSMutableArray<PPPatternObject*> *patterns;
@property (readonly, strong, nonatomic) NSMutableArray<PPFXBusObject*> *buses;
@property (readwrite, copy, null_resettable) NSString *title;
@property (readwrite, copy, null_resettable) NSString *information;
@property (readonly, weak, nullable) PPDriver *attachedDriver;

@property BOOL usesLinearPitchTable;
@property BOOL limitPitchToMODTable;
@property BOOL showsCopyright;
@property int newPitch;
@property int newSpeed;
@property MADByte generalPitch;
@property MADByte generalSpeed;
@property MADByte generalVolume;

- (instancetype)init;

/// Creates a music object from the supplied MADK file ONLY
- (nullable instancetype)initWithURL:(NSURL *)url error:(out NSError* __nullable __autoreleasing* __nullable)error NS_DESIGNATED_INITIALIZER;
- (nullable instancetype)initWithPath:(NSString *)url error:(out NSError* __nullable __autoreleasing* __nullable)error;

/// Creates a music object from any supported tracker type.
- (nullable instancetype)initWithURL:(NSURL *)url library:(PPLibrary *)theLib error:(out NSError* __nullable __autoreleasing* __nullable)error;
- (nullable instancetype)initWithPath:(NSString *)url library:(PPLibrary *)theLib error:(out NSError* __nullable __autoreleasing* __nullable)error;

/// Creates a music object from the specified music type.
/// If the type isn't available, it returns nil.
- (nullable instancetype)initWithURL:(NSURL *)url type:(in const char*)type library:(PPLibrary *)theLib error:(out NSError* __nullable __autoreleasing* __nullable)error NS_DESIGNATED_INITIALIZER;
- (nullable instancetype)initWithPath:(NSString *)path type:(in const char*)type library:(PPLibrary *)theLib error:(out NSError* __nullable __autoreleasing* __nullable)error;
- (nullable instancetype)initWithURL:(NSURL *)url stringType:(NSString*)type library:(PPLibrary *)theLib error:(out NSError* __nullable __autoreleasing* __nullable)error;
- (nullable instancetype)initWithPath:(NSString *)path stringType:(NSString*)type library:(PPLibrary *)theLib error:(out NSError* __nullable __autoreleasing* __nullable)error;

/// Creates a music object from any supported tracker type, also attaching a driver to the music.
- (nullable instancetype)initWithURL:(NSURL *)url driver:(PPDriver *)theLib error:(out NSError* __nullable __autoreleasing* __nullable)error;
- (nullable instancetype)initWithPath:(NSString *)path driver:(PPDriver *)theLib error:(out NSError* __nullable __autoreleasing* __nullable)error;
- (nullable instancetype)initWithURL:(NSURL *)url type:(in const char*)type driver:(PPDriver *)theLib error:(out NSError* __nullable __autoreleasing* __nullable)error;
- (nullable instancetype)initWithPath:(NSString *)path type:(in const char*)type driver:(PPDriver *)theLib error:(out NSError* __nullable __autoreleasing* __nullable)error;
- (nullable instancetype)initWithURL:(NSURL *)url stringType:(NSString *)type driver:(PPDriver *)theDriv error:(out NSError* __nullable __autoreleasing* __nullable)error;
- (nullable instancetype)initWithPath:(NSString *)path stringType:(NSString*)type driver:(PPDriver *)theDriv error:(out NSError* __nullable __autoreleasing* __nullable)error;

/*!
 *	Initializes a music object based on a music struct, copying if specified.
 */
- (instancetype)initWithMusicStruct:(MADMusic*)theStruct copy:(BOOL)copyData NS_DESIGNATED_INITIALIZER;

/*!
 *	Initializes a music object based on a music struct, copying it.
 */
- (instancetype)initWithMusicStruct:(MADMusic*)theStruct;

+ (MADErr)info:(MADInfoRec*)theInfo fromTrackerAtURL:(NSURL*)thURL usingLibrary:(PPLibrary*)theLib;

/// Save music to a URL in MADK format.
- (BOOL)saveMusicToURL:(NSURL *)tosave error:(out NSError* __nullable __autoreleasing* __nullable)error;
- (BOOL)saveMusicToURL:(NSURL *)tosave compress:(BOOL)mad1Comp error:(out NSError* __nullable __autoreleasing* __nullable)error;

- (BOOL)exportMusicToURL:(NSURL *)tosave format:(NSString*)form library:(PPLibrary*)otherLib error:(out NSError* __nullable __autoreleasing* __nullable)error;

/// This method sets the music object as the playback music
- (void)attachToDriver:(PPDriver *)theDriv;

- (void)addInstrumentObject:(PPInstrumentObject *)object;
- (void)replaceObjectInInstrumentsAtIndex:(NSInteger)index withObject:(PPInstrumentObject *)object;
@property (readonly) NSInteger countOfInstruments;
- (PPInstrumentObject*)instrumentObjectAtIndex:(NSInteger)idx;
- (void)clearInstrumentsAtIndexes:(NSIndexSet *)indexes;
- (void)clearInstrumentObjectAtIndex:(NSInteger)index;

@property (readonly) MADMusic *internalMadMusicStruct NS_RETURNS_INNER_POINTER;

- (MADErr)exportInstrumentListToURL:(NSURL*)outURL;
- (BOOL)addInstrument:(PPInstrumentObject*)theIns;
- (BOOL)importInstrumentListFromURL:(NSURL *)insURL error:(out NSError * __nullable __autoreleasing*__nullable)theErr;

@end

NS_ASSUME_NONNULL_END

#endif
