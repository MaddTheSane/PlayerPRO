//
//  BoxEditorPreferenceController.m
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "BoxEditorPreferenceController.h"
#import "UserDefaultKeys.h"

@implementation BoxEditorPreferenceController
@synthesize loopField;
@synthesize markersCheck;
@synthesize notesCheck;
@synthesize octaveCheck;
@synthesize offsetField;

+ (instancetype)newPreferenceView
{
	return [[self alloc] init];
}

- (NSString*)identifier
{
	return PPBoxPrefID;
}

- (NSString*)toolbarItemLabel
{
	return NSLocalizedStringFromTable(@"Box Editor", @"PreferenceNames", @"Box Editor");
}

- (NSImage*)toolbarItemImage
{
	return nil;
}

- (instancetype)init
{
	if (self = [super initWithNibName:@"BoxPrefs" bundle:nil]) {
		[self setTitle:NSLocalizedStringFromTable(@"Box Editor", @"PreferenceNames", @"Box Editor")];
	}
	return self;
}

- (void)awakeFromNib
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[notesCheck setState:[defaults boolForKey:PPBENotesProjection]];
	[octaveCheck setState:[defaults boolForKey:PPBEOctaveMarkers]];
	[markersCheck setState:[defaults boolForKey:PPBEMarkersEnabled]];
	[loopField setIntegerValue:[defaults integerForKey:PPBEMarkersLoop]];
	[offsetField setIntegerValue:[defaults integerForKey:PPBEMarkersOffset]];
}

- (IBAction)changeMarkerCheck:(id)sender
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setBool:[notesCheck state] forKey:PPBENotesProjection];
	[defaults setBool:[octaveCheck state] forKey:PPBEOctaveMarkers];
	[defaults setBool:[markersCheck state] forKey:PPBEMarkersEnabled];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPBoxEditorPreferencesDidChangeNotification object:self];
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
