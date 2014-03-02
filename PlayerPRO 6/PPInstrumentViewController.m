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
#import "OpenPanelViewController.h"
#import "InstrumentInfoController.h"
#import "PPFilterPlugHandler.h"
#import "PPFilterPlugObject.h"
#import "PPDocument.h"
#include <PlayerPROCore/PPPlug.h>
#include <PlayerPROCore/RDriverInt.h>
#include "PPByteswap.h"
#import "PPErrors.h"
#import "UserDefaultKeys.h"
#import "PPApp_AppDelegate.h"

@interface PPInstrumentViewController ()
@property (weak) PPDriver *theDriver;
- (void)loadInstrumentsFromMusic;
@end

@implementation PPInstrumentViewController
@synthesize currentDocument;
@synthesize importer;
@synthesize theDriver;
@synthesize undoManager;
@synthesize filterHandler;

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

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil;
{
	if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
		// Initialization code here.
		NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
		[center addObserver:self selector:@selector(colorsDidChange:) name:PPColorsDidChange object:nil];
		instrumentInfo = [[InstrumentInfoController alloc] init];
		instrumentInfo.delegate = self;
		self.filterHandler = [(PPApp_AppDelegate*)[[NSApplication sharedApplication] delegate] filterHandler];
		self.importer = [(PPApp_AppDelegate*)[[NSApplication sharedApplication] delegate] instrumentPlugHandler];
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

#if 0
- (BOOL)importSampleFromURL:(NSURL *)sampURL makeUserSelectInstrument:(BOOL)selIns error:(out NSError *__autoreleasing*)theErr;
{
	//TODO: handle selIns
	OSType plugType = 0;
	OSErr theOSErr = [importer identifyInstrumentFile:sampURL type:&plugType];
	if (theOSErr != noErr) {
		if (theErr)
			*theErr = CreateErrorFromMADErrorType(theOSErr);
		
		return NO;
	};
	short theSamp = 0;
	short theIns = 0;
	theOSErr = [importer importInstrumentOfType:plugType instrument:theIns sample:&theSamp URL:sampURL];
	if (theOSErr != noErr) {
		if (theErr)
			*theErr = CreateErrorFromMADErrorType(theOSErr);
		
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
	NSMutableData *outData = [[NSMutableData alloc] init];
	if (!outData) {
		return MADNeedMemory;
	}
	
	int i, x;
	{
		__block InstrData *tempInstrData = calloc(sizeof(InstrData), MAXINSTRU);
		if (!tempInstrData) {
			return MADNeedMemory;
		}
		memcpy(tempInstrData, [currentDocument.wrapper internalMadMusicStruct]->fid, sizeof(InstrData) * MAXINSTRU);
		
		dispatch_apply(MAXINSTRU, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT , 0), ^(size_t x) {
			ByteSwapInstrData(&tempInstrData[x]);
		});
		[outData appendBytes:tempInstrData length:sizeof(InstrData) * MAXINSTRU];
		free(tempInstrData);
	}
	for (i = 0; i < MAXINSTRU ; i++)
	{
		for (x = 0; x < [currentDocument.wrapper internalMadMusicStruct]->fid[i].numSamples ; x++)
		{
			sData tempData, *curData = [currentDocument.wrapper internalMadMusicStruct]->sample[i * MAXSAMPLE +  x];
			sData32 writeData;
			memcpy(&tempData, curData, sizeof(sData));
			ByteSwapsData(&tempData);
			memcpy(&writeData, &tempData, sizeof(sData32));
			writeData.data = 0;
			[outData appendBytes:&writeData length:sizeof(sData32)];
			{
				Ptr dataData = malloc(curData->size);
				if (!dataData)
					return MADNeedMemory;
				
				memcpy(dataData, curData->data, curData->size);
				if (curData->amp == 16) {
					__block short *shortPtr = (short*) dataData;
					
					dispatch_apply(curData->size / 2, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT , 0), ^(size_t y) {
						PPBE16(&shortPtr[y]);
					});
				}
				[outData appendBytes:dataData length:curData->size];
				free(dataData);
			}
		}
	}
	BOOL successful = [outData writeToURL:outURL atomically:YES];
	return successful ? noErr : MADWritingErr;
}

- (BOOL)importInstrumentListFromURL:(NSURL *)insURL error:(out NSError *__autoreleasing*)theErr
{
	NSData *fileData = [NSData dataWithContentsOfURL:insURL];
	if (!fileData) {
		if (theErr) {
			*theErr = CreateErrorFromMADErrorType(MADReadingErr);
		}
		return NO;
	}
	short		x, i;
	long		inOutCount, filePos = 0;
	__block InstrData *tempInstrData;
	{
		tempInstrData = calloc(sizeof(InstrData), MAXINSTRU);
		if (tempInstrData == NULL) {
			if (theErr) {
				*theErr = CreateErrorFromMADErrorType(MADIncompatibleFile);
			}
			return NO;
		}
		
		// **** HEADER ***
		inOutCount = sizeof(InstrData) * MAXINSTRU;
		if ([fileData length] <= inOutCount) {
			if (theErr) {
				*theErr = CreateErrorFromMADErrorType(MADIncompatibleFile);
			}
			free(tempInstrData);
			return NO;
		}
		[fileData getBytes:tempInstrData range:NSMakeRange(filePos, inOutCount)];
		
		dispatch_apply(MAXINSTRU, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT , 0), ^(size_t x) {
			ByteSwapInstrData(&tempInstrData[x]);
		});
		filePos += inOutCount;
	}
	
	//Clean up old instruments
	
	sData **tmpsData = calloc(sizeof(sData*), MAXINSTRU * MAXSAMPLE);
	
	// **** INSTRUMENTS ***
	for (i = 0; i < MAXINSTRU ; i++) {
		for (x = 0; x < tempInstrData[i].numSamples ; x++) {
			sData	*curData;
			
			// ** Read Sample header **
			
			curData = tmpsData[i * MAXSAMPLE +  x] = (sData*)malloc(sizeof(sData));
			if (curData == NULL) {
				if (theErr) {
					*theErr = CreateErrorFromMADErrorType(MADNeedMemory);
				}
				free(tempInstrData);
				dispatch_apply(MAXSAMPLE * MAXINSTRU, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(size_t x) {
					if (tmpsData[x]) {
						if (tmpsData[x]->data) {
							free(tmpsData[x]->data);
						}
						free(tmpsData[x]);
					}
				});

				free(tmpsData);
				return NO;
			}
			
			inOutCount = sizeof(sData32);
			[fileData getBytes:curData range:NSMakeRange(filePos, inOutCount)];
			ByteSwapsData(curData);
			filePos += inOutCount;
			
			// ** Read Sample DATA
			
			curData->data = malloc(curData->size);
			if (curData->data == NULL)
			{
				if (theErr) {
					*theErr = CreateErrorFromMADErrorType(MADNeedMemory);
				}
				
				free(tempInstrData);
				dispatch_apply(MAXSAMPLE * MAXINSTRU, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(size_t x) {
					if (tmpsData[x]) {
						if (tmpsData[x]->data) {
							free(tmpsData[x]->data);
						}
						free(tmpsData[x]);
					}
				});
				
				free(tmpsData);
				return NO;
			}

			inOutCount = curData->size;
			[fileData getBytes:curData->data range:NSMakeRange(filePos, inOutCount)];
			filePos += inOutCount;
			if (curData->amp == 16) {
				__block short *shortPtr = (short*)curData->data;
				
				dispatch_apply(inOutCount / 2, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT , 0), ^(size_t y) {
					PPBE16(&shortPtr[y]);
				});
			}
		}
	}
	// *********************
		
	if (theErr) {
		*theErr = nil;
	}
#if 0
	for (x = 0; x < MAXINSTRU ; x++) MADKillInstrument(*curMusic, x);
	memcpy((*curMusic)->fid, tempInstrData, inOutCount);
	free(tempInstrData);

	dispatch_apply(MAXSAMPLE * MAXINSTRU, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(size_t x) {
		if ((*curMusic)->sample[x]) {
			if ((*curMusic)->sample[x]->data) {
				free((*curMusic)->sample[x]->data);
			}
			free((*curMusic)->sample[x]);
		}
	});
	free((*curMusic)->sample);
	(*curMusic)->sample = tmpsData;
	
	(*curMusic)->hasChanged = TRUE;
	[instrumentView reloadData];
	[instrumentView selectRowIndexes:[NSIndexSet indexSetWithIndex:0] byExtendingSelection:NO];
	[instrumentView scrollToBeginningOfDocument:nil];
#endif
	
	return YES;
}
#endif

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
		if ([self importSampleFromURL:[openPanel URL] makeUserSelectInstrument:NO error:&err] == NO)
		{
			[[NSAlert alertWithError:err] runModal];
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
	NSInteger tag = [sender tag];
	short sampNum = tag % MAXSAMPLE;
	short instrNum = tag / MAXSAMPLE;
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
			BE = start + ((i+1) * sampleSize) / larg;
			
			if (isStereo) {
				BS /=2;
				BS *=2;
				BE /=2;
				BE *=2;
				
				BS += channel;
				BE += channel;
			}
			
			temp =(theShortSample[BS]  + 0x8000);
			minY = maxY = temp;
			temp *= high;
			temp  /= (1 << 16);
			CGContextAddLineToPoint(ctxRef, trueH + i, temp + trueV);
			
			if (BS != BE) {
				for (x = BS; x < BE; x++) {
					temp = (theShortSample[x]  + 0x8000);
					
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
			
			temp = (unsigned char) (theSample[BS] - 0x80);
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
		[waveFormImage setImage:nil];
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
	[waveFormImage setImage:[self waveformImageFromSample:object]];
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
