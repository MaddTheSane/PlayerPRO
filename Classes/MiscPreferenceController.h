//
//  MiscPreferenceController.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface MiscPreferenceController : NSViewController {
    IBOutlet NSButton *addExt;
    IBOutlet NSButton *madCompression;
    IBOutlet NSButton *noLoadMixerFromFile;
	IBOutlet NSButton *oscDrawLines;
}
- (IBAction)changePrefs:(id)sender;
@end
