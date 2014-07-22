//
//  PPMusicObject.h
//  PPMacho
//
//  Created by C.W. Betts on 12/1/12.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROKit/PPObjectProtocol.h>

@class PPDriver;
@class PPLibrary;
@class PPInstrumentObject;

//Only use this class for playback!
@interface PPMusicObject : NSObject
@property (readonly) int totalPatterns;
@property (readonly) int totalPartitions;
@property (readonly) int partitionLength;
@property (readonly) short totalTracks;
@property (readonly) short totalInstruments;
@property (readonly, weak) PPDriver *attachedDriver;
@property (readonly) NSURL *filePath;

//Creates a music object from the supplied MADK (PPMusicObject) or MAD bundle (PPMusicObjectWrapper)
- (instancetype)initWithURL:(NSURL *)url;
- (instancetype)initWithPath:(NSString *)url;

//Creates a music object from any supported tracker type.
- (instancetype)initWithURL:(NSURL *)url library:(PPLibrary *)theLib;
- (instancetype)initWithPath:(NSString *)url library:(PPLibrary *)theLib;

//Creates a music object from any supported tracker type, also attaching a driver to the music.
- (instancetype)initWithURL:(NSURL *)url driver:(PPDriver *)theLib;
- (instancetype)initWithPath:(NSString *)url driver:(PPDriver *)theLib;

+ (MADErr)info:(PPInfoRec*)theInfo fromTrackerAtURL:(NSURL*)thURL usingLibrary:(PPLibrary*)theLib;

- (NSDictionary*)musicClasses;

- (NSString*)internalFileName;
- (NSString*)madInfo;

//Save music to a URL in MADK format in PPMusicObject, and MAD bundle in PPMusicObjectWrapper
- (MADErr)saveMusicToURL:(NSURL *)tosave;
- (MADErr)saveMusicToURL:(NSURL *)tosave compress:(BOOL)mad1Comp;
- (MADErr)exportMusicToURL:(NSURL *)tosave format:(NSString*)form library:(PPLibrary*)otherLib;

//This method sets the music object as the playback music
- (void)attachToDriver:(PPDriver *)theDriv;

//Creates a music struct for use outside of PlayerPROKit.
- (MADMusic *)copyMadMusicStruct;

- (MADMusic *)internalMadMusicStruct NS_RETURNS_INNER_POINTER;

- (NSArray *)instruments;

- (MADErr)exportInstrumentListToURL:(NSURL*)outURL;

@end

@interface PPMusicObjectWrapper : PPMusicObject <NSCopying>
@property (readonly, unsafe_unretained) NSArray *sDatas;
@property (readonly, strong) NSMutableArray *instruments;
@property (readonly, strong) NSMutableArray *patterns;
@property (readonly, strong) NSMutableArray *buses;
@property (readonly) OSType madType;
@property (readwrite, copy, nonatomic) NSString *internalFileName;
@property (readwrite, copy, nonatomic) NSString *madInfo;
@property (copy) NSString *madAuthor;

//Use to create a blank music object.
- (instancetype)init;

//Import from another PPMusicObject
- (instancetype)initFromMusicObject:(PPMusicObject*)oldFormat;

//Load a MAD bundle from a path
- (instancetype)initWithPath:(NSString *)url;

//Load a MAD bundle from a URL
- (instancetype)initWithURL:(NSURL *)url;

//Creates a MADK tracker file
- (MADErr)exportMusicToURL:(NSURL *)tosave;

- (MADErr)createCopyMusicToURL:(NSURL *)tosave;

+ (MADErr)info:(PPInfoRec *)theInfo fromTrackerAtURL:(NSURL *)thURL;

- (BOOL)addInstrument:(PPInstrumentObject*)theIns;
- (BOOL)importInstrumentListFromURL:(NSURL *)insURL error:(out NSError *__autoreleasing*)theErr;

#pragma mark Document-based code
//For use with document classes, like NSDocument or UIDocument
//Load a MAD bundle from a file wrapper.
- (instancetype)initWithFileWrapper:(NSFileWrapper*)wrapper;

//The file wrapper
@property (strong, readonly) NSFileWrapper* musicWrapper;

@end
