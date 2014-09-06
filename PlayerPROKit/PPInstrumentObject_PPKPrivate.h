//
//  PPInstrumentObject_PPKPrivate.h
//  PPMacho
//
//  Created by C.W. Betts on 1/8/14.
//
//

#import "PPInstrumentObject.h"
#import "PPMusicObject.h"

@interface PPEnvelopeObject ()
- (instancetype)initWithEnvRecPointer:(EnvRec*)theEnv;
@end

@interface PPInstrumentObject ()
{
	NSMutableArray	*samples;
}
- (instancetype)initWithMusic:(PPMusicObject *)mus;
- (instancetype)initWithMusicStruct:(MADMusic *)theMus atIndex:(NSInteger)ind;

@property InstrData theInstrument;
@property (readwrite) NSInteger number;
@end
