/*	CROP			*/
/*	v 1.0			*/
/*	1999 by ROSSET	*/

#include <PlayerPROCore/MAD.h>
#include <PlayerPROCore/FileUtils.h>
#include <PlayerPROCore/PPPlug.h>

static OSErr mainCrop(void			*unused,
					  sData			*theData,
					  long			SelectionStart,
					  long			SelectionEnd,
					  PPInfoPlug	*thePPInfoPlug,
					  short			StereoMode) // StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	size_t i;
	
	switch (theData->amp) {
		case 8:
		{
			char *SamplePtr = theData->data + SelectionStart;
			for (i = 0; i < SelectionEnd - SelectionStart; i++) {
				theData->data[i] = SamplePtr[i];
			}
			
			theData->size = (SInt32)(SelectionEnd - SelectionStart);
			theData->data = realloc(theData->data, theData->size);
		}
			break;
			
		case 16:
		{
			short	*SamplePtr = ((short*) theData->data) + (SelectionStart / 2);
			short	*data = (short*) theData->data;
			
			for (i = 0; i < (SelectionEnd - SelectionStart) / 2; i++) {
				data[i] = SamplePtr[i];
			}
			
			theData->size = (SInt32)(SelectionEnd - SelectionStart);
			theData->data = realloc(theData->data, theData->size);
		}
			break;
	}
	
	return noErr;
}

#define PLUGUUID (CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x10, 0x9F, 0xB9, 0x42, 0x42, 0x09, 0x4A, 0x66, 0xAB, 0x04, 0xFB, 0x2A, 0xF0, 0xB2, 0x96, 0x9F))
//109FB942-4203-4A66-AB04-FB2AF0B2969F
#define PLUGMAIN mainCrop
#define PLUGINFACTORY CropFactory

#include "CFPlugin-FilterBridge.c"
