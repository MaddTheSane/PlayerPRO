//
//  SampleCellView.h
//  PPMacho
//
//  Created by C.W. Betts on 2/15/13.
//
//

#import <Cocoa/Cocoa.h>

@interface SampleCellView : NSTableCellView <NSComboBoxDataSource, NSComboBoxDelegate>
{
	IBOutlet NSPopUpButton *modeButton;
	IBOutlet NSPopUpButton *ampButton;
	IBOutlet NSComboBox *rateBox;
	IBOutlet NSTextField *volumeField;
	IBOutlet NSTextField *nameField;
	IBOutlet NSTextField *loopStartField;
	IBOutlet NSTextField *loopSizeField;
	IBOutlet NSPopUpButton *loopTypeButton;
	IBOutlet NSTextField *realNoteField;
	IBOutlet NSTextField *realNoteLabel;
	IBOutlet NSTextField *instrumentIndexLabel;
}

- (IBAction)modeChanged:(id)sender;
- (IBAction)amplitudeChanged:(id)sender;
- (IBAction)rateChanged:(id)sender;
- (IBAction)loopTypeChanged:(id)sender;

@end
