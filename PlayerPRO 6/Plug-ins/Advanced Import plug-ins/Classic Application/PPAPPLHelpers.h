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

__private_extern MADErr LoadMADK(const char *_Null_unspecified MADPtr, size_t size, MADMusic * _Null_unspecified theMAD, MADDriverSettings *_Null_unspecified init);
__private_extern MADErr TESTMADK(const void *_Null_unspecified MADPtr);

__private_extern id __nullable GetValueUsingKVO(id _Null_unspecified object);
