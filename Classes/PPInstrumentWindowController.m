//
//  PPInstrumentWindowController.m
//  PPMacho
//
//  Created by C.W. Betts on 12/31/12.
//
//

#import "PPInstrumentWindowController.h"
#import "PPInstrumentImporter.h"
#import "PPInstrumentImporterObject.h"
#import "PPInstrumentObject.h"
#import "PPSampleObject.h"
#import "PPInstrumentCellView.h"
#import "OpenPanelViewController.h"
#import "InstrumentInfoController.h"
#import "PPFilterPlugHandler.h"
#import "PPFilterPlugObject.h"
#include <PlayerPROCore/PPPlug.h>
#include <PlayerPROCore/RDriverInt.h>
#import "PPErrors.h"
#import "ARCBridge.h"

@interface PPInstrumentWindowController ()
- (void)loadInstrumentsFromMusic;

@end

@implementation PPInstrumentWindowController

@synthesize importer;
@synthesize curMusic;
@synthesize theDriver;
@synthesize undoManager;
@synthesize filterHandler;

- (void)setCurMusic:(MADMusic **)acurMusic
{
	curMusic = acurMusic;
	[self loadInstrumentsFromMusic];
}

- (void)colorsDidChange:(NSNotification*)aNot
{
	
}

- (void)writeInstrumentsBackToMusic
{
	for (PPInstrumentObject *obj in instruments) {
		[obj writeBackToMusic];
	}
}

- (void)writeInstrumentAtIndexBackToMusic:(short)idx
{
	[[instruments objectAtIndex:idx] writeBackToMusic];
}

- (void)writeSampleAtIndex:(short)sampIdx withInstrumentAtIndexBackToMusic:(short)insIdx
{
	PPInstrumentObject *obj = [instruments objectAtIndex:insIdx];
	[obj writeSampleAtIndexBackToMusic:sampIdx];
}

- (void)loadInstrumentsFromMusic
{
	int i;
	[instruments removeAllObjects];
	for (i = 0; i < MAXINSTRU; i++) {
		PPInstrumentObject *obj = [[PPInstrumentObject alloc] initWithMusic:*curMusic instrumentIndex:i];
		[instruments addObject:obj];
		RELEASEOBJ(obj);
	}
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

- (id)initWithWindow:(NSWindow *)window
{
    self = [super initWithWindow:window];
    if (self) {
        // Initialization code here.
		NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
		[center addObserver:self selector:@selector(colorsDidChange:) name:PPColorsDidChange object:nil];
		[center addObserver:self selector:@selector(musicDidChange:) name:PPMusicDidChange object:nil];
		instruments = [[NSMutableArray alloc] initWithCapacity:MAXINSTRU];
		instrumentInfo = [[InstrumentInfoController alloc] init];
		instrumentInfo.delegate = self;
    }
    
    return self;
}

- (id)init
{
	return self = [self initWithWindowNibName:@"PPInstrumentWindowController"];
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
	if (theOSErr != noErr)
	{
		if (theErr) {
			*theErr = AUTORELEASEOBJ(CreateErrorFromMADErrorType(theOSErr));
		}
		return NO;
	};
	short theSamp = 0;
	short theIns = 0;
	theOSErr = [importer importInstrumentOfType:plugType instrument:theIns sample:&theSamp URL:sampURL];
	if (theOSErr != noErr) {
		if (theErr) {
			*theErr = AUTORELEASEOBJ(CreateErrorFromMADErrorType(theOSErr));
		}
		return NO;
	} else {
		if (theErr) {
			*theErr = nil;
		}
		PPInstrumentObject *insObj = [[PPInstrumentObject alloc] initWithMusic:*curMusic instrumentIndex:theIns];
		[self replaceObjectInInstrumentsAtIndex:theIns withObject:insObj];
		RELEASEOBJ(insObj);
		[instrumentView reloadData];
		(*curMusic)->hasChanged = TRUE;
		return YES;
	}
}

static inline void ByteSwapInstrData(InstrData *toSwap)
{
	PPBE16( &toSwap->numSamples);
	PPBE16( &toSwap->firstSample);
	PPBE16( &toSwap->volFade);
	
	PPBE16( &toSwap->MIDI);
	PPBE16( &toSwap->MIDIType);
	__block InstrData *blockHappy = toSwap;
	dispatch_apply(12, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT , 0), ^(size_t x) {
		PPBE16( &blockHappy->volEnv[ x].pos);
		PPBE16( &blockHappy->volEnv[ x].val);
		
		PPBE16( &blockHappy->pannEnv[ x].pos);
		PPBE16( &blockHappy->pannEnv[ x].val);
		
		PPBE16( &blockHappy->pitchEnv[ x].pos);
		PPBE16( &blockHappy->pitchEnv[ x].val);
	});
}

static inline void ByteSwapsData(sData *toSwap)
{
	PPBE32( &toSwap->size);
	PPBE32( &toSwap->loopBeg);
	PPBE32( &toSwap->loopSize);
	PPBE16( &toSwap->c2spd);
}

- (OSErr)exportInstrumentListToURL:(NSURL*)outURL
{
	NSMutableData *outData = [[NSMutableData alloc] init];
	int i, x;
	__block InstrData *tempInstrData = calloc(sizeof(InstrData), MAXINSTRU);
	memcpy(tempInstrData, (*curMusic)->fid, sizeof(InstrData) * MAXINSTRU);
	
	dispatch_apply(MAXINSTRU, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT , 0), ^(size_t x) {
		ByteSwapInstrData(&tempInstrData[x]);
	});
	[outData appendBytes:tempInstrData length:sizeof(InstrData)* MAXINSTRU];
	free(tempInstrData);
	for( i = 0; i < MAXINSTRU ; i++)
	{
		for( x = 0; x < (*curMusic)->fid[ i].numSamples ; x++)
		{
			sData *tempData = malloc(sizeof(sData)), *curData = (*curMusic)->sample[ i * MAXSAMPLE +  x];
			sData32 writeData;
			memcpy(tempData, curData, sizeof(sData));
			ByteSwapsData(tempData);
			memcpy(&writeData, tempData, sizeof(sData32));
			writeData.data = 0;
			[outData appendBytes:&writeData length:sizeof(sData32)];
			free(tempData);
			{
				Ptr dataData = malloc(curData->size);
				memcpy(dataData, curData->data, curData->size);
				if (curData->amp == 16) {
					__block short	*shortPtr = (short*) dataData;
					
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
	RELEASEOBJ(outData);
	return successful ? noErr : MADWritingErr;
}

- (BOOL)importInstrumentListFromURL:(NSURL *)insURL error:(out NSError *__autoreleasing*)theErr
{
	NSData *fileData = [NSData dataWithContentsOfURL:insURL];
	if (!fileData) {
		if (theErr) {
			*theErr = AUTORELEASEOBJ(CreateErrorFromMADErrorType(MADReadingErr));
		}
		return NO;
	}
	short		x, i;
	long		inOutCount, filePos = 0;
	
	__block InstrData *tempInstrData = calloc(sizeof(InstrData), MAXINSTRU);
	
	// **** HEADER ***
	inOutCount = sizeof( InstrData) * MAXINSTRU;
	if ([fileData length] <= inOutCount) {
		if (theErr) {
			*theErr = AUTORELEASEOBJ(CreateErrorFromMADErrorType(MADIncompatibleFile));
		}
		return NO;
	}
	[fileData getBytes:tempInstrData range:NSMakeRange(filePos, inOutCount)];
	
	dispatch_apply(MAXINSTRU, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT , 0), ^(size_t x) {
		ByteSwapInstrData(&tempInstrData[x]);
	});
			
	for( x = 0; x < MAXINSTRU ; x++) MADKillInstrument( *curMusic, x);

	memcpy((*curMusic)->fid, tempInstrData, inOutCount);
	filePos += inOutCount;
	free(tempInstrData);
	
	//Clean up old instruments
	dispatch_apply(MAXSAMPLE * MAXINSTRU, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(size_t x) {
		if ((*curMusic)->sample[x]) {
			if ((*curMusic)->sample[x]->data) {
				free((*curMusic)->sample[x]->data);
			}
			free((*curMusic)->sample[x]);
		}
	});
	
	// **** INSTRUMENTS ***
	for( i = 0; i < MAXINSTRU ; i++)
	{
		for( x = 0; x < (*curMusic)->fid[ i].numSamples ; x++)
		{
			sData	*curData;
			
			// ** Read Sample header **
			
			curData = (*curMusic)->sample[ i * MAXSAMPLE +  x] = (sData*) malloc( sizeof( sData));
			if( curData == NULL)
			{
				if (theErr) {
					*theErr = AUTORELEASEOBJ(CreateErrorFromMADErrorType(MADNeedMemory));
				}
				return NO;
			}
			
			inOutCount = sizeof( sData32);
			[fileData getBytes:curData range:NSMakeRange(filePos, inOutCount)];
			ByteSwapsData(curData);
			filePos += inOutCount;
			
			// ** Read Sample DATA
			
			curData->data = malloc( curData->size);
			if( curData->data == NULL)
			{
				if (theErr) {
					*theErr = AUTORELEASEOBJ(CreateErrorFromMADErrorType(MADNeedMemory));
				}
				return NO;
			}

			inOutCount = curData->size;
			[fileData getBytes:curData->data range:NSMakeRange(filePos, inOutCount)];
			filePos += inOutCount;
			if (curData->amp == 16) {
				__block short	*shortPtr = (short*) curData->data;
				
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
	(*curMusic)->hasChanged = TRUE;
	[instrumentView reloadData];
	[instrumentView selectRowIndexes:[NSIndexSet indexSetWithIndex:0] byExtendingSelection:NO];
	[instrumentView scrollToBeginningOfDocument:nil];
	
	return YES;
}

- (IBAction)importInstrument:(id)sender
{
	NSInteger plugCount = [importer plugInCount];
	NSMutableDictionary *fileDict = [NSMutableDictionary dictionaryWithCapacity:plugCount];
	NSInteger i;
	for (i = 0; i < plugCount; i++) {
		PPInstrumentImporterObject *obj = [importer plugInAtIndex:i];
		[fileDict setObject:obj.UTITypes forKey:obj.menuName];
	}
	NSOpenPanel *openPanel = RETAINOBJ([NSOpenPanel openPanel]);
	OpenPanelViewController *vc = [[OpenPanelViewController alloc] initWithOpenPanel:openPanel trackerDictionary:nil playlistDictionary:nil instrumentDictionary:fileDict additionalDictionary:nil];
	[vc setupDefaults];
	if ([openPanel runModal] == NSFileHandlingPanelOKButton) {
		NSError *err = nil;
		if ([self importSampleFromURL:[openPanel URL] makeUserSelectInstrument:NO error:&err] == NO)
		{
			NSAlert *alert = [NSAlert alertWithError:err];
			[alert runModal];
		}
	}
	
	RELEASEOBJ(vc);
	RELEASEOBJ(openPanel);
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
		if ([object sampleCount] > 0) {
			object = [object childAtIndex:0];
		} else {
			object = nil;
		}
	}

	if (object == nil) {
		NSBeep();
		RELEASEOBJ(tmpCtrl);
		return;
	}
	
	[tmpCtrl setTag:[object instrumentIndex] * MAXSAMPLE + [object sampleIndex]];
	[self playSample:tmpCtrl];
	RELEASEOBJ(tmpCtrl);
}

- (IBAction)showInstrumentInfo:(id)sender
{
	PPInstrumentObject *ctxt = nil;
	id object = [instrumentView itemAtRow:[instrumentView selectedRow]];
	if ([object isKindOfClass:[PPInstrumentObject class]]) {
		ctxt = object;
	} else if ([object isKindOfClass:[PPSampleObject class]]) {
		ctxt = [instruments objectAtIndex:[object instrumentIndex]];
	}
	instrumentInfo.instrument = ctxt;
	
	[NSApp beginSheet:[instrumentInfo window] modalForWindow:[self window] modalDelegate:instrumentInfo didEndSelector:@selector(sheetDidEnd:returnCode:contextInfo:) contextInfo:NULL];
}

- (IBAction)playSample:(id)sender
{
	NSInteger tag = [sender tag];
	short sampNum = tag % MAXSAMPLE;
	short instrNum = tag / MAXSAMPLE;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
	[instrumentView selectRowIndexes:[NSIndexSet indexSetWithIndex:0] byExtendingSelection:NO];
}

- (void)dealloc
{
	RELEASEOBJ(importer);
	RELEASEOBJ(instruments);
	RELEASEOBJ(instrumentInfo);
	RELEASEOBJ(undoManager);
	RELEASEOBJ(filterHandler);
	
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	SUPERDEALLOC;
}

static void DrawCGSampleInt(long 	start,
							long 	tSS,
							long 	tSE,
							long 	high,
							long	larg,
							long	trueV,
							long	trueH,
							short	channel,
							PPSampleObject	*curData,
							CGContextRef ctxRef)
{
	CGContextSaveGState(ctxRef);
	
	long long		i;
	long			sampleSize = [curData dataSize];
	CGFloat			temp;
	const char*		theSample = [curData.data bytes];
	const short		*theShortSample = (const short*) theSample;
	long long		BS, BE, x;
	BOOL isStereo = curData.stereo;
	CGFloat			minY, maxY;
	
	if( curData.amplitude == 16)
	{
		sampleSize /= 2;
		start /= 2;
		
		BS = start + (tSS * sampleSize) / larg;
		if( isStereo)
		{
			BS /= 2;	BS *=2;
			BS += channel;
		}
		temp = (theShortSample[ BS]  + 0x8000);
		temp *= high;	temp  /= (1 << 16);
		CGContextMoveToPoint(ctxRef, trueH + tSS, trueV + temp);
		
		for( i = tSS; i < tSE; i++)
		{
			BS = start + (i * sampleSize) / larg;
			BE = start + ((i+1) * sampleSize) / larg;
			
			if( isStereo)
			{
				BS /=2;		BS *=2;
				BE /=2;		BE *=2;
				
				BS += channel;
				BE += channel;
			}
			
			temp =(theShortSample[ BS]  + 0x8000);
			minY = maxY = temp;
			temp *= high;		temp  /= (1 << 16);
			CGContextAddLineToPoint(ctxRef, trueH + i, temp + trueV);
			
			if( BS != BE)
			{
				for( x = BS; x < BE; x++)
				{
					temp = (theShortSample[ x]  + 0x8000);
					
					if( temp > maxY) maxY = temp;
					if( temp < minY) minY = temp;
					
					if( isStereo) x++;
				}
				
				maxY *= high;		maxY /= (1 << 16);
				minY *= high;		minY /= (1 << 16);
				
				CGContextMoveToPoint(ctxRef, trueH + i, minY + trueV);
				CGContextAddLineToPoint(ctxRef, trueH + i, maxY + trueV);
			}
		}
	}
	else
	{
		BS = start + (tSS * sampleSize) / larg;
		if( isStereo)
		{
			BS /= 2;	BS *=2;
			BS += channel;
		}
		
		temp = (unsigned char) (theSample[ BS] - 0x80);
		temp *= high;	temp /= (1 << 8);
		
		CGContextMoveToPoint(ctxRef, trueH + tSS, trueV + temp);
		
		for( i = tSS; i < tSE; i++)
		{
			BS = start + (i * sampleSize) / larg;
			BE = start + ((i+1) * sampleSize) / larg;
			
			if( isStereo)
			{
				BS /=2;		BS *=2;
				BE /=2;		BE *=2;
				
				BS += channel;
				BE += channel;
			}
			
			temp = (unsigned char) (theSample[ BS] - 0x80);
			minY = maxY = temp;
			temp *= high;		temp /= (1 << 8);
			CGContextAddLineToPoint(ctxRef, trueH + i, temp + trueV);
			
			if( BS != BE)
			{
				for( x = BS; x < BE; x++)
				{
					temp = (unsigned char) (theSample[ x] - 0x80);
					
					if( temp > maxY) maxY = temp;
					if( temp < minY) minY = temp;
					
					if( isStereo) x++;
				}
				maxY *= high;		maxY /= (1 << 8);
				minY *= high;		minY /= (1 << 8);
				
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
	if (defaultSpace == NULL) {
		defaultSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	}
	
	CGContextRef bitmapContext = CGBitmapContextCreateWithData(NULL, imageSize.width, imageSize.height, 8, rowBytes, defaultSpace, kCGImageAlphaPremultipliedLast, NULL, NULL);
	CGContextClearRect(bitmapContext, CGRectMake(0, 0, imageSize.width, imageSize.height));
	{
		NSSize lineSize = [waveFormImage convertSizeToBacking:NSMakeSize(1, 1)];
		CGContextSetLineWidth(bitmapContext, lineSize.height);
	}
	if (datIsStereo){
		CGColorRef colorRef = CGColorCreateGenericRGB(0, 0, 1, .75);
		CGContextSetStrokeColorWithColor(bitmapContext, colorRef);
		CGColorRelease(colorRef);
		DrawCGSampleInt(0, 0, imageSize.width, imageSize.height, imageSize.width, 0, 0, 1, theDat, bitmapContext);
	}
	{
		CGColorRef colorRef = CGColorCreateGenericRGB(1, 0, 0, datIsStereo ? .75 : 1);
		CGContextSetStrokeColorWithColor(bitmapContext, colorRef);
		CGColorRelease(colorRef);
		DrawCGSampleInt(0, 0, imageSize.width, imageSize.height, imageSize.width, 0, 0, 0, theDat, bitmapContext);
	}
	if ([theDat loopSize])
	{
		CGColorRef colorRef = CGColorCreateGenericRGB(1, 0.1, .5, 0.8);
		CGContextSetStrokeColorWithColor(bitmapContext, colorRef);
		CGColorRelease(colorRef);
		CGRect loopRect = CGRectMake(0, 0, imageSize.width, imageSize.height);
		NSSize lineSize = [waveFormImage convertSizeToBacking:NSMakeSize(2, 2)];
		NSSize padSize = [waveFormImage convertSizeToBacking:NSMakeSize(1, 1)];
		CGContextSetLineWidth(bitmapContext, lineSize.height);
		loopRect.origin.x =  ([theDat loopBegin] * imageSize.width / (double)[theDat dataSize]);
		loopRect.origin.y += padSize.width;
		loopRect.size.width = [theDat loopSize] * imageSize.width / (double)[theDat dataSize];
		loopRect.size.height -= padSize.width * 2;
		CGContextStrokeRect(bitmapContext, loopRect);
	}

	theCGimg = CGBitmapContextCreateImage(bitmapContext);
	
	CGContextRelease(bitmapContext);
	
	NSImage *img = [[NSImage alloc] initWithCGImage:theCGimg size:[waveFormImage frame].size];
	CGImageRelease(theCGimg);

	return AUTORELEASEOBJ(img);
}

- (void)outlineViewSelectionDidChange:(NSNotification *)notification
{
	id object = [instrumentView itemAtRow:[instrumentView selectedRow]];
	
	if ([object isKindOfClass:[PPInstrumentObject class]]) {
		if ([object sampleCount] > 0) {
			object = [object childAtIndex:0];
		} else {
			object = nil;
		}
	}
	if (!object) {
		[instrumentSize setTitleWithMnemonic:PPDoubleDash];
		[instrumentLoopStart setTitleWithMnemonic:PPDoubleDash];
		[instrumentLoopSize setTitleWithMnemonic:PPDoubleDash];
		[instrumentVolume setTitleWithMnemonic:PPDoubleDash];
		[instrumentRate setTitleWithMnemonic:PPDoubleDash];
		[instrumentNote setTitleWithMnemonic:PPDoubleDash];
		[instrumentBits setTitleWithMnemonic:PPDoubleDash];
		[instrumentMode setTitleWithMnemonic:PPDoubleDash];
		[waveFormImage setImage:nil];
		return;
	}
	[instrumentSize setIntegerValue:[object dataSize]];
	[instrumentLoopStart setIntegerValue:[object loopBegin]];
	[instrumentLoopSize setIntegerValue:[object loopSize]];
	[instrumentVolume setTitleWithMnemonic:[NSString stringWithFormat:@"%u", [(PPSampleObject*)object volume]]];
	[instrumentRate setTitleWithMnemonic:[NSString stringWithFormat:@"%u", [object c2spd]]];
	[instrumentNote setTitleWithMnemonic:[NSString stringWithFormat:@"%d", [object relativeNote]]];
	[instrumentBits setTitleWithMnemonic:[NSString stringWithFormat:@"%u", [object amplitude]]];
	[instrumentMode setTitleWithMnemonic:[NSString stringWithFormat:@"%u", [object loopType]]];
	[waveFormImage setImage:[self waveformImageFromSample:object]];
}

- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item
{
	if (item == nil) {
		return [instruments count];
	}
	if ([item isKindOfClass:[PPInstrumentObject class]]) {
		return [item countOfChildren];
	}
	return 0;
}

- (id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item
{
	if (item == nil) {
		return [instruments objectAtIndex:index];
	}
	if ([item isKindOfClass:[PPInstrumentObject class]]) {
		return [item childAtIndex:index];
	}
	return nil;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
	if ([item isKindOfClass:[PPInstrumentObject class]]) {
		return [item countOfChildren] != 0;
	}
	return NO;
}

- (NSView *)outlineView:(NSOutlineView *)outlineView viewForTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
	PPInstrumentCellView *theView = [outlineView makeViewWithIdentifier:[tableColumn identifier] owner:nil];
	theView.controller = self;
	if ([item isKindOfClass:[PPInstrumentObject class]]) {
		theView.isSample = NO;
		[theView.textField setTitleWithMnemonic:[item name]];
		[theView.numField setTitleWithMnemonic:[NSString stringWithFormat:@"%03ld", (long)[item number] + 1]];
		theView.isBlank = [item countOfChildren] <= 0;
	} else if ([item isKindOfClass:[PPSampleObject class]]) {
		theView.isSample = YES;
		[theView.textField setTitleWithMnemonic:[item name]];
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
	[instruments replaceObjectAtIndex:index withObject:object];
	(*curMusic)->hasChanged = TRUE;
}

@end
