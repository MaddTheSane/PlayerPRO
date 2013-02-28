//
//  ClassicEditorPreferenceController.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface ClassicEditorPreferenceController : NSViewController {

	IBOutlet NSTextField *tempoUnitValue;
	IBOutlet NSTextField *tempoNumberValue;
	IBOutlet NSTextField *markersLoopValue;
	IBOutlet NSTextField *markersOffsetValue;
	IBOutlet NSButton *markersCheck;
	IBOutlet NSButton *notesLengthCheck;
	IBOutlet NSPopUpButton *trackHeightButton;
}

- (IBAction)changeTrackHeight:(id)sender;
- (IBAction)toggleNoteLength:(id)sender;
- (IBAction)toggleMarkers:(id)sender;

@end
