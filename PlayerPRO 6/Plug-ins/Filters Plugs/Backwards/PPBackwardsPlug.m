//
//  PPBackwardsPlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/11/14.
//
//

#import "PPBackwardsPlug.h"
@import PlayerPROKit.PPSampleObject;

@implementation PPBackwardsPlug

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
	size_t			i;
	unsigned short	temp1, temp2;
	NSMutableData	*ourData = theData.data.mutableCopy;
	
	switch (theData.amplitude) {
		case 8:
		{
			char *orgPtr = ourData.mutableBytes;
			char *destPtr = ourData.mutableBytes;
			
			orgPtr += selRange.location;
			destPtr += NSMaxRange(selRange) - 1;
			
			for (i = 0; i < selRange.length / 2; i++) {	//just swap values
				temp1 = *orgPtr;
				temp2 = *destPtr;
				
				*orgPtr++ = temp2;
				*destPtr-- = temp1;
				
				if (StereoMode) {
					orgPtr++;
					destPtr--;
					i++;
				}
			}
		}
			break;
			
		case 16:
		{
			unsigned short *orgPtr	= (unsigned short*)ourData.mutableBytes;
			unsigned short *destPtr	= orgPtr;
			
			orgPtr += selRange.location / 2;
			destPtr += (NSMaxRange(selRange) - 1) / 2;
			
			for (i = 0; i < selRange.length / 4; i++) {
				temp1 = *orgPtr;
				temp2 = *destPtr;
				*orgPtr++ = temp2;
				*destPtr-- = temp1;
				
				if (StereoMode) {
					orgPtr++;
					destPtr--;
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
