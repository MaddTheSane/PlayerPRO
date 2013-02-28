//
//  DigitalEditorPreferenceControler.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface DigitalEditorPreferenceControler : NSViewController {

	IBOutlet NSButton *PPDEEffect;
	IBOutlet NSButton *PPDENote;
	IBOutlet NSButton *PPDEInstrument;
	IBOutlet NSButton *PPDEArgument;
	IBOutlet NSButton *PPDEVolume;
	
	IBOutlet NSButton *PPDEMarkerSelect;
	IBOutlet NSTextField *PPDEMarkerOffset;
	IBOutlet NSTextField *PPDEMarkerLoop;
	IBOutlet NSColorWell *PPDEMarkerColor;
	
	
	IBOutlet NSButtonCell *PPDEMouseClickControl;
	IBOutlet NSButtonCell *PPDEMouseClickShift;
	IBOutlet NSButtonCell *PPDEMouseClickCommand;
	IBOutlet NSButtonCell *PPDEMouseClickOption;
	
	IBOutlet NSMatrix *PPDELineHeightMatrix;
	IBOutlet NSMatrix *PPDEMusicTraceMatrix;
	IBOutlet NSMatrix *PPDEPatternWrappingMatrix;
	IBOutlet NSMatrix *PPDEDragAsMatrix;
}
- (IBAction)cellInformationCheckClicked:(id)sender;
- (IBAction)markersCheckClicked:(id)sender;
- (IBAction)mouseCheckClicked:(id)sender;
- (IBAction)lineHeightClicked:(id)sender;
- (IBAction)musicTraceClicked:(id)sender;
- (IBAction)patternWrappingClicked:(id)sender;
- (IBAction)dragAsClicked:(id)sender;
- (IBAction)changeColor:(id)sender;

@end
