//
//  PPErrors.h
//  PPMacho
//
//  Created by C.W. Betts on 12/10/12.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/MADDefs.h>

__BEGIN_DECLS
extern __nonnull NSString * const PPMADErrorDomain;

extern __nullable NSError *PPCreateErrorFromMADErrorType(MADErr theErr) NS_RETURNS_RETAINED;
extern __nullable NSError *PPCreateErrorFromMADErrorTypeConvertingToCocoa(MADErr theErr, BOOL convertToCocoa) NS_RETURNS_RETAINED;
extern BOOL PPErrorIsUserCancelled(__nonnull NSError *theErr);

__END_DECLS
