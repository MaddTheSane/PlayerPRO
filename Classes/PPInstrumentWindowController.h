//
//  PPInstrumentWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 12/31/12.
//
//

#import <Cocoa/Cocoa.h>
#include <PlayerPROCore/PlayerPROCore.h>

@class PPInstrumentImporter;

@interface PPInstrumentWindowController : NSWindowController <NSOutlineViewDataSource, NSOutlineViewDelegate>
{
	PPInstrumentImporter *importer;
	MADMusic **curMusic;
	NSMutableArray *instruments;
	IBOutlet __weak NSTextField *instrumentSize;
	IBOutlet __weak NSTextField *instrumentLoopStart;
	IBOutlet __weak NSTextField *instrumentLoopSize;
	IBOutlet __weak NSTextField *instrumentVolume;
	IBOutlet __weak NSTextField *instrumentRate;
	IBOutlet __weak NSTextField *instrumentNote;
	IBOutlet __weak NSTextField *instrumentBits;
	IBOutlet __weak NSTextField *instrumentMode;

	IBOutlet __weak NSImageView *waveFormImage;
	IBOutlet __weak NSOutlineView *instrumentView;
}

@property (readwrite, retain) PPInstrumentImporter *importer;
@property (readwrite, nonatomic) MADMusic **curMusic;

- (void)writeInstrumentsBackToMusic;
- (void)writeInstrumentAtIndexBackToMusic:(short)idx;
- (void)writeSampleAtIndex:(short)sampIdx withInstrumentAtIndexBackToMusic:(short)insIdx;

- (IBAction)exportInstrument:(id)sender;
- (IBAction)deleteInstrument:(id)sender;
- (IBAction)deleteSample:(id)sender;

@end
