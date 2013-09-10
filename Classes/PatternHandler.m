//
//  PatternHandler.m
//  PPMacho
//
//  Created by C.W. Betts on 2/3/13.
//
//

#import "PatternHandler.h"
#import "UserDefaultKeys.h"
#import "ARCBridge.h"

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

@implementation PatternHandler

@synthesize theMus;
@synthesize theRec;
@synthesize undoManager;

- (void)musicDidChange:(NSNotification *)aNot
{
	//TODO: load patterns into pattern list
	[patternList removeAllObjects];
}

- (id)initWithMusic:(MADMusic **)mus
{
	if (self = [super init]) {
		theMus = mus;
		[[NSNotificationCenter defaultCenter]  addObserver:self selector:@selector(musicDidChange:) name:PPMusicDidChange object:nil];
		patternList = [[NSMutableArray alloc] init];
	}
	return self;
}

- (void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
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

@end
