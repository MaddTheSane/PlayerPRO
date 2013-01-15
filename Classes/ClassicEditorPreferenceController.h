//
//  ClassicEditorPreferenceController.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface ClassicEditorPreferenceController : NSViewController {

	IBOutlet __weak NSTextField *tempoUnitValue;
	IBOutlet __weak NSTextField *tempoNumberValue;
	IBOutlet __weak NSTextField *markersLoopValue;
	IBOutlet __weak NSTextField *markersOffsetValue;
	IBOutlet __weak NSButton *markersCheck;
	IBOutlet __weak NSButton *notesLengthCheck;
	IBOutlet __weak NSPopUpButton *trackHeightButton;
}

- (IBAction)changeTrackHeight:(id)sender;
- (IBAction)toggleNoteLength:(id)sender;
- (IBAction)toggleMarkers:(id)sender;

@end
