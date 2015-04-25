//
//  PPErrors.h
//  PPMacho
//
//  Created by C.W. Betts on 12/10/12.
//
//

#ifndef __PLAYERPROKIT_PPERRORS_H__
#define __PLAYERPROKIT_PPERRORS_H__

#import <Foundation/Foundation.h>
#include <PlayerPROCore/MADDefs.h>

__BEGIN_DECLS
extern NSString * __nonnull const PPMADErrorDomain;

extern NSError* __nullable PPCreateErrorFromMADErrorType(MADErr theErr) NS_RETURNS_RETAINED;
extern NSError* __nullable PPCreateErrorFromMADErrorTypeConvertingToCocoa(MADErr theErr, BOOL convertToCocoa) NS_RETURNS_RETAINED;
extern BOOL PPErrorIsUserCancelled(NSError * __nonnull theErr);

__END_DECLS

#endif
