//
//  PPInstrumentWindowController.m
//  PPMacho
//
//  Created by C.W. Betts on 12/31/12.
//
//

#import <PlayerPROKit/PlayerPROKit.h>
#import "PPInstrumentViewController.h"
#import "PPInstrumentPlugHandler.h"
#import "PPInstrumentImporterObject.h"
#import "PPInstrumentCellView.h"
#import "InstrumentInfoController.h"
#import "PPFilterPlugHandler.h"
#import "PPFilterPlugObject.h"
#import "PPDocument.h"
#include <PlayerPROCore/PPPlug.h>
#include <PlayerPROCore/RDriverInt.h>
#include "PPByteswap.h"
#import "UserDefaultKeys.h"
#import "PlayerPRO_6-Swift.h"

@interface PPInstrumentObject (Private)
- (InstrData)theInstrument;
- (void)setTheInstrument:(InstrData)theInstrument;
@end

@interface PPInstrumentViewController ()
@property (weak) PPDriver *theDriver;
- (void)loadInstrumentsFromMusic;
@end

@implementation PPInstrumentViewController
@synthesize currentDocument;
@synthesize importer;
@synthesize theDriver;
@synthesize instrumentOutline = instrumentView;
@synthesize filterHandler;
@synthesize infoDrawer;
@synthesize instrumentBits;
@synthesize instrumentLoopSize;
@synthesize instrumentLoopStart;
@synthesize instrumentMode;
@synthesize instrumentNote;
@synthesize instrumentRate;
@synthesize instrumentSize;
@synthesize instrumentVolume;

- (void)colorsDidChange:(NSNotification*)aNot
{
	
}

- (void)loadInstrumentsFromMusic
{
	if (instrumentView) {
		[instrumentView reloadData];
		[instrumentView selectRowIndexes:[NSIndexSet indexSetWithIndex:0] byExtendingSelection:NO];
		[instrumentView scrollToBeginningOfDocument:nil];
	}
}

- (void)musicDidChange:(NSNotification *)aNot
{
	[self loadInstrumentsFromMusic];
}

- (instancetype)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil;
{
	if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
		// Initialization code here.
		NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
		[center addObserver:self selector:@selector(colorsDidChange:) name:PPColorsDidChange object:nil];
		instrumentInfo = [[InstrumentInfoController alloc] init];
		instrumentInfo.delegate = self;
		self.filterHandler = [(AppDelegate*)[[NSApplication sharedApplication] delegate] filterHandler];
		self.importer = [(AppDelegate*)[[NSApplication sharedApplication] delegate] instrumentPlugHandler];
	}
	
	return self;
}

- (IBAction)toggleInfo:(id)sender
{
	[infoDrawer toggle:sender];
}

- (BOOL)importSampleFromURL:(NSURL *)sampURL
{
	return [self importSampleFromURL:sampURL error:NULL];
}

- (BOOL)importSampleFromURL:(NSURL *)sampURL error:(out NSError *__autoreleasing*)theErr;
{
	return [self importSampleFromURL:sampURL makeUserSelectInstrument:NO error:theErr];
}

- (BOOL)importSampleFromURL:(NSURL *)sampURL makeUserSelectInstrument:(BOOL)selIns error:(out NSError *__autoreleasing*)theErr;
{
	//TODO: handle selIns
	OSType plugType = 0;
	OSErr theOSErr = [importer identifyInstrumentFile:sampURL type:&plugType];
	if (theOSErr != MADNoErr) {
		if (theErr)
			*theErr = PPCreateErrorFromMADErrorType(theOSErr);
		
		return NO;
	};
	short theSamp = 0;
	short theIns = 0;

	InstrData *tmpInstr = &[currentDocument.wrapper internalMadMusicStruct]->fid[theIns];
	sData **tmpsData = &[currentDocument.wrapper internalMadMusicStruct]->sample[theIns];
	theOSErr = [importer importInstrumentOfType:plugType instrumentReference:tmpInstr sampleReference:tmpsData sample:&theSamp URL:sampURL plugInfo:NULL];
	if (theOSErr != MADNoErr) {
		if (theErr)
			*theErr = PPCreateErrorFromMADErrorType(theOSErr);
		
		return NO;
	} else {
		if (theErr)
			*theErr = nil;
		
		PPInstrumentObject *insObj = [[PPInstrumentObject alloc] initWithMusic:currentDocument.wrapper instrumentIndex:theIns];
		[self replaceObjectInInstrumentsAtIndex:theIns withObject:insObj];
		[instrumentView reloadData];
		//(*curMusic)->hasChanged = TRUE;
		return YES;
	}
}

- (OSErr)exportInstrumentListToURL:(NSURL*)outURL
{
	return [currentDocument.wrapper exportInstrumentListToURL:outURL];
}

- (BOOL)importInstrumentListFromURL:(NSURL *)insURL error:(out NSError *__autoreleasing*)theErr
{
	return [currentDocument.wrapper importInstrumentListFromURL:insURL error:theErr];
}

- (IBAction)importInstrument:(id)sender
{
	NSInteger plugCount = [importer plugInCount];
	NSMutableDictionary *fileDict = [NSMutableDictionary dictionaryWithCapacity:plugCount];
	for (PPInstrumentImporterObject *obj in importer) {
		fileDict[obj.menuName] = obj.UTITypes;
	}
	NSOpenPanel *openPanel = [NSOpenPanel openPanel];
	OpenPanelViewController *vc = [[OpenPanelViewController alloc] initWithOpenPanel:openPanel trackerDictionary:nil playlistDictionary:nil instrumentDictionary:fileDict additionalDictionary:nil];
	[vc setupDefaults];
	if ([openPanel runModal] == NSFileHandlingPanelOKButton) {
		NSError *err;
		if ([self importSampleFromURL:[openPanel URL] makeUserSelectInstrument:NO error:&err] == NO) {
			[[NSAlert alertWithError:err] beginSheetModalForWindow:[self.currentDocument windowForSheet] completionHandler:^(NSModalResponse returnCode) {
				//do nothing
			}];
		}
	}
}

- (IBAction)exportInstrument:(id)sender
{
	
}

- (IBAction)deleteInstrument:(id)sender
{
	
}

- (IBAction)deleteSample:(id)sender
{
	
}

- (IBAction)playInstrument:(id)sender
{
	NSControl *tmpCtrl = [[NSControl alloc] initWithFrame:NSZeroRect];
	
	id object = [instrumentView itemAtRow:[instrumentView selectedRow]];
	
	if ([object isKindOfClass:[PPInstrumentObject class]]) {
		if ([object countOfSamples] > 0) {
			object = [object childAtIndex:0];
		} else {
			object = nil;
		}
	}

	if (object == nil) {
		NSBeep();
		return;
	}
	
	[tmpCtrl setTag:[object instrumentIndex] * MAXSAMPLE + [object sampleIndex]];
	[self playSample:tmpCtrl];
}

- (IBAction)showInstrumentInfo:(id)sender
{
	PPInstrumentObject *ctxt;
	id object = [instrumentView itemAtRow:[instrumentView selectedRow]];
	if ([object isKindOfClass:[PPInstrumentObject class]]) {
		ctxt = object;
	} else if ([object isKindOfClass:[PPSampleObject class]]) {
		ctxt = currentDocument.wrapper.instruments[[object instrumentIndex]];
	}
	instrumentInfo.instrument = ctxt;
	
	[NSApp beginSheet:[instrumentInfo window] modalForWindow:[self.currentDocument windowForSheet] modalDelegate:instrumentInfo didEndSelector:@selector(instrumentSheetDidEnd:returnCode:contextInfo:) contextInfo:NULL];
}

- (void)playSampleFromInstrument:(short)insNum sampleNumber:(short)sampNum volume:(Byte)theVol note:(Byte)theNote
{
	
}

- (void)playSampleFromInstrument:(short)insNum sampleNumber:(short)sampNum
{
	[self playSampleFromInstrument:insNum sampleNumber:sampNum volume:0xFF note:0xFF];
}

- (IBAction)playSample:(id)sender
{
#if 0
	NSInteger tag = [sender tag];
	short sampNum = tag % MAXSAMPLE;
	short instrNum = tag / MAXSAMPLE;
#endif
}

- (void)awakeFromNib
{
	[super awakeFromNib];
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
	[instrumentView selectRowIndexes:[NSIndexSet indexSetWithIndex:0] byExtendingSelection:NO];
	self.theDriver = self.currentDocument.theDriver;
}

- (void)dealloc
{	
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}

#pragma mark NSOutlineView delegates and data ref calls

- (NSImage *)waveformImageFromSample:(PPSampleObject *)theDat
{
	NSSize imageSize = [self.waveFormImage convertSizeToBacking:[self.waveFormImage frame].size];
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
	NSSize lineSize = [self.waveFormImage convertSizeToBacking:NSMakeSize(1, 1)];
	CGContextSetLineWidth(bitmapContext, lineSize.height);
		CGRect aRect;
		aRect.size = imageSize;
		aRect.origin = NSMakePoint(0, 0);
	if (datIsStereo){
		CGColorRef colorRef = CGColorCreateGenericRGB(0, 0, 1, .75);
		CGContextSetStrokeColorWithColor(bitmapContext, colorRef);
		CGColorRelease(colorRef);
		[PPSampleObject drawSampleWithStart:0 tSS:0 rectangle:aRect channel:1 currentData:theDat context:bitmapContext];
		//[PPSampleObject drawSampleWithStart:0 tSS:0 tSE:imageSize.width high:imageSize.height larg:imageSize.width trueV:0 trueH:0 channel:1 currentData:theDat context:bitmapContext];
	}
	
	CGColorRef colorRef = CGColorCreateGenericRGB(1, 0, 0, datIsStereo ? 0.75 : 1);
	CGContextSetStrokeColorWithColor(bitmapContext, colorRef);
	CGColorRelease(colorRef);
	//[PPSampleObject drawSampleWithStart:0 tSS:0 tSE:imageSize.width high:imageSize.height larg:imageSize.width trueV:0 trueH:0 channel:0 currentData:theDat context:bitmapContext];
	[PPSampleObject drawSampleWithStart:0 tSS:0 rectangle:aRect channel:0 currentData:theDat context:bitmapContext];

	if ([theDat loopSize]) {
		CGColorRef colorRef = CGColorCreateGenericRGB(1, 0.1, .5, 0.8);
		CGContextSetStrokeColorWithColor(bitmapContext, colorRef);
		CGColorRelease(colorRef);
		CGRect loopRect = CGRectMake(0, 0, imageSize.width, imageSize.height);
		NSSize lineSize = [self.waveFormImage convertSizeToBacking:NSMakeSize(2, 2)];
		NSSize padSize = [self.waveFormImage convertSizeToBacking:NSMakeSize(1, 1)];
		CGContextSetLineWidth(bitmapContext, lineSize.height);
		loopRect.origin.x =  ([theDat loopBegin] * imageSize.width / (double)[theDat.data length]);
		loopRect.origin.y += padSize.width;
		loopRect.size.width = [theDat loopSize] * imageSize.width / (double)[theDat.data length];
		loopRect.size.height -= padSize.width * 2;
		CGContextStrokeRect(bitmapContext, loopRect);
	}
	
	theCGimg = CGBitmapContextCreateImage(bitmapContext);
	CGContextRelease(bitmapContext);
	
	NSImage *img = [[NSImage alloc] initWithCGImage:theCGimg size:[self.waveFormImage frame].size];
	CGImageRelease(theCGimg);
	
	return img;
}

- (void)outlineViewSelectionDidChange:(NSNotification *)notification
{
	id object = [instrumentView itemAtRow:[instrumentView selectedRow]];
	
	if ([object isKindOfClass:[PPInstrumentObject class]]) {
		if ([object countOfSamples] > 0)
			object = [object childAtIndex:0];
		else
			object = nil;
		
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
		self.waveFormImage.image = nil;
		return;
	}
	[instrumentSize setIntegerValue:[[object data] length]];
	[instrumentLoopStart setIntegerValue:[object loopBegin]];
	[instrumentLoopSize setIntegerValue:[object loopSize]];
	[instrumentVolume setIntegerValue:[(PPSampleObject*)object volume]];
	[instrumentRate setStringValue:[NSString stringWithFormat:@"%u Hz", [object c2spd]]];
	[instrumentNote setStringValue:[NSString stringWithFormat:@"%d", [object relativeNote]]]; //TODO: properly set note.
	[instrumentBits setStringValue:[NSString stringWithFormat:@"%u-bit", [object amplitude]]];
	[instrumentMode setStringValue:([object loopType] == ePingPongLoop ? @"Ping-pong" : @"Classic")];
	self.waveFormImage.image = [self waveformImageFromSample:object];
}

- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item
{
	if (item == nil) {
		return [currentDocument.wrapper.instruments count];
	}
	if ([item isKindOfClass:[PPInstrumentObject class]]) {
		return [item countOfSamples];
	}
	return 0;
}

- (id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item
{
	if (item == nil) {
		return currentDocument.wrapper.instruments[index];
	}
	if ([item isKindOfClass:[PPInstrumentObject class]]) {
		return [item childAtIndex:index];
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

- (NSView *)outlineView:(NSOutlineView *)outlineView viewForTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
	PPInstrumentCellView *theView = [outlineView makeViewWithIdentifier:[tableColumn identifier] owner:nil];
	theView.controller = self;
	if ([item isKindOfClass:[PPInstrumentObject class]]) {
		theView.isSample = NO;
		[theView.textField setStringValue:[item name]];
		[theView.numField setStringValue:[NSString stringWithFormat:@"%03ld", (long)[item number] + 1]];
		theView.isBlank = [item countOfSamples] <= 0;
	} else if ([item isKindOfClass:[PPSampleObject class]]) {
		theView.isSample = YES;
		[theView.textField setStringValue:[item name]];
		if ([item loopSize]) {
			theView.isLoopingSample = YES;
		} else {
			theView.isLoopingSample = NO;
		}
		[theView.sampleButton setTag:[item instrumentIndex] * MAXSAMPLE + [item sampleIndex]];
		theView.isBlank = NO;
	}
	
	return theView;
}

- (void)replaceObjectInInstrumentsAtIndex:(NSUInteger)index withObject:(id)object
{
	currentDocument.wrapper.instruments[index] = object;
	//(*curMusic)->hasChanged = TRUE;
}

@end
