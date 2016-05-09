//
//  PPPatternObject_PPKPrivate.h
//  PPMacho
//
//  Created by C.W. Betts on 1/8/14.
//
//

#include <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROKit/PPPatternObject.h>

@interface PPPatternObject ()
@property (nonatomic) PatHeader patternHeader;
@property (strong) NSMutableArray<PPMadCommandObject*> *commands;
@property (readwrite, weak) PPMusicObject *musicWrapper;
@property (readwrite) NSInteger index;
@end
