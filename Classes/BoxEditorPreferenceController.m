//
//  BoxEditorPreferenceController.m
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "BoxEditorPreferenceController.h"
#import "UserDefaultKeys.h"


@implementation BoxEditorPreferenceController

-(id)init
{
	if (![super initWithNibName:@"BoxPrefs" bundle:nil]) {
		[self autorelease];
		return nil;
	}
	[self setTitle:@"Box Editor"];
	return self;
}

-(void)awakeFromNib {
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
}

@end
