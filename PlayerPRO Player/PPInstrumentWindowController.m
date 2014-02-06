//
//  PPInstrumentWindowController.m
//  PPMacho
//
//  Created by C.W. Betts on 12/31/12.
//
//

#import "PPInstrumentWindowController.h"
#import "PPInstrumentObject.h"
#import "PPSampleObject.h"
#import "PPInstrumentCellView.h"
#import "OpenPanelViewController.h"
#import "InstrumentInfoController.h"
#import "PPFilterPlugObject.h"
#include <PlayerPROCore/PPPlug.h>
#include <PlayerPROCore/RDriverInt.h>
#include "PPByteswap.h"
#import "PPErrors.h"
#import "UserDefaultKeys.h"
#import "AppDelegate.h"

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

- (id)initWithWindow:(NSWindow *)window
{
    if (self = [super initWithWindow:window]) {
        // Initialization code here.
		[[NSApp delegate] addObserver:self forKeyPath:@"music" options:(NSKeyValueObservingOptionNew | NSKeyValueObservingOptionOld) context:nil];
    }
    
    return self;
}

- (IBAction)toggleInfo:(id)sender
{
	[infoDrawer toggle:sender];
}

- (id)init
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
	[[NSApp delegate] removeObserver:self forKeyPath:@"music"];
}

#pragma mark NSOutlineView delegates and data ref calls

static void DrawCGSampleInt(long start, long tSS, long tSE, long high, long larg, long trueV, long trueH, short channel, PPSampleObject *curData, CGContextRef ctxRef)
{
	CGContextSaveGState(ctxRef);
	
	long		i;
	size_t		sampleSize = [curData.data length];
	CGFloat		temp;
	const char	*theSample = [curData.data bytes];
	const short	*theShortSample = (const short*)theSample;
	long		BS, BE, x;
	BOOL		isStereo = curData.stereo;
	CGFloat		minY, maxY;
	
	if (curData.amplitude == 16) {
		sampleSize /= 2;
		start /= 2;
		
		BS = start + (tSS * sampleSize) / larg;
		if (isStereo) {
			BS /= 2;
			BS *=2;
			BS += channel;
		}
		temp = (theShortSample[BS]  + 0x8000);
		temp *= high;
		temp  /= (1 << 16);
		CGContextMoveToPoint(ctxRef, trueH + tSS, trueV + temp);
		
		for (i = tSS; i < tSE; i++) {
			BS = start + (i * sampleSize) / larg;
			BE = start + ((i + 1) * sampleSize) / larg;
			
			if (isStereo) {
				BS /=2;
				BS *=2;
				BE /=2;
				BE *=2;
				
				BS += channel;
				BE += channel;
			}
			
			temp =(theShortSample[BS] + 0x8000);
			minY = maxY = temp;
			temp *= high;
			temp /= (1 << 16);
			CGContextAddLineToPoint(ctxRef, trueH + i, temp + trueV);
			
			if (BS != BE) {
				for (x = BS; x < BE; x++) {
					temp = (theShortSample[x] + 0x8000);
					
					if (temp > maxY)
						maxY = temp;
					if (temp < minY)
						minY = temp;
					
					if (isStereo)
						x++;
				}
				
				maxY *= high;
				maxY /= (1 << 16);
				minY *= high;
				minY /= (1 << 16);
				
				CGContextMoveToPoint(ctxRef, trueH + i, minY + trueV);
				CGContextAddLineToPoint(ctxRef, trueH + i, maxY + trueV);
			}
		}
	} else {
		BS = start + (tSS * sampleSize) / larg;
		if (isStereo) {
			BS /= 2;
			BS *=2;
			BS += channel;
		}
		
		temp = (unsigned char)(theSample[BS] - 0x80);
		temp *= high;
		temp /= (1 << 8);
		
		CGContextMoveToPoint(ctxRef, trueH + tSS, trueV + temp);
		
		for (i = tSS; i < tSE; i++) {
			BS = start + (i * sampleSize) / larg;
			BE = start + ((i + 1) * sampleSize) / larg;
			
			if (isStereo) {
				BS /=2;
				BS *=2;
				BE /=2;
				BE *=2;
				
				BS += channel;
				BE += channel;
			}
			
			temp = (unsigned char)(theSample[BS] - 0x80);
			minY = maxY = temp;
			temp *= high;
			temp /= (1 << 8);
			CGContextAddLineToPoint(ctxRef, trueH + i, temp + trueV);
			
			if (BS != BE) {
				for (x = BS; x < BE; x++) {
					temp = (unsigned char)(theSample[x] - 0x80);
					
					if (temp > maxY)
						maxY = temp;
					if (temp < minY)
						minY = temp;
					
					if (isStereo)
						x++;
				}
				maxY *= high;
				maxY /= (1 << 8);
				minY *= high;
				minY /= (1 << 8);
				
				CGContextMoveToPoint(ctxRef, trueH + i, minY + trueV);
				CGContextAddLineToPoint(ctxRef, trueH + i, maxY + trueV);
			}
		}
	}
	CGContextStrokePath(ctxRef);
	CGContextRestoreGState(ctxRef);
}

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
	if (datIsStereo){
		CGColorRef colorRef = CGColorCreateGenericRGB(0, 0, 1, .75);
		CGContextSetStrokeColorWithColor(bitmapContext, colorRef);
		CGColorRelease(colorRef);
		DrawCGSampleInt(0, 0, imageSize.width, imageSize.height, imageSize.width, 0, 0, 1, theDat, bitmapContext);
	}
	
	CGColorRef colorRef = CGColorCreateGenericRGB(1, 0, 0, datIsStereo ? 0.75 : 1);
	CGContextSetStrokeColorWithColor(bitmapContext, colorRef);
	CGColorRelease(colorRef);
	DrawCGSampleInt(0, 0, imageSize.width, imageSize.height, imageSize.width, 0, 0, 0, theDat, bitmapContext);
	
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
	[instrumentNote setStringValue:[NSString stringWithFormat:@"%d", [object relativeNote]]]; //TODO: properly set note.
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
