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
#define PPCOLOR(num) [colorWell ## num setColor:[NSColor PPDecodeColorWithData:[defaults dataForKey:PPCColor ## num]]]
	PPCOLORPOPULATE();
#undef PPCOLOR
}

- (IBAction)changeColorWell:(id)sender
{
	NSString *keyToChange = nil;
#define PPCOLOR(num) } else if (sender == colorWell ## num) \
{\
keyToChange = PPCColor ## num

	if (sender == nil) {
		return;
		PPCOLORPOPULATE();
	} else {
		return;
	}
	[[NSUserDefaults standardUserDefaults] setObject:[[sender color] PPencodeColor] forKey:keyToChange];
	[[NSNotificationCenter defaultCenter] postNotificationName:PPColorsDidChange object:self];
#undef PPCOLOR
}

@end
