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
+ (MADErr)testPcmdFileAtURL:(__nonnull NSURL*)theURL;
+ (__nonnull NSString *)stringFromPcmdData:(in __nonnull const Pcmd*)myPcmd;
+ (__nonnull NSData*)dataFromPcmd:(in __nonnull const Pcmd*)thePcmd;
- (__nonnull Pcmd*)newPcmdWithTrackRange:(NSRange)trackRange positionRange:(NSRange)posRange;
- (MADErr)importPcmdFromURL:(__nonnull NSURL*)theURL;
- (MADErr)exportPcmdToURL:(__nonnull NSURL*)theURL withTrackRange:(NSRange)trackRange positionRange:(NSRange)posRange;
- (MADErr)importPcmdFromPointer:(in __nonnull const Pcmd*)thePcmd;

/**
 * For use with Swift, because it doesn't like variable-length C arrays used in \c Pcmd
 */
- (MADErr)importIntPcmdFromPointer:(__nonnull IntPcmd*)theIntPcmd freeCommandsWhenDone:(BOOL)freeCmds;
@end

#endif
