//
//  PPErrors.h
//  PPMacho
//
//  Created by C.W. Betts on 12/10/12.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PPDefs.h>

__BEGIN_DECLS
extern NSString * const PPMADErrorDomain;

extern NSError *PPCreateErrorFromMADErrorType(MADErr theErr) NS_RETURNS_RETAINED;
extern NSError *PPCreateErrorFromMADErrorTypeIgnoringUserCancelled(MADErr theErr, BOOL ignoreUserCancelled) NS_RETURNS_RETAINED;
__END_DECLS
