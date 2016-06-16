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

NS_ASSUME_NONNULL_BEGIN

@interface PPFilterPlugObject : PPPlugInObject
- (nonnull instancetype)init UNAVAILABLE_ATTRIBUTE;
- (nullable instancetype)initWithBundle:(NSBundle *)aBund NS_DESIGNATED_INITIALIZER;

- (void)beginRunWithData:(PPSampleObject*)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver*)driver parentDocument:(NSDocument*)document handler:(PPPlugErrorBlock)handler;

@end

NS_ASSUME_NONNULL_END
