//
//  PPInstrumentObject_PPKPrivate.h
//  PPMacho
//
//  Created by C.W. Betts on 1/8/14.
//
//

#import "PPInstrumentObject.h"

@interface PPInstrumentObject ()
{
	NSMutableArray	*samples;
}

@property InstrData theInstrument;
- (id)initWithMusicStruct:(MADMusic*)mus musicObject:(PPMusicObjectWrapper*)musObj instrumentIndex:(short)insIdx;
@end
