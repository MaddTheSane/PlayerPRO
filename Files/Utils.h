//
//  Utils.h
//  PPMacho
//
//  Created by C.W. Betts on 1/24/14.
//
//

#ifndef PPMacho_Utils_h
#define PPMacho_Utils_h

#include <CoreServices/CoreServices.h>

#ifdef __cplusplus
extern "C" {
#endif

OSType String2OSType(StringPtr theStr);
OSStatus MyInvalWindowRect(WindowRef window, const Rect *bounds);
void MyPPBeep();

#ifdef __cplusplus
}
#endif

#endif
