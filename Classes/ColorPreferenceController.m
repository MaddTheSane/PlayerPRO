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

#define PPCOLOR(num) @synthesize colorWell ## num
PPCOLORPOPULATE();
#undef PPCOLOR

-(id)init
{
	if (self = [super initWithNibName:@"ColorPrefs" bundle:nil]) {
		[self setTitle:@"Colors"];
	}
	return self;
}

-(void)awakeFromNib
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
#define PPCOLOR(num) if (colorWell ## num)\
[colorWell ## num setColor:[NSColor PPDecodeColorWithData:[defaults dataForKey:PPCColor ## num]]]
	PPCOLORPOPULATE();
#undef PPCOLOR
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
