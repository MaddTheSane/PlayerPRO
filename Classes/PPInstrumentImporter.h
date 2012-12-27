//
//  PPInstrumentImporter.h
//  PPMacho
//
//  Created by C.W. Betts on 12/26/12.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

@interface PPInstrumentImporter : NSObject
{
	NSMutableArray *instrumentIEArray;
	MADMusic **curMusic;
}
- (id)initWithMusic:(MADMusic**)theMus;

- (NSInteger)plugInCount;

- (OSErr)exportInstrumentOfType:(OSType)theType instrument:(short)ins sample:(short*)samp URL:(NSURL*)theURL;
- (OSErr)importInstrumentOfType:(OSType)theType instrument:(short)ins sample:(short*)samp URL:(NSURL*)theURL;
- (OSErr)testInstrumentFile:(NSURL *)toTest type:(OSType)theType;
- (BOOL)isPlugAvailable:(OSType)kind type:(OSType*)theType;
- (OSErr)identifyInstrumentFile:(NSURL*)ref type:(OSType*)outType;
@end
