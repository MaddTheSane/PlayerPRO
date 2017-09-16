//
//  MidiHardwarePreferenceController.m
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "MidiHardwarePreferenceController.h"
#import "UserDefaultKeys.h"


@implementation MidiHardwarePreferenceController

+ (instancetype)newPreferenceView
{
	return [[self alloc] init];
}

- (NSString*)identifier
{
	return PPMIDIPrefID;
}

- (NSString*)toolbarItemLabel
{
	return NSLocalizedStringFromTable(@"MIDI Hardware", @"PreferenceNames", @"MIDI Hardware");
}

- (NSImage*)toolbarItemImage
{
	// Try to use system-available image. Because the icon might change in the future
	NSURL *localAppDir = [[NSFileManager defaultManager] URLForDirectory:NSApplicationDirectory inDomain:NSLocalDomainMask appropriateForURL:nil create:NO error:NULL];
	NSURL *audMidSetupURL = [localAppDir URLByAppendingPathComponent:@"Utilities"];
	audMidSetupURL = [audMidSetupURL URLByAppendingPathComponent:@"Audio MIDI Setup.app"];
	NSBundle *audMidSetupBundle = [NSBundle bundleWithURL:audMidSetupURL];
	NSImage *midiIcon = [audMidSetupBundle imageForResource:@"AudioMIDISetup"];
	if (midiIcon) {
		return midiIcon;
	}
	// Fall-back to known icon
	return [NSImage imageNamed:@"Preferences/MIDI"];
}


- (instancetype)init
{
	if (self = [super initWithNibName:@"MidiPrefs" bundle:nil]) {
		[self setTitle:NSLocalizedStringFromTable(@"MIDI Hardware", @"PreferenceNames", @"MIDI Hardware")];
	}
	return self;
}

- (void)awakeFromNib
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	defaults = nil;
}

- (BOOL)hasResizableWidth
{
	return NO;
}

- (BOOL)hasResizableHeight
{
	return NO;
}

@end
