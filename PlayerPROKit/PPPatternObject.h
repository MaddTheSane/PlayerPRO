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

#ifndef NS_DESIGNATED_INITIALIZER
#define NS_DESIGNATED_INITIALIZER
#endif

@interface PPPatternObject : NSObject <NSFastEnumeration, PPObject>
@property (readonly) NSInteger index;
@property (copy, null_resettable) NSString *patternName;
@property (readonly, weak, nullable) PPMusicObject *musicWrapper;
@property int patternSize;
@property (readonly) NSInteger lengthOfCommands;

- (nonnull PPMadCommandObject *)objectAtIndexedSubscript:(NSInteger)index;

- (nullable instancetype)initWithMusic:(nonnull PPMusicObject *)mus NS_DESIGNATED_INITIALIZER;
- (nullable instancetype)initWithMusic:(nonnull PPMusicObject *)mus patternAtIndex:(short)ptnIdx;

- (nonnull PPMadCommandObject*)getCommandFromPosition:(short)PosX channel:(short)TrackIdX;
- (void)replaceCommandAtPosition:(short)PosX channel:(short)TrackIdX cmd:(Cmd)aCmd;
- (void)replaceCommandAtPosition:(short)PosX channel:(short)TrackIdX command:(nonnull PPMadCommandObject*)aCmd;
- (void)modifyCommandAtPosition:(short)PosX channel:(short)TrackIdX commandBlock:(void (^ __nonnull )(Cmd *__nonnull))block;
- (void)modifyCommandAtPosition:(short)PosX channel:(short)TrackIdX madCommandBlock:(void (^ __nonnull)(PPMadCommandObject*__nonnull))block;

@end

#endif
