//
//  InstrumentInfoController.h
//  PPMacho
//
//  Created by C.W. Betts on 2/9/13.
//
//

#import <Cocoa/Cocoa.h>
#import "ARCBridge.h"

@class PPInstrumentObject;
@class PPInstrumentWindowController;

@interface InstrumentInfoController : NSWindowController <NSTableViewDataSource, NSTableViewDelegate>
{
	PPInstrumentObject *instrument;
	__arcweak PPInstrumentWindowController *delegate;
	IBOutlet NSView *pianoView;
	IBOutlet NSTextField *instrumentNumber;
	IBOutlet NSTextField *instrumentName;
	IBOutlet NSMatrix *panningMatrix;
	IBOutlet NSMatrix *volumeMatrix;
	
	IBOutlet NSButton *volumeFixedSpeedButton;
	IBOutlet NSButton *volumeSustainButton;
	IBOutlet NSButton *volumeLoopButton;
	
	IBOutlet NSButton *panningLoopButton;
	IBOutlet NSButton *panningFixedSpeedButton;
	IBOutlet NSButton *panningSustainButton;

	IBOutlet NSTableView *sampleTable;
}

@property (readwrite, copy, nonatomic) PPInstrumentObject *instrument;
@property (readwrite, arcweak) PPInstrumentWindowController *delegate;
- (IBAction)okayButton:(id)sender;
- (IBAction)cancelButton:(id)sender;
- (IBAction)volumeEditButtonPressed:(id)sender;
- (IBAction)volumeCheckClicked:(id)sender;

- (IBAction)panningCheckClicked:(id)sender;
- (IBAction)panningEditButtonPressed:(id)sender;

@end
