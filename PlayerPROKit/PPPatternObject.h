//
//  PPPatternObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/17/13.
//
//

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

- (__nonnull PPMadCommandObject *)objectAtIndexedSubscript:(NSInteger)index;

- (instancetype)initWithMusic:(__nonnull PPMusicObject *)mus NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithMusic:(__nonnull PPMusicObject *)mus patternAtIndex:(short)ptnIdx;

- (__nonnull PPMadCommandObject*)getCommandFromPosition:(short)PosX channel:(short)TrackIdX;
- (void)replaceCommandAtPosition:(short)PosX channel:(short)TrackIdX cmd:(Cmd)aCmd;
- (void)replaceCommandAtPosition:(short)PosX channel:(short)TrackIdX command:(__nonnull PPMadCommandObject*)aCmd;
- (void)modifyCommandAtPosition:(short)PosX channel:(short)TrackIdX commandBlock:(void (^ __nonnull )(__nonnull Cmd*))block;
- (void)modifyCommandAtPosition:(short)PosX channel:(short)TrackIdX madCommandBlock:(void (^ __nonnull)(__nonnull PPMadCommandObject*))block;

@end
