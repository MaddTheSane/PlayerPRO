//
//  PPInstrumentImporterObject.h
//  PPMacho
//
//  Created by C.W. Betts on 12/27/12.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>
#import "PPPlugInObject.h"

@interface PPInstrumentImporterObject : PPPlugInObject
{
	PPInstrumentPlugin	**xxxx;
	NSArray				*UTITypes;
	OSType				mode;
	BOOL				isSamp;
}

@property (readonly) NSArray *UTITypes;
@property (readonly) OSType mode;
@property (readonly) BOOL isSamp;

- (id)initWithBundle:(NSBundle *)theBund;
- (OSErr)importInstrument:(NSURL *)fileToImport instrumentDataReference:(InstrData*)insData sampleDataReference:(sData**)sdataref instrumentSample:(short*)insNum function:(OSType)imporexp;
- (OSErr)importInstrument:(NSURL *)fileToImport instrumentDataReference:(InstrData*)insData sampleDataReference:(sData**)sdataref instrumentSample:(short*)insSamp function:(OSType)imporexp plugInfo:(PPInfoPlug*)plugInfo;

@end
