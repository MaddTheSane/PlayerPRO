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
@property (readonly) int totalPartitions;
@property (readonly) int partitionLength;
@property (readonly) short totalTracks;
@property (readonly) short totalInstruments;

@property (readonly, weak) PPDriver *attachedDriver;
@property (readonly) NSURL *filePath;

- (id)init;
- (id)initWithURL:(NSURL *)url library:(PPLibrary *)theLib;
- (id)initWithPath:(NSString *)url library:(PPLibrary *)theLib;

- (id)initWithURL:(NSURL *)url driver:(PPDriver *)theLib;
- (id)initWithPath:(NSString *)url driver:(PPDriver *)theLib;
- (id)initWithURL:(NSURL *)url driver:(PPDriver *)theLib setAsCurrentMusic:(BOOL)toSet;
- (id)initWithPath:(NSString *)url driver:(PPDriver *)theLib setAsCurrentMusic:(BOOL)toSet;

//This method does not set the music object as the playback music
- (void)attachToDriver:(PPDriver *)theDriv;
//This method does set the music object as the playback music
- (void)attachToDriver:(PPDriver *)theDriv setAsCurrentMusic:(BOOL)toSet;


@end

@interface PPMusicObjectWrapper : PPMusicObject

- (id)init;
- (id)initFromMusicObject:(PPMusicObject*)oldFromat;
- (id)initWithPath:(NSString *)url;
- (id)initWithURL:(NSURL *)url;
@property (strong) NSArray *sDatas;
@property (strong) NSArray *instruments;


@end
