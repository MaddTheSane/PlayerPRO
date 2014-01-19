//
//  PPErrors.h
//  PPMacho
//
//  Created by C.W. Betts on 12/10/12.
//
//

#import <Foundation/Foundation.h>

extern NSString * const PPMADErrorDomain;

NSError *CreateErrorFromMADErrorType(OSErr theErr) NS_RETURNS_RETAINED;

