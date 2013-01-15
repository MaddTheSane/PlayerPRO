//
//  MiscPreferenceController.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface MiscPreferenceController : NSViewController {
    IBOutlet __weak NSButton *addExt;
    IBOutlet __weak NSButton *madCompression;
    IBOutlet __weak NSButton *noLoadMixerFromFile;
	IBOutlet __weak NSButton *oscDrawLines;
}
- (IBAction)changePrefs:(id)sender;
@end
