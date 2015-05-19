//
//  PPAPPLHelpers.h
//  PPMacho
//
//  Created by C.W. Betts on 11/29/14.
//
//

#import <Foundation/Foundation.h>
#import <PlayerPROCore/RDriver.h>

#define __private_extern __attribute__((visibility("hidden")))

__private_extern MADErr LoadMADK(const char *MADPtr, size_t size, MADMusic *theMAD, MADDriverSettings *init);
__private_extern MADErr TESTMADK(const void* MADPtr);

