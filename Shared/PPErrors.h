//
//  PPErrors.h
//  PPMacho
//
//  Created by C.W. Betts on 12/10/12.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PPDefs.h>

extern NSString * const PPMADErrorDomain;

NSError *CreateErrorFromMADErrorType(MADErr theErr) NS_RETURNS_RETAINED;

