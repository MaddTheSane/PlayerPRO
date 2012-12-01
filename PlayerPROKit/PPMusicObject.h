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
	MADMusic *currentMusic;
}
- (id)init;
//- (id)initWithURL:(NSURL *)url library:(PPLibrary *)theLib;
//- (id)initWithPath:(NSString *)url library:(PPLibrary *)theLib;
- (id)initWithURL:(NSURL *)url driver:(PPDriver *)theLib;
- (id)initWithPath:(NSString *)url driver:(PPDriver *)theLib;
- (id)initWithURL:(NSURL *)url driver:(PPDriver *)theLib setAsCurrentMusic:(BOOL)toSet;
- (id)initWithPath:(NSString *)url driver:(PPDriver *)theLib setAsCurrentMusic:(BOOL)toSet;

- (void)attachToDriver:(PPDriver *)theDriv;

@end
