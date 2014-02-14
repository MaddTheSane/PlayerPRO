//
//  SampleCellView.h
//  PPMacho
//
//  Created by C.W. Betts on 2/15/13.
//
//

#import <Cocoa/Cocoa.h>

@interface SampleCellView : NSTableCellView <NSComboBoxDataSource, NSComboBoxDelegate>
@property (weak) IBOutlet NSPopUpButton *modeButton;
@property (weak) IBOutlet NSPopUpButton *ampButton;
@property (weak) IBOutlet NSComboBox *rateBox;
@property (weak) IBOutlet NSTextField *volumeField;
@property (weak) IBOutlet NSTextField *nameField;
@property (weak) IBOutlet NSTextField *loopStartField;
@property (weak) IBOutlet NSTextField *loopSizeField;
@property (weak) IBOutlet NSPopUpButton *loopTypeButton;
@property (weak) IBOutlet NSTextField *relNoteField;
@property (weak) IBOutlet NSTextField *relNoteLabel;
@property (weak) IBOutlet NSTextField *instrumentIndexLabel;

- (IBAction)modeChanged:(id)sender;
- (IBAction)amplitudeChanged:(id)sender;
- (IBAction)rateChanged:(id)sender;
- (IBAction)loopTypeChanged:(id)sender;

@end
