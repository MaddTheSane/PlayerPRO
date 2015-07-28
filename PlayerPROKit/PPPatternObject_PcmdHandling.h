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
+ (MADErr)testPcmdFileAtURL:(NSURL*)theURL;
+ (NSString *)stringFromPcmdData:(in const Pcmd*)myPcmd;
+ (NSData*)dataFromPcmd:(in const Pcmd*)thePcmd;
- (Pcmd*)newPcmdWithTrackRange:(NSRange)trackRange positionRange:(NSRange)posRange;
- (MADErr)importPcmdFromURL:(NSURL*)theURL;
- (MADErr)exportPcmdToURL:(NSURL*)theURL withTrackRange:(NSRange)trackRange positionRange:(NSRange)posRange;
- (MADErr)importPcmdFromPointer:(in const Pcmd*)thePcmd;

/**
 * For use with Swift, because it doesn't like variable-length C arrays used in \c Pcmd
 */
- (MADErr)importIntPcmdFromPointer:(IntPcmd*)theIntPcmd freeCommandsWhenDone:(BOOL)freeCmds;
@end

NS_ASSUME_NONNULL_END

#endif
