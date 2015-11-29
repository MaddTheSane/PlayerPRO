//
//  PPPatternObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/17/13.
//
//

#ifndef __PLAYERPROKIT_PPPATTERNOBJECT_H__
#define __PLAYERPROKIT_PPPATTERNOBJECT_H__

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROKit/PPObjectProtocol.h>

@class PPMadCommandObject;
@class PPMusicObject;

NS_ASSUME_NONNULL_BEGIN

@interface PPPatternObject : NSObject <NSFastEnumeration, PPObject>
@property (readonly) NSInteger index;
@property (copy, null_resettable) NSString *patternName;
@property (readonly, weak, nullable) PPMusicObject *musicWrapper;
@property int patternSize;
@property (readonly) NSInteger lengthOfCommands;

- (PPMadCommandObject *)objectAtIndexedSubscript:(NSInteger)index;

- (nullable instancetype)initWithMusic:(PPMusicObject *)mus NS_DESIGNATED_INITIALIZER;
- (nullable instancetype)initWithMusic:(PPMusicObject *)mus patternAtIndex:(short)ptnIdx;

- (PPMadCommandObject*)getCommandFromPosition:(short)PosX channel:(short)TrackIdX;
- (void)replaceCommandAtPosition:(short)PosX channel:(short)TrackIdX cmd:(Cmd)aCmd;
- (void)replaceCommandAtPosition:(short)PosX channel:(short)TrackIdX command:(PPMadCommandObject*)aCmd;
- (void)modifyCommandAtPosition:(short)PosX channel:(short)TrackIdX commandBlock:(void (^)(Cmd *))block;
- (void)modifyCommandAtPosition:(short)PosX channel:(short)TrackIdX madCommandBlock:(void (^)(PPMadCommandObject*))block;

@end

NS_ASSUME_NONNULL_END

#endif
