//
//  PPInstrumentPlugHandler.h
//  PPMacho
//
//  Created by C.W. Betts on 9/7/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>

@class PPInstrumentImporterObject;

@interface PPInstrumentPlugHandler : NSObject <NSFastEnumeration>

- (NSInteger)plugInCount;
- (PPInstrumentImporterObject*)plugInAtIndex:(NSUInteger)idx;

- (void)addPlugInFromPath:(NSString*)thePath;
- (void)addPlugInFromURL:(NSURL *)urlpath;
- (void)addPlugInFromBundle:(NSBundle *)theBund;

- (OSErr)exportInstrumentOfType:(OSType)theType instrumentReference:(InstrData*)ins sampleReference:(sData**)sampRef sample:(short*)samp URL:(NSURL*)theURL plugInfo:(PPInfoPlug *)plugInfo;
- (OSErr)importInstrumentOfType:(OSType)theType instrumentReference:(InstrData*)ins sampleReference:(sData**)sampRef sample:(short*)samp URL:(NSURL*)theURL plugInfo:(PPInfoPlug *)plugInfo;
- (OSErr)testInstrumentFile:(NSURL *)toTest type:(OSType)theType;
- (BOOL)isPlugAvailable:(OSType)kind type:(OSType*)theType;
- (OSErr)identifyInstrumentFile:(NSURL*)ref type:(OSType*)outType;

@end
