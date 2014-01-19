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

-(id)init
{
	if (self = [super initWithNibName:@"MidiPrefs" bundle:nil]) {
		[self setTitle:@"MIDI Hardware"];
	}
	return self;
}

-(void)awakeFromNib
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	
}

@end
