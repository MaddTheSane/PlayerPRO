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

NS_ASSUME_NONNULL_BEGIN

#if !__i386__
__private_extern NSData * PPInstrumentToData(PPInstrumentObject *ourData) NS_RETURNS_RETAINED;
__private_extern NSData *PPSampleToData(PPSampleObject *sampObj) NS_RETURNS_RETAINED;

__private_extern PPInstrumentObject *PPDataToInstrument(NSData *ourData) NS_RETURNS_RETAINED;
__private_extern PPSampleObject *PPDataToSample(NSData *ourData) NS_RETURNS_RETAINED;
#endif

__private_extern NSData *MADInstrumentToData(InstrData* insData, sData * __nonnull* __nonnull sampleData) NS_RETURNS_RETAINED;
__private_extern NSData *MADSampleToData(sData * sampleData) NS_RETURNS_RETAINED;

__private_extern InstrData *MADDataToInstrument(NSData *ourData, sData *__nonnull *__nonnull * __nonnull sampleData);
__private_extern sData * __nullable MADDataToSample(NSData *ourData);

NS_ASSUME_NONNULL_END

