//
//  BoxEditorPreferenceController.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface BoxEditorPreferenceController : NSViewController {

	IBOutlet NSButton *notesCheck;
	IBOutlet NSButton *octaveCheck;
	IBOutlet NSTextField *loopField;
	IBOutlet NSTextField *offsetField;
	IBOutlet NSButton *markersCheck;
}
- (IBAction)changeMarkerCheck:(id)sender;

@end
