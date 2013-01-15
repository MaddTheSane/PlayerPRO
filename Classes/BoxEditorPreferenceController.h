//
//  BoxEditorPreferenceController.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface BoxEditorPreferenceController : NSViewController {

	IBOutlet __weak NSButton *notesCheck;
	IBOutlet __weak NSButton *octaveCheck;
	IBOutlet __weak NSTextField *loopField;
	IBOutlet __weak NSTextField *offsetField;
	IBOutlet __weak NSButton *markersCheck;
}
- (IBAction)changeMarkerCheck:(id)sender;

@end
