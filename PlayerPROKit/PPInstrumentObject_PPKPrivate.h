//
//  PPInstrumentObject_PPKPrivate.h
//  PPMacho
//
//  Created by C.W. Betts on 1/8/14.
//
//

#import "PPInstrumentObject.h"
#import "PPMusicObject.h"

@interface PPInstrumentObject ()
{
	NSMutableArray	*samples;
}
- (instancetype)initWithMusic:(PPMusicObjectWrapper*)mus;
- (instancetype)initWithMusicStruct:(MADMusic*)theMus atIndex:(NSInteger)ind;

@property InstrData theInstrument;
@property (readwrite) NSInteger number;
@property NSMutableArray *_volumeEnvelope;
@property NSMutableArray *_panningEnvelope;
@property NSMutableArray *_pitchEnvelope;
@end
