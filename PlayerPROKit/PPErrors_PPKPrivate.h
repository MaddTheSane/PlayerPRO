//
//  PPErrors_PPKPrivate.h
//  PPMacho
//
//  Created by C.W. Betts on 8/4/15.
//
//

#ifndef PPErrors_PPKPrivate_h
#define PPErrors_PPKPrivate_h

#import "PPErrors.h"

#ifndef __private_extern
#define __private_extern __attribute__((visibility("hidden")))
#endif

__private_extern extern NSString * __nullable stringForKeyAndError(NSString * __nonnull key, MADErr errCode);

#endif /* PPErrors_PPKPrivate_h */
