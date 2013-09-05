//
//  PPToneGeneratorController.h
//  PPMacho
//
//  Created by C.W. Betts on 8/10/13.
//
//

#import <Cocoa/Cocoa.h>
#include "ToneGenerator.h"

@interface PPToneGeneratorController : NSWindowController
{
	sData *theData;
	PPInfoPlug *ppInfoPlug;
	long audioLength;
	long audioAmplitude;
	long audioFrequency;
	ToneGenerator generator;
	
	IBOutlet NSMatrix *stereoOrMono;
	IBOutlet NSMatrix *audioBitRate;
	BOOL disabledData;
	
	NSArray *toneRadios;
	IBOutlet NSButton *silentRadio;
	IBOutlet NSButton *triangleRadio;
	IBOutlet NSButton *squareRadio;
	IBOutlet NSButton *waveRadio;
}
@property (readwrite) sData *theData;
@property (readwrite) PPInfoPlug *ppInfoPlug;
@property (readwrite) long audioLength;
@property (readwrite) long audioAmplitude;
@property (readwrite) long audioFrequency;
@property (readwrite) ToneGenerator generator;

- (IBAction)okayOrCancel:(id)sender;
- (IBAction)toggleToneType:(id)sender;

@end
