//
//  PPInstrumentCellView.h
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import <Cocoa/Cocoa.h>

//@class PPSampleButton;
@class PPInstrumentViewController;

@interface PPInstrumentCellView : NSTableCellView
{
	IBOutlet NSImageView *loopImage;
	BOOL isSample;
	BOOL isLoopingSample;
	BOOL isBlank;
}

@property (readwrite, weak) IBOutlet NSTextField *numField;
@property (readwrite, weak) IBOutlet NSButton *sampleButton;
@property (readwrite, weak) PPInstrumentViewController *controller;
@property (readwrite, nonatomic) BOOL isSample;
@property (readwrite, nonatomic) BOOL isLoopingSample;
@property (readwrite, nonatomic) BOOL isBlank;

- (IBAction)playSample:(id)sender;

@end
