//
//  PPInvertPlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/11/14.
//
//

#import "PPInvertPlug.h"
#import <PlayerPROKit/PPSampleObject.h>

@implementation PPInvertPlug

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
	int i, temp;
	NSMutableData *ourData = [theData.data mutableCopy];
	Ptr ourPtr = ourData.mutableBytes;
	
	switch (theData.amplitude) {
		case 8:
		{
			char *SamplePtr = (ourPtr) + selRange.location;
			for (i = 0; i < selRange.length; i++) {
				temp = *SamplePtr;
				temp = 0xFF - temp;
				*SamplePtr++ = temp;
				
				if (StereoMode) {
					SamplePtr++;
					i++;
				}
			}
		}
			break;
			
		case 16:
		{
			short *SamplePtr = (short*) ourPtr + (selRange.location / 2);
			for (i = 0; i < (selRange.length) / 2; i++) {
				temp = (int)*SamplePtr;
				temp = 0xFFFF - temp;
				*SamplePtr++ = temp;
				
				if (StereoMode) {
					SamplePtr++;
					i++;
				}
			}
		}
			break;
	}
	theData.data = ourData;
	
	return MADNoErr;
}

@end
