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
	IBOutlet NSImageView *waveFormImage;
	IBOutlet NSOutlineView *instrumentView;
}

@property (readwrite, retain) PPInstrumentImporter *importer;
@property (readwrite, nonatomic) MADMusic **curMusic;

- (IBAction)exportInstrument:(id)sender;
- (IBAction)deleteInstrument:(id)sender;
- (IBAction)deleteSample:(id)sender;

@end
