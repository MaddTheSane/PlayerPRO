//
//  PPCrossFadePlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/11/14.
//
//

#import "PPCrossFadePlug.h"
@import PlayerPROKit.PPSampleObject;

@implementation PPCrossFadePlug

- (BOOL)hasUIConfiguration
{
	return NO;
}

- (instancetype)initForPlugIn
{
	return self = [self init];
}

- (MADErr)runWithData:(inout PPSampleObject *)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver *)driver
{
	NSMutableData *ourData = [theData.data mutableCopy];
	switch (theData.amplitude) {
		case 8:
		{
			long	i, j, length = (selRange.length) / 2;
			long	temp, tempStart, tempEnd;
			Ptr		StartPtr, EndPtr;
			
			length = MIN(length, theData.data.length - NSMaxRange(selRange));
			length = MIN(length, selRange.location);
			length *= 2;
			
			for (j = 0; j < 2; j++) {	//do it twice
				StartPtr = ourData.mutableBytes + selRange.location - (length / 2);
				EndPtr = ourData.mutableBytes + NSMaxRange(selRange) - (length / 2);
				for (i = 0; i < length; i++) {
					tempStart = *StartPtr;
					tempEnd = *EndPtr;
					
					temp = ((tempEnd * i) + (tempStart * (length - i))) / length;
					if (temp > 127)
						temp = 127;
					else if (temp < -127 )
						temp = -127;
					*StartPtr++ = temp;
					
					temp = ((tempStart * i) + (tempEnd * (length - i))) / length;
					if (temp > 127)
						temp = 127;
					else if (temp < -127 )
						temp = -127;
					*EndPtr++ = temp;
				}
			}
		}
			break;
			
		default:
		case 16:
		{
			long	i, j, length = selRange.length / 2;
			long	temp, tempStart, tempEnd;
			short	*StartPtr, *EndPtr;
			
			length = MIN(length, theData.data.length - NSMaxRange(selRange));
			length = MIN(length, selRange.location);
			//length *= 2;
			
			for (j = 0; j < 2; j++) {	//do it twice
				StartPtr = (short*) ourData.mutableBytes + (selRange.location / 2) - (length / 2);
				EndPtr = (short*) ourData.mutableBytes + (NSMaxRange(selRange) / 2) - (length / 2);
				for (i = 0; i < length; i++) {
					tempStart = *StartPtr;
					tempEnd = *EndPtr;
					
					temp = ((tempEnd * i) + (tempStart * (length - i))) / length;
					if (temp >= (short)0x7FFF)
						temp = 0x7FFF;	// overflow ?
					else if (temp <= (short)0x8000)
						temp = (short)0x8000;
					*StartPtr++ = temp;
					
					temp = ((tempStart * i) + (tempEnd * (length - i))) / length;
					if (temp >= (short)0x7FFF)
						temp = 0x7FFF;	// overflow ?
					else if (temp <= (short)0x8000)
						temp = (short)0x8000;
					*EndPtr++ = temp;
				}
			}
		}
			break;
	}
	
	theData.data = ourData;
	
	return MADNoErr;
}

@end
