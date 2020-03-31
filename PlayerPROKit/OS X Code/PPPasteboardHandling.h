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

extern NSPasteboardType const kPPKSamplePasteboardUTI NS_SWIFT_NAME(PPSampleObject.pasteboardType);
extern NSPasteboardType const kPPKMADCommandPasteboardUTI NS_SWIFT_NAME(PPMadCommandObject.pasteboardType);
extern NSPasteboardType const kPPKPatternPasteboardUTI NS_SWIFT_NAME(PPPatternObject.pasteboardType);
extern NSPasteboardType const kPPKFXBusPasteboardUTI NS_SWIFT_NAME(PPFXBusObject.pasteboardType);
extern NSPasteboardType const kPPKFXSetPasteboardUTI NS_SWIFT_NAME(PPFXSetObject.pasteboardType);
extern NSPasteboardType const kPPKEnvelopePasteboardUTI NS_SWIFT_NAME(PPEnvelopeObject.pasteboardType);
extern NSPasteboardType const kPPKInstrumentPasteboardUTI NS_SWIFT_NAME(PPInstrumentObject.pasteboardType);

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
