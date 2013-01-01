//
//  PPInstrumentWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 12/31/12.
//
//

#import <Cocoa/Cocoa.h>

@class PPInstrumentImporter;

@interface PPInstrumentWindowController : NSWindowController
{
	PPInstrumentImporter *importer;
}

@property (readwrite, retain) PPInstrumentImporter *importer;

- (IBAction)exportInstrument:(id)sender;

@end
