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

@interface PPFilterPlugController : NSObject 
{
	PPInfoPlug	theInfo;
}

@property (readwrite, nonatomic) MADDriverRec **driverRec;

- (id)initWithMusic:(MADMusic**)theMus;

- (PPFilterPlugObject*)plugInAtIndex:(NSUInteger)idx;
- (NSUInteger)plugInCount;

- (void)addPlugInFromPath:(NSString*)thePath;
- (void)addPlugInFromURL:(NSURL *)urlpath;
- (void)addPlugInFromBundle:(NSBundle *)theBund;

- (OSErr)callDigitalPlugAtIndex:(NSInteger)idx sampleData:(sData*)theInsData startLength:(long)start endLength:(long)end stereoMode:(short)stereo;

@end
