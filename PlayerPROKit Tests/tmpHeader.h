//
//  tmpHeader.h
//  PPMacho
//
//  Created by C.W. Betts on 10/28/14.
//
//

#ifndef PPMacho_tmpHeader_h
#define PPMacho_tmpHeader_h

#include <PlayerPROCore/PlayerPROCore.h>

@class XCTestCase;

#ifdef __cplusplus
extern "C" {
#endif

void testDebugFunc(short, const char*__nullable, const char*__nullable);
extern void (*__null_unspecified cDebugFunc)(short, const char*__nullable, const char*__nullable);

void cXTCFail(short line, const char* __nullable file, const char*__nullable info);

extern XCTestCase *__nullable currentTestClass;
	
#ifdef __cplusplus
}
#endif


#endif
