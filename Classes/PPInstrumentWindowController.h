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
	IBOutlet NSImageView *waveFormImage;
	IBOutlet NSTreeController *instrumentTreeController;
}

@property (readwrite, retain) PPInstrumentImporter *importer;
@property (readwrite) MADMusic **curMusic;

- (IBAction)exportInstrument:(id)sender;
- (IBAction)deleteInstrument:(id)sender;

@end
