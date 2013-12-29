//
//  MiscPreferenceController.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface MiscPreferenceController : NSViewController
@property (weak) IBOutlet NSButtonCell *addExt;
@property (weak) IBOutlet NSButtonCell *madCompression;
@property (weak) IBOutlet NSButtonCell *noLoadMixerFromFile;
@property (weak) IBOutlet NSButton *oscDrawLines;

- (IBAction)changePrefs:(id)sender;
@end
