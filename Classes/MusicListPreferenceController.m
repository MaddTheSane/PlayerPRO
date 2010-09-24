//
//  MusicListPreferenceController.m
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "MusicListPreferenceController.h"
#import "UserDefaultKeys.h"

@implementation MusicListPreferenceController

- (IBAction)changePlayedMusic:(id)sender {
    
}

- (IBAction)toggleButton:(id)sender {
    
}

-(id)init {
	if (![super initWithNibName:@"MusicListPrefs" bundle:nil]) {
		return nil;
	}
	[self setTitle:@"Music List"];
	return self;
}

-(void)awakeFromNib {
	
}

@end
