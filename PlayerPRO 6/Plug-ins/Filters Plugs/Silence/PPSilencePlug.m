//
//  PPSilencePlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/7/14.
//
//

#import "PPSilencePlug.h"
@import PlayerPROKit.PPSampleObject;

@implementation PPSilencePlug

- (BOOL)hasUIConfiguration
{
	return NO;
}

- (MADErr)runWithData:(inout PPSampleObject*)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver*)driver
{
	long	i;
	NSMutableData *ourOwnData = [theData.data mutableCopy];
	Ptr		Sample8Ptr = ourOwnData.mutableBytes;
	short	*Sample16Ptr = ourOwnData.mutableBytes;
	NSUInteger SelectionStart = selRange.location;
	NSUInteger SelectionEnd = NSMaxRange(selRange);
	
	if (ourOwnData == NULL || theData.data.length == 0) {
		return MADNoErr;
	}
	
	switch (theData.amplitude) {
		case 8:
			Sample8Ptr += SelectionStart;
			
			for (i = 0; i < SelectionEnd - SelectionStart; i++) {
				*Sample8Ptr = 0;
				
				if (StereoMode) {
					Sample8Ptr++;
					i++;
				}
				
				Sample8Ptr++;
			}
			break;
			
		case 16:
			Sample16Ptr += SelectionStart / 2;							// Div 2, because it's in bytes !!!
			
			for (i = 0; i < (SelectionEnd - SelectionStart) / 2; i++) {	// Div 2, because it's in bytes !!!
				*Sample16Ptr = 0;
				
				if (StereoMode){
					Sample16Ptr++;
					i++;
				}
				
				Sample16Ptr++;
			}
			break;
	}
	
	theData.data = ourOwnData;
	
	return MADNoErr;
}

@end
