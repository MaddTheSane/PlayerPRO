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

@property (weak, nullable) IBOutlet NSTextField *numField;
@property (weak, nullable) IBOutlet NSButton *sampleButton;
@property (weak, nullable) InstrumentPanelController *controller;
@property (nonatomic, getter=isSample) BOOL sample;
@property (nonatomic, getter=isLoopingSample) BOOL loopingSample;
@property (nonatomic, getter=isBlank) BOOL blank;

- (IBAction)playSample:(nullable id)sender;

@end
