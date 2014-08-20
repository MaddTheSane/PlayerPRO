//
//  ClassicEditorPreferenceController.m
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "ClassicEditorPreferenceController.h"
#import "UserDefaultKeys.h"
#ifdef PLAYERPRO6
#import "PlayerPRO_6-Swift.h"
#else
#import "PlayerPRO_Player-Swift.h"
#endif

typedef struct _tagCoupling {
	int amount;
	NSInteger tag;
} TagCoupling;

static const TagCoupling TrackHeightCoupling[] = {{100, 1}, {110, 2}, {120, 3}, {130, 4}, {140, 5},
	{160, 6}, {180, 7}, {200, 8}};

@implementation ClassicEditorPreferenceController
@synthesize markersCheck;
@synthesize markersLoopValue;
@synthesize markersOffsetValue;
@synthesize notesLengthCheck;
@synthesize tempoNumberValue;
@synthesize tempoUnitValue;
@synthesize trackHeightButton;

+ (instancetype)newPreferenceView
{
	return [[self alloc] init];
}

-(id)init
{
	if (self = [super initWithNibName:@"ClassicPrefs" bundle:nil]) {
		[self setTitle:NSLocalizedStringFromTable(@"Classic Editor", @"PreferenceNames", @"Classic Editor")];
	}
	return self;
}

- (IBAction)changeTrackHeight:(id)sender
{
	int toSet = 0;
	NSInteger tag = [sender tag];
	int i = 0;
	size_t sizeofCoupling = sizeof(TrackHeightCoupling) / sizeof(TrackHeightCoupling[0]);
	for (i = 0; i < sizeofCoupling; i++) {
		if (TrackHeightCoupling[i].tag == tag) {
			toSet = TrackHeightCoupling[i].amount;
			break;
		}
	}
	if (toSet == 0) {
		toSet = 130;
	}
	[[NSUserDefaults standardUserDefaults] setInteger:toSet forKey:PPCETrackHeight];
	[[NSNotificationCenter defaultCenter] postNotificationName:PPClassicalEditorPreferencesDidChange object:self];
}

- (IBAction)toggleNoteLength:(id)sender
{
	[[NSUserDefaults standardUserDefaults] setBool:[notesLengthCheck state] forKey:PPCEShowNotesLen];
	[[NSNotificationCenter defaultCenter] postNotificationName:PPClassicalEditorPreferencesDidChange object:self];

}

- (IBAction)toggleMarkers:(id)sender
{
	NSInteger state = [markersCheck state];
	[[NSUserDefaults standardUserDefaults] setBool:state forKey:PPCEShowMarkers];
	[markersLoopValue setEnabled:state];
	[markersOffsetValue setEnabled:state];
	[[NSNotificationCenter defaultCenter] postNotificationName:PPClassicalEditorPreferencesDidChange object:self];
}

- (void)awakeFromNib
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	{
		NSInteger theRate = [defaults integerForKey:PPCETrackHeight];
		NSInteger toSet = 0;
		int i = 0;
		size_t sizeofCoupling = sizeof(TrackHeightCoupling) / sizeof(TagCoupling);
		for (i = 0; i < sizeofCoupling; i++) {
			if (TrackHeightCoupling[i].amount == theRate) {
				toSet = TrackHeightCoupling[i].tag;
				break;
			}
		}
		if (toSet == 0) {
			toSet = 4;
		}
		[trackHeightButton selectItemAtIndex:toSet - 1];
	}

	[tempoUnitValue setIntegerValue:[defaults integerForKey:PPCETempoUnit]];
	[tempoNumberValue setIntegerValue:[defaults integerForKey:PPCETempoNum]];
	[markersLoopValue setIntegerValue:[defaults integerForKey:PPCEMarkerLoop]];
	[markersOffsetValue setIntegerValue:[defaults integerForKey:PPCEMarkerOffset]];
	[notesLengthCheck setState:[defaults boolForKey:PPCEShowNotesLen]];
	
	BOOL markersVal = [defaults boolForKey:PPCEShowMarkers];
	[markersLoopValue setEnabled:markersVal];
	[markersOffsetValue setEnabled:markersVal];
	[markersCheck setState:markersVal];
}

@end
