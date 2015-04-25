//
//  PPSampleObject_PPKPrivate.h
//  PPMacho
//
//  Created by C.W. Betts on 1/17/14.
//
//

#import <PlayerPROKit/PPSampleObject.h>

@interface PPSampleObject ()
- (instancetype)initWithSDataPointer:(inout sData *)theData;
- (void)setToSDataPointer:(inout sData *)theData;
@end
