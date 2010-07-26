//
//  SoundOutputController.m
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "SoundOutputController.h"


@implementation SoundOutputController

-(id)init
{
	if (![super initWithNibName:@"SoundOutput" bundle:nil]) {
		return nil;
	}
	[self setTitle:@"Sound Output"];
	return self;
}

@end
