//
//  PPToneGeneratorController.m
//  PPMacho
//
//  Created by C.W. Betts on 8/10/13.
//
//

#import "PPToneGeneratorController.h"
#include <PlayerPROCore/MADPlug.h>
#include <math.h>
#include <tgmath.h>
@import PlayerPROKit;

#define KHZ 22254.54545

static Ptr CreateAudio8Ptr(long AudioLength, long AudioFreq, long AudioAmp, ToneGenerator AudioType, bool stereo)
{
	Ptr		Audio8Ptr;
	int		i, temp, inter = 0, x, dest;
	bool	UpDown;
	
	if (stereo)
		AudioLength *= 2L;
	
	Audio8Ptr = malloc(AudioLength);
	if (Audio8Ptr == NULL)
		return NULL;
	
	switch (AudioType) {
		case wave:
			for (i = 0, x = 0; i < AudioLength; i++, x++) {
				temp = 127.0 * sin((((double)x * (long double)AudioFreq * M_PI * 2.0) / KHZ));
				
				/** Amplitude resizing **/
				temp *= AudioAmp;
				temp /= 100;
				
				/** Overshoot **/
				if (temp >= 127)
					temp = 127;
				else if (temp <= -127 )
					temp = -127;
				
				Audio8Ptr[i] = temp;
				if (stereo) {
					i++;
					Audio8Ptr[i] = temp;
				}
			}
			break;
			
		case silence:
			memset(Audio8Ptr, 0, AudioLength);
			break;
			
		case square:
			for (i = 0, x = 0, dest = -1; i < AudioLength; i++) {
				if (i > dest) {
					x++;
					dest = (x * KHZ) / (AudioFreq * 2);
					if (stereo)
						dest *= 2;
					if (inter == -127)
						inter = 127;
					else
						inter = -127;
				}
				
				temp = inter;
				
				/** Amplitude resizing **/
				temp *= AudioAmp;
				temp /= 100;
				
				/** Overshoot **/
				if (temp >= 127) temp = 127;
				else if (temp <= -127 ) temp = -127;
				
				Audio8Ptr[i] = temp;
				if (stereo) {
					i++;
					Audio8Ptr[i] = temp;
				}
			}
			break;
			
		case triangle:
			UpDown = true;
			for (i = 0, x = 0, dest = -1; i < AudioLength; i++) {
				if (i > dest) {
					x++;
					dest = (x * KHZ) / (AudioFreq * 2);
					if (stereo) dest *= 2;
					inter = dest - i;
					
					UpDown = !UpDown;
				}
				
				if (UpDown)
					temp = (256 * (dest - i)) / inter;
				else
					temp = (256 * (inter - (dest - i))) / inter;
				
				temp -= 127;
				
				/** Amplitude resizing **/
				temp *= AudioAmp;
				temp /= 100;
				
				/** Overshoot **/
				if (temp >= 127)
					temp = 127;
				else if (temp <= -127 )
					temp = -127;
				
				Audio8Ptr[i] = temp;
				if (stereo) {
					i++;
					Audio8Ptr[i] = temp;
				}
			}
			break;
	}
	
	return Audio8Ptr;
}

static short* CreateAudio16Ptr(long AudioLength, long AudioFreq, long AudioAmp, ToneGenerator AudioType, bool stereo)
{
	short	*Audio16Ptr;
	int		i, temp, inter = 0, x, dest;
	bool	UpDown;
	
	if (stereo)
		AudioLength *= 2L;
	
	Audio16Ptr = (short*)malloc(AudioLength*2);
	if (Audio16Ptr == NULL)
		return NULL;
	
	switch (AudioType) {
		case wave:
			for (i = 0, x = 0; i < AudioLength; i++, x++) {
				temp = 32767.0 * sin((((double)x * (double)AudioFreq * M_PI * 2.0) / KHZ));
				
				/** Amplitude resizing **/
				temp *= AudioAmp;
				temp /= 100;
				
				/** Overshoot **/
				if (temp >= (short)0x7FFF)
					temp = 0x7FFF;
				else if (temp <= (short)0x8000 )
					temp = (short)0x8000;
				
				Audio16Ptr[i] = temp;
				if (stereo) {
					i++;
					Audio16Ptr[i] = temp;
				}
			}
			break;
			
		case silence:
			memset(Audio16Ptr, 0, AudioLength);
			break;
			
		case square:
			for (i = 0, x = 0, dest = -1; i < AudioLength; i++) {
				if (i > dest) {
					x++;
					dest = (x * KHZ) / (AudioFreq * 2);
					if (stereo)
						dest *= 2;
					if (inter == -32767)
						inter = 32767;
					else
						inter = -32767;
				}
				
				temp = inter;
				
				/** Amplitude resizing **/
				temp *= AudioAmp;
				temp /= 100;
				
				/** Overshoot **/
				if (temp >= (short)0x7FFF)
					temp = 0x7FFF;
				else if (temp <= (short)0x8000)
					temp = (short)0x8000;
				
				Audio16Ptr[i] = temp;
				if (stereo) {
					i++;
					Audio16Ptr[i] = temp;
				}
			}
			break;
			
		case triangle:
			UpDown = true;
			for (i = 0, x = 0, dest = -1; i < AudioLength; i++) {
				if (i > dest) {
					x++;
					dest = (x * KHZ) / (AudioFreq * 2);
					if (stereo)
						dest *= 2;
					inter = dest - i;
					
					UpDown = !UpDown;
				}
				
				if (UpDown)
					temp = (65535 * (dest - i)) / inter;
				else
					temp = (65535 * (inter - (dest - i))) / inter;
				
				temp -= 32767;
				
				/** Amplitude resizing **/
				temp *= AudioAmp;
				temp /= 100;
				
				/** Overshoot **/
				if (temp >= (short)0x7FFF)
					temp = 0x7FFF;
				else if (temp <= (short)0x8000 )
					temp = (short)0x8000;
				
				Audio16Ptr[i] = temp;
				if (stereo) {
					i++;
					Audio16Ptr[i] = temp;
				}
			}
			break;
	}
	
	return Audio16Ptr;
}

@interface PPToneGeneratorController ()
@property (strong) NSArray *toneRadios;
@property BOOL disabledData;
@property char *audio8Ptr;
@property short *audio16Ptr;
- (void)clearAudioPointers;
@end

@implementation PPToneGeneratorController
@synthesize audioAmplitude;
@synthesize audioFrequency;
@synthesize audioLength;
@synthesize generator;
@synthesize toneRadios;
@synthesize disabledData;
@synthesize audioBitRate;
@synthesize stereoOrMono;
@synthesize silentRadio;
@synthesize triangleRadio;
@synthesize squareRadio;
@synthesize waveRadio;
@synthesize audio8Ptr;
@synthesize audio16Ptr;
@synthesize theData;

- (instancetype)initWithWindow:(NSWindow *)window
{
	if (self = [super initWithWindow:window]) {
		
	}
	
	return self;
}

- (IBAction)cancel:(id)sender
{
	[(NSApplication*)NSApp endSheet:self.window];
	_currentBlock(MADUserCanceledErr);
}

- (IBAction)okay:(id)sender
{
	if (!disabledData) {
		if ([stereoOrMono selectedCell] == [stereoOrMono cellAtRow:0 column:0]) {
			theData.stereo = NO;
		} else {
			//SHOULD be true
			theData.stereo = YES;
		}
		if ([audioBitRate selectedCell] == [audioBitRate cellAtRow:0 column:0]) {
			theData.amplitude = 8;
		} else /*if ([audioBitRate selectedCell] == [audioBitRate cellAtRow:0 column:1])*/ {
			//also SHOULD be true
			theData.amplitude = 16;
		}
	}
	char *resultPtr;
	[self clearAudioPointers];
	
	switch (theData.amplitude) {
		case 8:
			audio8Ptr = CreateAudio8Ptr(audioLength, audioFrequency, audioAmplitude * 100, generator, theData.stereo);
			break;
			
		case 16:
		default:
			audio16Ptr = CreateAudio16Ptr(audioLength, audioFrequency, audioAmplitude * 100, generator, theData.stereo);
			audioLength *= 2;
			break;
	}
	
	if (theData.stereo)
		audioLength *= 2;
	NSMutableData *resultData = [[NSMutableData alloc] initWithLength:(theData.data.length - (self.selectionEnd - self.selectionStart) + audioLength)];
	resultPtr = resultData.mutableBytes;
	
	memcpy(resultPtr, theData.data.bytes, self.selectionStart);
	
	if (theData.amplitude == 8)
		memcpy(resultPtr + self.selectionStart, audio8Ptr, audioLength);
	else
		memcpy(resultPtr + self.selectionStart, audio16Ptr, audioLength);
	
	memcpy(resultPtr + self.selectionStart + audioLength, theData.data.bytes + self.selectionEnd, theData.data.length - self.selectionEnd);
	
	theData.data = [resultData copy];
	[(NSApplication*)NSApp endSheet:self.window];
	_currentBlock(MADNoErr);
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
	if (audioLength <= 0)
		audioLength = 2000;
	if (theData.stereo) {
		[stereoOrMono selectCellAtRow:0 column:1];
	} else {
		[stereoOrMono selectCellAtRow:0 column:0];
	}
	switch (theData.amplitude) {
		case 8:
			[audioBitRate selectCellAtRow:0 column:0];
			break;
			
		default:
		case 16:
			[audioBitRate selectCellAtRow:0 column:1];
			break;
	}
	
	if (theData.data != nil && theData.data.length > 0) {
		for (NSCell *cell in [[stereoOrMono cells] arrayByAddingObjectsFromArray:[audioBitRate cells]]) {
			[cell setEnabled:NO];
		}
		
		self.disabledData = YES;
	}
	self.toneRadios = @[silentRadio, triangleRadio, squareRadio, waveRadio];
}

- (IBAction)toggleToneType:(id)sender
{
	generator = (ToneGenerator)[sender tag];
	for (NSButtonCell *radio in toneRadios) {
		[radio setIntegerValue:(generator == [radio tag]) ? NSOnState : NSOffState];
	}
}

- (IBAction)playSample:(id)sender
{
	[self clearAudioPointers];
	
	switch(theData.amplitude) {
		case 8:
			audio8Ptr = CreateAudio8Ptr(audioLength, audioFrequency, audioAmplitude, generator, theData.stereo);
			if (audio8Ptr != NULL) {
				[_theDriver playSoundDataFromPointer:audio8Ptr withSize:audioLength fromChannel:0 amplitude:0xFF bitRate:theData.c2spd isStereo:theData.stereo];
			}
			break;
			
		case 16:
			audio16Ptr	= CreateAudio16Ptr(audioLength, audioFrequency, audioAmplitude, generator, theData.stereo);
			if (audio16Ptr != NULL) {
				[_theDriver playSoundDataFromPointer:audio16Ptr withSize:audioLength * 2 fromChannel:0 amplitude:0xFF bitRate:theData.c2spd isStereo:theData.stereo];
			}
			break;
	}
}

- (void)dealloc
{
	[self clearAudioPointers];
}

- (void)clearAudioPointers
{
	//Stop any audio that's playing on the driver
	[self.theDriver stopDriver];
	
	if (audio8Ptr) {
		free(audio8Ptr);
		audio8Ptr = NULL;
	}
	
	if (audio16Ptr) {
		free(audio16Ptr);
		audio16Ptr = NULL;
	}
}

@end
