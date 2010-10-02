//
//  MiscPreferenceController.m
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "MiscPreferenceController.h"


@implementation MiscPreferenceController

-(id)init
{
	if (![super initWithNibName:@"MiscPrefs" bundle:nil]) {
		[self autorelease];
		return nil;
	}
	[self setTitle:@"Misc."];
	return self;
}

@end
