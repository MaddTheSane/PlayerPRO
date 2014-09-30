//
//  PPInstrumentCellView.h
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import <Cocoa/Cocoa.h>

//@class PPSampleButton;
@class InstrumentPanelController;

@interface PPInstrumentCellView : NSTableCellView
{
	IBOutlet NSImageView *loopImage;
	BOOL isSample;
	BOOL isLoopingSample;
	BOOL isBlank;
}

@property (weak) IBOutlet NSTextField *numField;
@property (weak) IBOutlet NSButton *sampleButton;
@property (weak) InstrumentPanelController *controller;
@property (nonatomic, getter=isSample) BOOL sample;
@property (nonatomic, getter=isLoopingSample) BOOL loopingSample;
@property (nonatomic, getter=isBlank) BOOL blank;

- (IBAction)playSample:(id)sender;

@end
