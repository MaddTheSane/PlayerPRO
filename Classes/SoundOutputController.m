//
//  SoundOutputController.m
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "SoundOutputController.h"


@implementation SoundOutputController

enum {
	rate11Khz = 0,
	rate22Khz,
	rate44Khz,
	rate48Khz
};

enum {
	bits8 = 0,
	bits16,
	bits24
};

enum {
	soundDriverCoreAudio = 0,
	soundDriverCarbon,
	soundDriverCoreMIDI
};

-(NSInteger)currentRate {
	id curSelected = [rate selectedCell];
	if ([rate cellAtRow:0 column:0] == curSelected) {
		return rate11Khz;
	} else if ([rate cellAtRow:0 column:1] == curSelected) {
		return rate22Khz;
	} else if ([rate cellAtRow:0 column:2] == curSelected) {
		return rate44Khz;
	} else if ([rate cellAtRow:0 column:3] == curSelected) {
		return rate48Khz;
	} else {
		return -1;
	}
}

-(NSInteger)currentBits {
	id curSelected = [outputBits selectedCell];
	if ([outputBits cellAtRow:0 column:0] == curSelected) {
		return soundDriverCoreAudio;
	} else if ([outputBits cellAtRow:1 column:0] == curSelected) {
		return soundDriverCarbon;
	} else if ([outputBits cellAtRow:2 column:0] == curSelected) {
		return soundDriverCoreMIDI;
	} else {
		return -1;
	}
}

-(NSInteger)currentSoundDriver {
	id curSelected = [soundDriver selectedCell];
	if ([outputBits cellAtRow:0 column:0] == curSelected) {
		return bits8;
	} else if ([outputBits cellAtRow:0 column:1] == curSelected) {
		return bits16;
	} else if ([outputBits cellAtRow:0 column:2] == curSelected) {
		return bits24;
	} else {
		return -1;
	}
}

- (IBAction)changeBits:(id)sender {
    
}

- (IBAction)changeDriver:(id)sender {
    
}

- (IBAction)changeRate:(id)sender {
    
}

- (IBAction)changeChecked:(id)sender {
    
}

- (IBAction)changeOversampling:(id)sender {
    
}

- (IBAction)changeReverbAmount:(id)sender {
    
}

- (IBAction)changeReverbPercent:(id)sender {
    
}

- (IBAction)changeStereoDelay:(id)sender {
    
}

-(id)init {
	if (![super initWithNibName:@"SoundOutput" bundle:nil]) {
		return nil;
	}
	[self setTitle:@"Sound Output"];
	return self;
}

@end
