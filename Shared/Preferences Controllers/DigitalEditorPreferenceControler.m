//
//  DigitalEditorPreferenceControler.m
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "DigitalEditorPreferenceControler.h"
#import "UserDefaultKeys.h"
#import "NSColor+PPPreferences.h"


@implementation DigitalEditorPreferenceControler
@synthesize PPDEArgument;
@synthesize PPDEDragAsMatrix;
@synthesize PPDEEffect;
@synthesize PPDEInstrument;
@synthesize PPDELineHeightMatrix;
@synthesize PPDEMarkerColor;
@synthesize PPDEMarkerLoop;
@synthesize PPDEMarkerOffset;
@synthesize PPDEMouseClickCommand;
@synthesize PPDEMouseClickControl;
@synthesize PPDEMouseClickOption;
@synthesize PPDEMouseClickShift;
@synthesize PPDEMusicTraceMatrix;
@synthesize PPDENote;
@synthesize PPDEPatternWrappingMatrix;
@synthesize PPDEVolume;

- (id)init
{
	if (self = [super initWithNibName:@"DigitalPrefs" bundle:nil]) {
		[self setTitle:NSLocalizedStringFromTable(@"Digital Editor", @"PreferenceNames", @"Digital Editor")];
	}
	return self;
}

- (BOOL)lineHeight
{
	id curSelected = [PPDELineHeightMatrix selectedCell];
	if ([PPDELineHeightMatrix cellAtRow:0 column:0] == curSelected) {
		return YES;
	} else if ([PPDELineHeightMatrix cellAtRow:0 column:1] == curSelected) {
		return NO;
	} else {
		return NO;
	}
}

- (BOOL)musicTrace
{
	id curSelected = [PPDEMusicTraceMatrix selectedCell];
	if ([PPDEMusicTraceMatrix cellAtRow:0 column:0] == curSelected) {
		return YES;
	} else if ([PPDEMusicTraceMatrix cellAtRow:0 column:1] == curSelected) {
		return NO;
	} else {
		return NO;
	}
}

- (BOOL)patternWrapping
{
	id curSelected = [PPDEPatternWrappingMatrix selectedCell];
	if ([PPDEPatternWrappingMatrix cellAtRow:0 column:0] == curSelected) {
		return NO;
	} else if ([PPDEPatternWrappingMatrix cellAtRow:0 column:1] == curSelected) {
		return YES;
	} else {
		return NO;
	}
}

- (BOOL)PPDEDragAs
{
	id curSelected = [PPDEDragAsMatrix selectedCell];
	if ([PPDEDragAsMatrix cellAtRow:0 column:0] == curSelected) {
		return YES;
	} else if ([PPDEDragAsMatrix cellAtRow:0 column:1] == curSelected) {
		return NO;
	} else {
		return NO;
	}
}

- (IBAction)cellInformationCheckClicked:(id)sender
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setBool:[PPDEEffect state] forKey:PPDEShowEffect];
	[defaults setBool:[PPDENote state] forKey:PPDEShowNote];
	[defaults setBool:[PPDEInstrument state] forKey:PPDEShowInstrument];
	[defaults setBool:[PPDEArgument state] forKey:PPDEShowArgument];
	[defaults setBool:[PPDEVolume state] forKey:PPDEShowVolume];

	[[NSNotificationCenter defaultCenter] postNotificationName:PPDigitalEditorPrefrencesDidChange object:self];
}

- (IBAction)markersCheckClicked:(id)sender
{
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPDigitalEditorPrefrencesDidChange object:self];
}

- (IBAction)mouseCheckClicked:(id)sender
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setBool:[PPDEMouseClickControl state] forKey:PPDEMouseClickControlPref];
	[defaults setBool:[PPDEMouseClickShift state] forKey:PPDEMouseClickShiftPref];
	[defaults setBool:[PPDEMouseClickCommand state] forKey:PPDEMouseClickCommandPref];
	[defaults setBool:[PPDEMouseClickOption state] forKey:PPDEMouseClickOptionPref];

	[[NSNotificationCenter defaultCenter] postNotificationName:PPDigitalEditorPrefrencesDidChange object:self];
}

- (IBAction)lineHeightClicked:(id)sender
{
	[[NSUserDefaults standardUserDefaults] setBool:[self lineHeight] forKey:PPDELineHeightNormal];
	[[NSNotificationCenter defaultCenter] postNotificationName:PPDigitalEditorPrefrencesDidChange object:self];
}

- (IBAction)musicTraceClicked:(id)sender
{
	[[NSUserDefaults standardUserDefaults] setBool:[self musicTrace] forKey:PPDEMusicTraceOn];
	[[NSNotificationCenter defaultCenter] postNotificationName:PPDigitalEditorPrefrencesDidChange object:self];
}

- (IBAction)patternWrappingClicked:(id)sender
{
	[[NSUserDefaults standardUserDefaults] setBool:[self patternWrapping] forKey:PPDEPatternWrappingPartition];
	[[NSNotificationCenter defaultCenter] postNotificationName:PPDigitalEditorPrefrencesDidChange object:self];
}

- (IBAction)dragAsClicked:(id)sender
{
	[[NSUserDefaults standardUserDefaults] setBool:[self PPDEDragAs] forKey:PPDEDragAsPcmd];
	[[NSNotificationCenter defaultCenter] postNotificationName:PPDigitalEditorPrefrencesDidChange object:self];
}

- (IBAction)changeColor:(id)sender
{
	[[NSUserDefaults standardUserDefaults] setObject:[[PPDEMarkerColor color] PPencodeColor] forKey:PPDEMarkerColorPref];
	[[NSNotificationCenter defaultCenter] postNotificationName:PPDigitalEditorPrefrencesDidChange object:self];
}

-(void)awakeFromNib
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[PPDEEffect setState:[defaults boolForKey:PPDEShowEffect]];
	[PPDENote setState:[defaults boolForKey:PPDEShowNote]];
	[PPDEInstrument setState:[defaults boolForKey:PPDEShowInstrument]];
	[PPDEArgument setState:[defaults boolForKey:PPDEShowArgument]];
	[PPDEVolume setState:[defaults boolForKey:PPDEShowVolume]];
	
	//[PPDEMarkerSelect setState:[defaults boolForKey:PPDEShowMarkers]];
	[PPDEMarkerOffset setStringValue:[defaults stringForKey:PPDEMarkerOffsetPref]];
	[PPDEMarkerLoop setStringValue:[defaults stringForKey:PPDEMarkerLoopPref]];
	[PPDEMarkerColor setColor:[NSColor PPDecodeColorWithData:[defaults dataForKey:PPDEMarkerColorPref]]];
	
	[PPDEMouseClickControl setState:[defaults boolForKey:PPDEMouseClickControlPref]];
	[PPDEMouseClickShift setState:[defaults boolForKey:PPDEMouseClickShiftPref]];
	[PPDEMouseClickCommand setState:[defaults boolForKey:PPDEMouseClickCommandPref]];
	[PPDEMouseClickOption setState:[defaults boolForKey:PPDEMouseClickOptionPref]];
	
	if ([defaults boolForKey:PPDELineHeightNormal]) {
		[PPDELineHeightMatrix selectCellAtRow:0 column:0];
	}else {
		[PPDELineHeightMatrix selectCellAtRow:0 column:1];
	}
	
	if ([defaults boolForKey:PPDEMusicTraceOn]) {
		[PPDEMusicTraceMatrix selectCellAtRow:0 column:0];
	}else {
		[PPDEMusicTraceMatrix selectCellAtRow:0 column:1];
	}

	if ([defaults boolForKey:PPDEPatternWrappingPartition]) {
		[PPDEPatternWrappingMatrix selectCellAtRow:0 column:1];
	}else {
		[PPDEPatternWrappingMatrix selectCellAtRow:0 column:0];
	}

	if ([defaults boolForKey:PPDEDragAsPcmd]) {
		[PPDEDragAsMatrix selectCellAtRow:0 column:0];
	}else {
		[PPDEDragAsMatrix selectCellAtRow:0 column:1];
	}
}

@end
