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

@interface PPInstrumentWindowController : NSWindowController
{
	PPInstrumentImporter *importer;
	MADMusic **curMusic;
	IBOutlet NSImageView *waveFormImage;
}

@property (readwrite, retain) PPInstrumentImporter *importer;
@property (readwrite) MADMusic **curMusic;

- (IBAction)exportInstrument:(id)sender;

@end
