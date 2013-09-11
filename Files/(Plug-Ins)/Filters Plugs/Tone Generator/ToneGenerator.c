/*	ToneGenerator	*/
/*	v 1.1			*/
/*	1995 by ANR 	*/


#include <PlayerPROCore/MAD.h>
#include <PlayerPROCore/FileUtils.h>
#include <PlayerPROCore/PPPlug.h>
#include "ToneGenerator.h"
#include <math.h>

#define		KHZ				22254.54545

Ptr CreateAudio8Ptr( long AudioLength, long AudioFreq, long AudioAmp, long AudioType, Boolean stereo)
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

short* CreateAudio16Ptr( long AudioLength, long AudioFreq, long AudioAmp, long AudioType, Boolean stereo)
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

static OSErr mainToneGenerator(void				*unused,
							   sData			*theData,
							   long			SelectionStart,
							   long			SelectionEnd,
							   PPInfoPlug		*thePPInfoPlug,
							   short			StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	long		AudioLength;
	int			AudioFreq, AudioAmp;
	short		itemHit;
	
	itemHit = silence;
	
	AudioLength = SelectionEnd - SelectionStart;
	if( theData->amp == 16) AudioLength /= 2;
	if( theData->stereo) AudioLength /= 2;
	
	AudioFreq	= 440;
	AudioAmp	= 100;
	
	/********************/
	
	Audio8Ptr	= NULL;	Audio16Ptr	= NULL;

	itemHit = RunToneGeneratorPlug(theData, thePPInfoPlug, &AudioLength, StereoMode, &AudioAmp, &AudioFreq);
	
	if( itemHit != cancelTone)
	{
		Ptr		resultPtr;
		
		if( Audio16Ptr != NULL)	{	free( Audio16Ptr);			Audio16Ptr = NULL;}
		if( Audio8Ptr != NULL)	{	free( Audio8Ptr);			Audio8Ptr = NULL;}
		
		switch( theData->amp)
		{
			case 8:		Audio8Ptr	= CreateAudio8Ptr( AudioLength, AudioFreq, AudioAmp, itemHit, theData->stereo);
				break;
				
			case 16:
			default:
				Audio16Ptr	= CreateAudio16Ptr( AudioLength, AudioFreq, AudioAmp, itemHit, theData->stereo);
				AudioLength *= 2;
			break;
		}
		
		if( theData->stereo) AudioLength *= 2;
		
		resultPtr = malloc( theData->size - (SelectionEnd - SelectionStart) + AudioLength);
		
		memmove( theData->data, resultPtr, SelectionStart);
		
		if( theData->amp == 8) memmove( Audio8Ptr, resultPtr + SelectionStart, AudioLength);
		else memmove( Audio16Ptr, resultPtr + SelectionStart, AudioLength);
		
		memmove( theData->data + SelectionEnd, resultPtr + SelectionStart + AudioLength, theData->size - SelectionEnd);
		
		free( theData->data);		free( Audio8Ptr);
		theData->data = resultPtr;
		theData->size = theData->size  - (SelectionEnd - SelectionStart) + AudioLength;
	}
	

	return noErr;
}

// 25FA16EC-75FF-4514-9C84-7202360044B9
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x25, 0xFA, 0x16, 0xEC, 0x75, 0xFF, 0x45, 0x14, 0x9C, 0x84, 0x72, 0x02, 0x36, 0x00, 0x44, 0xB9)
#define PLUGMAIN mainToneGenerator
#define PLUGINFACTORY ToneGeneratorFactory

#include "CFPlugin-FilterBridge.c"
