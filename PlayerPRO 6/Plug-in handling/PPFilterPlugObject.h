//
//  PPFilterPlugObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/22/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#import "PPPlugInObject.h"

@interface PPFilterPlugObject : PPPlugInObject
- (instancetype)init UNAVAILABLE_ATTRIBUTE;
- (instancetype)initWithBundle:(NSBundle *)aBund NS_DESIGNATED_INITIALIZER;

- (void)beginRunWithData:(PPSampleObject*)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver*)driver parentDocument:(NSDocument*)document handler:(PPPlugErrorBlock)handler;

@end
