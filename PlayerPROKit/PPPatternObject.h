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

@interface PPPatternObject : NSObject <NSCoding, NSFastEnumeration>
@property (readonly) NSInteger index;
@property (copy) NSString *patternName;
@property (readonly, weak) PPMusicObjectWrapper *musicWrapper;
@property int patternSize;
- (id)initWithMusic:(PPMusicObjectWrapper *)mus;
- (id)initWithMusic:(PPMusicObjectWrapper *)mus patternAtIndex:(short)ptnIdx;

@end
