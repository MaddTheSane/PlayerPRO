//
//  PPInstrumentImporterObject.h
//  PPMacho
//
//  Created by C.W. Betts on 12/27/12.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#import "PPInstrumentImporterObject.h"

@interface PPInstrumentImporterCompatObject : PPInstrumentImporterObject
- (nonnull instancetype)init UNAVAILABLE_ATTRIBUTE;
- (nullable instancetype)initWithBundle:(nonnull NSBundle *)theBund NS_DESIGNATED_INITIALIZER;
@end
