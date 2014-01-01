//
//  PPToneGeneratorController.m
//  PPMacho
//
//  Created by C.W. Betts on 8/10/13.
//
//

#import "PPToneGeneratorController.h"
#include <PlayerPROCore/PPPlug.h>
#include <math.h>

#define KHZ 22254.54545

static Ptr CreateAudio8Ptr(long AudioLength, long AudioFreq, long AudioAmp, ToneGenerator AudioType, Boolean stereo)
{
	Ptr		Audio8Ptr;
	SInt32	i, temp, inter = 0, x, dest;
	Boolean	UpDown;
	
	if (stereo)
		AudioLength *= 2L;
	
	Audio8Ptr = malloc(AudioLength);
	if (Audio8Ptr == NULL)
		return NULL;
	
	switch (AudioType)
	{
		case wave:
			for (i = 0, x = 0; i < AudioLength; i++, x++) {
				temp = 127.0 * sin((((double)x * (double)AudioFreq * M_PI * 2.0) / KHZ));
				
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

static short* CreateAudio16Ptr(long AudioLength, long AudioFreq, long AudioAmp, ToneGenerator AudioType, Boolean stereo)
{
	short	*Audio16Ptr;
	SInt32	i, temp, inter = 0, x, dest;
	Boolean	UpDown;
	
	if (stereo)
		AudioLength *= 2L;
	
	Audio16Ptr = (short*)malloc(AudioLength*2);
	if (Audio16Ptr == NULL)
		return NULL;
	
	switch (AudioType)
	{
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

- (id)initWithWindow:(NSWindow *)window
{
	if (self = [super initWithWindow:window]) {
		disabledData = NO;
		isMultipleIstanceSafe = YES;
		
		dispatch_block_t tmpBlock = ^{
			if (!disabledData) {
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
				
				char *resultPtr;
				
				if (audio16Ptr != NULL)	{
					free(audio16Ptr);
					audio16Ptr = NULL;
				}
				if (audio8Ptr != NULL) {
					free(audio8Ptr);
					audio8Ptr = NULL;
				}
				
				switch (theData->amp)
				{
					case 8:
						audio8Ptr = CreateAudio8Ptr(audioLength, audioFrequency, audioAmplitude * 100, generator, theData->stereo);
						break;
						
					case 16:
					default:
						audio16Ptr	= CreateAudio16Ptr(audioLength, audioFrequency, audioAmplitude * 100, generator, theData->stereo);
						audioLength *= 2;
						break;
				}
				
				if (theData->stereo)
					audioLength *= 2;
				
				resultPtr = malloc(theData->size - (self.selectionEnd - self.selectionStart) + audioLength);
				
				memmove(theData->data, resultPtr, self.selectionStart);
				
				if (theData->amp == 8)
					memmove(audio8Ptr, resultPtr + self.selectionStart, audioLength);
				else
					memmove(audio16Ptr, resultPtr + self.selectionStart, audioLength);
				
				memmove(theData->data + self.selectionEnd, resultPtr + self.selectionStart + audioLength, theData->size - self.selectionEnd);
				
				free(theData->data);
				
				theData->data = resultPtr;
				theData->size = (SInt32)(theData->size  - (self.selectionEnd - self.selectionStart) + audioLength);
				
			}
		};
		
		self.plugBlock = tmpBlock;
	}
	
	return self;
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
	if (audioLength <= 0)
		audioLength = 2000;
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
	static RPlaySoundUPP mPlay;
	mPlay = infoPlug->RPlaySound;
		
	switch(theData->amp)
	{
		case 8:
			if (audio8Ptr != NULL) {
				free(audio8Ptr);
				audio8Ptr = NULL;
			}
			audio8Ptr = CreateAudio8Ptr(audioLength, audioFrequency, audioAmplitude, generator, theData->stereo);
			if (audio8Ptr != NULL) {
				mPlay(infoPlug->driverRec, audio8Ptr, audioLength, 0, 0xFF, theData->amp, 0, 0, theData->c2spd, theData->stereo);
			}
			break;
			
		case 16:
			if (audio16Ptr != NULL) {
				free(audio16Ptr);
				audio16Ptr = NULL;
			}
			audio16Ptr	= CreateAudio16Ptr(audioLength, audioFrequency, audioAmplitude, generator, theData->stereo);
			if (audio16Ptr != NULL) {
				mPlay(infoPlug->driverRec, (Ptr)audio16Ptr, audioLength*2, 0, 0xFF, theData->amp, 0, 0, theData->c2spd, theData->stereo);
			}
			break;
	}
}

- (void)dealloc
{
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

// StereoMode = 0 apply on all channels, = 1 apply on current channel
static OSErr mainToneGenerator(void *unused, sData *theData, long SelectionStart, long SelectionEnd, PPInfoPlug *thePPInfoPlug, short StereoMode)
{
	long	AudioLength;
	int		AudioFreq, AudioAmp;
	
	AudioLength = SelectionEnd - SelectionStart;
	if (theData->amp == 16)
		AudioLength /= 2;
	if (theData->stereo)
		AudioLength /= 2;
	
	AudioFreq	= 440;
	AudioAmp	= 100;
	
	/********************/
	
	PPToneGeneratorController *controller = [[PPToneGeneratorController alloc] initWithWindowNibName:@"PPToneGeneratorController" infoPlug:thePPInfoPlug];
	
	controller.theData = theData;
	controller.audioLength = AudioLength;
	controller.audioAmplitude = (double)(AudioAmp) / 100.0;
	controller.audioFrequency = AudioFreq;
	controller.selectionStart = SelectionStart;
	controller.selectionEnd = SelectionEnd;
	controller.stereoMode = StereoMode ? YES : NO;
	
	return [controller runAsSheet];
}

// 25FA16EC-75FF-4514-9C84-7202360044B9
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x25, 0xFA, 0x16, 0xEC, 0x75, 0xFF, 0x45, 0x14, 0x9C, 0x84, 0x72, 0x02, 0x36, 0x00, 0x44, 0xB9)
#define PLUGMAIN mainToneGenerator
#define PLUGINFACTORY ToneGeneratorFactory

#include "CFPlugin-FilterBridge.c"
