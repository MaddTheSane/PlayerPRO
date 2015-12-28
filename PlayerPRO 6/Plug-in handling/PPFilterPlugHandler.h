//
//  PPFilterPlugHandler.h
//  PPMacho
//
//  Created by C.W. Betts on 1/22/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

@class PPFilterPlugObject;

NS_ASSUME_NONNULL_BEGIN

@interface PPFilterPlugHandler : NSObject <NSFastEnumeration>
- (instancetype)init NS_DESIGNATED_INITIALIZER;

@property (readonly, copy) NSArray<PPFilterPlugObject*> *plugInArray;
- (PPFilterPlugObject*)plugInAtIndex:(NSUInteger)idx;
@property (readonly) NSInteger plugInCount;
- (PPFilterPlugObject *)objectAtIndexedSubscript:(NSInteger)index;

- (void)beginWithPlugAtIndex:(NSInteger)idx data:(PPSampleObject*)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver*)driver parentDocument:(NSDocument*)document handler:(PPPlugErrorBlock)handler;

- (void)addPlugInFromPath:(NSString*)thePath;
- (void)addPlugInFromURL:(NSURL *)urlpath;
- (void)addPlugInFromBundle:(NSBundle *)theBund;
@end

NS_ASSUME_NONNULL_END
