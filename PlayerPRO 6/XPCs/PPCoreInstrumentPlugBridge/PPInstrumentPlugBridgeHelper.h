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
PPInstrumentObject *PPDataToInstrument(NSData *ourData) NS_RETURNS_RETAINED;
#endif

NSData *InstrumentToData(InstrData* insData, sData ** sampleData) NS_RETURNS_RETAINED;
InstrData *DataToInstrument(NSData *ourData, sData ***sampleData);
