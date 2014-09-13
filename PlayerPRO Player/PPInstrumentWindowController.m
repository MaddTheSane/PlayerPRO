//
//  PPInstrumentWindowController.m
//  PPMacho
//
//  Created by C.W. Betts on 12/31/12.
//
//

#import "PPInstrumentWindowController.h"
#include <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROKit/PlayerPROKit.h>
@import PlayerPROKit;
#include "PPByteswap.h"
#import "UserDefaultKeys.h"
#import "AppDelegate.h"
#import "PlayerPRO_Player-Swift.h"

@implementation PPInstrumentWindowController
@synthesize infoDrawer;
@synthesize instrumentBits;
@synthesize instrumentLoopSize;
@synthesize instrumentLoopStart;
@synthesize instrumentMode;
@synthesize instrumentNote;
@synthesize instrumentRate;
@synthesize instrumentSize;
@synthesize instrumentView;
@synthesize instrumentVolume;
@synthesize waveFormImage;

- (instancetype)initWithWindow:(NSWindow *)window
{
	if (self = [super initWithWindow:window]) {
		// Initialization code here.
		[(NSObject*)[NSApp delegate] addObserver:self forKeyPath:@"music" options:(NSKeyValueObservingOptionNew | NSKeyValueObservingOptionOld) context:nil];
	}
	
	return self;
}

- (IBAction)toggleInfo:(id)sender
{
	[infoDrawer toggle:sender];
}

- (instancetype)init
{
	return self = [self initWithWindowNibName:@"InstrumentWindow"];
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
	//[instrumentView reloadData];
	[instrumentView selectRowIndexes:[NSIndexSet indexSetWithIndex:0] byExtendingSelection:NO];
}

- (void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[(NSObject*)[NSApp delegate] removeObserver:self forKeyPath:@"music"];
}

#pragma mark NSOutlineView delegates and data ref calls

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
	if ([keyPath isEqualToString:@"music"]) {
		[instrumentView reloadData];
	}
}

- (void)outlineViewSelectionDidChange:(NSNotification *)notification
{
	id object = [instrumentView itemAtRow:[instrumentView selectedRow]];
	
	if ([object isKindOfClass:[PPInstrumentObject class]]) {
		if ([object countOfSamples] > 0) {
			object = [object samplesObjectAtIndex:0];
		} else {
			object = nil;
		}
	}
	if (!object) {
		[instrumentSize setStringValue:PPDoubleDash];
		[instrumentLoopStart setStringValue:PPDoubleDash];
		[instrumentLoopSize setStringValue:PPDoubleDash];
		[instrumentVolume setStringValue:PPDoubleDash];
		[instrumentRate setStringValue:PPDoubleDash];
		[instrumentNote setStringValue:PPDoubleDash];
		[instrumentBits setStringValue:PPDoubleDash];
		[instrumentMode setStringValue:PPDoubleDash];
		[waveFormImage setImage:nil];
		return;
	}
	[instrumentSize setIntegerValue:[object dataSize]];
	[instrumentLoopStart setIntegerValue:[object loopBegin]];
	[instrumentLoopSize setIntegerValue:[object loopSize]];
	[instrumentVolume setIntegerValue:[(PPSampleObject*)object volume]];
	[instrumentRate setStringValue:[NSString stringWithFormat:@"%u Hz", [object c2spd]]];
	instrumentNote.stringValue = [PPSampleObject octaveNameFromNote:[object relativeNote]]; 
	[instrumentBits setStringValue:[NSString stringWithFormat:@"%u-bit", [object amplitude]]];
	[instrumentMode setStringValue: [object loopType] == ePingPongLoop ? @"Ping-pong" : @"Classic"];
	waveFormImage.image = [object waveformImageUsingView:waveFormImage];
}

- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item
{
	if (item == nil) {
		return [((PlayerAppDelegate*)[NSApp delegate]).music.instruments count];
	}
	if ([item isKindOfClass:[PPInstrumentObject class]]) {
		return [item countOfSamples];
	}
	return 0;
}

- (id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item
{
	if (item == nil) {
		return ((PlayerAppDelegate*)[NSApp delegate]).music.instruments[index];
	}
	if ([item isKindOfClass:[PPInstrumentObject class]]) {
		return [item samplesObjectAtIndex:index];
	}
	return nil;
}

- (id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item;
{
	if ([item isKindOfClass:[PPInstrumentObject class]] || [item isKindOfClass:[PPSampleObject class]]) {
		return [item name];
	}
	return nil;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
	if ([item isKindOfClass:[PPInstrumentObject class]]) {
		return [item countOfSamples] != 0;
	}
	return NO;
}

@end
