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

void testDebugFunc(short, const char*, const char*);
extern void (*cDebugFunc)(short, const char*, const char*);

void cXTCFail(short line, const char* file, const char* info);

extern XCTestCase *currentTestClass;
	
#ifdef __cplusplus
}
#endif


#endif
