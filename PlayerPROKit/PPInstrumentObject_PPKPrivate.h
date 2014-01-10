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
- (instancetype)initWithMusic:(PPMusicObjectWrapper*)mus;

@property InstrData theInstrument;
@property (readwrite) NSInteger number;
@end
