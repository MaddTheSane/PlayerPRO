//
//  DigitalEditorPreferenceControler.m
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "DigitalEditorPreferenceControler.h"


@implementation DigitalEditorPreferenceControler

-(id)init
{
	if (![super initWithNibName:@"DigitalPrefs" bundle:nil]) {
		[self autorelease];
		return nil;
	}
	[self setTitle:@"Digital Editor"];
	return self;
}

- (IBAction)cellInformationCheckClicked:(id)sender {
	
}

- (IBAction)markersCheckClicked:(id)sender {
	
}

- (IBAction)mouseCheckClicked:(id)sender {
	
}

-(void)awakeFromNib {
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
}

@end
