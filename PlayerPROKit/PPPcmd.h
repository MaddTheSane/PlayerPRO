//
//  PPPcmd.h
//  PPMacho
//
//  Created by C.W. Betts on 11/5/14.
//
//

#import <Foundation/Foundation.h>
#import <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROKit/PPObjectProtocol.h>

@interface PPPcmd : NSObject <NSFastEnumeration, PPObject>
@property short tracks;
@property short length;
@property short startTrack;
@property short startPosition;
@property (readonly) NSArray *commands;

- (instancetype)init NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithPcmd:(Pcmd*)thePcmd;
- (instancetype)initWithIntPcmd:(IntPcmd)thePcmd;
- (instancetype)initWithIntPcmd:(IntPcmd)thePcmd freeWhenDone:(BOOL)aFree;

- (Cmd)getCommand:(short)row track:(short)track;
- (void)replaceCmd:(short)row track:(short)track command:(Cmd)command;
- (void)modifyCmdAtRow:(short)row track:(short)track block:(void (^)(Cmd*))block;

@end
