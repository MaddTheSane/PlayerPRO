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
}

- (OSErr)testInstrumentFile:(NSURL *)toTest;
- (BOOL)isPlugAvailable:(OSType)kind type:(OSType*)theType;
- (OSErr)identifyInstrumentFile:(NSURL*)ref type:(OSType*)outType;
@end
