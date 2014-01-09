//
//  PPPatternObject_PPKPrivate.h
//  PPMacho
//
//  Created by C.W. Betts on 1/8/14.
//
//

#include <PlayerPROCore/PlayerPROCore.h>
#import "PPPatternObject.h"

@interface PPPatternObject ()
@property PatHeader patternHeader;
@property (strong) NSMutableArray *commands;
@property (readwrite, weak) PPMusicObjectWrapper *musicWrapper;
@property (readwrite) NSInteger index;
@end
