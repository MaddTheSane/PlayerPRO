//
//  BoxEditorPreferenceController.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <MASPreferences/MASPreferencesViewController.h>

@interface BoxEditorPreferenceController : NSViewController <MASPreferencesViewController>
@property (weak) IBOutlet NSButton *notesCheck;
@property (weak) IBOutlet NSButton *octaveCheck;
@property (weak) IBOutlet NSFormCell *loopField;
@property (weak) IBOutlet NSFormCell *offsetField;
@property (weak) IBOutlet NSButton *markersCheck;

+ (instancetype)newPreferenceView NS_RETURNS_RETAINED;

- (IBAction)changeMarkerCheck:(id)sender;

@end
