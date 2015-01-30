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

#ifndef __private_extern
#define __private_extern __attribute__((visibility("hidden")))
#endif

#if __LP64__
__private_extern NSData *PPInstrumentToData(PPInstrumentObject *ourData) NS_RETURNS_RETAINED;
__private_extern NSData *PPSampleToData(PPSampleObject *sampObj) NS_RETURNS_RETAINED;

__private_extern PPInstrumentObject *PPDataToInstrument(NSData *ourData) NS_RETURNS_RETAINED;
__private_extern PPSampleObject *PPDataToSample(NSData *ourData) NS_RETURNS_RETAINED;
#endif

__private_extern NSData *MADInstrumentToData(InstrData* insData, sData ** sampleData) NS_RETURNS_RETAINED;
__private_extern NSData *MADSampleToData(sData * sampleData) NS_RETURNS_RETAINED;

__private_extern InstrData *MADDataToInstrument(NSData *ourData, sData ***sampleData);
__private_extern sData *MADDataToSample(NSData *ourData);
