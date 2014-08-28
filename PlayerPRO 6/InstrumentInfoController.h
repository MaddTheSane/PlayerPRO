//
//  InstrumentInfoController.h
//  PPMacho
//
//  Created by C.W. Betts on 2/9/13.
//
//

#import <Cocoa/Cocoa.h>

@class PPInstrumentObject;
@class InstrumentViewController;
@class PPSmallPianoView;

@interface InstrumentInfoController : NSWindowController <NSTableViewDataSource, NSTableViewDelegate>
@property (weak) IBOutlet PPSmallPianoView *pianoView;
@property (weak) IBOutlet NSTextField *instrumentNumber;
@property (weak) IBOutlet NSTextField *instrumentName;
@property (weak) IBOutlet NSMatrix *panningMatrix;
@property (weak) IBOutlet NSMatrix *volumeMatrix;
	
@property (weak) IBOutlet NSButton *volumeFixedSpeedButton;
@property (weak) IBOutlet NSButton *volumeSustainButton;
@property (weak) IBOutlet NSButton *volumeLoopButton;
	
@property (weak) IBOutlet NSButton *panningLoopButton;
@property (weak) IBOutlet NSButton *panningFixedSpeedButton;
@property (weak) IBOutlet NSButton *panningSustainButton;

@property (weak) IBOutlet NSTableView *sampleTable;
@property (copy, nonatomic) PPInstrumentObject *instrument;
@property (weak) InstrumentViewController *delegate;

- (IBAction)okayButton:(id)sender;
- (IBAction)cancelButton:(id)sender;
- (IBAction)volumeEditButtonPressed:(id)sender;
- (IBAction)volumeCheckClicked:(id)sender;

- (IBAction)panningCheckClicked:(id)sender;
- (IBAction)panningEditButtonPressed:(id)sender;

- (void)instrumentSheetDidEnd:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo;

@end
