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
