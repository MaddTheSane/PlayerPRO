//
//  PPInstrumentCellView.h
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import <Cocoa/Cocoa.h>

//@class PPSampleButton;

@interface PPInstrumentCellView : NSTableCellView
{
	IBOutlet __weak NSTextField *numField;
	IBOutlet __weak NSButton *sampleButton;
	IBOutlet __weak NSImageView *loopImage;
	BOOL isSample;
	BOOL isLoopingSample;
}

@property (readwrite, weak) NSTextField *numField;
@property (readwrite, weak) NSButton *sampleButton;
@property (readwrite/*, getter = isSample, setter = setIsSample: */, nonatomic) BOOL isSample;
@property (readwrite, nonatomic) BOOL isLoopingSample;
@end
