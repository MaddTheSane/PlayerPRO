//
//  ClassicEditorPreferenceController.m
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "ClassicEditorPreferenceController.h"
#import "UserDefaultKeys.h"


@implementation ClassicEditorPreferenceController

-(id)init
{
	if (![super initWithNibName:@"ClassicPrefs" bundle:nil]) {
		[self autorelease];
		return nil;
	}
	[self setTitle:@"Classic Editor"];
	return self;
}

-(void)awakeFromNib {
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
}

@end
