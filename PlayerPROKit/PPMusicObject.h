//
//  PPMusicObject.h
//  PPMacho
//
//  Created by C.W. Betts on 12/1/12.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

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
- (OSErr)exportMusicToURL:(NSURL *)tosave format:(NSString*)form library:(PPLibrary*)otherLib;

//Creates a music object from the supplied MADK (PPMusicObject) or MAD bundle (PPMusicObjectWrapper)
- (id)initWithURL:(NSURL *)url;
- (id)initWithPath:(NSString *)url;

//Creates a music object from any supported tracker type.
- (id)initWithURL:(NSURL *)url library:(PPLibrary *)theLib;
- (id)initWithPath:(NSString *)url library:(PPLibrary *)theLib;

//Creates a music object from any supported tracker type, also attaching a driver to the music.
- (id)initWithURL:(NSURL *)url driver:(PPDriver *)theLib;
- (id)initWithPath:(NSString *)url driver:(PPDriver *)theLib;

//This method sets the music object as the playback music
- (void)attachToDriver:(PPDriver *)theDriv;

@end

@interface PPMusicObjectWrapper : PPMusicObject <NSCopying>
@property (strong) NSMutableArray *sDatas;
@property (strong) NSMutableArray *instruments;
@property (readonly) OSType madType;
@property (readwrite, strong, nonatomic) NSString *internalFileName; //This is actually copied, but I don't want Clang to complain
@property (readwrite, strong, nonatomic) NSString *madInfo; //Ditto

//Use to create a blank music object.
- (id)init;

//Import from another PPMusicObject
- (id)initFromMusicObject:(PPMusicObject*)oldFormat;

//Creates a MADK tracker file
- (OSErr)exportMusicToURL:(NSURL *)tosave;

//Load a MAD bundle from a path
- (id)initWithPath:(NSString *)url;

//Load a MAD bundle from a URL
- (id)initWithURL:(NSURL *)url;

//Creates a music struct for use outside of PlayerPROKit.
- (MADMusic *)newMadMusicStruct;

+ (PPInfoRec)infoFromTrackerAtURL:(NSURL*)thURL;

#pragma mark Document-based code
//For use with document classes, like NSDocument or UIDocument
//Load a MAD bundle from a file wrapper.
- (id)initWithFileWrapper:(NSFileWrapper*)wrapper;

//The file wrapper
@property (strong, readonly) NSFileWrapper* musicWrapper;

@end
