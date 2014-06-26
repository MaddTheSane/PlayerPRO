//
//  VSTFunctions.h
//  PPMacho
//
//  Created by C.W. Betts on 2/6/14.
//
//

#ifndef PPMacho_VSTFunctions_h
#define PPMacho_VSTFunctions_h

#include "RDriver.h"

#ifdef __cplusplus
extern "C" {
#endif

void		DisposeVSTEffect(VSTEffect *myEffect);
VSTEffect*	CreateVSTEffect(short effectID);
short		ConvertUniqueIDToIndex(uint32_t);
void		ApplyVSTSets(VSTEffect* myEffect, FXSets* set);
bool		IsVSTChanEffect(MADDriverRec *intDriver, short channel);
void		ProcessVSTPlug(MADDriverRec *intDriver, int *data, int datasize, short channel);

#ifdef __cplusplus
}
#endif

#endif
