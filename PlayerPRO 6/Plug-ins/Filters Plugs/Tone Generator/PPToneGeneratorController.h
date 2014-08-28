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

typedef NS_ENUM(char, ToneGenerator)
{
	silence = 3,
	triangle = 4,
	square = 5,
	wave = 6
};

@interface PPToneGeneratorController : PPFilterPluginWindowController
@property (weak) IBOutlet NSMatrix *stereoOrMono;
@property (weak) IBOutlet NSMatrix *audioBitRate;
@property (weak) IBOutlet NSButton *silentRadio;
@property (weak) IBOutlet NSButton *triangleRadio;
@property (weak) IBOutlet NSButton *squareRadio;
@property (weak) IBOutlet NSButton *waveRadio;
@property long audioLength;
@property double audioAmplitude;
@property int audioFrequency;
@property ToneGenerator generator;

- (IBAction)toggleToneType:(id)sender;
@end
