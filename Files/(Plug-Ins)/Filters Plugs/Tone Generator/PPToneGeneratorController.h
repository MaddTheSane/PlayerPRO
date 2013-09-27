//
//  PPToneGeneratorController.h
//  PPMacho
//
//  Created by C.W. Betts on 8/10/13.
//
//

#import <Cocoa/Cocoa.h>
#include <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROKit/PlayerPROKit.h>

typedef enum _ToneGenerator
{
	cancelTone,
	silence = 3,
	triangle = 4,
	square = 5,
	wave = 6
} ToneGenerator;

@interface PPToneGeneratorController : PPFilterPluginWindowController
{
	long audioLength;
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
@property (readwrite) long audioLength;
@property (readwrite) double audioAmplitude;
@property (readwrite) int audioFrequency;
@property (readwrite) ToneGenerator generator;

- (IBAction)toggleToneType:(id)sender;

@end
