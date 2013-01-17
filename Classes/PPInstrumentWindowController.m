//
//  PPInstrumentWindowController.m
//  PPMacho
//
//  Created by C.W. Betts on 12/31/12.
//
//

#import "PPInstrumentWindowController.h"
#import "PPInstrumentImporter.h"
#import "PPInstrumentObject.h"
#import "PPSampleObject.h"
#import "PPInstrumentCellView.h"
#include <PlayerPROCore/PPPlug.h>
#import "ARCBridge.h"

@interface PPInstrumentWindowController ()
- (void)loadInstrumentsFromMusic;

@end

static inline void SwapPcmd(Pcmd *toswap)
{
	if (!toswap) {
		return;
	}
	PPBE32(&toswap->structSize);
	PPBE16(&toswap->length);
	PPBE16(&toswap->posStart);
	PPBE16(&toswap->tracks);
	PPBE16(&toswap->trackStart);
}

@implementation PPInstrumentWindowController

@synthesize importer;
@synthesize curMusic;

- (void)setCurMusic:(MADMusic **)acurMusic
{
	curMusic = acurMusic;
	[self loadInstrumentsFromMusic];
}

- (OSErr)testPcmdFileAtURL:(NSURL*)theURL
{
	OSErr err = noErr;
	Pcmd thePcmd;
	NSData *pcmdData = [[NSData alloc] initWithContentsOfURL:theURL];
	if (!pcmdData) {
		return MADReadingErr;
	}
	[pcmdData getBytes:&thePcmd length:sizeof(thePcmd)];
	SwapPcmd(&thePcmd);
	if (thePcmd.structSize != [pcmdData length]) {
		err = MADIncompatibleFile;
	}
	RELEASEOBJ(pcmdData);
	return err;
}

- (OSErr)importPcmdFromURL:(NSURL*)theURL
{
	OSErr theErr = noErr;
	theErr = [self testPcmdFileAtURL:theURL];
	if (theErr) {
		return theErr;
	}
	Pcmd *thePcmd;
	NSData *pcmdData = [[NSData alloc] initWithContentsOfURL:theURL];
	if (!pcmdData) {
		return MADReadingErr;
	}
	NSInteger pcmdLen = [pcmdData length];
	
	thePcmd = malloc(pcmdLen);
	if (!thePcmd) {
		RELEASEOBJ(pcmdData);
		return MADNeedMemory;
	}
	[pcmdData getBytes:thePcmd length:pcmdLen];
	RELEASEOBJ(pcmdData);
	SwapPcmd(thePcmd);
	
	if (thePcmd->structSize != pcmdLen) {
		free(thePcmd);
		return MADIncompatibleFile;
	}
	
	//TODO: put Pcmd data onto the music file
	
	free(thePcmd);
	
	return noErr;
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
		
    }
    
    return self;
}

- (id)init
{
	return self = [self initWithWindowNibName:@"PPInstrumentWindowController"];
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

- (void)windowDidLoad
{
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
	[instrumentSize setTitleWithMnemonic:PPDoubleDash];
	[instrumentLoopStart setTitleWithMnemonic:PPDoubleDash];
	[instrumentLoopSize setTitleWithMnemonic:PPDoubleDash];
	[instrumentVolume setTitleWithMnemonic:PPDoubleDash];
	[instrumentRate setTitleWithMnemonic:PPDoubleDash];
	[instrumentNote setTitleWithMnemonic:PPDoubleDash];
	[instrumentBits setTitleWithMnemonic:PPDoubleDash];
	[instrumentMode setTitleWithMnemonic:PPDoubleDash];
}

- (void)dealloc
{
	RELEASEOBJ(importer);
	RELEASEOBJ(instruments);
	
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
	Ptr				theSample = malloc(sampleSize);
	memcpy(theSample, [curData.data bytes], sampleSize);
	short			*theShortSample = (short*) theSample;
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
	free(theSample);
	CGContextStrokePath(ctxRef);
	CGContextRestoreGState(ctxRef);
}


static void DataProviderReleaseCallback(void *info, const void *data,
										 size_t size)
{
	free((void*)data);
}

- (NSImage *)waveformImageFromSample:(PPSampleObject *)theDat
{
	NSSize imageSize = [waveFormImage convertSizeToBacking:[waveFormImage frame].size];
	imageSize.height *= 2;
	imageSize.width *= 2;
	CGImageRef theCGimg = NULL;
	NSUInteger rowBytes = 4 * imageSize.width;
	void *imageBuffer = malloc(rowBytes * imageSize.height);
	static CGColorSpaceRef defaultSpace = NULL;
	if (defaultSpace == NULL) {
		defaultSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	}
	
	CGContextRef bitmapContext = CGBitmapContextCreate(imageBuffer, imageSize.width, imageSize.height, 8, rowBytes, defaultSpace, kCGImageAlphaPremultipliedLast);
	CGContextClearRect(bitmapContext, CGRectMake(0, 0, imageSize.width, imageSize.height));
	CGContextSetLineWidth(bitmapContext, 1);
	if (theDat.stereo){
		CGColorRef colorRef = CGColorCreateGenericRGB(0, 0, 1, .75);
		CGContextSetStrokeColorWithColor(bitmapContext, colorRef);
		CGColorRelease(colorRef);
		DrawCGSampleInt(0, 0, imageSize.width, imageSize.height, imageSize.width, 0, 0, 1, theDat, bitmapContext);
	}
	{
		CGColorRef colorRef = CGColorCreateGenericRGB(1, 0, 0, theDat.stereo ? .75 : 1);
		CGContextSetStrokeColorWithColor(bitmapContext, colorRef);
		CGColorRelease(colorRef);
		DrawCGSampleInt(0, 0, imageSize.width, imageSize.height, imageSize.width, 0, 0, 0, theDat, bitmapContext);
	}

	CGContextRelease(bitmapContext);
	CGDataProviderRef dataProvider = CGDataProviderCreateWithData(NULL, imageBuffer, rowBytes * imageSize.height, DataProviderReleaseCallback);
	
	theCGimg = CGImageCreate(imageSize.width, imageSize.height, 8, 32, rowBytes, defaultSpace, kCGImageAlphaPremultipliedLast, dataProvider, NULL, true, kCGRenderingIntentDefault);
	CGDataProviderRelease(dataProvider);

	NSImage *img = [[NSImage alloc] initWithCGImage:theCGimg size:[waveFormImage frame].size];
	CGImageRelease(theCGimg);

	return AUTORELEASEOBJ(img);
}

+ (NSString*)getStringFromSize:(int)theSize
{
	//TODO: localize
	if (theSize > 1000 * 1000) {
		//This should never happen!
		[NSString stringWithFormat:@"%.2f MiB", theSize / (1024.0 * 1024.0)];
	} else if (theSize > 1000) {
		return [NSString stringWithFormat:@"%.2f kiB", theSize / 1024.0];
	} else {
		return [NSString stringWithFormat:@"%d Bytes", theSize];
	}
}

- (void)outlineViewSelectionDidChange:(NSNotification *)notification
{
	id object = [instrumentView itemAtRow:[instrumentView selectedRow]];
	
	if ([object isKindOfClass:[PPInstrumentObject class]]) {
		if ([object sampleCount] > 0) {
			object = [object childAtIndex:0];
		}else {
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
	[instrumentSize setTitleWithMnemonic:[PPInstrumentWindowController getStringFromSize:[object dataSize]]];
	[instrumentLoopStart setTitleWithMnemonic:[PPInstrumentWindowController getStringFromSize:[object loopBegin]]];
	[instrumentLoopSize setTitleWithMnemonic:[PPInstrumentWindowController getStringFromSize:[object loopSize]]];
	[instrumentVolume setTitleWithMnemonic:[NSString stringWithFormat:@"%d", [(PPSampleObject*)object volume]]];
	[instrumentRate setTitleWithMnemonic:[NSString stringWithFormat:@"%d", [object c2spd]]];
	[instrumentNote setTitleWithMnemonic:[NSString stringWithFormat:@"%d", [object relativeNote]]];
	[instrumentBits setTitleWithMnemonic:[NSString stringWithFormat:@"%d", [object amplitude]]];
	[instrumentMode setTitleWithMnemonic:[NSString stringWithFormat:@"%d", [object loopType]]];
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
	if ([item isKindOfClass:[PPInstrumentObject class]]) {
		theView.isSample = NO;
		[theView.textField setTitleWithMnemonic:[item name]];
		[theView.numField setTitleWithMnemonic:[NSString stringWithFormat:@"%03ld", (long)[item number] + 1]];

	} else if ([item isKindOfClass:[PPSampleObject class]])
	{
		theView.isSample = YES;
		[theView.textField setTitleWithMnemonic:[item name]];
		if ([item loopSize]) {
			theView.isLoopingSample = YES;
		} else {
			theView.isLoopingSample = NO;
		}
	}
	
	return theView;
}

@end
