//
//  PPInstrumentPlugBridgeHelper.h
//  PPMacho
//
//  Created by C.W. Betts on 9/20/14.
//
//

#import <Foundation/Foundation.h>
#import <PlayerPROCore/PlayerPROCore.h>
#if __LP64__
#import <PlayerPROKit/PlayerPROKit.h>
#endif

#if __LP64__
NSData *PPInstrumentToData(PPInstrumentObject *ourData) NS_RETURNS_RETAINED;
NSData *PPSampleToData(PPSampleObject *sampObj) NS_RETURNS_RETAINED;

PPInstrumentObject *PPDataToInstrument(NSData *ourData) NS_RETURNS_RETAINED;
PPSampleObject *PPDataToSample(NSData *ourData) NS_RETURNS_RETAINED;
#endif

NSData *MADInstrumentToData(InstrData* insData, sData ** sampleData) NS_RETURNS_RETAINED;
NSData *MADSampleToData(sData * sampleData) NS_RETURNS_RETAINED;

InstrData *MADDataToInstrument(NSData *ourData, sData ***sampleData);
sData *MADDataToSample(NSData *ourData);
