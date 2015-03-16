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
@property (copy) NSString *patternName;
@property (readonly, weak) PPMusicObject *musicWrapper;
@property int patternSize;
@property (readonly) NSInteger lengthOfCommands;

- (PPMadCommandObject *)objectAtIndexedSubscript:(NSInteger)index;

- (instancetype)initWithMusic:(PPMusicObject *)mus NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithMusic:(PPMusicObject *)mus patternAtIndex:(short)ptnIdx;

- (PPMadCommandObject*)getCommandFromPosition:(short)PosX channel:(short)TrackIdX;
- (void)replaceCommandAtPosition:(short)PosX channel:(short)TrackIdX cmd:(Cmd)aCmd;
- (void)replaceCommandAtPosition:(short)PosX channel:(short)TrackIdX command:(PPMadCommandObject*)aCmd;
- (void)modifyCommandAtPosition:(short)PosX channel:(short)TrackIdX commandBlock:(void (^)(Cmd*))block;
- (void)modifyCommandAtPosition:(short)PosX channel:(short)TrackIdX madCommandBlock:(void (^)(PPMadCommandObject*))block;

@end
