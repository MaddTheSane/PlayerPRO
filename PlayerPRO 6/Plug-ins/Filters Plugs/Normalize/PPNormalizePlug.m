//
//  PPNormalizePlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/11/14.
//
//

#import "PPNormalizePlug.h"
@import PlayerPROKit.PPSampleObject;

@implementation PPNormalizePlug

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
	int	i, peak = 0, temp;
	NSMutableData *ourData = [theData.data mutableCopy];
	Ptr outPtr = ourData.mutableBytes;
	
	switch (theData.amplitude) {
		case 8:
		{
			Ptr	SamplePtr = (outPtr) + PPSelectionStart(selRange);
			for (i = 0; i < selRange.length; i++) {
				temp = *SamplePtr++;
				
				peak = MAX(peak, abs(temp));
				
				if (StereoMode) {
					SamplePtr++;
					i++;
				}
			}
			
			if (peak != 0) {
				peak = (0x80 * 0x10000) / peak;
				
				SamplePtr = (outPtr) + PPSelectionStart(selRange);
				for (i = 0; i < selRange.length; i++) {
					temp = *SamplePtr;
					
					temp = (peak * temp) / 0x10000;
					
					if (temp > 127)
						temp = 127;
					else if (temp < -127 )
						temp = -127;
					
					*SamplePtr++ = temp;
					
					if (StereoMode) {
						SamplePtr++;
						i++;
					}
				}
			}
		} break;
			
		case 16:
		{
			short	*SamplePtr = (short*) outPtr + (PPSelectionStart(selRange) / 2);
			
			for (i = 0; i < (selRange.length) / 2; i++) {
				temp = (int)*SamplePtr++;
				peak = MAX(peak, abs(temp));
				
				if (StereoMode) {
					SamplePtr++;
					i++;
				}
			}
			
			if (peak != 0) {
				peak = (0x8000 * 0x10000) / peak;
				
				SamplePtr = (short*) outPtr + (PPSelectionStart(selRange) / 2);
				for (i = 0; i < (selRange.length) / 2; i++) {
					temp = (int)*SamplePtr;
					
					temp = -(peak * temp) / 0x10000;
					
					*SamplePtr++ = temp;
					
					if (StereoMode) {
						SamplePtr++;
						i++;
					}
				}
			}
		}
			break;
	}
	theData.data = ourData;
	
	
	return MADNoErr;
}

@end
