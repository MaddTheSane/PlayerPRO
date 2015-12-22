//
//  PPInstrumentObject_PPKPrivate.h
//  PPMacho
//
//  Created by C.W. Betts on 1/8/14.
//
//

#import <PlayerPROKit/PPInstrumentObject.h>
#import <PlayerPROKit/PPMusicObject.h>

@interface PPEnvelopeObject ()
- (instancetype)initWithEnvRecPointer:(EnvRec*)theEnv;
@end

@interface PPInstrumentObject ()
{
	NSMutableArray<PPSampleObject*>	*samples;
}
- (instancetype)initWithMusic:(PPMusicObject *)mus;
- (instancetype)initWithMusicStruct:(MADMusic *)theMus atIndex:(NSInteger)ind;

@property InstrData theInstrument;
@property (readwrite) NSInteger number;
@end
