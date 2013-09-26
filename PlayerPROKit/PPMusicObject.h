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

//Use to create a blank music object. NOT recommended for PPMusicObject
- (id)init;

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
@property (readwrite, strong, nonatomic) NSString *internalFileName;
@property (readwrite, strong, nonatomic) NSString *madInfo;

//Use to create a blank music object. Use on PPMusicObjectWrapper is Okay
- (id)init;

//Import from another PPMusicObject
- (id)initFromMusicObject:(PPMusicObject*)oldFromat;

//Creates a MADK tracker file
- (OSErr)exportMusicToURL:(NSURL *)tosave;

//Load a MAD bundle from a path
- (id)initWithPath:(NSString *)url;

//Load a MAD bundle from a URL
- (id)initWithURL:(NSURL *)url;

//Load a MAD bundle from a file wrapper. Not recommended to use it outside of NSDocument-based apps.
- (id)initWithFileWrapper:(NSFileWrapper*)wrapper;

//creates a file wrapper for use with an NSDocument class
- (NSFileWrapper*)musicWrapper;

//Creates a music struct for use outside of PlayerPROKit.
- (MADMusic *)newMadMusicStruct;

@end
