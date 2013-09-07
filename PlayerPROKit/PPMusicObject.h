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
{
	PPDriver *attachedDriver;
}
@property (readonly) PPDriver *attachedDriver;
//The following three functions must be attached to a driver before deallocating or memory will leak!
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
