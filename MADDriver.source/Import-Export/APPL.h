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

__private_extern OSErr MADFG2Mad(char *MADPtr, long size, MADMusic *theMAD, MADDriverSettings *init);
__private_extern OSErr ExtractMADFGInfo(void *info, void *AlienFile);
__private_extern OSErr TestMADFGFile(void *AlienFile);

__private_extern OSErr MADH2Mad(char *MADPtr, size_t size, MADMusic *theMAD, MADDriverSettings *init);
__private_extern OSErr ExtractMADHInfo(void *info, void *AlienFile);
__private_extern OSErr TESTMADH(void* alienFile);

__private_extern OSErr MADI2Mad(char *MADPtr, size_t size, MADMusic *theMAD, MADDriverSettings *init);
__private_extern OSErr ExtractMADIInfo(void *info, void *AlienFile);
__private_extern OSErr TESTMADI(void* alienFile);

#endif
