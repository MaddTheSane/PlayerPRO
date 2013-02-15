//
//  PPInstrumentImporter.h
//  PPMacho
//
//  Created by C.W. Betts on 12/26/12.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>

@class PPInstrumentImporterObject;

@interface PPInstrumentImporter : NSObject
{
	NSMutableArray *instrumentIEArray;
	MADMusic **curMusic;
	MADDriverRec **driverRec;
	PPInfoPlug plugInfo;
}

@property (readwrite, nonatomic) MADDriverRec **driverRec;

- (id)initWithMusic:(MADMusic**)theMus;

- (NSInteger)plugInCount;
- (PPInstrumentImporterObject*)plugInAtIndex:(NSUInteger)idx;

- (void)addPlugInFromPath:(NSString*)thePath;
- (void)addPlugInFromURL:(NSURL *)urlpath;
- (void)addPlugInFromBundle:(NSBundle *)theBund;

- (OSErr)exportInstrumentOfType:(OSType)theType instrument:(short)ins sample:(short*)samp URL:(NSURL*)theURL;
- (OSErr)importInstrumentOfType:(OSType)theType instrument:(short)ins sample:(short*)samp URL:(NSURL*)theURL;
- (OSErr)testInstrumentFile:(NSURL *)toTest type:(OSType)theType;
- (BOOL)isPlugAvailable:(OSType)kind type:(OSType*)theType;
- (OSErr)identifyInstrumentFile:(NSURL*)ref type:(OSType*)outType;
@end
