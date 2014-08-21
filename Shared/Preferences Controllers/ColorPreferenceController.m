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
#import "PPColorPreferenceObject.h"

@implementation ColorPreferenceController
@synthesize colors;

+ (instancetype)newPreferenceView
{
	return [[self alloc] init];
}

-(id)init
{
	if (self = [super initWithNibName:@"ColorPrefs" bundle:nil]) {
		[self setTitle:NSLocalizedStringFromTable(@"Colors", @"PreferenceNames", @"Colors")];
	}
	return self;
}

-(void)awakeFromNib
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSMutableArray *tmpColor = [[NSMutableArray alloc] initWithCapacity:96];
	PPColorPreferenceObject *colorObj;
	
#define PPCOLOR(num) { \
colorObj = [[PPColorPreferenceObject alloc] initWithColor:[NSColor PPDecodeColorWithData:[defaults dataForKey: PPCColor ## num ]] index: num - 1 ];\
[tmpColor addObject:colorObj];\
}
	PPCOLORPOPULATE();
#undef PPCOLOR
	self.colors = tmpColor;
}

#if 0
-(void)setPersonModelArray:(NSMutableArray *)a {
	personModelArray = a;
}

-(NSArray*)colorsModelArray {
	return personModelArray;
}
#endif

- (IBAction)changeColorWell:(id)sender
{
#if 0
	NSString *keyToChange;
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
#endif
}

@end
