//
//  ClassicEditorPreferenceController.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface ClassicEditorPreferenceController : NSViewController
@property (weak) IBOutlet NSTextField *tempoUnitValue;
@property (weak) IBOutlet NSTextField *tempoNumberValue;
@property (weak) IBOutlet NSTextField *markersLoopValue;
@property (weak) IBOutlet NSTextField *markersOffsetValue;
@property (weak) IBOutlet NSButton *markersCheck;
@property (weak) IBOutlet NSButton *notesLengthCheck;
@property (weak) IBOutlet NSPopUpButton *trackHeightButton;

- (IBAction)changeTrackHeight:(id)sender;
- (IBAction)toggleNoteLength:(id)sender;
- (IBAction)toggleMarkers:(id)sender;

@end
