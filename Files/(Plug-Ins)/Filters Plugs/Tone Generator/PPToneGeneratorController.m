//
//  PPToneGeneratorController.m
//  PPMacho
//
//  Created by C.W. Betts on 8/10/13.
//
//

#import "PPToneGeneratorController.h"

@interface PPToneGeneratorController ()
@property (retain) NSArray *toneRadios;
@property BOOL disabledData;
@end

char *Audio8Ptr = NULL;
short *Audio16Ptr = NULL;

int RunToneGeneratorPlug(sData *theData, PPInfoPlug *thePPInfoPlug, long *audioLen, int stereoMode, int *audioAmp, int *audioFreq)
{
	PPToneGeneratorController *controller = [[PPToneGeneratorController alloc] initWithWindowNibName:@"PPToneGeneratorController"];
	controller.theData = theData;
	controller.ppInfoPlug = thePPInfoPlug;
	controller.audioLength = *audioLen;
	controller.audioAmplitude = (double)(*audioAmp) / 100.0;
	controller.audioFrequency = *audioFreq;
	
	NSInteger retVal = [NSApp runModalForWindow:controller.window];
	[controller close];
	if (retVal !=  cancelTone) {
		*audioLen = controller.audioLength;
		*audioAmp = controller.audioAmplitude * 100;
		*audioFreq = controller.audioFrequency;
	}
	if (Audio16Ptr) {
		free(Audio16Ptr); Audio16Ptr = NULL;
	}
	if (Audio8Ptr) {
		free(Audio8Ptr); Audio8Ptr = NULL;
	}
	return (int)retVal;
}

@implementation PPToneGeneratorController
@synthesize audioAmplitude;
@synthesize audioFrequency;
@synthesize audioLength;
@synthesize generator;
@synthesize ppInfoPlug;
@synthesize theData;
@synthesize toneRadios;
@synthesize disabledData;

- (id)initWithWindow:(NSWindow *)window
{
    self = [super initWithWindow:window];
    if (self) {
        disabledData = NO;
    }
    
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
	if( audioLength <= 0) audioLength = 2000;
	if (theData->stereo) {
		[stereoOrMono selectCellAtRow:0 column:1];
	} else {
		[stereoOrMono selectCellAtRow:0 column:0];
	}
	switch (theData->amp) {
		case 8:
			[audioBitRate selectCellAtRow:0 column:0];
			break;
			
		default:
		case 16:
			[audioBitRate selectCellAtRow:0 column:1];
			
			break;
	}
	
	if (theData->size > 0) {
		for (NSCell *cell in [[stereoOrMono cells] arrayByAddingObjectsFromArray:[audioBitRate cells]]) {
			[cell setEnabled:NO];
		}
		disabledData = YES;
	}
	self.toneRadios = @[silentRadio, triangleRadio, squareRadio, waveRadio];
}

- (IBAction)okayOrCancel:(id)sender
{
	if ([sender tag] != 1 && !disabledData) {
		if ([stereoOrMono selectedCell] == [stereoOrMono cellAtRow:0 column:0]) {
			theData->stereo = FALSE;
		} else {
			//SHOULD be true
			theData->stereo = TRUE;
		}
		if ([audioBitRate selectedCell] == [audioBitRate cellAtRow:0 column:0]) {
			theData->amp = 8;
		} else /*if ([audioBitRate selectedCell] == [audioBitRate cellAtRow:0 column:1])*/ {
			//also SHOULD be true
			theData->amp = 16;
		}
	}
	[NSApp stopModalWithCode:([sender tag] == 1) ? cancelTone : generator];
}

- (IBAction)toggleToneType:(id)sender
{
	generator = (int)[sender tag];
	for (NSButton *radio in toneRadios) {
		[radio setIntegerValue:(generator == [radio tag]) ? NSOnState : NSOffState];
	}
}

- (IBAction)playSample:(id)sender
{
	static RPlaySoundUPP mPlay;
	mPlay = ppInfoPlug->RPlaySound;
		
	switch( theData->amp)
	{
		case 8:
			if( Audio8Ptr != NULL) { free( Audio8Ptr);			Audio8Ptr = NULL;}
			Audio8Ptr	= CreateAudio8Ptr( audioLength, audioFrequency, audioAmplitude, generator, theData->stereo);
			if( Audio8Ptr != NULL)
			{
				mPlay( ppInfoPlug->driverRec, Audio8Ptr, audioLength, 0, 0xFF, theData->amp, 0, 0, theData->c2spd, theData->stereo);
			}
			break;
			
		case 16:
			if( Audio16Ptr != NULL) { free( (Ptr) Audio16Ptr);	Audio16Ptr = NULL;}
			Audio16Ptr	= CreateAudio16Ptr( audioLength, audioFrequency, audioAmplitude, generator, theData->stereo);
			if( Audio16Ptr != NULL)
			{
				mPlay( ppInfoPlug->driverRec, (Ptr) Audio16Ptr, audioLength*2, 0, 0xFF, theData->amp, 0, 0, theData->c2spd, theData->stereo);
			}
			break;
	}
}

@end
