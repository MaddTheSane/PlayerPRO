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
	IBOutlet __arcweak NSTextField *numField;
	IBOutlet __arcweak NSButton *sampleButton;
	IBOutlet NSImageView *loopImage;
	__arcweak PPInstrumentWindowController *controller;
	BOOL isSample;
	BOOL isLoopingSample;
	BOOL isBlank;
}

@property (readwrite, arcweak) NSTextField *numField;
@property (readwrite, arcweak) NSButton *sampleButton;
@property (readwrite, arcweak) PPInstrumentWindowController *controller;
@property (readwrite, nonatomic) BOOL isSample;
@property (readwrite, nonatomic) BOOL isLoopingSample;
@property (readwrite, nonatomic) BOOL isBlank;

- (IBAction)playSample:(id)sender;

@end
