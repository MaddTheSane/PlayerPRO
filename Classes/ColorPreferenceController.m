//
//  ColorPreferenceController.m
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "ColorPreferenceController.h"
#import "UserDefaultKeys.h"


@implementation ColorPreferenceController

-(id)init
{
	if (self = [super initWithNibName:@"ColorPrefs" bundle:nil]) {
		[self setTitle:@"Color"];
	}
	return self;
}

-(void)awakeFromNib {
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
}

@end
