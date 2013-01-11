//
//  PPInstrumentCellView.h
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import <Cocoa/Cocoa.h>

@interface PPInstrumentCellView : NSTableCellView
{
	IBOutlet NSTextField *numField;
	IBOutlet NSTextField *insNameField;
}

@property (readwrite, retain) NSTextField *numField;
@property (readwrite, retain) NSTextField *insNameField;

@end
