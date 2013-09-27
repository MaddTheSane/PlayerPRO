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

#define		KHZ				22254.54545

static char *Audio8Ptr = NULL;
static short *Audio16Ptr = NULL;

static Ptr CreateAudio8Ptr( long AudioLength, long AudioFreq, long AudioAmp, long AudioType, Boolean stereo)
{
	Ptr		Audio8Ptr;
	SInt32	i, temp, inter = 0, x, dest;
	Boolean	UpDown;
	
	if( stereo) AudioLength *= 2L;
	
	Audio8Ptr = malloc( AudioLength);
	if( Audio8Ptr == NULL) return NULL;
	
	switch( AudioType)
	{
		case wave:
			for( i = 0, x = 0; i < AudioLength; i++, x++)
			{
				temp = 127.0 * sin( ( ((double) x * (double) AudioFreq * M_PI * 2.0) / KHZ));
				
				/** Amplitude resizing **/
				temp *= AudioAmp;
				temp /= 100;
				
				/** Overshoot **/
				if( temp >= 127) temp = 127;
				else if( temp <= -127 ) temp = -127;
				
				Audio8Ptr[ i] = temp;
				if( stereo)
				{
					i++;
					Audio8Ptr[ i] = temp;
				}
			}
			break;
			
		case silence:
			for( i = 0; i < AudioLength; i++) Audio8Ptr[ i] = 0x00;
			break;
			
		case square:
			for( i = 0, x = 0, dest = -1; i < AudioLength; i++)
			{
				if( i > dest)
				{
					x++;
					dest = (x * KHZ) / (AudioFreq * 2);
					if( stereo) dest *= 2;
					if( inter == -127) inter = 127;
					else inter = -127;
				}
				
				temp = inter;
				
				/** Amplitude resizing **/
				temp *= AudioAmp;
				temp /= 100;
				
				/** Overshoot **/
				if( temp >= 127) temp = 127;
				else if( temp <= -127 ) temp = -127;
				
				Audio8Ptr[ i] = temp;
				if( stereo)
				{
					i++;
					Audio8Ptr[ i] = temp;
				}
			}
			break;
			
		case triangle:
			UpDown = true;
			for( i = 0, x = 0, dest = -1; i < AudioLength; i++)
			{
				if( i > dest)
				{
					x++;
					dest = (x * KHZ) / (AudioFreq * 2);
					if( stereo) dest *= 2;
					inter = dest - i;
					
					UpDown = !UpDown;
				}
				
				if( UpDown) temp = (256 * (dest - i)) / inter;
				else temp = (256 * (inter - (dest - i))) / inter;
				
				temp -= 127;
				
				/** Amplitude resizing **/
				temp *= AudioAmp;
				temp /= 100;
				
				/** Overshoot **/
				if( temp >= 127) temp = 127;
				else if( temp <= -127 ) temp = -127;
				
				Audio8Ptr[ i] = temp;
				if( stereo)
				{
					i++;
					Audio8Ptr[ i] = temp;
				}
			}
			break;
	}
	
	return Audio8Ptr;
}

static short* CreateAudio16Ptr( long AudioLength, long AudioFreq, long AudioAmp, long AudioType, Boolean stereo)
{
	short	*Audio16Ptr;
	SInt32	i, temp, inter = 0, x, dest;
	Boolean	UpDown;
	
	if( stereo) AudioLength *= 2L;
	
	Audio16Ptr = (short*) malloc( AudioLength*2);
	if( Audio16Ptr == NULL) return NULL;
	
	switch( AudioType)
	{
		case wave:
			for( i = 0, x = 0; i < AudioLength; i++, x++)
			{
				temp = 32767.0 * sin( ( ((double) x * (double) AudioFreq * M_PI * 2.0) / KHZ));
				
				/** Amplitude resizing **/
				temp *= AudioAmp;
				temp /= 100;
				
				/** Overshoot **/
				if( temp >= (short) 0x7FFF) temp = 0x7FFF;
				else if( temp <= (short) 0x8000 ) temp = (short) 0x8000;
				
				Audio16Ptr[ i] = temp;
				if( stereo)
				{
					i++;
					Audio16Ptr[ i] = temp;
				}
			}
			break;
			
		case silence:
			for( i = 0; i < AudioLength; i++) Audio16Ptr[ i] = 0x00;
			break;
			
		case square:
			for( i = 0, x = 0, dest = -1; i < AudioLength; i++)
			{
				if( i > dest)
				{
					x++;
					dest = (x * KHZ) / (AudioFreq * 2);
					if( stereo) dest *= 2;
					if( inter == -32767) inter = 32767;
					else inter = -32767;
				}
				
				temp = inter;
				
				/** Amplitude resizing **/
				temp *= AudioAmp;
				temp /= 100;
				
				/** Overshoot **/
				if( temp >= (short) 0x7FFF) temp = 0x7FFF;
				else if( temp <= (short) 0x8000 ) temp = (short) 0x8000;
				
				Audio16Ptr[ i] = temp;
				if( stereo)
				{
					i++;
					Audio16Ptr[ i] = temp;
				}
			}
			break;
			
		case triangle:
			UpDown = true;
			for( i = 0, x = 0, dest = -1; i < AudioLength; i++)
			{
				if( i > dest)
				{
					x++;
					dest = (x * KHZ) / (AudioFreq * 2);
					if( stereo) dest *= 2;
					inter = dest - i;
					
					UpDown = !UpDown;
				}
				
				if( UpDown) temp = (65535 * (dest - i)) / inter;
				else temp = (65535 * (inter - (dest - i))) / inter;
				
				temp -= 32767;
				
				/** Amplitude resizing **/
				temp *= AudioAmp;
				temp /= 100;
				
				/** Overshoot **/
				if( temp >= (short) 0x7FFF) temp = 0x7FFF;
				else if( temp <= (short) 0x8000 ) temp = (short) 0x8000;
				
				Audio16Ptr[ i] = temp;
				if( stereo)
				{
					i++;
					Audio16Ptr[ i] = temp;
				}
			}
			break;
	}
	
	return Audio16Ptr;
}

@interface PPToneGeneratorController ()
@property (retain) NSArray *toneRadios;
@property BOOL disabledData;
@end

@implementation PPToneGeneratorController
@synthesize audioAmplitude;
@synthesize audioFrequency;
@synthesize audioLength;
@synthesize generator;
@synthesize toneRadios;
@synthesize disabledData;

- (id)initWithWindow:(NSWindow *)window
{
	self = [super initWithWindow:window];
	if (self) {
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
				
				Ptr		resultPtr;
				
				if( Audio16Ptr != NULL)	{	free( Audio16Ptr);			Audio16Ptr = NULL;}
				if( Audio8Ptr != NULL)	{	free( Audio8Ptr);			Audio8Ptr = NULL;}
				
				switch( theData->amp)
				{
					case 8:
						Audio8Ptr	= CreateAudio8Ptr( audioLength, audioFrequency, audioAmplitude * 100, generator, theData->stereo);
						break;
						
					case 16:
					default:
						Audio16Ptr	= CreateAudio16Ptr( audioLength, audioFrequency, audioAmplitude * 100, generator, theData->stereo);
						audioLength *= 2;
						break;
				}
				
				if( theData->stereo) audioLength *= 2;
				
				resultPtr = malloc( theData->size - (self.selectionEnd - self.selectionStart) + audioLength);
				
				memmove( theData->data, resultPtr, self.selectionStart);
				
				if( theData->amp == 8) memmove( Audio8Ptr, resultPtr + self.selectionStart, audioLength);
				else memmove( Audio16Ptr, resultPtr + self.selectionStart, audioLength);
				
				memmove( theData->data + self.selectionEnd, resultPtr + self.selectionStart + audioLength, theData->size - self.selectionEnd);
				
				free( theData->data);		free( Audio8Ptr);
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
	mPlay = infoPlug->RPlaySound;
		
	switch( theData->amp)
	{
		case 8:
			if( Audio8Ptr != NULL) { free( Audio8Ptr);			Audio8Ptr = NULL;}
			Audio8Ptr	= CreateAudio8Ptr( audioLength, audioFrequency, audioAmplitude, generator, theData->stereo);
			if( Audio8Ptr != NULL)
			{
				mPlay( infoPlug->driverRec, Audio8Ptr, audioLength, 0, 0xFF, theData->amp, 0, 0, theData->c2spd, theData->stereo);
			}
			break;
			
		case 16:
			if( Audio16Ptr != NULL) { free( (Ptr) Audio16Ptr);	Audio16Ptr = NULL;}
			Audio16Ptr	= CreateAudio16Ptr( audioLength, audioFrequency, audioAmplitude, generator, theData->stereo);
			if( Audio16Ptr != NULL)
			{
				mPlay( infoPlug->driverRec, (Ptr) Audio16Ptr, audioLength*2, 0, 0xFF, theData->amp, 0, 0, theData->c2spd, theData->stereo);
			}
			break;
	}
}

@end

static OSErr mainToneGenerator(void				*unused,
							   sData			*theData,
							   long				SelectionStart,
							   long				SelectionEnd,
							   PPInfoPlug		*thePPInfoPlug,
							   short			StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	long	AudioLength;
	int		AudioFreq, AudioAmp;
	//short	itemHit = silence;
	
	AudioLength = SelectionEnd - SelectionStart;
	if( theData->amp == 16) AudioLength /= 2;
	if( theData->stereo) AudioLength /= 2;
	
	AudioFreq	= 440;
	AudioAmp	= 100;
	
	/********************/
	
	Audio8Ptr	= NULL;
	Audio16Ptr	= NULL;
	
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
