//
//  PPPatternObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/17/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROKit/PPMadCommandObject.h>
#import <PlayerPROKit/PPMusicObject.h>
#import <PlayerPROKit/PPObjectProtocol.h>

#ifndef NS_DESIGNATED_INITIALIZER
#define NS_DESIGNATED_INITIALIZER
#endif

@interface PPPatternObject : NSObject <NSFastEnumeration, PPObject>
@property (readonly) NSInteger index;
@property (copy) NSString *patternName;
@property (readonly, weak) PPMusicObject *musicWrapper;
@property int patternSize;

- (PPMadCommandObject *)objectAtIndexedSubscript:(NSInteger)index;

- (instancetype)initWithMusic:(PPMusicObject *)mus NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithMusic:(PPMusicObject *)mus patternAtIndex:(short)ptnIdx;
@end
