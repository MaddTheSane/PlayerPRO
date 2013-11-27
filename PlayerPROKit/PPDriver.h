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
@property (readwrite, nonatomic, strong) PPMusicObject *currentMusic;
@property (readonly) PPLibrary *theLibrary;

- (id)initWithLibrary:(PPLibrary *)theLib;
- (id)initWithLibrary:(PPLibrary *)theLib settings:(MADDriverSettings *)theSettings;

@property (readonly) MADDriverSettings driverSettings;
- (OSErr)changeDriverSettingsToSettings:(MADDriverSettings)theSett;

- (void)beginExport;
- (void)endExport;
- (BOOL)isExporting;
	
- (void)cleanDriver;
	
- (BOOL)directSaveToPointer:(void*)thePtr settings:(MADDriverSettings*)theSett;
- (NSInteger)audioLength;
	
- (OSErr)play;
- (OSErr)pause;
- (OSErr)stop;
	
- (PPMusicObject *)loadMusicFile:(NSString *)path;
- (PPMusicObject *)loadMusicURL:(NSURL*)url;

@end
