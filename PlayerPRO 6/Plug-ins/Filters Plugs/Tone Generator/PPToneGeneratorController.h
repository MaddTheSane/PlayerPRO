//
//  PPToneGeneratorController.h
//  PPMacho
//
//  Created by C.W. Betts on 8/10/13.
//
//

#import <Cocoa/Cocoa.h>
#include <PlayerPROCore/PlayerPROCore.h>
@import PlayerPROKit.PPPlugIns;

typedef NS_ENUM(NSInteger, ToneGenerator)
{
	silence = 3,
	triangle = 4,
	square = 5,
	wave = 6
};

@interface PPToneGeneratorController : NSWindowController
@property (weak) IBOutlet NSMatrix *stereoOrMono;
@property (weak) IBOutlet NSMatrix *audioBitRate;
@property (weak) IBOutlet NSButton *silentRadio;
@property (weak) IBOutlet NSButton *triangleRadio;
@property (weak) IBOutlet NSButton *squareRadio;
@property (weak) IBOutlet NSButton *waveRadio;
@property (strong) PPSampleObject *theData;
@property (weak) PPDriver *theDriver;
@property NSInteger selectionStart;
@property NSInteger selectionEnd;
@property BOOL stereoMode;
@property int audioLength;
@property double audioAmplitude;
@property int audioFrequency;
@property ToneGenerator generator;
@property (strong) PPPlugErrorBlock currentBlock;

- (IBAction)toggleToneType:(id)sender;

- (IBAction)okay:(id)sender;
- (IBAction)cancel:(id)sender;

@end
