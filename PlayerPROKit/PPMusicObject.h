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

@interface PPMusicObject : NSObject

+ (OSErr)info:(PPInfoRec*)theInfo fromTrackerAtURL:(NSURL*)thURL usingLibrary:(PPLibrary*)theLib;

@property (readonly, strong, nonatomic) NSString *internalFileName;
@property (readonly, strong, nonatomic) NSString *madInfo;

@property (readonly) int totalPatterns;
@property (readonly) int totalPartitions;
@property (readonly) int partitionLength;
@property (readonly) short totalTracks;
@property (readonly) short totalInstruments;

@property (readonly, weak) PPDriver *attachedDriver;
@property (readonly) NSURL *filePath;

//Save music to a URL in MADK format in PPMusicObject, and MAD bundle in PPMusicObjectWrapper
- (OSErr)saveMusicToURL:(NSURL *)tosave;
- (OSErr)saveMusicToURL:(NSURL *)tosave compress:(BOOL)mad1Comp;
- (OSErr)exportMusicToURL:(NSURL *)tosave format:(NSString*)form library:(PPLibrary*)otherLib;

//Creates a music object from the supplied MADK (PPMusicObject) or MAD bundle (PPMusicObjectWrapper)
- (instancetype)initWithURL:(NSURL *)url;
- (instancetype)initWithPath:(NSString *)url;

//Creates a music object from any supported tracker type.
- (instancetype)initWithURL:(NSURL *)url library:(PPLibrary *)theLib;
- (instancetype)initWithPath:(NSString *)url library:(PPLibrary *)theLib;

//Creates a music object from any supported tracker type, also attaching a driver to the music.
- (instancetype)initWithURL:(NSURL *)url driver:(PPDriver *)theLib;
- (instancetype)initWithPath:(NSString *)url driver:(PPDriver *)theLib;

//This method sets the music object as the playback music
- (void)attachToDriver:(PPDriver *)theDriv;

//Creates a music struct for use outside of PlayerPROKit.
- (MADMusic *)copyMadMusicStruct;

- (MADMusic *)internalMadMusicStruct NS_RETURNS_INNER_POINTER;

@end

@interface PPMusicObjectWrapper : PPMusicObject <NSCopying>
@property (readonly, unsafe_unretained) NSArray *sDatas;
@property (readonly, strong) NSMutableArray *instruments;
@property (readonly, strong) NSMutableArray *patterns;
@property (readonly, strong) NSMutableArray *buses;
@property (readonly) OSType madType;
@property (readwrite, strong, nonatomic) NSString *internalFileName; //This is actually copied, but I don't want Clang to complain
@property (readwrite, strong, nonatomic) NSString *madInfo; //Ditto
@property (copy) NSString *madAuthor;

//Use to create a blank music object.
- (instancetype)init;

//Import from another PPMusicObject
- (instancetype)initFromMusicObject:(PPMusicObject*)oldFormat;

//Creates a MADK tracker file
- (OSErr)exportMusicToURL:(NSURL *)tosave;

//Load a MAD bundle from a path
- (instancetype)initWithPath:(NSString *)url;

//Load a MAD bundle from a URL
- (instancetype)initWithURL:(NSURL *)url;

- (OSErr)createCopyMusicToURL:(NSURL *)tosave;

+ (OSErr)info:(PPInfoRec *)theInfo fromTrackerAtURL:(NSURL *)thURL;

#pragma mark Document-based code
//For use with document classes, like NSDocument or UIDocument
//Load a MAD bundle from a file wrapper.
- (instancetype)initWithFileWrapper:(NSFileWrapper*)wrapper;

//The file wrapper
@property (strong, readonly) NSFileWrapper* musicWrapper;

@end
