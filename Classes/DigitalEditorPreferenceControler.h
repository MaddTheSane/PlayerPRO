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
	
	
	IBOutlet NSButton *PPDEMouseClickControl;
	IBOutlet NSButton *PPDEMouseClickShift;
	IBOutlet NSButton *PPDEMouseClickCommand;
	IBOutlet NSButton *PPDEMouseClickOption;
}
- (IBAction)cellInformationCheckClicked:(id)sender;
- (IBAction)markersCheckClicked:(id)sender;
- (IBAction)mouseCheckClicked:(id)sender;

@end
