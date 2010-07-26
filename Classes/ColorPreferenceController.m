//
//  ColorPreferenceController.m
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "ColorPreferenceController.h"


@implementation ColorPreferenceController

-(id)init
{
	if (![super initWithNibName:@"ColorPrefs" bundle:nil]) {
		return nil;
	}
	[self setTitle:@"Color"];
	return self;
}

@end
