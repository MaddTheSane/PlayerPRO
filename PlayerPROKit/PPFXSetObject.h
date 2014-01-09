//
//  PPFXSetObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/8/14.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

@interface PPFXSetObject : NSObject
@property (readonly) FXSets theSet;
@property short track;
@property short identifier;
@property int fxIdentifier;
@property short argumentNumbers;
@property (readonly, strong) NSMutableArray *sets;
@property (copy) NSString *name;
@end
