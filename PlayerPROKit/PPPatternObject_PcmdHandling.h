//
//  PPPatternObject_PcmdHandling.h
//  PPMacho
//
//  Created by C.W. Betts on 2/11/14.
//
//

#ifndef __PLAYERPROKIT_PPPATTERNOBJECT_PCMDHANDLING__
#define __PLAYERPROKIT_PPPATTERNOBJECT_PCMDHANDLING__

#import "PPPatternObject.h"
#include <PlayerPROCore/MADPlug.h>

@interface PPPatternObject (PcmdHandling)
+ (MADErr)testPcmdFileAtURL:(NSURL*)theURL;
+ (NSString *)stringFromPcmdData:(const Pcmd*)myPcmd;
+ (NSData*)dataFromPcmd:(const Pcmd*)thePcmd;
- (Pcmd*)newPcmdWithTrackRange:(NSRange)trackRange positionRange:(NSRange)posRange;
- (MADErr)importPcmdFromURL:(NSURL*)theURL;
- (MADErr)exportPcmdToURL:(NSURL*)theURL withTrackRange:(NSRange)trackRange positionRange:(NSRange)posRange;
@end

#endif
