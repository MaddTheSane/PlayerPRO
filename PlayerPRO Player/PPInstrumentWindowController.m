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

- (NSImage *)waveformImageFromSample:(PPSampleObject *)theDat
{
	NSSize imageSize = [waveFormImage convertSizeToBacking:[waveFormImage frame].size];
	BOOL datIsStereo = theDat.stereo;
	imageSize.height *= 2;
	imageSize.width *= 2;
	CGImageRef theCGimg = NULL;
	NSUInteger rowBytes = 4 * imageSize.width;
	static CGColorSpaceRef defaultSpace = NULL;
	if (defaultSpace == NULL)
		defaultSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	
	CGContextRef bitmapContext = CGBitmapContextCreateWithData(NULL, imageSize.width, imageSize.height, 8, rowBytes, defaultSpace, (CGBitmapInfo)kCGImageAlphaPremultipliedLast, NULL, NULL);
	CGContextClearRect(bitmapContext, CGRectMake(0, 0, imageSize.width, imageSize.height));
	NSSize lineSize = [waveFormImage convertSizeToBacking:NSMakeSize(1, 1)];
	CGContextSetLineWidth(bitmapContext, lineSize.height);
	CGRect aRect;
	aRect.size = imageSize;
	aRect.origin = NSMakePoint(0, 0);
	
	if (datIsStereo){
		CGColorRef colorRef = CGColorCreateGenericRGB(0, 0, 1, .75);
		CGContextSetStrokeColorWithColor(bitmapContext, colorRef);
		CGColorRelease(colorRef);
		[PPSampleObject drawSampleWithStart:0 tSS:0 rectangle:aRect channel:1 currentData:theDat context:bitmapContext];
	}
	
	CGColorRef colorRef = CGColorCreateGenericRGB(1, 0, 0, datIsStereo ? 0.75 : 1);
	CGContextSetStrokeColorWithColor(bitmapContext, colorRef);
	CGColorRelease(colorRef);
	[PPSampleObject drawSampleWithStart:0 tSS:0 tSE:imageSize.width high:imageSize.height larg:imageSize.width trueV:0 trueH:0 channel:0 currentData:theDat context:bitmapContext];
	
	if ([theDat loopSize]) {
		CGColorRef colorRef = CGColorCreateGenericRGB(1, 0.1, .5, 0.8);
		CGContextSetStrokeColorWithColor(bitmapContext, colorRef);
		CGColorRelease(colorRef);
		CGRect loopRect = CGRectMake(0, 0, imageSize.width, imageSize.height);
		NSSize lineSize = [waveFormImage convertSizeToBacking:NSMakeSize(2, 2)];
		NSSize padSize = [waveFormImage convertSizeToBacking:NSMakeSize(1, 1)];
		CGContextSetLineWidth(bitmapContext, lineSize.height);
		loopRect.origin.x =  ([theDat loopBegin] * imageSize.width / (double)[theDat.data length]);
		loopRect.origin.y += padSize.width;
		loopRect.size.width = [theDat loopSize] * imageSize.width / (double)[theDat.data length];
		loopRect.size.height -= padSize.width * 2;
		CGContextStrokeRect(bitmapContext, loopRect);
	}
	
	theCGimg = CGBitmapContextCreateImage(bitmapContext);
	CGContextRelease(bitmapContext);
	
	NSImage *img = [[NSImage alloc] initWithCGImage:theCGimg size:[waveFormImage frame].size];
	CGImageRelease(theCGimg);
	
	return img;
}

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
	[waveFormImage setImage:[self waveformImageFromSample:object]];
}

- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item
{
	if (item == nil) {
		return [((AppDelegate*)[NSApp delegate]).music.instruments count];
	}
	if ([item isKindOfClass:[PPInstrumentObject class]]) {
		return [item countOfSamples];
	}
	return 0;
}

- (id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item
{
	if (item == nil) {
		return ((AppDelegate*)[NSApp delegate]).music.instruments[index];
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
