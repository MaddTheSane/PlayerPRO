//
//  PPToneGeneratorController.m
//  PPMacho
//
//  Created by C.W. Betts on 8/10/13.
//
//

#import "PPToneGeneratorController.h"
#include <PlayerPROCore/MADPlug.h>
#include <tgmath.h>
#import <PlayerPROKit/PlayerPROKit.h>

#define KHZ 22254.54545

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

+ (Ptr)createAudio8PtrWithLength:(int)AudioLength frequency:(int)AudioFreq amplitude:(double)AudioAmp generator:(ToneGenerator)AudioType stereo:(BOOL)stereo
{
	Ptr		Audio8Ptr;
	int		i, temp, inter = 0, x, dest;
	bool	UpDown;
	
	if (stereo) {
		AudioLength *= 2;
	}
	
	Audio8Ptr = malloc(AudioLength);
	if (Audio8Ptr == NULL)
		return NULL;
	
	switch (AudioType) {
		case wave:
			for (i = 0, x = 0; i < AudioLength; i++, x++) {
				double temp = 127.0 * sin((x * AudioFreq * M_PI * 2.0) / KHZ);
				
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

+ (NSData *)audio8DataWithLength:(int)AudioLength frequency:(int)AudioFreq amplitude:(double)AudioAmp generator:(ToneGenerator)AudioType stereo:(BOOL)stereo
{
	void *ourData = [self createAudio8PtrWithLength:AudioLength frequency:AudioFreq amplitude:AudioAmp generator:AudioType stereo:stereo];
	
	if (ourData == NULL) {
		return nil;
	}
	
	if (stereo) {
		AudioLength *= 2;
	}
	
	return [[NSData alloc] initWithBytesNoCopy:ourData length:AudioLength];
}

+ (short*)createAudio16PtrWithLength:(int)AudioLength frequency:(int)AudioFreq amplitude:(double)AudioAmp generator:(ToneGenerator)AudioType stereo:(BOOL)stereo
{
	short	*Audio16Ptr;
	int		i, temp, inter = 0, x, dest;
	bool	UpDown;
	
	if (stereo)
		AudioLength *= 2;
	
	Audio16Ptr = (short*)malloc(AudioLength*2);
	if (Audio16Ptr == NULL)
		return NULL;
	
	switch (AudioType) {
		case wave:
			for (i = 0, x = 0; i < AudioLength; i++, x++) {
				double temp = 32767.0 * sin((x * AudioFreq * M_PI * 2.0) / KHZ);
				
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

+ (NSData *)audio16DataWithLength:(int)AudioLength frequency:(int)AudioFreq amplitude:(double)AudioAmp generator:(ToneGenerator)AudioType stereo:(BOOL)stereo
{
	void *ourData = [self createAudio16PtrWithLength:AudioLength frequency:AudioFreq amplitude:AudioAmp generator:AudioType stereo:stereo];
	
	if (ourData == NULL) {
		return nil;
	}
	
	if (stereo) {
		AudioLength *= 2;
	}
	
	return [[NSData alloc] initWithBytesNoCopy:ourData length:AudioLength * 2];
}


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
			audio8Ptr = [[self class] createAudio8PtrWithLength:audioLength frequency:audioFrequency amplitude:audioAmplitude * 100 generator:generator stereo:theData.stereo];
			break;
			
		case 16:
		default:
			audio16Ptr = [[self class] createAudio16PtrWithLength:audioLength frequency:audioFrequency amplitude:audioAmplitude * 100 generator:generator stereo:theData.stereo];
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
	
	theData.data = resultData;
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
			cell.enabled = NO;
		}
		
		self.disabledData = YES;
	}
	self.toneRadios = @[silentRadio, triangleRadio, squareRadio, waveRadio];
}

- (IBAction)toggleToneType:(id)sender
{
	generator = (ToneGenerator)[sender tag];
	for (NSButton *radio in toneRadios) {
		[radio setIntegerValue:(generator == [radio tag]) ? NSOnState : NSOffState];
	}
}

- (IBAction)playSample:(id)sender
{
	[self clearAudioPointers];
	
	NSData *ourData;
	
	switch(theData.amplitude) {
		case 8:
			ourData = [[self class] audio8DataWithLength:audioLength frequency:audioFrequency amplitude:audioAmplitude generator:generator stereo:theData.stereo];
			if (ourData) {
				[_theDriver playSoundDataFromData:ourData fromChannel:0 amplitude:0xFF bitRate:theData.c2spd isStereo:theData.stereo];
			}
			audio8Ptr = [[self class] createAudio8PtrWithLength:audioLength frequency:audioFrequency amplitude:audioAmplitude generator:generator stereo:theData.stereo];
			break;
			
		case 16:
			ourData = [[self class] audio16DataWithLength:audioLength frequency:audioFrequency amplitude:audioAmplitude generator:generator stereo:theData.stereo];
			if (ourData) {
				[_theDriver playSoundDataFromData:ourData fromChannel:0 amplitude:0xFF bitRate:theData.c2spd isStereo:theData.stereo];
			}
			audio16Ptr = [[self class] createAudio16PtrWithLength:audioLength frequency:audioFrequency amplitude:audioAmplitude generator:generator stereo:theData.stereo];

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
