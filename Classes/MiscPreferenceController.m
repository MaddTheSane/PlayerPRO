//
//  MiscPreferenceController.m
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "MiscPreferenceController.h"
#import "UserDefaultKeys.h"


@implementation MiscPreferenceController
- (IBAction)changePrefs:(id)sender {
    
}


-(id)init
{
	if (![super initWithNibName:@"MiscPrefs" bundle:nil]) {
		[self autorelease];
		return nil;
	}
	[self setTitle:@"Misc."];
	return self;
}

-(void)awakeFromNib {
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
}

@end
