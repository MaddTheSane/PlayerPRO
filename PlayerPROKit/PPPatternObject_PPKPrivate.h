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
{
	PatHeader patternHeader;
}
@property (strong) NSMutableArray *commands;

@end
