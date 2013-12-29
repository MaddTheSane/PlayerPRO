//
//  DigitalEditorPreferenceControler.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface DigitalEditorPreferenceControler : NSViewController
@property (weak) IBOutlet NSButton *PPDEEffect;
@property (weak) IBOutlet NSButton *PPDENote;
@property (weak) IBOutlet NSButton *PPDEInstrument;
@property (weak) IBOutlet NSButton *PPDEArgument;
@property (weak) IBOutlet NSButton *PPDEVolume;
	
@property (weak) IBOutlet NSButton *PPDEMarkerSelect;
@property (weak) IBOutlet NSTextField *PPDEMarkerOffset;
@property (weak) IBOutlet NSTextField *PPDEMarkerLoop;
@property (weak) IBOutlet NSColorWell *PPDEMarkerColor;
	
	
@property (weak) IBOutlet NSButtonCell *PPDEMouseClickControl;
@property (weak) IBOutlet NSButtonCell *PPDEMouseClickShift;
@property (weak) IBOutlet NSButtonCell *PPDEMouseClickCommand;
@property (weak) IBOutlet NSButtonCell *PPDEMouseClickOption;
	
@property (weak) IBOutlet NSMatrix *PPDELineHeightMatrix;
@property (weak) IBOutlet NSMatrix *PPDEMusicTraceMatrix;
@property (weak) IBOutlet NSMatrix *PPDEPatternWrappingMatrix;
@property (weak) IBOutlet NSMatrix *PPDEDragAsMatrix;

- (IBAction)cellInformationCheckClicked:(id)sender;
- (IBAction)markersCheckClicked:(id)sender;
- (IBAction)mouseCheckClicked:(id)sender;
- (IBAction)lineHeightClicked:(id)sender;
- (IBAction)musicTraceClicked:(id)sender;
- (IBAction)patternWrappingClicked:(id)sender;
- (IBAction)dragAsClicked:(id)sender;
- (IBAction)changeColor:(id)sender;

@end
