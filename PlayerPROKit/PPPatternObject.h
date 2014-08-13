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

@interface PPPatternObject : NSObject <NSFastEnumeration, PPObject>
@property (readonly) NSInteger index;
@property (copy) NSString *patternName;
@property (readonly, weak) PPMusicObject *musicWrapper;
@property int patternSize;
- (instancetype)initWithMusic:(PPMusicObject *)mus;
- (instancetype)initWithMusic:(PPMusicObject *)mus patternAtIndex:(short)ptnIdx;
@end
