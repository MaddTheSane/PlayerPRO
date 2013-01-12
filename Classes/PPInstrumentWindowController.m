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
}

- (void)dealloc
{
	RELEASEOBJ(importer);
	RELEASEOBJ(instruments);
	
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	SUPERDEALLOC;
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

@end
