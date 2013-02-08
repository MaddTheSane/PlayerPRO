//
//  PPInstrumentCellView.h
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import <Cocoa/Cocoa.h>
#import "ARCBridge.h"

//@class PPSampleButton;
@class PPInstrumentWindowController;

@interface PPInstrumentCellView : NSTableCellView
{
	IBOutlet __weak NSTextField *numField;
	IBOutlet __weak NSButton *sampleButton;
	IBOutlet __weak NSImageView *loopImage;
	__weak PPInstrumentWindowController *controller;
	BOOL isSample;
	BOOL isLoopingSample;
	BOOL isBlank;
}

@property (readwrite, arcweak) NSTextField *numField;
@property (readwrite, arcweak) NSButton *sampleButton;
@property (readwrite/*, getter = isSample, setter = setIsSample: */, nonatomic) BOOL isSample;
@property (readwrite, nonatomic) BOOL isLoopingSample;
@property (readwrite, arcweak) PPInstrumentWindowController *controller;
@property (readwrite, nonatomic) BOOL isBlank;

- (IBAction)playSample:(id)sender;

@end
