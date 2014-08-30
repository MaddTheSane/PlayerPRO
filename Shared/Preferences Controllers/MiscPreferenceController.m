//
//  MiscPreferenceController.m
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "MiscPreferenceController.h"
#import "UserDefaultKeys.h"

@implementation MiscPreferenceController
@synthesize addExt;
@synthesize madCompression;
@synthesize noLoadMixerFromFile;
@synthesize oscDrawLines;

+ (instancetype)newPreferenceView
{
	return [[self alloc] init];
}

- (IBAction)changePrefs:(id)sender
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];

	[defaults setBool:[addExt state] forKey:PPMAddExtension];
	[defaults setBool:[madCompression state] forKey:PPMMadCompression];
	[defaults setBool:[noLoadMixerFromFile state] forKey:PPMNoLoadMixerFromFiles];
	[defaults setBool:[oscDrawLines state] forKey:PPMOscilloscopeDrawLines];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPMiscPreferencesDidChange object:self];
}

- (instancetype)init
{
	if (self = [super initWithNibName:@"MiscPrefs" bundle:nil]) {
		[self setTitle:NSLocalizedStringFromTable(@"Misc.", @"PreferenceNames", @"Misc.")];
	}
	return self;
}

- (void)awakeFromNib
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[addExt setState:[defaults boolForKey:PPMAddExtension]];
	[madCompression setState:[defaults boolForKey:PPMMadCompression]];
	[noLoadMixerFromFile setState:[defaults boolForKey:PPMNoLoadMixerFromFiles]];
	[oscDrawLines setState: [defaults boolForKey:PPMOscilloscopeDrawLines]];
}

@end
