//
//  DigitalEditorPreferenceControler.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface DigitalEditorPreferenceControler : NSViewController {

	IBOutlet __weak NSButton *PPDEEffect;
	IBOutlet __weak NSButton *PPDENote;
	IBOutlet __weak NSButton *PPDEInstrument;
	IBOutlet __weak NSButton *PPDEArgument;
	IBOutlet __weak NSButton *PPDEVolume;
	
	IBOutlet __weak NSButton *PPDEMarkerSelect;
	IBOutlet __weak NSTextField *PPDEMarkerOffset;
	IBOutlet __weak NSTextField *PPDEMarkerLoop;
	IBOutlet __weak NSColorWell *PPDEMarkerColor;
	
	
	IBOutlet __weak NSButtonCell *PPDEMouseClickControl;
	IBOutlet __weak NSButtonCell *PPDEMouseClickShift;
	IBOutlet __weak NSButtonCell *PPDEMouseClickCommand;
	IBOutlet __weak NSButtonCell *PPDEMouseClickOption;
	
	IBOutlet __weak NSMatrix *PPDELineHeightMatrix;
	IBOutlet __weak NSMatrix *PPDEMusicTraceMatrix;
	IBOutlet __weak NSMatrix *PPDEPatternWrappingMatrix;
	IBOutlet __weak NSMatrix *PPDEDragAsMatrix;
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
