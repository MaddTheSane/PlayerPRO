//
//  PPPatternObject_PcmdHandling.h
//  PPMacho
//
//  Created by C.W. Betts on 2/11/14.
//
//

#import <PlayerPROKit/PPPatternObject.h>
#include <PlayerPROCore/PPPlug.h>

@interface PPPatternObject (PcmdHandling)
+ (OSErr)testPcmdFileAtURL:(NSURL*)theURL;
+ (NSString *)stringFromPcmdData:(Pcmd*)myPcmd;
- (Pcmd*)newPcmdWithTrackRange:(NSRange)trackRange positionRange:(NSRange)posRange;
- (OSErr)importPcmdFromURL:(NSURL*)theURL;
- (OSErr)exportPcmdToURL:(NSURL*)theURL withTrackRange:(NSRange)trackRange positionRange:(NSRange)posRange;
@end
