//
//  PPDriver.h
//  PPMacho
//
//  Created by C.W. Betts on 11/28/12.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

@class PPLibrary;
@class PPMusicObject;

@interface PPDriver : NSObject
{
	MADDriverRec *theRec;
	PPLibrary *thePPLib;
	PPMusicObject *currentMusic;
}

@property (readwrite, nonatomic, retain) PPMusicObject *currentMusic;
@property (readonly) PPLibrary *theLibrary;

- (id)initWithLibrary:(PPLibrary *)theLib;
- (id)initWithLibrary:(PPLibrary *)theLib settings:(MADDriverSettings *)theSettings;

- (OSErr)loadMusicFile:(NSString *)path;
- (OSErr)loadMusicURL:(NSURL*)url;

@end
