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

@property (weak) IBOutlet NSTextField *numField;
@property (weak) IBOutlet NSButton *sampleButton;
@property (weak) PPInstrumentViewController *controller;
@property (nonatomic) BOOL isSample;
@property (nonatomic) BOOL isLoopingSample;
@property (nonatomic) BOOL isBlank;

- (IBAction)playSample:(id)sender;

@end
