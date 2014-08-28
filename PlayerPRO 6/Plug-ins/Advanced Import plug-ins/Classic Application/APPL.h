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

__private_extern MADErr MADFG2Mad(char *MADPtr, long size, MADMusic *theMAD, MADDriverSettings *init);
__private_extern MADErr ExtractMADFGInfo(void *info, void *AlienFile);
__private_extern MADErr TestMADFGFile(void *AlienFile);

__private_extern MADErr MADH2Mad(char *MADPtr, size_t size, MADMusic *theMAD, MADDriverSettings *init);
__private_extern MADErr ExtractMADHInfo(void *info, void *AlienFile);
__private_extern MADErr TESTMADH(void* alienFile);

__private_extern MADErr MADI2Mad(char *MADPtr, size_t size, MADMusic *theMAD, MADDriverSettings *init);
__private_extern MADErr ExtractMADIInfo(void *info, void *AlienFile);
__private_extern MADErr TESTMADI(void* alienFile);

#endif
