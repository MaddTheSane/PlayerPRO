//
//  PPPatternObject_PcmdHandling.h
//  PPMacho
//
//  Created by C.W. Betts on 2/11/14.
//
//

#ifndef __PLAYERPROKIT_PPPATTERNOBJECT_PCMDHANDLING__
#define __PLAYERPROKIT_PPPATTERNOBJECT_PCMDHANDLING__

#import <PlayerPROKit/PPPatternObject.h>
#include <PlayerPROCore/MADPlug.h>

NS_ASSUME_NONNULL_BEGIN

@interface PPPatternObject (PcmdHandling)
+ (BOOL)testPcmdFileAtURL:(NSURL*)theURL error:(NSError**)error;
+ (NSString *)stringFromPcmdData:(in const Pcmd*)myPcmd;
+ (NSData*)dataFromPcmd:(in const Pcmd*)thePcmd;
- (Pcmd*)newPcmdWithTrackRange:(NSRange)trackRange positionRange:(NSRange)posRange;
- (BOOL)importPcmdFromURL:(NSURL*)theURL error:(NSError**)error;
- (BOOL)exportPcmdToURL:(NSURL*)theURL withTrackRange:(NSRange)trackRange positionRange:(NSRange)posRange error:(NSError**)error;
- (BOOL)importPcmdFromPointer:(in const Pcmd*)thePcmd error:(NSError**)error;

- (BOOL)importPcmdFromString:(NSString*)pcmdStr error:(NSError**)error;

/**
 * For use with Swift, because it doesn't like variable-length C arrays used in \c Pcmd
 */
- (BOOL)importIntPcmdFromPointer:(IntPcmd*)theIntPcmd freeCommandsWhenDone:(BOOL)freeCmds error:(NSError**)error;
@end

NS_ASSUME_NONNULL_END

#endif
