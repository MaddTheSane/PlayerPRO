//
//  PianoPreferencesController.m
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "PianoPreferencesController.h"
#import "UserDefaultKeys.h"


@implementation PianoPreferencesController

+ (instancetype)newPreferenceView
{
	return [[self alloc] init];
}

- (NSString*)identifier
{
	return PPPianoPrefID;
}

- (NSString*)toolbarItemLabel
{
	return NSLocalizedStringFromTable(@"Piano", @"PreferenceNames", @"Piano");
}

- (NSImage*)toolbarItemImage
{
	return nil;
}

- (instancetype)init
{
	if (self = [super initWithNibName:@"PianoPrefs" bundle:nil]) {
		[self setTitle:NSLocalizedStringFromTable(@"Piano", @"PreferenceNames", @"Piano")];
	}
	return self;
}

- (void)awakeFromNib
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	defaults = nil;
}

- (BOOL)hasResizableWidth
{
	return NO;
}

- (BOOL)hasResizableHeight
{
	return NO;
}

@end
