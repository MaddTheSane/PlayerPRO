//
//  ColorPreferenceController.m
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "ColorPreferenceController.h"
#import "UserDefaultKeys.h"
#import "NSColor+PPPreferences.h"


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
	[colorWell1 setColor:[NSColor PPDecodeColorWithData:[defaults dataForKey:PPCColor1]]];
	[colorWell2 setColor:[NSColor PPDecodeColorWithData:[defaults dataForKey:PPCColor2]]];
	[colorWell3 setColor:[NSColor PPDecodeColorWithData:[defaults dataForKey:PPCColor3]]];
	[colorWell4 setColor:[NSColor PPDecodeColorWithData:[defaults dataForKey:PPCColor4]]];
	[colorWell5 setColor:[NSColor PPDecodeColorWithData:[defaults dataForKey:PPCColor5]]];
	[colorWell6 setColor:[NSColor PPDecodeColorWithData:[defaults dataForKey:PPCColor6]]];
	[colorWell7 setColor:[NSColor PPDecodeColorWithData:[defaults dataForKey:PPCColor7]]];
	[colorWell8 setColor:[NSColor PPDecodeColorWithData:[defaults dataForKey:PPCColor8]]];
}

- (IBAction)changeColorWell:(id)sender
{
	NSString *keyToChange = nil;
	
	if (sender == colorWell1) {
		keyToChange = PPCColor1;
	} else if (sender == colorWell2){
		keyToChange = PPCColor2;
	} else if (sender == colorWell3){
		keyToChange = PPCColor3;
	} else if (sender == colorWell4){
		keyToChange = PPCColor4;
	} else if (sender == colorWell5){
		keyToChange = PPCColor5;
	} else if (sender == colorWell6){
		keyToChange = PPCColor6;
	} else if (sender == colorWell7){
		keyToChange = PPCColor7;
	} else if (sender == colorWell8){
		keyToChange = PPCColor8;
	} else {
		return;
	}
	[[NSUserDefaults standardUserDefaults] setObject:[[sender color] PPencodeColor] forKey:keyToChange];
	[[NSNotificationCenter defaultCenter] postNotificationName:PPColorsDidChange object:self];
}

@end
