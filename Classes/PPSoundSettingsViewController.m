//
//  PPSoundSettingsViewController.m
//  PPMacho
//
//  Created by C.W. Betts on 1/17/13.
//
//

#import "PPSoundSettingsViewController.h"
#import "PPPreferences.h"

enum {
	rate11Khz,
	rate22Khz,
	rate44Khz,
	rate48Khz
};

enum {
	bits8,
	bits16,
	bits20,
	bits24
};

static const TagCoupling StereoDelayCoupling[] = {{1, 1}, {10, 2}, {25, 3}, {30, 4} ,{35, 5},
	{40, 6}, {45, 7}, {50, 8}, {60, 9}, {70, 10}, {80, 11}, {90, 12}, {100, 13}, {200, 14}};

static const TagCoupling ReverbAmountCoupling[] = {{25, 1}, {30, 2}, {35, 3}, {40, 4}, {45, 5},
	{50, 6}, {60, 7}, {70, 8}, {80, 9}, {90, 10}, {100, 11}, {200, 12}, {300, 13}, {400, 14}};

static const TagCoupling ReverbPercentCoupling[] = {{10, 1}, {20, 2}, {30, 3}, {40, 4},
	{50, 5}, {60, 6}, {70, 7}};

static const TagCoupling OversamplingCoupling[] = {{1, 1}, {2, 2}, {3, 3}, {4, 4},
	{6, 5}, {8, 6}, {10, 7}, {16, 8}, {20, 9}, {25, 10}, {30, 11}};

@interface PPSoundSettingsViewController ()
@property BOOL reverbActive;
@property BOOL surroundActive;
@property BOOL stereoDelayActive;
@property BOOL oversamplingActive;
@end

@implementation PPSoundSettingsViewController
@synthesize outputBits;
@synthesize oversampling;
@synthesize oversamplingActive;
@synthesize oversamplingNum;
@synthesize rate;
@synthesize reverb;
@synthesize reverbActive;
@synthesize reverbNum;
@synthesize reverbPercent;
@synthesize soundDriver;
@synthesize stereoDelay;
@synthesize stereoDelayActive;
@synthesize stereoDelayNum;
@synthesize surround;
@synthesize surroundActive;

@synthesize delegate;

-(void)setCurrentSoundDriver:(NSInteger)theDriver
{
	switch (theDriver) {
		case CoreAudioDriver:
			[soundDriver selectCellAtRow:0 column:0];
			break;
			
		case SoundManagerDriver:
			[soundDriver selectCellAtRow:1 column:0];
			break;
			
		case MIDISoundDriver:
			[soundDriver selectCellAtRow:2 column:0];
			break;
			
		default:
			[soundDriver selectCellAtRow:3 column:0];
			break;
	}
}

-(void)setCurrentBits:(NSInteger)bits
{
	switch (bits) {
		case bits8:
			[outputBits selectCellAtRow:0 column:0];
			break;
			
		case bits16:
		default:
			[outputBits selectCellAtRow:0 column:1];
			break;
			
		case bits20:
			[outputBits selectCellAtRow:0 column:2];
			break;
			
		case bits24:
			[outputBits selectCellAtRow:0 column:3];
			break;
	}
}

-(void)setCurrentRate:(NSInteger)currRate
{
	switch (currRate) {
		case rate11Khz:
			[rate selectCellAtRow:0 column:0];
			break;
			
		case rate22Khz:
			[rate selectCellAtRow:0 column:1];
			break;
			
		case rate44Khz:
		default:
			[rate selectCellAtRow:0 column:2];
			break;
			
		case rate48Khz:
			[rate selectCellAtRow:0 column:3];
			break;
	}
}

- (void)settingsFromDriverSettings:(MADDriverSettings*)sett
{
	BOOL oversamplingState = sett->oversampling > 1;
	BOOL reverbState = sett->Reverb;
	BOOL stereoDelayState = sett->MicroDelaySize > 0;
	
	reverbActive = reverbState;
	oversamplingActive = oversamplingState;
	stereoDelayActive = stereoDelayState;
	surroundActive = sett->surround;
	
	if ([self view]) {
		[oversampling setState:oversamplingState];
		[reverb setState:reverbState];
		[stereoDelay setState:stereoDelayState];
		[surround setState:sett->surround];
		
		[oversamplingNum setEnabled:oversamplingState];
		[reverbNum setEnabled:reverbState];
		[reverbPercent setEnabled:reverbState];
		[stereoDelayNum setEnabled:stereoDelayState];
		
		[self setCurrentSoundDriver:sett->driverMode];
		
		{
			NSInteger unConvBits = sett->outPutBits, ConvBits;
			switch (unConvBits) {
				case 8:
					ConvBits = bits8;
					break;
					
				default:
				case 16:
					ConvBits = bits16;
					break;
					
				case 20:
					ConvBits = bits20;
					break;
					
				case 24:
					ConvBits = bits24;
					break;
					
			}
			[self setCurrentBits:ConvBits];
		}
		{
			NSInteger unConvRate = sett->outPutRate, convRate;
			switch (unConvRate) {
				case 11025:
					convRate = rate11Khz;
					break;
					
				case 22050:
					convRate = rate22Khz;
					break;
					
				default:
				case 44100:
					convRate = rate44Khz;
					break;
					
				case 48000:
					convRate = rate48Khz;
					break;
			}
			[self setCurrentRate:convRate];
		}
		{
			NSInteger theRate = sett->MicroDelaySize;
			NSInteger toSet = 0;
			int i = 0;
			size_t sizeofCoupling = sizeof(StereoDelayCoupling) / sizeof(TagCoupling);
			for (i = 0; i < sizeofCoupling; i++) {
				if (StereoDelayCoupling[i].amount == theRate) {
					toSet = StereoDelayCoupling[i].tag;
					break;
				}
			}
			if (toSet == 0) {
				toSet = 4;
			}
			[stereoDelayNum selectItemAtIndex:toSet - 1];
		}
		{
			NSInteger reverbAmount = sett->ReverbSize;
			NSInteger toSet = 0;
			int i = 0;
			size_t sizeofCoupling = sizeof(ReverbAmountCoupling) / sizeof(TagCoupling);
			for (i = 0; i < sizeofCoupling; i++) {
				if (ReverbAmountCoupling[i].amount == reverbAmount) {
					toSet = ReverbAmountCoupling[i].tag;
					break;
				}
			}
			if (toSet == 0) {
				toSet = 4;
			}
			[reverbNum selectItemAtIndex:toSet - 1];
		}
		{
			NSInteger reverbPercentage = sett->ReverbStrength;
			NSInteger toSet = 0;
			int i = 0;
			size_t sizeofCoupling = sizeof(ReverbPercentCoupling) / sizeof(TagCoupling);
			for (i = 0; i < sizeofCoupling; i++) {
				if (ReverbPercentCoupling[i].amount == reverbPercentage) {
					toSet = ReverbPercentCoupling[i].tag;
					break;
				}
			}
			if (toSet == 0) {
				toSet = 3;
			}
			[reverbPercent selectItemAtIndex:toSet - 1];
		}
		{
			NSInteger oversamplingAmount = sett->oversampling;
			NSInteger toSet = 0;
			int i = 0;
			size_t sizeofCoupling = sizeof(OversamplingCoupling) / sizeof(TagCoupling);
			for (i = 0; i < sizeofCoupling; i++) {
				if (OversamplingCoupling[i].amount == oversamplingAmount) {
					toSet = OversamplingCoupling[i].tag;
					break;
				}
			}
			if (toSet == 0) {
				toSet = 1;
			}
			[oversamplingNum selectItemAtIndex:toSet - 1];
		}
		if (![delegate respondsToSelector:@selector(soundOutDriverDidChange:)]) {
			[soundDriver setEnabled:NO];
		}
	}
}

- (id)init
{
	return [self initWithNibName:@"PPSoundSettingsViewController" bundle:nil];
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
	self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
	if (self) {
		// Initialization code here.
	}
	
	return self;
}

- (NSInteger)currentRate
{
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
		return rate44Khz;
	}
}

- (NSInteger)currentSoundDriver
{
	id curSelected = [soundDriver selectedCell];
	if ([soundDriver cellAtRow:0 column:0] == curSelected) {
		return CoreAudioDriver;
	} else if ([soundDriver cellAtRow:1 column:0] == curSelected) {
		return SoundManagerDriver;
	} else if ([soundDriver cellAtRow:2 column:0] == curSelected) {
		return MIDISoundDriver;
	} else {
		return NoHardwareDriver;
	}
}

- (NSInteger)currentBits
{
	id curSelected = [outputBits selectedCell];
	if ([outputBits cellAtRow:0 column:0] == curSelected) {
		return bits8;
	} else if ([outputBits cellAtRow:0 column:1] == curSelected) {
		return bits16;
	} else if ([outputBits cellAtRow:0 column:2] == curSelected) {
		return bits20;
	} else if ([outputBits cellAtRow:0 column:3] == curSelected) {
		return bits24;
	} else {
		return bits16;
	}
}

- (IBAction)changeBits:(id)sender
{
	NSInteger theBits = [self currentBits];
	NSInteger returnBits;
	switch (theBits) {
		case bits8:
			returnBits = 8;
			break;
			
		default:
		case bits16:
			returnBits = 16;
			break;
			
		case bits20:
			returnBits = 20;
			break;
			
		case bits24:
			returnBits = 24;
			break;
	}
	if (delegate) {
		[delegate soundOutBitsDidChange:returnBits];
	}
}

- (IBAction)changeDriver:(id)sender
{
	NSInteger driver = [self currentSoundDriver];
	if (delegate && [delegate respondsToSelector:@selector(soundOutDriverDidChange:)]) {
		[delegate soundOutDriverDidChange:driver];
	}
}

- (IBAction)changeRate:(id)sender
{
	NSInteger rate1 = [self currentRate];
	unsigned int returnRate;
	switch (rate1) {
		case rate11Khz:
			returnRate = 11025;
			break;
			
		case rate22Khz:
			returnRate = 22050;
			break;
			
		default:
		case rate44Khz:
			returnRate = 44100;
			break;
			
		case rate48Khz:
			returnRate = 48000;
			break;
	}
	if (delegate) {
		[delegate soundOutRateDidChange:returnRate];
	}
}

- (short)stereoDelayFromTag:(NSInteger)theTag
{
	int toSet = 0;
	int i = 0;
	size_t sizeofCoupling = sizeof(StereoDelayCoupling) / sizeof(TagCoupling);
	for (i = 0; i < sizeofCoupling; i++) {
		if (StereoDelayCoupling[i].tag == theTag) {
			toSet = StereoDelayCoupling[i].amount;
			break;
		}
	}
	if (toSet == 0) {
		toSet = 30;
	}
	
	return toSet;
}

- (short)oversamplingFromTag:(NSInteger)theTag
{
	int toSet = 0;
	int i = 0;
	size_t sizeofCoupling = sizeof(OversamplingCoupling) / sizeof(TagCoupling);
	for (i = 0; i < sizeofCoupling; i++) {
		if (OversamplingCoupling[i].tag == theTag) {
			toSet = OversamplingCoupling[i].amount;
			break;
		}
	}
	if (toSet == 0) {
		toSet = 1;
	}
	
	return toSet;
}

- (IBAction)changeChecked:(id)sender
{
	NSInteger reverbState = [reverb state];
	NSInteger stereoDelayState = [stereoDelay state];
	NSInteger oversamplingState = [oversampling state];
	BOOL surroundState = [surround state];
	
	[oversamplingNum setEnabled:oversamplingState];
	[reverbNum setEnabled:reverbState];
	[reverbPercent setEnabled:reverbState];
	[stereoDelayNum setEnabled:stereoDelayState];
	
	if (delegate) {
		if (!!reverbState != reverbActive) {
			[delegate soundOutReverbDidChangeActive:reverbState];
		}
		if (!!stereoDelayState != stereoDelayActive) {
			[delegate soundOutStereoDelayDidChangeActive:stereoDelayState];
			if (stereoDelayState) {
				[delegate soundOutStereoDelayAmountDidChange:[self stereoDelayFromTag:[[stereoDelayNum selectedItem] tag]]];
			}
		}
		if (!!oversamplingState != oversamplingActive) {
			[delegate soundOutOversamplingDidChangeActive:oversamplingState];
			if (oversamplingState) {
				[delegate soundOutOversamplingAmountDidChange:[self oversamplingFromTag:[[oversamplingNum selectedItem] tag]]];
			}
		}
		if (surroundState != surroundActive) {
			[delegate soundOutSurroundDidChangeActive:surroundState];
		}
	}
	reverbActive = reverbState;
	stereoDelayActive = stereoDelayState;
	oversamplingActive = oversamplingState;
	surroundActive = surroundState;
}

- (IBAction)changeOversampling:(id)sender
{
	int toSet = [self oversamplingFromTag:[sender tag]];
	if (delegate) {
		[delegate soundOutOversamplingAmountDidChange:toSet];
	}
}

- (IBAction)changeReverbAmount:(id)sender
{
	int toSet = 0;
	NSInteger tag = [sender tag];
	int i = 0;
	size_t sizeofCoupling = sizeof(ReverbAmountCoupling) / sizeof(TagCoupling);
	for (i = 0; i < sizeofCoupling; i++) {
		if (ReverbAmountCoupling[i].tag == tag) {
			toSet = ReverbAmountCoupling[i].amount;
			break;
		}
	}
	if (toSet == 0) {
		toSet = 25;
	}
	if (delegate) {
		[delegate soundOutReverbSizeDidChange:toSet];
	}
}

- (IBAction)changeReverbPercent:(id)sender
{
	int toSet = 0;
	NSInteger tag = [sender tag];
	int i = 0;
	size_t sizeofCoupling = sizeof(ReverbPercentCoupling) / sizeof(TagCoupling);
	for (i = 0; i < sizeofCoupling; i++) {
		if (ReverbPercentCoupling[i].tag == tag) {
			toSet = ReverbPercentCoupling[i].amount;
			break;
		}
	}
	if (toSet == 0) {
		toSet = 30;
	}
	if (delegate) {
		[delegate soundOutReverbStrengthDidChange:toSet];
	}
}

- (IBAction)changeStereoDelay:(id)sender
{
	int toSet = [self stereoDelayFromTag:[sender tag]];
	if (delegate) {
		[delegate soundOutStereoDelayAmountDidChange:toSet];
	}
}

@end
