//
//  PPPasteboardHandling.h
//  PPMacho
//
//  Created by C.W. Betts on 8/12/14.
//
//

#ifndef __PLAYERPROKIT_PPPASTEBOARDHANDLING_H__
#define __PLAYERPROKIT_PPPASTEBOARDHANDLING_H__

#import <AppKit/NSPasteboard.h>

#import <PlayerPROKit/PPSampleObject.h>
#import <PlayerPROKit/PPInstrumentObject.h>
#import <PlayerPROKit/PPPatternObject.h>
#import <PlayerPROKit/PPMadCommandObject.h>
#import <PlayerPROKit/PPErrors.h>
#import <PlayerPROKit/PPFXBusObject.h>
#import <PlayerPROKit/PPFXSetObject.h>

NS_ASSUME_NONNULL_BEGIN

extern NSPasteboardType const kPPKSamplePasteboardUTI;
extern NSPasteboardType const kPPKMADCommandPasteboardUTI;
extern NSPasteboardType const kPPKPatternPasteboardUTI;
extern NSPasteboardType const kPPKFXBusPasteboardUTI;
extern NSPasteboardType const kPPKFXSetPasteboardUTI;
extern NSPasteboardType const kPPKEnvelopePasteboardUTI;
extern NSPasteboardType const kPPKInstrumentPasteboardUTI;

NS_ASSUME_NONNULL_END

@interface PPSampleObject() <NSPasteboardReading, NSPasteboardWriting>
@end

@interface PPMadCommandObject() <NSPasteboardReading, NSPasteboardWriting>
@end

@interface PPPatternObject() <NSPasteboardReading, NSPasteboardWriting>
@end

@interface PPFXBusObject() <NSPasteboardReading, NSPasteboardWriting>
@end

@interface PPFXSetObject() <NSPasteboardReading, NSPasteboardWriting>
@end

@interface PPEnvelopeObject() <NSPasteboardReading, NSPasteboardWriting>
@end

@interface PPInstrumentObject() <NSPasteboardReading, NSPasteboardWriting>
@end

#endif
