//
//  Preferences.m
//  PPMacho
//
//  Created by C.W. Betts on 7/14/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "PPPreferences.h"
#import "SoundOutputController.h"
#import "PianoPreferencesController.h"
#import "MusicListPreferenceController.h"
#import "ColorPreferenceController.h"
#import "MiscPreferenceController.h"
#import "BoxEditorPreferenceController.h"
#import "DigitalEditorPreferenceControler.h"
#import "ClassicEditorPreferenceController.h"
#import "MidiHardwarePreferenceController.h"

@implementation PPPreferences

- (id)init 
{
    self = [super initWithWindowNibName:@"Preferences"];
    if (self != nil) {
		viewControllers =[[NSMutableArray alloc] initWithCapacity:3];
		
		NSViewController *vc;
		vc = [[SoundOutputController alloc] init];
		[viewControllers addObject:vc];
		[vc release];
		
		//vc = [[PianoPreferencesController alloc] init];
		//[viewControllers addObject:vc];
		//[vc release];
		
		vc = [[MusicListPreferenceController alloc] init];
		[viewControllers addObject:vc];
		[vc release];
		
		/*vc = [[ColorPreferenceController alloc] init];
		[viewControllers addObject:vc];
		[vc release];*/
		
		vc = [[MiscPreferenceController alloc] init];
		[viewControllers addObject:vc];
		[vc release];
		
		/*vc = [[BoxEditorPreferenceController alloc] init];
		[viewControllers addObject:vc];
		[vc release];
		
		vc = [[DigitalEditorPreferenceControler alloc] init];
		[viewControllers addObject:vc];
		[vc release];
		
		vc = [[ClassicEditorPreferenceController alloc] init];
		[viewControllers addObject:vc];
		[vc release];
		
		vc = [[MidiHardwarePreferenceController alloc] init];
		[viewControllers addObject:vc];
		[vc release];*/
		
    }
    return self;
}
- (void)dealloc
{
	[viewControllers release];
	
	[super dealloc];
}

- (NSString *)windowNibName 
{
    return @"preferences";
}

-(void)displayViewController:(NSViewController *)vc
{
	//try to end editing
	NSWindow *w = [box window];
	BOOL ended = [w makeFirstResponder:w];
	
	if (!ended) {
		NSBeep();
		return;
	}
	
	//Put the view in the box
	NSView *v = [vc view];
	
	//Compute the new window frame
	NSSize currentSize = [[box contentView] frame].size;
	NSSize newSize = [v frame].size;
	CGFloat deltaWidth = newSize.width - currentSize.width;
	CGFloat deltaHeight = newSize.height - currentSize.height;
	NSRect windowFrame = [w frame];
	windowFrame.size.height += deltaHeight;
	windowFrame.origin.y -= deltaHeight;
	windowFrame.size.width += deltaWidth;
	
	//Clear the box for resizing
	[box setContentView:nil];
	[w setFrame:windowFrame display:YES animate:YES];
	
	[box setContentView:v];
}

-(IBAction)changeViewController:(id)sender
{
	NSInteger i = [sender tag];
	NSViewController *vc = [viewControllers objectAtIndex:i];
	[self displayViewController:vc];
}

-(void)awakeFromNib
{
	NSMenu *menu = [popUp menu];
	NSInteger i, itemCount;
	itemCount = [viewControllers count];
	
	for(i = 0; i < itemCount; i++)
	{
		NSViewController *vc = [viewControllers objectAtIndex:i];
		NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:[vc title] action:@selector(changeViewController:) keyEquivalent:@""];
		[mi setTag:i];
		[menu addItem:mi];
		[mi release];
	}
	
	//initially show the first controller
	[self displayViewController:[viewControllers objectAtIndex:0]];
	[popUp selectItemAtIndex:0];
}

@end
