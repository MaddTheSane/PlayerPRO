//
//  PPDigitalPlugInHandler.h
//  PPMacho
//
//  Created by C.W. Betts on 1/1/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>
@class PPDigitalPlugInObject;

@interface PPDigitalPlugHandler : NSObject <NSFastEnumeration>
- (instancetype)init NS_DESIGNATED_INITIALIZER;

- (MADErr)callDigitalPlugIn:(NSUInteger)plugNum pcmd:(Pcmd*)myPcmd plugInfo:(PPInfoPlug *)theInfo;

@property (readonly) NSArray *plugInArray;
- (PPDigitalPlugInObject*)plugInAtIndex:(NSUInteger)idx;
- (NSUInteger)plugInCount;

- (void)addPlugInFromPath:(NSString*)thePath;
- (void)addPlugInFromURL:(NSURL *)urlpath;
- (void)addPlugInFromBundle:(NSBundle *)theBund;
@end
