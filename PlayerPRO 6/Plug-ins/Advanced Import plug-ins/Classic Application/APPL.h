//
//  APPL.h
//  PPMacho
//
//  Created by C.W. Betts on 2/12/14.
//
//

#ifndef PPMacho_APPL_h
#define PPMacho_APPL_h

#include <PlayerPROCore/PlayerPROCore.h>

#define __private_extern __attribute__((visibility("hidden")))

__private_extern MADErr MADFG2Mad(const char *MADPtr, size_t size, MADMusic *theMAD, MADDriverSettings *init);
__private_extern MADErr ExtractMADFGInfo(void *info, void *AlienFile);
__private_extern MADErr TestMADFGFile(const void *AlienFile);

__private_extern MADErr MADH2Mad(const char *MADPtr, size_t size, MADMusic *theMAD, MADDriverSettings *init);
__private_extern MADErr ExtractMADHInfo(void *info, void *AlienFile);
__private_extern MADErr TESTMADH(const void* alienFile);

__private_extern MADErr MADI2Mad(const char *MADPtr, size_t size, MADMusic *theMAD, MADDriverSettings *init);
__private_extern MADErr ExtractMADIInfo(void *info, void *AlienFile);
__private_extern MADErr TESTMADI(const void* alienFile);

#endif
