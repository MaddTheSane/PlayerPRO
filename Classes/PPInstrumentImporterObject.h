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
	@private
	PPInstrumentPlugin	**xxxx;
}

@property (readonly, copy) NSArray *UTITypes;
@property (readonly) OSType mode;
@property (readonly) BOOL isSample;

- (id)initWithBundle:(NSBundle *)theBund;
- (OSErr)importInstrument:(NSURL *)fileToImport instrumentDataReference:(InstrData*)insData sampleDataReference:(sData**)sdataref instrumentSample:(short*)insNum function:(OSType)imporexp UNAVAILABLE_ATTRIBUTE;
- (OSErr)importInstrument:(NSURL *)fileToImport instrumentDataReference:(InstrData*)insData sampleDataReference:(sData**)sdataref instrumentSample:(short*)insSamp function:(OSType)imporexp plugInfo:(PPInfoPlug*)plugInfo;

@end
