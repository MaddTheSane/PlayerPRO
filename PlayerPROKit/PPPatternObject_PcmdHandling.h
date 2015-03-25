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

@interface PPPatternObject (PcmdHandling)
+ (MADErr)testPcmdFileAtURL:(nonnull NSURL*)theURL;
+ (nonnull NSString *)stringFromPcmdData:(in nonnull const Pcmd*)myPcmd;
+ (nonnull NSData*)dataFromPcmd:(in nonnull const Pcmd*)thePcmd;
- (nonnull Pcmd*)newPcmdWithTrackRange:(NSRange)trackRange positionRange:(NSRange)posRange;
- (MADErr)importPcmdFromURL:(nonnull NSURL*)theURL;
- (MADErr)exportPcmdToURL:(nonnull NSURL*)theURL withTrackRange:(NSRange)trackRange positionRange:(NSRange)posRange;
- (MADErr)importPcmdFromPointer:(in nonnull const Pcmd*)thePcmd;

/**
 * For use with Swift, because it doesn't like variable-length C arrays used in \c Pcmd
 */
- (MADErr)importIntPcmdFromPointer:(nonnull IntPcmd*)theIntPcmd freeCommandsWhenDone:(BOOL)freeCmds;
@end

#endif
