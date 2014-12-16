//
//  PPEnvelopeObject32.h
//  PPMacho
//
//  Created by C.W. Betts on 9/21/14.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

@interface PPEnvelopeObject32 : NSObject <NSSecureCoding>
{
	@private
	EnvRec envelopeRec;
}
@property (readonly) EnvRec envelopeRec;
@property short position;
@property short value;
- (instancetype)init;
- (instancetype)initWithEnvRec:(EnvRec)theRec;
@end
