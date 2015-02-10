//
//  PPInstrumentPlugHandler.h
//  PPMacho
//
//  Created by C.W. Betts on 9/7/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

@class PPInstrumentImporterObject;

@interface PPInstrumentPlugHandler : NSObject <NSFastEnumeration>
- (instancetype)init NS_DESIGNATED_INITIALIZER;

@property (readonly) NSArray *plugInArray;
@property (readonly) NSInteger plugInCount;
- (PPInstrumentImporterObject*)plugInAtIndex:(NSUInteger)idx;
- (PPInstrumentImporterObject *)objectAtIndexedSubscript:(NSInteger)index;

- (void)addPlugInFromPath:(NSString*)thePath;
- (void)addPlugInFromURL:(NSURL *)urlpath;
- (void)addPlugInFromBundle:(NSBundle *)theBund;

- (MADErr)exportInstrumentOfType:(OSType)theType instrumentReference:(InstrData*)ins sampleReference:(sData**)sampRef sample:(short*)samp URL:(NSURL*)theURL plugInfo:(PPInfoPlug *)plugInfo;
- (MADErr)importInstrumentOfType:(OSType)theType instrumentReference:(InstrData*)ins sampleReference:(sData**)sampRef sample:(short*)samp URL:(NSURL*)theURL plugInfo:(PPInfoPlug *)plugInfo;
- (MADErr)testInstrumentFile:(NSURL *)toTest type:(OSType)theType;
- (BOOL)isPlugAvailable:(OSType)kind;
- (MADErr)identifyInstrumentFile:(NSURL*)ref type:(OSType*)outType;

@end
