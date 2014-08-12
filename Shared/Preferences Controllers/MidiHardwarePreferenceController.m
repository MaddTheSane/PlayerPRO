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

-(id)init
{
	if (self = [super initWithNibName:@"MidiPrefs" bundle:nil]) {
		[self setTitle:NSLocalizedStringFromTable(@"MIDI Hardware", @"PreferenceNames", @"MIDI Hardware")];
	}
	return self;
}

-(void)awakeFromNib
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	
}

@end
