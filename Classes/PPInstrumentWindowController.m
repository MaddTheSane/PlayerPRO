//
//  PPInstrumentWindowController.m
//  PPMacho
//
//  Created by C.W. Betts on 12/31/12.
//
//

#import "PPInstrumentWindowController.h"
#import "PPInstrumentImporter.h"
//#import "PPInstrumentObject.h"
#include <PlayerPROCore/PPPlug.h>
#import "ARCBridge.h"

@interface PPInstrumentWindowController ()

@end

@implementation PPInstrumentWindowController

@synthesize importer;
@synthesize curMusic;

- (OSErr)testPcmdFileAtURL:(NSURL*)theURL
{
	OSErr err = noErr;
	Pcmd thePcmd;
	NSData *pcmdData = [[NSData alloc] initWithContentsOfURL:theURL];
	if (!pcmdData) {
		return MADReadingErr;
	}
	[pcmdData getBytes:&thePcmd length:sizeof(thePcmd)];
	PPBE32(&thePcmd.structSize);
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
	PPBE32(&thePcmd->structSize);
	PPBE16(&thePcmd->length);
	PPBE16(&thePcmd->posStart);
	PPBE16(&thePcmd->tracks);
	PPBE16(&thePcmd->trackStart);
	
	//TODO: put Pcmd data onto the music file
	
	free(thePcmd);
	
	return noErr;
}

- (void)colorsDidChange:(NSNotification*)aNot
{
	
}

- (id)initWithWindow:(NSWindow *)window
{
    self = [super initWithWindow:window];
    if (self) {
        // Initialization code here.
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(colorsDidChange:) name:PPColorsDidChange object:nil];
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

- (void)windowDidLoad
{
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

- (void)dealloc
{
	RELEASEOBJ(importer);
	
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	SUPERDEALLOC;
}

- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item
{
	
}

- (id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item
{
	
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
	
}

@end
