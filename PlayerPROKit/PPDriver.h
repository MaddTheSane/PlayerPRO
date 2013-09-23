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
	PPMusicObject *currentMusic;
}

@property (readwrite, nonatomic, strong) PPMusicObject *currentMusic;
@property (readonly) PPLibrary *theLibrary;

- (id)initWithLibrary:(PPLibrary *)theLib;
- (id)initWithLibrary:(PPLibrary *)theLib settings:(MADDriverSettings *)theSettings;

- (void)beginExport;
- (void)endExport;
	
- (void)cleanDriver;
	
- (BOOL)directSaveToPointer:(void*)thePtr settings:(MADDriverSettings*)theSett;
- (NSInteger)audioLength;
	
- (OSErr)play;
	
- (void)loadMusicFile:(NSString *)path;
- (void)loadMusicURL:(NSURL*)url;

@end
