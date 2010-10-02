//
//  MidiHardwarePreferenceController.m
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "MidiHardwarePreferenceController.h"


@implementation MidiHardwarePreferenceController

-(id)init
{
	if (![super initWithNibName:@"MidiPrefs" bundle:nil]) {
		[self autorelease];
		return nil;
	}
	[self setTitle:@"MIDI Hardware"];
	return self;
}

@end
