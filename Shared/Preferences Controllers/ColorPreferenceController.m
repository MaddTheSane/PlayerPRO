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
#import "PlayerPRO_6-Swift.h"

@implementation ColorPreferenceController
{
	NSMutableArray *_colors;
}

- (NSArray*)colors
{
	return [NSArray arrayWithArray:_colors];
}

- (void)setColors:(NSArray *)colors
{
	_colors = [colors mutableCopy];
}

- (void)replaceObjectInColorsAtIndex:(NSInteger)index withObject:(ColorPreferenceObject*)object
{
	_colors[index] = object;
}

+ (instancetype)newPreferenceView
{
	return [[self alloc] init];
}

- (NSString*)preferenceIdentifier
{
	return PPColorPrefID;
}

- (instancetype)init
{
	if (self = [super initWithNibName:@"ColorPrefs" bundle:nil]) {
		[self setTitle:NSLocalizedStringFromTable(@"Colors", @"PreferenceNames", @"Colors")];
	}
	return self;
}

- (void)awakeFromNib
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSMutableArray *tmpColor = [[NSMutableArray alloc] initWithCapacity:96];
	ColorPreferenceObject *colorObj;
	
#define PPCOLOR(num) { \
colorObj = [[ColorPreferenceObject alloc] initWithColor:[NSColor PPDecodeColorWithData:[defaults dataForKey: PPCColor ## num ]] index: num - 1 ];\
[tmpColor addObject:colorObj];\
}
	PPCOLORPOPULATE();
#undef PPCOLOR
	self.colors = tmpColor;
}

- (NSInteger)indexOfColorWell:(NSColorWell*)cw
{
	NSView *collectionViewItem = [cw superview];
	NSInteger index = [[_colorWells subviews] indexOfObject:collectionViewItem];
	return index;
}

- (IBAction)changeColorWell:(id)sender
{
	NSColor *sentColor = [sender color];
	NSData *encData = [sentColor PPencodeColor];
	NSString *keyToChange;
	NSInteger wellIdx = [self indexOfColorWell:sender];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPColorsDidChange
														object:self
													  userInfo:@{PPColorChangedValue: @(wellIdx),
																 PPColorChangedColor: sentColor}];
#define PPCOLOR(num) case num - 1: \
keyToChange = PPCColor ## num; \
break
	
	
	switch (wellIdx) {
			PPCOLORPOPULATE();
		default:
			break;
	}
#undef PPCOLOR
	if (keyToChange != nil) {
		[[NSUserDefaults standardUserDefaults] setObject:encData forKey:keyToChange];
	}
}

- (void)viewDidDisappear
{
	if ([super respondsToSelector:@selector(viewDidDisappear)]) {
		[super viewDidDisappear];
	}
	
	if ([NSColorPanel sharedColorPanelExists]) {
		[[NSColorPanel sharedColorPanel] close];
	}
}

@end
