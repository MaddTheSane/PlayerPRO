//
//  PPFilterPlugHandler.h
//  PPMacho
//
//  Created by C.W. Betts on 1/22/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
@class PPFilterPlugObject;

@interface PPFilterPlugHandler : NSObject <NSFastEnumeration>
- (id)init;
- (PPFilterPlugObject*)plugInAtIndex:(NSUInteger)idx;
- (NSUInteger)plugInCount;

- (OSErr)callDigitalPlugAtIndex:(NSInteger)idx sampleData:(sData*)theInsData startLength:(long)start endLength:(long)end stereoMode:(short)stereo info:(PPInfoPlug *)theInfo;

- (void)addPlugInFromPath:(NSString*)thePath;
- (void)addPlugInFromURL:(NSURL *)urlpath;
- (void)addPlugInFromBundle:(NSBundle *)theBund;
@end
