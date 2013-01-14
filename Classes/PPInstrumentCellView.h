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
	IBOutlet __unsafe_unretained NSTextField *numField;
	IBOutlet __unsafe_unretained NSButton *sampleButton;
	BOOL isSample;
}

@property (readwrite, assign) NSTextField *numField;
@property (readwrite, assign) NSButton *sampleButton;
@property (readwrite/*, getter = isSample, setter = setIsSample: */, nonatomic) BOOL isSample;
@end
