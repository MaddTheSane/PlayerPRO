//
//  PPPasteboardHandling.h
//  PPMacho
//
//  Created by C.W. Betts on 8/12/14.
//
//

#import <AppKit/NSPasteboard.h>

#import <PlayerPROKit/PPSampleObject.h>
#import <PlayerPROKit/PPInstrumentObject.h>
#import <PlayerPROKit/PPPatternObject.h>
#import <PlayerPROKit/PPMadCommandObject.h>
#import <PlayerPROKit/PPErrors.h>
#import <PlayerPROKit/PPFXBusObject.h>
#import <PlayerPROKit/PPFXSetObject.h>

extern NSString * const kPPKSamplePasteboardUTI;
extern NSString * const kPPKMADCommandPasteboardUTI;
extern NSString * const kPPKPatternPasteboardUTI;
extern NSString * const kPPKFXBusPasteboardUTI;
extern NSString * const kPPKFXSetPasteboardUTI;
extern NSString * const kPPKEnvelopePasteboardUTI;
extern NSString * const kPPKInstrumentPasteboardUTI;

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
