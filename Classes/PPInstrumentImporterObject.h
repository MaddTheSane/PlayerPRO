//
//  PPInstrumentImporterObject.h
//  PPMacho
//
//  Created by C.W. Betts on 12/27/12.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

@interface PPInstrumentImporterObject : NSObject
{
	void				**xxxx;
	NSString			*menuName;
	NSString			*authorString;
	NSArray				*UTITypes;
	NSBundle			*file;
	OSType				type;
	OSType				mode;
	BOOL				isSamp;
	UInt32				version;
}

@property (readonly) NSString *menuName;
@property (readonly) NSString *authorString;
@property (readonly) NSArray *UTITypes;
@property (readonly) NSBundle *file;
@property (readonly) OSType type;
@property (readonly) OSType mode;
@property (readonly) BOOL isSamp;
@property (readonly) UInt32 version;

- (id)initWithBundle:(NSBundle *)theBund;
- (OSErr)importInstrument:(NSURL *)fileToImport instrumentDataReference:(InstrData*)insData sampleDataReference:(sData**)sdataref instrumentSample:(short*)insNum function:(OSType)imporexp;
- (OSErr)importInstrument:(NSURL *)fileToImport instrumentDataReference:(InstrData*)insData sampleDataReference:(sData**)sdataref instrumentSample:(short*)insSamp function:(OSType)imporexp plugInfo:(PPInfoPlug*)plugInfo;

@end
