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

__private_extern__ OSErr MADFG2Mad(char *MADPtr, long size, MADMusic *theMAD, MADDriverSettings *init);
__private_extern__ OSErr ExtractMADFGInfo(void *info, void *AlienFile);
__private_extern__ OSErr TestMADFGFile(void *AlienFile);

__private_extern__ OSErr MADH2Mad(char *MADPtr, size_t size, MADMusic *theMAD, MADDriverSettings *init);
__private_extern__ OSErr ExtractMADHInfo(void *info, void *AlienFile);
__private_extern__ OSErr TESTMADH(void* alienFile);

__private_extern__ OSErr MADI2Mad(char *MADPtr, size_t size, MADMusic *theMAD, MADDriverSettings *init);
__private_extern__ OSErr ExtractMADIInfo(void *info, void *AlienFile);
__private_extern__ OSErr TESTMADI(void* alienFile);

#endif
