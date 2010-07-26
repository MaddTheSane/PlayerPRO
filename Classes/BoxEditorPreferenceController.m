//
//  BoxEditorPreferenceController.m
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "BoxEditorPreferenceController.h"


@implementation BoxEditorPreferenceController

-(id)init
{
	if (![super initWithNibName:@"BoxPrefs" bundle:nil]) {
		return nil;
	}
	[self setTitle:@"Box Editor"];
	return self;
}

@end
