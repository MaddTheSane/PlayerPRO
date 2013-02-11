//
//  PPPatternObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/17/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

@class PPMadCommandObject;

@interface PPPatternObject : NSObject
{
	MADMusic *curMusic;
	NSInteger index;
	NSMutableArray *commands;
	PatHeader patternHeader;
}

- (id)initWithMusic:(MADMusic *)mus patternAtIndex:(short)ptnIdx;

@end
